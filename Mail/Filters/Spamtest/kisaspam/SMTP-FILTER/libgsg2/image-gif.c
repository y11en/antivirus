/* -*- C -*-
 * File: image-gif.c
 *
 * Created: Fri Feb 27 14:10:01 2004
 */

#include <smtp-filter/libgsg2/gsg.h>
#include <smtp-filter/ungif/lib/gif_lib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif


int GifReader (GifFileType* gif,GifByteType* buf, int sz0)
{
    unsigned int sz = (unsigned int ) sz0;
    binary_data_t *bd = (binary_data_t *)gif->UserData;
//    printf("GifReader called  with %d/%d/%d bytes of data\n",bd->used,bd->readp,sz);
    if(bd->used <= bd->readp) return -1;
    if(sz>bd->used-bd->readp) sz=bd->used-bd->readp;
    if(sz<40)
	{
	    size_t i;
	    for(i=0;i<sz;i++)
		buf[i]=bd->data[bd->readp+i];
	}
    else
	memmove(buf,bd->data+bd->readp,sz);
    bd->readp+=sz;
    return sz;
}


static int    InterlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
static int    InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */

/* unpack_gif will return:
     not scaled image for GSG7 <= 1500*1500, (after GSG7 image will be scaled and used for GSG2-4 <= 1000*1000)
  or scaled image for GSG2-4 (GSG7 will be ignored) ( >= 1500*1500 ) ->  ( <= 1000*1000 )
  or error code: IMAGE_TOO_SMALL,IMAGE_TOO_LARGE (>0), gif-error (<0)
     IMAGE_TOO_LARGE_FOR_GSG7 - is not error, this is only precondition for GSG2-4
*/
#define TRANSPARANCE_FLAG   0x1
#define RESTORE_MASK        0x1C
#define RESTORE_BCKGROUND   0x8
#define RESTORE_PREVOIUS    0xC

unsigned char uncolor_pixel (binary_data_t *bd, GifFileType* gif, int color_index)
{
    ColorMapObject *ColorMap;
    if (NULL == gif)
        return 0;
    ColorMap = (gif->Image.ColorMap
		? gif->Image.ColorMap
		: gif->SColorMap);
    if (NULL == ColorMap)
        return (unsigned char)color_index;
	if (color_index < 0 || color_index >= ColorMap->ColorCount)
		return (unsigned char)color_index;
    return (unsigned char) pvt_rgb_gray_convert(bd, ColorMap->Colors[color_index].Red,
                                    ColorMap->Colors[color_index].Green,
					                ColorMap->Colors[color_index].Blue );
}    

int unpack_gif(binary_data_t *bd, char need_only_not_scaled)
{
    GifFileType *gif;
    GifByteType *Extension;
    GifRecordType RecordType;
    int Row, Col, Width, Height,i,j,ExtCode;
    char image_too_large_for_gsg7 =0;
    size_t x,y;
    int Scale = 1;
    float AScale = 1.0;
    int itl, i2, j2, k;
    int pixels,XSize,YSize;
    

    if(!bd ||!bd->data || bd->used < 6) return -1;
    if(!bd->image.gray) return -1;

    if ( NULL == bd->next_frame ) // first frame loading
    {
        bd->readp = 0;
        gif = DGifOpen(bd,GifReader);
        if(!gif) return -1;
    }
    else
        gif = (GifFileType*)bd->next_frame;

    if ( gif->SWidth < 1 ||gif->SHeight < 1 )
    {
		DGifCloseFile(gif);
        return IMAGE_TOO_SMALL;
    }
    // gsg-7 need source raster without scaling (scaling does lump symbols, and gsg-7 couldn' work)
    image_too_large_for_gsg7 = (char)
            ( gif->SWidth > MAX_IMAGE_WIDTH || gif->SHeight > MAX_IMAGE_HEIGHT
              || (gif->SHeight*gif->SWidth) > MAX_IMAGE_PIXELS);
    if (image_too_large_for_gsg7)
    {
        if (need_only_not_scaled)
        {
    		DGifCloseFile(gif);
            return IMAGE_TOO_LARGE_FOR_GSG7;
        }
        // source image is too large for allocated buffer
        if(gif->SWidth > 800 && gif->SHeight>800)
            AScale = 0.125;
        else if(gif->SWidth > 400 && gif->SHeight>400)
            AScale = 0.25;
        else if(gif->SWidth > 200 && gif->SHeight>200)
            AScale = 0.5;
    }
    assert (AScale > 0.0);
    Scale = (int)(1.0/AScale);
    assert (Scale > 0);

    XSize = (int) (gif->SWidth * AScale + 0.5);
    YSize = (int) (gif->SHeight * AScale + 0.5);

    if(XSize < 5 || YSize < 5)
	{
	    DGifCloseFile(gif);
	    return IMAGE_TOO_SMALL;
	}
    if(XSize > MAX_IMAGE_WIDTH || YSize > MAX_IMAGE_HEIGHT
       || (pixels = XSize*YSize) > MAX_IMAGE_PIXELS || gif->SWidth > MAX_IMAGE_WIDTH*MAX_GIF_RESCALE)
	{
	    DGifCloseFile(gif);
	    return IMAGE_TOO_LARGE;
	}

    if ( ( RESTORE_BCKGROUND == (bd->image.prev_flag & RESTORE_MASK) || 
           RESTORE_PREVOIUS  == (bd->image.prev_flag & RESTORE_MASK)    ) &&
         bd->image.prev_l <= bd->image.prev_r && bd->image.prev_t <= bd->image.prev_b )
    {
        for ( y = bd->image.prev_t; y <= bd->image.prev_b; y ++ )
            for ( x = bd->image.prev_l; x <= bd->image.prev_r; x ++ )
            {
                i = y*XSize+x;
                    if ( RESTORE_BCKGROUND == (bd->image.prev_flag & RESTORE_MASK) )
                    bd->image.gray[i] = uncolor_pixel(bd, gif, gif->SBackGroundColor);
                else if ( RESTORE_PREVOIUS == (bd->image.prev_flag & RESTORE_MASK) && bd->image.pfev_gray )
                    bd->image.gray [i] = bd->image.pfev_gray [i];
            }
    }

    bd->image.size_x = XSize;
    bd->image.size_y = YSize;

    if (NULL == bd->next_frame) // first frame
        memset(bd->image.gray,gif->SBackGroundColor,pixels);
    // else - save previous frame as background of transparence

    do {
	if (DGifGetRecordType(gif, &RecordType) == GIF_ERROR) 
	    {
		DGifCloseFile(gif);
		return -1;
	    }
	switch (RecordType) 
	    {
	    case IMAGE_DESC_RECORD_TYPE:
        {
		    if (DGifGetImageDesc(gif) == GIF_ERROR) 
		        {
			    DGifCloseFile(gif);
			    return -1;
		        }
		    Col = (int) (gif->Image.Left * AScale + 0.5);
		    Width = (int) (gif->Image.Width * AScale + 0.5);
		    Row = (int) (gif->Image.Top * AScale + 0.5);
		    Height = (int) (gif->Image.Height * AScale + 0.5);
		    if (Col < 0) Col = 0;
		    if (Row < 0) Row = 0;
		    if (Col + Width > XSize) Width = XSize - Col;
		    if (Row + Height > YSize) Height = YSize - Row;
		    if (gif->Image.Left +gif->Image.Width > gif->SWidth ||
		        gif->Image.Top + gif->Image.Height > gif->SHeight) 
		        {
			    DGifCloseFile(gif);
			    return -1;
		        }

            if ( RESTORE_PREVOIUS  == (bd->image.prev_flag & RESTORE_MASK) )
            {
                /*** BACKUP CURRENT FRAME ***/
                if (NULL == bd->image.pfev_gray)
                    bd->image.pfev_gray = calloc(MAX_IMAGE_PIXELS, 1);
                if (NULL == bd->image.pfev_gray)
                {
			        DGifCloseFile(gif);
			        return -1;
                }
                bd->image.prev_b = bd->image.prev_t = Row;
                if (Height > 0)
                    bd->image.prev_b += Height-1;
                bd->image.prev_r = bd->image.prev_l = Col;
                if (Width > 0)
                    bd->image.prev_r += Width-1;
                for ( y = bd->image.prev_t; y <= bd->image.prev_b; y ++ )
                    for ( x = bd->image.prev_l; x <= bd->image.prev_r; x ++ )
                    {
                        i = y*XSize+x;
                        bd->image.pfev_gray [i] = bd->image.gray [i];
                    }
            }

            /* for gif->Image.Interlace Need to perform 4 passes on the images: */
            for ( itl = 0; itl < (gif->Image.Interlace?4:1); itl++)
                for (i = (gif->Image.Interlace?InterlacedOffset[itl]:0);
                     i <  gif->Image.Height;
                     i += (gif->Image.Interlace?InterlacedJumps[itl]:1) )
				{
				    if (DGifGetLine(gif, (unsigned char*)bd->image.hbuf,gif->Image.Width) == GIF_ERROR)
					{
					    DGifCloseFile(gif);
					    return -1;
					}
                    if ( (i + gif->Image.Top) % Scale == 0 )
                    {
                        i2 = ( i + gif->Image.Top) / Scale;
                        for (j = 0; j < gif->Image.Width; j ++)
                        {
                            if ( (j+gif->Image.Left) % Scale == 0 )
                            {
                                j2 = (j + gif->Image.Left) / Scale;
                                k = i2*XSize+j2;
                                if (k < pixels)
                                {
                                    if ( (bd->image.prev_flag & TRANSPARANCE_FLAG) && 
                                        ( bd->image.prev_transp_ind == ((unsigned char*)bd->image.hbuf)[j] ) )
                                    {
                                        if (NULL == bd->next_frame)
                                            bd->image.gray[k] = uncolor_pixel (bd, gif, gif->SBackGroundColor);
                                        else
                                            ; // stay previous pixel
                                    }
                                    else
                                        bd->image.gray[k] = uncolor_pixel(bd, gif, ((unsigned char*)bd->image.hbuf)[j]);
                                }
                            }
                        }
                    }
				}
        }
		break;
	    case EXTENSION_RECORD_TYPE: /* always precede the IMAGE_DESC_RECORD_TYPE in the animated gif */
		/* Skip any extension blocks in file: */
        {
		    if (DGifGetExtension(gif, &ExtCode, &Extension) == GIF_ERROR) 
		        {
			    DGifCloseFile(gif);
			    return -1;
		        }
            else
            {
                if ( 0xF9 == ExtCode)
                {
                    bd->image.prev_flag = Extension [1];
                    bd->image.prev_frame_duration = *(unsigned short*)&Extension [2];
                    bd->all_frames_duration_hsec += bd->image.prev_frame_duration;
                    bd->image.prev_transp_ind = Extension [4];
                }
                else
                {
                    bd->image.prev_flag = bd->image.prev_transp_ind = 0;
                    bd->image.prev_frame_duration = 0;
                }
            }
		    while (Extension != NULL) 
		        {
			    if (DGifGetExtensionNext(gif, &Extension) == GIF_ERROR) 
			        {
				    DGifCloseFile(gif);
				    return -1;
			        }
		        }
        }
		break;
	    case TERMINATE_RECORD_TYPE:
		break;
	    default:		    /* Should be traps by DGifGetRecordType. */
		break;
	}
    }
    while (RecordType != TERMINATE_RECORD_TYPE && RecordType != IMAGE_DESC_RECORD_TYPE);
    
    if (RecordType == IMAGE_DESC_RECORD_TYPE)
        bd->next_frame = (void*)gif;
    else
        bd->next_frame = NULL;
//end
    if ( NULL == bd->next_frame ) // no frames more, close file
        DGifCloseFile(gif);

    if (image_too_large_for_gsg7)
        return IMAGE_TOO_LARGE_FOR_GSG7; // this is not error, but only precondition for GSG2-4 processing

    return 0;
}
int close_gif (binary_data_t *bd)
{
    if (0 == bd || 0 == bd->next_frame)
        return -1;
    DGifCloseFile((GifFileType*)bd->next_frame);
    return 0;
}
