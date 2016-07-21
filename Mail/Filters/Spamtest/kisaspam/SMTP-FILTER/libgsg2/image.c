/* -*- C -*-
 * File: image.c
 *
 * Created: Fri Feb 27 14:05:19 2004
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <smtp-filter/libgsg2/gsg.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif


#ifdef _WIN32
unsigned int pvt_rgb_gray_convert (binary_data_t *dt, register int r, register int g, register int b)
{

  register unsigned long * ctab = dt->rgb_ycc_table;
  return ((ctab[r+R_Y_OFF] + ctab[g+G_Y_OFF] + ctab[b+B_Y_OFF]) >> SCALEBITS);
}

size_t image_pixels(image_data_t *ud)
{
    if(!ud || !ud->size_x || !ud->size_y) return 0;
    return ud->size_x*ud->size_y;
}
#endif //_WIN32


void image_data_free(image_data_t *id)
{
    if(id)
	{
	    if(id->gray) free(id->gray);
	    if(id->pfev_gray) free(id->pfev_gray);
	    if(id->graym) free(id->graym);
	    if(id->bw) free(id->bw);
	    if(id->hbuf) free(id->hbuf);
	    bzero(id,sizeof(*id));
	}
}

void image_data_clear(image_data_t *id)
{
    if(id)
	{
	    id->size_x = id->size_y =0;
            id->prev_flag = 0;
            id->prev_transp_ind = 0;
            id->prev_frame_duration = 0;

            id->prev_l = id->prev_r = id->prev_t = id->prev_b = 0;
	}

}

#define MAX(a,b) ((a)>(b)?(a):(b))

int image_data_init(image_data_t *id)
{
    if(!id) return -1;
    id->pfev_gray = 0;
    image_data_clear(id);
    id->gray = (unsigned char*)malloc(MAX_IMAGE_PIXELS);
    if(!id->gray) // enough space for GSG7
	goto EE;
    id->graym = (unsigned char*)malloc(MAX_IMAGE_PIXELS);
    if(!id->graym) // enough space for GSG2-4
	goto EE;
    id->hbuf = (unsigned int*)malloc(
	     MAX(MAX_IMAGE_WIDTH,MAX_IMAGE_HEIGHT)
	     *MAX(sizeof(id->hbuf[0]),MAX_GIF_RESCALE) );
	   
    if(!id->hbuf)
	goto EE;
    id->bw = (unsigned char*)malloc((MAX_IMAGE_PIXELS+NBBY-1)/NBBY);
    if(!id->bw)
	goto EE;
    return 0;
  EE:
    image_data_free(id);
    return -1;
}


typedef unsigned char gray;


#define SWAP_GRAY(a, b) { gray t; t = b; b = a; a = t; }
#define SWAP_SLICE(a, b) { gray *t; t = b; b = a; a = t; }

#ifndef _WIN32
inline 
#endif //_WIN32
void SORT_SLICE(gray a[]) {
  if (a[0] > a[1]) SWAP_GRAY(a[0], a[1]); 
  if (a[1] > a[2]) SWAP_GRAY(a[1], a[2]); 
  if (a[0] > a[1]) SWAP_GRAY(a[0], a[1]); 
}

#ifndef _WIN32
inline 
#endif //_WIN32
void MERGE_SLICE(gray a[], gray b[], gray bc[]) {
  int i, j, k;

  for (i = 0, j = 0, k = 0; i < 6; i++)
    if (j > 2)
      bc[i] = b[k++];
    else if (k > 2)
      bc[i] = a[j++];
    else 
      bc[i] = (gray) ((a[j] < b[k]) ? a[j++] : b[k++]);
}

#ifndef _WIN32
inline 
#endif /*_WIN32*/
gray CALC_MEDIAN(gray x[], gray bc[]) {
  int i, j, k, m=0;

  for (i = 0, j = 0, k = 1; i < 4; i++)
    if (j > 2)
      m = bc[k++];
    else
      m = (x[j] < bc[k]) ? x[j++] : bc[k++];

  return (gray)m;
}

static unsigned char bitmaps[] = {128,64,32,16,8,4,2,1};

#define IDX(row,col) (id->size_x*row+col)
#define PIXEL(id,row,col) (id->bw[(id->size_x*row+col)/8]& bitmaps[(id->size_x*row+col)%8]?255:0)

void bw2gray(image_data_t *id)
{
    int row,col;
    if(!id || !id->size_x || !id->size_y || !id->graym ||!id->bw)
	return;
    for(row=0;row<id->size_y;row++)
	for(col=0;col<id->size_x;col++)
	    id->graym[id->size_x*row+col] = (unsigned char)PIXEL(id,row,col);
}

int image_median_filter(image_data_t *id,int bw)
{
    size_t sz;
    size_t row,col,rows,cols;
    if(!id || !id->gray || !id->graym || (sz=image_pixels(id))<=0 || sz > MAX_IMAGE_PIXELS) 
	return -1;
    bzero(id->graym,sz);
    rows=id->size_y;
    cols=id->size_x;
    // border already set to zero by bzero
      for (row = 1; row < rows - 1; row++) 
	  {
	      gray _a[3], _b[3], _c[3], _d[3], bc[6];
	      gray *a = _a, *b = _b, *c = _c, *d = _d;
	      if(!bw)
		  {
		      a[0] = id->gray[IDX(row-1,0)];
		      a[1] = id->gray[IDX(row,0)];
		      a[2] = id->gray[IDX(row+1,0)];
		      SORT_SLICE(a);
		      
		      b[0] = id->gray[IDX(row-1,1)];
		      b[1] = id->gray[IDX(row,1)];
		      b[2] = id->gray[IDX(row+1,1)];
		      SORT_SLICE(b);
		  }
	      else
		  	      if(!bw)
		  {
		      a[0] = (unsigned char)PIXEL(id,row-1,0);
		      a[1] = (unsigned char)PIXEL(id,row,0);
		      a[2] = (unsigned char)PIXEL(id,row+1,0);
		      SORT_SLICE(a);
		      
		      b[0] = (unsigned char)PIXEL(id,row-1,1);
		      b[1] = (unsigned char)PIXEL(id,row,1);
		      b[2] = (unsigned char)PIXEL(id,row+1,1);
		      SORT_SLICE(b);
		  }

	      
	      for (col = 1; col < cols - 1; col += 2) 
		  {
		      
		      if(!bw)
			  {
			      c[0] = id->gray[IDX(row-1,col+1)];
			      c[1] = id->gray[IDX(row,col+1)];
			      c[2] = id->gray[IDX(row+1,col+1)];
			      
			      d[0] = id->gray[IDX(row-1,col+2)];
			      d[1] = id->gray[IDX(row,col+2)];
			      d[2] = id->gray[IDX(row+1,col+2)];
			  }
		      else
			  {
			      c[0] = (unsigned char)PIXEL(id,row-1,col+1);
			      c[1] = (unsigned char)PIXEL(id,row,col+1);
			      c[2] = (unsigned char)PIXEL(id,row+1,col+1);

			      d[0] = (unsigned char)PIXEL(id,row-1,col+2);
			      d[1] = (unsigned char)PIXEL(id,row,col+2);
			      d[2] = (unsigned char)PIXEL(id,row+1,col+2);
			  }
			  
		      
		      // sort slice c and d 
		      // slice a and b are already sorted
		      SORT_SLICE(c);
		      SORT_SLICE(d);
		      
		      // merge slice b and c producing slice bc
		      MERGE_SLICE(b, c, bc);
		      
		      // merge slice bc and a to calculate median 1
		      id->graym[IDX(row,col)] = CALC_MEDIAN(a, bc);
		      
		      // merge slice bc and d to calculate median 2
		      id->graym[IDX(row,col+1)] = CALC_MEDIAN(d, bc);
		      
		      SWAP_SLICE(a, c);
		      SWAP_SLICE(b, d);
	      }
	  }
      return 0;
}

int image_hist(image_data_t *id, int vertical, int filtered)
{
    float hsum[HSIZE],max;
    unsigned char *data;
    int i,j;
    int x1,x2;
    unsigned int size;
    float multip;
    if(!id || !id->gray || !id->graym || !id->hbuf || id->size_x < 8 || id->size_y < 8) return -1;
    if(id->size_x > MAX_IMAGE_WIDTH || id->size_y > MAX_IMAGE_HEIGHT)
	return -1;
    
    bzero(id->hbuf,sizeof(unsigned int)*MAX(id->size_x,id->size_y));

    data = filtered ? id->graym : id->gray;
    size = vertical ? id->size_y: id->size_x;
    if(vertical)
	{
	    for(i=0;(int)i<id->size_y;i++) // each row
		for(j=0;(int)j<=id->size_x;j++) // within row
		    id->hbuf[i]+=data[i*id->size_x+j];
	}
    else
	{
	    for(i=0;(int)i<id->size_y;i++) // each row
		for(j=0;(int)j<id->size_x;j++)
		    id->hbuf[j]+=data[i*id->size_x+j];
	}

    multip=(float)(size-1);
    multip/=HSIZE;
    max=0;
    for(i=0;i<HSIZE;i++)
	{
	    hsum[i]=0;
	    x1 = (unsigned int)(i*multip);
	    x2 = (unsigned int)((i+1)*multip);
	    for(j=x1;j<=x2;j++)
		hsum[i]+=id->hbuf[j];
	    hsum[i]/=(x2-x1+1);
	    hsum[i]/=size;
	    if(hsum[i]>max) max = hsum[i];
	}
    for(i=0;i<HSIZE;i++)
	{
	    hsum[i]*=7/max;
	    id->hg[i] = (unsigned char)hsum[i];
	}
    return 0;
}

int image_to_bw(image_data_t *id, int level, int filtered)
{
    unsigned char *data;
    size_t pixels,i;
    size_t count=0;
    if(!id || !id->gray || !id->graym || !id->bw 
       || (pixels = image_pixels(id))<1|| pixels>MAX_IMAGE_PIXELS) 
	return -1;

    bzero(id->bw,(pixels+7)/8);
    data = filtered ? id->graym : id->gray;
    for(i=0;i<pixels;i++)
	if(data[i]>level)
	    {
		id->bw[i/8] |= bitmaps[i%8];
		count++;
	    }
    return count;
}

static int image_make_gc(image_data_t *id,size_t start, size_t end)
{
    unsigned int i,sum=0,mult=0;
    if(!id) return -1;
    
    for (i=start;i<end;i++)
	{
	    sum+=id->hist[i];
	    mult+=id->hist[i]*i;
	}
    if(sum<=0) return -1;
    return mult/sum;
}

float lv[NLEVELS-1]={0.02F,0.05F,0.1F,0.9F};

int image_make_histogram(image_data_t *id, int filtered)
{
    volatile size_t i,sz;
    volatile int j;
    volatile double sum=0.0;
    unsigned char *data;
    if(!id || !id->gray || !id->graym || (sz=image_pixels(id))<=0 || sz > MAX_IMAGE_PIXELS) 
	return -1;

    bzero(id->hist,HISTOGRAM_SIZE*sizeof(unsigned int));
    data = filtered ? id->graym: id->gray;

    for(i=0;i<sz;i++)
	id->hist[data[i]%(HISTOGRAM_SIZE)]++;

    for(j=0;j<NLEVELS;j++)
	    id->levels[j]=-1;

    id->levels[1]=image_make_gc(id,0,HISTOGRAM_SIZE);

    for(i=0,sum=0.0;i<HISTOGRAM_SIZE;i++)
	{
	    sum+=id->hist[i];
	    if(sum > sz*(filtered?0.1:0.05))
		{
		    id->levels[0]=i;
		    break;
		}
	}
    for(i=0,sum=0.0;i<HISTOGRAM_SIZE;i++)
       {
           sum+=id->hist[i];
           if(sum > sz*0.9)
           {
               id->levels[2]= sum>sz*0.99?i-1:i;
               break;
           }
        }
    // check levels
    for(i=0;i<NLEVELS;i++)
	{
	    sum = 0;
	    if(id->levels[i]<0)
		continue;
	    for(j=0;(int)j<=id->levels[i];j++)
		sum+=id->hist[j];
	    if(sum < sz*0.01 || sum > sz *0.99)
		id->levels[i]=-1;
	}
    return 0;
}
