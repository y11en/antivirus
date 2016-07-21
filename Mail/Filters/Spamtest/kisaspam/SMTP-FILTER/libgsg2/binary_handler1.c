/* -*- C -*-
 * File: binary_handler1.c
 *
 * Created: Wed Feb 25 19:30:23 2004
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif

#include <smtp-filter/libgsg2/gsg.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifndef _WIN32
#include <jpeglib.h>
#else
#include <gnu/jpeg/jpeglib.h>
#endif

#include "_include/ntoh.h"

void md5list_free(md5_list_t *m)
{
    if(m)
	{
	    if(m->array) free(m->array);
	    free(m);
	}
}


md5_list_t *md5list_create(void)
{
    md5_list_t *ret = malloc(sizeof(md5_list_t));
    if(ret)
	{
        ret->array = (md5_element_t*)malloc(sizeof(md5_element_t)*MD5_ALLOC);
	    if(!ret->array)
		{
		    md5list_free(ret);
		    return NULL;
		}
	    ret->alloc = MD5_ALLOC;
	    ret->used = 0;
	}
    return ret;
}

int md5list_put_uniq(md5_list_t *m, md5sum *sum, unsigned int type,unsigned int no, unsigned int frameno, size_t size)
{
    size_t i;
    if(!m || !m->array || !m->alloc)
	return ERR_UNKNOWN;
    for(i=0;i<m->used;i++)
	if(m->array[i].type == type
	   && !memcmp(&m->array[i].md5,sum,sizeof(*sum)))
	    return ERR_OK;
    return md5list_put(m,sum,type,no, frameno, size);
}

int md5list_put(md5_list_t *m, md5sum *sum, unsigned int type, unsigned int no, unsigned int frameno, size_t size)
{
    if(!m || !m->array || !m->alloc)
	return ERR_UNKNOWN;
    if(m->used >= m->alloc)
	{
	    m->array = realloc(m->array,sizeof(md5_element_t)*(m->alloc*2));
	    if(!m->array)
		return ERR_UNKNOWN;
	    m->alloc*=2;
	}
    m->array[m->used].type    = type;
    m->array[m->used].imageno = no;
    m->array[m->used].frameno = frameno;
    m->array[m->used].size    = size;
    memmove(&m->array[m->used].md5,sum,sizeof(*sum));
    m->used++;
    return ERR_OK;
}

int binary_set_default_gsg_params(binary_data_t *bd)
{
    if (0 == bd)
        return -1;
    bd->min_animated_frame_duration_hsec = 0; // value is hundredth of second
    bd->max_frames_count = 1000;
    bd->max_duration_hsec = 60*100; // value is hundredth of second
    bd->background_detection_mode = BACKGROUND_DETECTION_AUTO;
    bd->min_duration_step_hsec = 0;
    osr_get_default_filter(&bd->osr_params);
    return 0;
}

void binary_free(binary_data_t **pbt)
{
    if(pbt && *pbt)
	{
		binary_data_t *bt = *pbt;
	    if(bt->md5list) md5list_free(bt->md5list);
	    if(bt->data) free(bt->data);
	    if(bt->rgb_ycc_table) free(bt->rgb_ycc_table);
	    image_data_free(&bt->image);
        if (bt->images_info) free(bt->images_info);
	    free(bt);
		*pbt = NULL;
	}
}


int binary_clear(binary_data_t *bt)
{
    if(!bt ||!bt->data ||!bt->alloc)
	return ERR_UNKNOWN;
    MD5Init(&bt->ctx);
    bt->used = 0;
    bt->processed=0;
    bt->partial = 0;
    image_data_clear(&bt->image);
    bt->next_frame = NULL;
    bt->all_frames_duration_hsec = 0;
    bt->prev_proc_hsec = 0;
    return ERR_OK;
}

int binary_clearall(binary_data_t *bt)
{
    if(!bt ||!bt->data ||!bt->alloc)
	return ERR_UNKNOWN;
    bzero(&bt->ctx,sizeof(bt->ctx));
    MD5Init(&bt->ctx);
    bt->used = 0;
    bt->processed=0;
    bt->partial = 0;
    bt->part_count=0;
    bt->image_count=0;
    bt->current_file_name = NULL;

    bt->generate_old_dumps = 0;
    bt->generate_gif_frames = 0;

    if(bt->md5list && bt->md5list->array)
	{
	    bzero(bt->md5list->array,bt->md5list->used*sizeof(md5_element_t));
	    bt->md5list->used = 0;
	}
    image_data_clear(&bt->image);

    bt->corrupted_image_count = 0;
    bt->frame_count = 0;
    bt->text_frame_count = 0;
    bt->all_frames_symbols_count = 0;
    bt->all_frames_words_count = 0;
    bt->all_frames_lines_count = 0;
    bt->best_frame_symbols_count = 0;
    bt->best_frame_words_count = 0;
    bt->best_frame_lines_count = 0;
    bt->best_frame_text_percentage = 0;
    bt->best_frame_width = 0;
    bt->best_frame_height = 0;

    bt->jpg_count = 0;
    bt->png_count = 0;
    bt->gif_count = 0;

    bt->next_frame = NULL;
    bt->prev_proc_hsec = 0;
    bt->all_frames_duration_hsec = 0;

    bzero (bt->images_info, bt->images_info_count*sizeof(images_info_t));

    return ERR_OK;
}

#define SCALEBITS	16	/* speediest right-shift on some machines */
#define CBCR_OFFSET	((INT32) CENTERJSAMPLE << SCALEBITS)
#define ONE_HALF	((INT32) 1 << (SCALEBITS-1))
#define FIX(x)		((INT32) ((x) * (1L<<SCALEBITS) + 0.5))

/* We allocate one big table and divide it up into eight parts, instead of
 * doing eight alloc_small requests.  This lets us use a single table base
 * address, which can be held in a register in the inner loops on many
 * machines (more than can hold all eight addresses, anyway).
 */

#define TABLE_SIZE	(8*(MAXJSAMPLE+1))


static unsigned long* _rgb_ycc_start (void)
{
  INT32 * rgb_ycc_tab;
  INT32 i;

  /* Allocate and fill in the conversion tables. */
  rgb_ycc_tab = (INT32 *) malloc(TABLE_SIZE * sizeof(INT32));
  if(!rgb_ycc_tab)
//  if(!(rgb_ycc_tab = (INT32 *) malloc(TABLE_SIZE * sizeof(INT32))))
      return NULL;
  for (i = 0; i <= MAXJSAMPLE; i++) {
    rgb_ycc_tab[i+R_Y_OFF] = FIX(0.29900) * i;
    rgb_ycc_tab[i+G_Y_OFF] = FIX(0.58700) * i;
    rgb_ycc_tab[i+B_Y_OFF] = FIX(0.11400) * i     + ONE_HALF;
    rgb_ycc_tab[i+R_CB_OFF] = (-FIX(0.16874)) * i;
    rgb_ycc_tab[i+G_CB_OFF] = (-FIX(0.33126)) * i;
    /* We use a rounding fudge-factor of 0.5-epsilon for Cb and Cr.
     * This ensures that the maximum output will round to MAXJSAMPLE
     * not MAXJSAMPLE+1, and thus that we don't have to range-limit.
     */
    rgb_ycc_tab[i+B_CB_OFF] = FIX(0.50000) * i    + CBCR_OFFSET + ONE_HALF-1;
/*  B=>Cb and R=>Cr tables are the same
    rgb_ycc_tab[i+R_CR_OFF] = FIX(0.50000) * i    + CBCR_OFFSET + ONE_HALF-1;
*/
    rgb_ycc_tab[i+G_CR_OFF] = (-FIX(0.41869)) * i;
    rgb_ycc_tab[i+B_CR_OFF] = (-FIX(0.08131)) * i;
  }
  return (unsigned long*)rgb_ycc_tab;
}





binary_data_t* binary_init(size_t sz)
{
    binary_data_t *ret = malloc(sizeof(binary_data_t));
    if(ret)
	{
	    bzero(ret,sizeof(*ret));
	    ret->data = malloc(sz);
	    ret->md5list = md5list_create();
        ret->images_info_count = 10;
        ret->images_info = calloc (ret->images_info_count, sizeof (images_info_t));
	    ret->rgb_ycc_table = _rgb_ycc_start();
	    if(!ret->data || !ret->md5list || image_data_init(&ret->image) || !ret->images_info)
		{
		    binary_free(&ret);
		    return NULL;
		}
	    ret->alloc = sz;
	    MD5Init(&ret->ctx);
        binary_set_default_gsg_params(ret);
	}
    return ret;
}


int binary_put(binary_data_t *bd, void *data, size_t sz)
{
    if(!bd) return ERR_UNKNOWN;
    MD5Update(&bd->ctx,data,sz);
    bd->processed+=sz;
    if(bd->used + sz > bd->alloc)
	{
	    bd->partial=1;
	    return ERR_OVERFLOW;
	}
    memmove(bd->data+bd->used,data,sz);
    bd->used+=sz;
    return ERR_OK;
}

void binary_opensigdb(binary_data_t *bd,char *s)
{
    if(bd && s)
	bd->sigarray = loadmd5sig(s);

}

void binary_setsigdb(binary_data_t *bd,md5sigarray *m)
{
    if(bd) bd->sigarray=m;
}

// return size of could loaded/saved bytes, or -1 if critical errors
int binary_serialize_gsg_params(binary_data_t *bd, unsigned char *params,
                          unsigned int size, char save /*save (1) or load (0)*/)
{
    /* please increment GSG_PARAMS_VERSION and actualize GSG_PARAMS_APPROX_SIZE
       if format has been modifyed */

#ifdef _MSC_VER
# pragma warning( push )
# pragma warning( disable : 4204 )
#endif _MSC_VER

    unsigned int* vals [] = {   (unsigned int*)&bd->min_animated_frame_duration_hsec,
                                &bd->max_frames_count,
                                &bd->max_duration_hsec,
                                &bd->background_detection_mode,

                                &bd->osr_params.min_symbol_width,
                                &bd->osr_params.max_symbol_width,
                                &bd->osr_params.min_symbol_height,
                                &bd->osr_params.max_symbol_height,
                                &bd->osr_params.max_symbol_fat_percent,
        
                                &bd->osr_params.min_symbols_in_word,
                                &bd->osr_params.max_symbols_in_word,
                                &bd->osr_params.min_words_in_line,
                                &bd->osr_params.min_lines_in_text,
        
                                &bd->osr_params.min_text_percentage,
                                &bd->min_duration_step_hsec };
#ifdef _MSC_VER
# pragma warning( pop ) 
#endif _MSC_VER

    unsigned int i = 0, vals_count = sizeof (vals) / sizeof(unsigned int*),
                        val_size = sizeof(unsigned int);
    unsigned int ver = save?GSG_PARAMS_VERSION:0;
    unsigned int l = 0;
    char size_ok = 1;

    if (0 == bd || 0 == params)
        return -1;

    if (l + GSG_PARAMS_MAGIC_SIZE > size)
        size_ok = 0;
    if (size_ok)
    {
        if (save)
            memcpy (&params[l], GSG_PARAMS_MAGIC, GSG_PARAMS_MAGIC_SIZE);
        else
            if (0 != memcmp (GSG_PARAMS_MAGIC, &params[l], GSG_PARAMS_MAGIC_SIZE) )
                return -1;
    }
    l += GSG_PARAMS_MAGIC_SIZE;

    if (l + sizeof(unsigned int) > size)
        size_ok = 0;
    if (size_ok)
    {
        if (save)
            *((u_int32_t *)&params[l]) = htonl(ver);
        else
            ver = ntohl(*((u_int32_t *)&params[l]));
    }
    l += sizeof(unsigned int);

    for ( i = 0; i < vals_count; i ++ )
    {
        if (l + val_size > size)
            size_ok = 0;
        if (size_ok)
        {
            if (save)
                *((u_int32_t *)&params[l]) = htonl(*vals [i]);
            else
                *vals [i] = ntohl(*((u_int32_t *)&params[l]));
        }
        l += val_size;
    }
    return l;
}
