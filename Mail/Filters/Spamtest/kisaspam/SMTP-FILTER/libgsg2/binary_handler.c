/* -*- C -*-
 * File: binary_handler.c
 *
 * Created: Wed Feb 25 10:30:03 2004
 */

#include <sys/types.h>
# ifndef _WIN32
#ifdef OS_FREEBSD
#  include <sys/syslimits.h>
#else
#  include <sys/param.h>
#endif
# endif /*_WIN32*/
#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
#include <syslog.h>
#endif //_WIN32
#include <_unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <smtp-filter/libgsg2/gsg.h>
#include <ctype.h>
#include <memory.h>

#include <smtp-filter/libosr/osr_c.h>
#include <smtp-filter/libosr/dump_c.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif

#ifdef DUMP
void dump_image(char *mask, int count, void *data, size_t size)
{
#ifdef _WIN32
    #define PATH_MAX 1024
#endif  /*_WIN32*/
    char filename[PATH_MAX];
    int fd;
	snprintf(filename,PATH_MAX,"%s-%d.dump",mask,count);
	fd = open(filename,O_CREAT|O_TRUNC|O_RDWR
#ifdef _WIN32
                                                  |O_BINARY
#endif  /*_WIN32*/
                   ,0666);
	if(fd>=0)
	{
		write(fd,data,size);
		close(fd);
	}
}
#endif

static int image_dump_bw_data(binary_data_t *bd, int filtered)
{
    int i;
    int ret = 0;
    size_t pixels;
    image_data_t *id;

    if(!bd||!bd->image.gray || !bd->image.graym || !bd->image.bw 
       || (pixels = image_pixels(&bd->image))<1 || pixels > MAX_IMAGE_PIXELS)
	return -1;
    id = &bd->image;

#ifdef DUMP
    if(!filtered)
        if (bd->generate_old_dumps)
	        dump_image("gray",1,id->gray,pixels);
#endif

    if(image_make_histogram(id,filtered)<0)
	return -1;
#if 0
    /* XXX alk: this code will not work with new md5list_put */
    if(id->size_x > 32)
	{
	    if(image_hist(id,0,filtered)<0)
		return -1;
	    md5list_put(bd->md5list,&id->hg,SIG_GSG5,bd->part_count);
	}
    if(id->size_y > 32)
	{
	    if(image_hist(id,1,filtered)<0)
		return -1;
	    md5list_put(bd->md5list,&id->hg,SIG_GSG5,bd->part_count);
	}
#endif
#ifdef DUMP
    if(!filtered)
        if (bd->generate_old_dumps)
	        dump_image("gray",2,id->gray,pixels);
#endif
    for(i=0;i<NLEVELS; i++)
	{
	    if(id->levels[i]>=0)
		{
		    MD5_CTX ctx;
		    md5sum md5;

           if(i>0 && id->levels[i]==id->levels[i-1])
               continue;
           if(image_to_bw(id,id->levels[i],filtered)<=0)
               return -1;
		    ret++;
		    bzero(&ctx,sizeof(ctx));
		    MD5Init(&ctx);
		    MD5Update(&ctx,id->bw,(pixels+7)/8);
		    MD5Final(md5,&ctx);
		    md5list_put(bd->md5list,&md5,filtered?SIG_GSG4:SIG_GSG3,bd->part_count, bd->images_info[bd->image_count].frames_count, bd->used);
#if 0
		    if(!filtered)
			{
                if (bd->generate_old_dumps)
                {
			        bw2gray(&bd->image);
			        dump_image("bw2gray-original",i,id->graym,pixels);
                }
			    
			    image_median_filter(&bd->image,1);
			    bzero(&ctx,sizeof(ctx));
			    MD5Init(&ctx);
			    MD5Update(&ctx,id->graym,pixels);
			    MD5Final(md5,&ctx);
			    md5list_put(bd->md5list,&md5,SIG_GSG6,bd->part_count, bd->images_info[bd->image_count].frames_count, bd->used);
                if (bd->generate_old_dumps)
			        dump_image("bw2gray-filtered",i,id->graym,pixels);
			}
#endif
		}
	}
    return ret;
}
 
#define CHECKSPACE 50
#define MINSIZE 5

int scale_gray_array_gif (binary_data_t *bd, unsigned char **backup, size_t *backup_size)
{
    float AScale = 0.0;
    int XSize,YSize,pixels;
    int	i;
    int old_w = bd->image.size_x, old_h = bd->image.size_y;
    int Scale = 1;
    int j, i2, j2, k;
    
    if(bd->image.size_x > 800 && bd->image.size_y >800)
        AScale = 0.125;
    else if(bd->image.size_x > 400 && bd->image.size_y >400)
        AScale = 0.25;
    else if(bd->image.size_x > 200 && bd->image.size_y >200)
        AScale = 0.5;
    if (AScale <= 0)
        return 0;
    Scale = (int)(1.0/AScale);
    if (1 == Scale)
        return 0;
    if (0 == Scale)
        return 1;

    XSize = (int) (bd->image.size_x * AScale + 0.5);
    YSize = (int) (bd->image.size_y * AScale + 0.5);

    if ( XSize < 5 || YSize < 5 )
        return IMAGE_TOO_SMALL;

    if(XSize > MAX_IMAGE_WIDTH || YSize > MAX_IMAGE_HEIGHT
       || (pixels = XSize*YSize) > MAX_IMAGE_PIXELS || bd->image.size_x > MAX_IMAGE_WIDTH*MAX_GIF_RESCALE)
        return IMAGE_TOO_LARGE;
    
    bd->image.size_x = XSize;
    bd->image.size_y = YSize;

    if (backup && backup_size)
    {
        *backup_size = pixels;
        *backup = malloc (pixels);
        if (0 == *backup)
            return -1;
        memcpy (*backup, bd->image.gray, pixels);
    }

    for (i = 0; i < old_h; i ++) 
        if ( i % Scale == 0 )
        {
            i2 = i / Scale;
            for (j = 0; j < old_w; j ++) 
                if ( j % Scale == 0 )
                {
                    j2 = j / Scale;
                    k = i2*XSize+j2;
                    if (k < pixels)
                        bd->image.gray[k] = bd->image.gray[i*old_w+j];
                }
        }

    return 0;
}
int scale_gray_array_png (binary_data_t *bd)
{
    return scale_gray_array_gif(bd, 0, 0);
}
int scale_gray_array_jpg (binary_data_t *bd)
{
    return scale_gray_array_gif(bd, 0, 0);
}

typedef enum
{
    PROCESS_FRAME,
    IGNORE_FRAME,
    STOP_PROCESS
} animation_action_type_t;

animation_action_type_t do_process_this_gif_frame (binary_data_t *bd)
{
    if (0 == bd)
        return STOP_PROCESS;

    if (0 == bd->next_frame )
    {
        if( 0 == bd->images_info[bd->image_count].processed_frames_count && 
            bd->images_info[bd->image_count].frames_count > 0 )
        {
            bd->images_info[bd->image_count].frames_count --;
            return PROCESS_FRAME;
        }
        else
            return STOP_PROCESS;
    }
    
    if ( bd->images_info[bd->image_count].frames_count >= bd->max_frames_count )
        return IGNORE_FRAME;
    if ( bd->all_frames_duration_hsec >= bd->max_duration_hsec + bd->image.prev_frame_duration )
        return IGNORE_FRAME;

    if (bd->image.prev_frame_duration < bd->min_animated_frame_duration_hsec)
        return IGNORE_FRAME;

    if (bd->all_frames_duration_hsec < bd->prev_proc_hsec + bd->min_duration_step_hsec )
        return IGNORE_FRAME;

    bd->prev_proc_hsec = bd->all_frames_duration_hsec;

    return PROCESS_FRAME;
}

typedef enum
{
    ENDPART_STATUS_NULL,
    ENDPART_STATUS_IMG_HEADER,
    ENDPART_STATUS_IMG_LOADED_JPEG,
    ENDPART_STATUS_IMG_LOADED_PNG,
    ENDPART_STATUS_IMG_LOADED_GIF
} endpart_status_t;

int binary_endpart(binary_data_t *bd)
{
    size_t image_size;
    md5sum md5;
    int bwd;
#ifdef GSG_BINARY_DUMP
    int fd;
    char *tmpl="/tmp/gsg.XXXXXXXX";
    char buf[128];
#endif
    int unpack_result = -1;
    char md5_gsg7_str [33];
    unsigned char *backup = 0;
    size_t backup_size = 0;
    endpart_status_t status = ENDPART_STATUS_NULL;

// GSG
    if(!bd) 
	return ERR_UNKNOWN;

    if (bd->image_count >= bd->images_info_count)
    {
        unsigned int new_size = 0;
        new_size = bd->image_count * 2 + 1;
        bd->images_info = realloc (bd->images_info, new_size*sizeof(images_info_t));
        if (0 == bd->images_info)
            return ERR_OVERFLOW;
        bzero (bd->images_info + bd->images_info_count, (new_size - bd->images_info_count)*sizeof(images_info_t));
        bd->images_info_count = new_size;
    }

    if(!bd->processed) // zero data
	goto END;

#ifdef GSG_BINARY_DUMP
    if(bd->used > 0)
	{
	    strncpy(buf,tmpl,128);
#ifdef /*_WIN32*/
        fd = open(buf, O_CREAT|O_BINARY|O_RDWR);
#else  /*_WIN32*/
	    fd = mkstemp(buf);
#endif /*_WIN32*/
	    if(fd>=0)
		{
		    write(fd,bd->data,bd->used);
		    close(fd);
		    chmod(buf,0644);
		}
	}
#endif
    
    MD5Final(md5,&bd->ctx);
    if(bd->used < MINSIZE)
	{
	    // nothing
	}
    else if( bd->used < CHECKSPACE)
	{
	    size_t i;
	    size_t spacecnt=0;
	    for(i=0;i<bd->used;i++)
		if(isspace(bd->data[i])) spacecnt++;
	    if(spacecnt*2<bd->used)
		md5list_put(bd->md5list,&md5,SIG_GSG1,bd->part_count, bd->images_info[bd->image_count].frames_count, bd->used);
		
	}
    else
	{
	    md5list_put(bd->md5list,&md5,SIG_GSG1,bd->part_count, bd->images_info[bd->image_count].frames_count, bd->used);
	}
    
    if(bd->partial)
       goto END;
// GSG2 Unpacked image
    bd->next_frame = 0;
    do 
    {
        if(bd->data && bd->used > 6)
        {
            MD5_CTX ctx;
            md5sum md5;
            osr_expert *expert = 0;

            md5_gsg7_str[0] = 0;
        
            if(!memcmp("GIF89a",bd->data,6) || !memcmp("GIF87a",bd->data,6))
            {
                status = ENDPART_STATUS_IMG_HEADER;
                if ( (unpack_result=unpack_gif(bd, UNPACKED_GIF_CAN_BE_SCALED)) < 0)
                    goto END;
                status = ENDPART_STATUS_IMG_LOADED_GIF;
                switch ( do_process_this_gif_frame (bd))
                {
                case STOP_PROCESS:
                    goto END;
                	break;
                case IGNORE_FRAME:
                    bd->images_info[bd->image_count].frames_count ++;
                    continue;
                	break;
                case PROCESS_FRAME:
                default:
                    bd->images_info[bd->image_count].processed_frames_count ++;
                    break;
                }
            }
            else if(bd->data[0]==255 && bd->data[1] == 216 && bd->data[2]==255
                && (!memcmp("JFIF",bd->data+6,4) ||!memcmp("Exif",bd->data+6,4)))
            {
                status = ENDPART_STATUS_IMG_HEADER;
                if((unpack_result=unpack_jpeg(bd))<0)
                    goto END;
                status = ENDPART_STATUS_IMG_LOADED_JPEG;
            }
            else if( !memcmp("\x89PNG",bd->data,4) )
            {
                status = ENDPART_STATUS_IMG_HEADER;
                if((unpack_result=unpack_png(bd))<0)
                    goto END;
                status = ENDPART_STATUS_IMG_LOADED_PNG;
            }
            else
                goto END;
                    
            image_size = image_pixels(&bd->image);
#if 0
            {
                int fd = open("dump.gray",O_CREAT|O_TRUNC|O_RDWR,0666);
                write(fd,bd->image.gray,image_size);
                close(fd);
            }
#endif
            if ( IMAGE_TOO_SMALL == unpack_result || IMAGE_TOO_LARGE == unpack_result )
                goto END;

            backup = 0; backup_size = 0;
            if (IMAGE_TOO_LARGE_FOR_GSG7 != unpack_result)
            {
                osr_expert *expert_b = 0, *expert_w = 0;
                // GSG7 cant use scaled image

                if ( BACKGROUND_DETECTION_ALTERNATIVE == bd->background_detection_mode )
                {
                    unsigned int lines_count_b = 0, lines_count_w = 0;
                    expert_b = osr_expert_create();
                    expert_w = osr_expert_create();
                    if (expert_b && expert_w)
                    {
                        osr_set_filter (expert_b, &bd->osr_params);
                        osr_use_background_black (expert_b);
                        osr_set_filter (expert_w, &bd->osr_params);
                        osr_use_background_white (expert_w);
                        if ( OSR_OK == osr_process_gray_array (expert_b, bd->image.gray, bd->image.size_x, bd->image.size_y) &&
                             OSR_OK == osr_process_gray_array (expert_w, bd->image.gray, bd->image.size_x, bd->image.size_y) )
                            if ( OSR_OK == osr_get_lines_count (expert_b, &lines_count_b) &&
                                 OSR_OK == osr_get_lines_count (expert_w, &lines_count_w) )
                            {
                                if (lines_count_b > lines_count_w)
                                {
                                    expert = expert_b;
                                    osr_expert_destroy(expert_w);
                                }
                                else
                                {
                                    expert = expert_w;
                                    osr_expert_destroy(expert_b);
                                }
                            }
                    }
                }
                else
                {
                    expert = osr_expert_create();
                    if (expert)
                    {
                        osr_set_filter (expert, &bd->osr_params);
                        osr_process_gray_array (expert, bd->image.gray, bd->image.size_x, bd->image.size_y);
                    }
                }

                if (expert)
                {
                    osr_err_code err = OSR_OK;
                    unsigned int symbols_count = 0;
                    unsigned int words_count = 0;
                    unsigned int lines_count = 0;
                    unsigned int text_percentage = 0;
                    unsigned int image_width = 0;
                    unsigned int image_height =0;
                    char is_text = 0;
            
                    if ( OSR_OK == err )
                        err = osr_get_verdict (expert, &is_text);
                    if ( OSR_OK == err )
                        err = osr_get_symbols_count (expert, &symbols_count);
                    if ( OSR_OK == err )
                        err = osr_get_words_count (expert, &words_count);
                    if ( OSR_OK == err )
                        err = osr_get_lines_count (expert, &lines_count);
                    if ( OSR_OK == err )
                        err = osr_get_GSG7_signature (expert, md5_gsg7_str);
                    if ( OSR_OK == err )
                        err = osr_get_words_pixels_percentage (expert, &text_percentage);
                    if ( OSR_OK == err )
                        err = osr_get_image_size (expert, &image_width, &image_height);
                    if ( OSR_OK == err )
                    {
                        if (is_text)
                            bd->text_frame_count ++;
                        bd->all_frames_symbols_count += symbols_count;
                        bd->all_frames_words_count += words_count;
                        bd->all_frames_lines_count += lines_count;
            
                        if ( ( lines_count > bd->best_frame_lines_count              ) ||
                             (      lines_count == bd->best_frame_lines_count &&
                                    words_count > bd->best_frame_words_count         ) ||
                             (      lines_count == bd->best_frame_lines_count &&
                                    words_count == bd->best_frame_words_count &&
                                    symbols_count > bd->best_frame_symbols_count     ) ||
                             (      lines_count == bd->best_frame_lines_count &&
                                    words_count == bd->best_frame_words_count &&
                                    symbols_count == bd->best_frame_symbols_count &&
                                    text_percentage > bd->best_frame_text_percentage )
                           )
                        {
                             bd->best_frame_lines_count = lines_count;
                             bd->best_frame_words_count = words_count;
                             bd->best_frame_symbols_count = symbols_count;
                             bd->best_frame_text_percentage = text_percentage;
                             bd->best_frame_width = image_width;
                             bd->best_frame_height = image_height;
                        }
                    }
#ifdef WITH_DUMP
                    if (bd->generate_old_dumps)
                        osr_get_dump(expert, "osr_dump.png", 1, 0, 0, 1);
#endif /* WITH_DUMP */
                    osr_expert_destroy(expert);
                }
                // scale image
                if (ENDPART_STATUS_IMG_LOADED_GIF == status)
                {
#ifdef WITH_DUMP
                    if (bd->generate_gif_frames && bd->current_file_name)
                    {
                        dump_t *dump = dump_create(bd->image.size_x, bd->image.size_y);
                        if (dump)
                        {
                            char dump_name [500];
                            if ( 0 == dump_set_gray_pixels (dump, bd->image.gray, image_size) )
                            {
                                sprintf (dump_name, "%s.kasanidump.%.2d.png", bd->current_file_name,
                                    bd->images_info[bd->image_count].frames_count);
                                dump_save(dump, dump_name);
                            }
                            dump_destroy(dump);
                        }
                    }
#endif /* WITH_DUMP */
                    /* bd->image.gray could be changed by scaling
                       lets save bd->image.gray into backup for processing transparence of next frame of animated gif
                       bd->image.gray will be restored from the backup after GSG2-4 
                       (backup size will not more than 200*200 bytes)
                    */
                    if ( 0 != scale_gray_array_gif (bd, &backup, &backup_size) )
        	            goto END;
                }
                else  if (ENDPART_STATUS_IMG_LOADED_PNG == status)
                {
                    if ( 0 != scale_gray_array_png (bd) )
        	            goto END;
                }
                else  if (ENDPART_STATUS_IMG_LOADED_JPEG == status)
                {
                    if ( 0 != scale_gray_array_jpg (bd) )
        	            goto END;
                }

                image_size = image_pixels(&bd->image);
            }
            
            bd->images_info[bd->image_count].width  = bd->image.size_x;
            bd->images_info[bd->image_count].height = bd->image.size_y;

            bzero(&ctx,sizeof(ctx));
            MD5Init(&ctx);
            MD5Update(&ctx,bd->image.gray,image_size);
            MD5Final(md5,&ctx);
            md5list_put(bd->md5list,&md5,SIG_GSG2,bd->part_count, bd->images_info[bd->image_count].frames_count, bd->used);
    
            // vertical image
            if(bd->image.size_x <8 ||
               (bd->image.size_x <16 && bd->image.size_y <100)
	        )
	        goto END;
            // horizontal image
            if(bd->image.size_y <8 ||
               (bd->image.size_y <16 && bd->image.size_x <100)
	        )
	        goto END;
    // GSG3-GSG4
            if(bd->image.gray)
	            bwd=image_dump_bw_data(bd,0);
            if( bd->image.gray && bd->image.graym && bwd>0)
	        {
	            image_median_filter(&bd->image,0);
#ifdef DUMP
                if (bd->generate_old_dumps)
	                dump_image("gray-filtered",0,bd->image.graym,image_pixels(&bd->image));
#endif
	            image_dump_bw_data(bd,1);
	        }
            if (backup)
            {
                /* restore not scaled backup (see comment above) */
                memcpy (bd->image.gray, backup, backup_size);
                free(backup);
            }
            if (md5_gsg7_str[0]) // gsg7 was deffered for good signatures order
            {
                md5sum md5_gsg7;
                str2md5(md5_gsg7, md5_gsg7_str);
                md5list_put(bd->md5list,&md5_gsg7,SIG_GSG7,bd->part_count, bd->images_info[bd->image_count].frames_count, bd->used);
            }
            bd->images_info[bd->image_count].frames_count ++;
        }
    } while ( bd->next_frame );

  END:
    switch(status)
    {
    case ENDPART_STATUS_NULL:
    	break;
    case ENDPART_STATUS_IMG_HEADER:
        bd->corrupted_image_count ++;
    	break;
    case ENDPART_STATUS_IMG_LOADED_JPEG:
        bd->jpg_count++;
    	break;
    case ENDPART_STATUS_IMG_LOADED_PNG:
        bd->png_count++;
    	break;
    case ENDPART_STATUS_IMG_LOADED_GIF:
        bd->gif_count++;
        if (bd->next_frame)
			close_gif (bd);
    	break;
    default:
    	break;
    }

    bd->frame_count += bd->images_info[bd->image_count].frames_count;
    bd->images_info[bd->image_count].part_no = bd->part_count;
    if (  ENDPART_STATUS_IMG_LOADED_JPEG == status  ||
          ENDPART_STATUS_IMG_LOADED_PNG  == status  ||
          ENDPART_STATUS_IMG_LOADED_GIF  == status     )
        bd->image_count++;
    bd->part_count++;
    binary_clear(bd);

    return 0;
}
