/* -*- C -*-
 * File: binary_handler.h
 *
 * Created: Wed Feb 25 19:28:35 2004
 */

#ifndef __libgsg_h
#define __libgsg_h

struct in_addr;

#include <sys/types.h>
#if defined (OS_LINUX) || defined (_WIN32)
#include <gnu/md5a/md5a.h>
#else
#include <md5.h>
#endif
#include <math.h>
#include <smtp-filter/libsigs/libsigs.h>
#include <smtp-filter/libosr/osr_c.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define LIBGSG_VER 3.4

#define LIBGSG_VAL_TO_TEXT_0(s) #s
#define LIBGSG_VAL_TO_TEXT(s) OSR_VAL_TO_TEXT_0(s)

#define LIBGSG_VER_TXT OSR_VAL_TO_TEXT(LIBGSG_VER)


#define ERR_OK		0
#define ERR_UNKNOWN	-1
#define ERR_OVERFLOW	-2

#define MAX_IMAGE_WIDTH 1500
#define MAX_IMAGE_HEIGHT 1500
#define MAX_IMAGE_PIXELS (MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT)
#define MAX_GIF_RESCALE 8
#define HISTOGRAM_SIZE 256

#define IMAGE_TOO_LARGE_FOR_GSG7 111
#define IMAGE_TOO_LARGE          112
#define IMAGE_TOO_SMALL          113

#define HSIZE 16
#ifndef NBBY
#define NBBY 8
#endif

#define GSG_PARAMS_MAGIC        "GSGP"
#define GSG_PARAMS_MAGIC_SIZE   4
#define GSG_PARAMS_VERSION      1
#define GSG_PARAMS_APPROX_SIZE  200


#define NGSG 8
#define HAVE_ANIMATEGIF

typedef enum __sig_type
{
    NONE	=0, // not used
    SIG_GSG1	=1,
    SIG_GSG2	=2,
    SIG_GSG3	=3,
    SIG_GSG4	=4,
#if 0
    SIG_GSG5	=5,
    SIG_GSG6	=6
#endif
    SIG_GSG7	=7
} sig_type;


typedef struct 
{
    md5sum		md5;
    unsigned int	type;
    unsigned int	imageno;
    unsigned int	frameno;
    size_t              size;
    int			dccmatch;
}md5_element_t;

struct __md5_list_t
{
    md5_element_t *array;
    size_t	alloc;
    size_t	used;
};
typedef struct __md5_list_t md5_list_t;

#define MD5_ALLOC 32




typedef enum __img_type
{
    BW=1,
    GRAY8=2,
    GRAY16=3,
    RGBINDEXED=4,
    RGB8=5,
    RGB16=6,
}img_type;

typedef struct {unsigned char r; unsigned char g; unsigned char b;} rgb8_t;

#define NLEVELS 5
typedef struct
{
    int size_x;
    int size_y;
//    unsigned char *bitmap;
//    int palette_size;
//    rgb8_t *palette;
    int levels[NLEVELS];
    unsigned char *gray;
    unsigned char *graym;
    unsigned char *bw;
    unsigned int *hbuf;
    unsigned char hg[HSIZE];
    unsigned int  hist[HISTOGRAM_SIZE];

    // previous frame of animated gif info (memory for pfev_frame_gray allocated by user):
    unsigned char *pfev_gray;
    unsigned char prev_flag, prev_transp_ind;
    unsigned int  prev_frame_duration;

    unsigned int prev_l, prev_r, prev_t, prev_b;

}image_data_t;

typedef struct __images_info
{
    unsigned int frames_count, width, height;
    unsigned int processed_frames_count;
    unsigned int part_no;
} images_info_t;

typedef enum 
{
    BACKGROUND_DETECTION_AUTO = 0,
    BACKGROUND_DETECTION_ALTERNATIVE
} background_detection_mode_t;

typedef struct __binary_data
{
    unsigned char	*data;
    size_t		alloc;
    size_t		used;
    size_t		readp;
    size_t		processed;
    md5_list_t		*md5list;
    md5sigarray		*sigarray;
    unsigned long *rgb_ycc_table;
    unsigned int	part_count;
    unsigned int	image_count;
    image_data_t	image;
    MD5_CTX		ctx;
    unsigned char	partial;
    images_info_t  *images_info;
    unsigned int    images_info_count;
    const char     *current_file_name;

    char generate_old_dumps;
    char generate_gif_frames;

    unsigned int corrupted_image_count;
    unsigned int frame_count;
    unsigned int text_frame_count;
    // all_frames - does mean "all processed frames"
    unsigned int all_frames_symbols_count;
    unsigned int all_frames_words_count;
    unsigned int all_frames_lines_count;

    unsigned int best_frame_symbols_count;
    unsigned int best_frame_words_count;
    unsigned int best_frame_lines_count;
    unsigned int best_frame_text_percentage;
    unsigned int best_frame_width;
    unsigned int best_frame_height;

    unsigned int jpg_count, png_count, gif_count;

    //// gsg input parameters (serialized with fstorage):
    //   - libosr parameters:
    osr_filter   osr_params;
    //   - frame processing filter:
    unsigned int background_detection_mode;
    unsigned int max_frames_count;
    unsigned int min_animated_frame_duration_hsec;
    unsigned int max_duration_hsec;
    unsigned int min_duration_step_hsec;

    //// temporary data
    void* next_frame; /* Animated image reading status. Used only for image loading. */
    unsigned int prev_proc_hsec; // signed int
    unsigned int all_frames_duration_hsec;

}binary_data_t;

#ifndef MAXJSAMPLE
#define MAXJSAMPLE 255
#endif
#define SCALEBITS	16	/* speediest right-shift on some machines */
#define R_Y_OFF		0			/* offset to R => Y section */
#define G_Y_OFF		(1*(MAXJSAMPLE+1))	/* offset to G => Y section */
#define B_Y_OFF		(2*(MAXJSAMPLE+1))	/* etc. */
#define R_CB_OFF	(3*(MAXJSAMPLE+1))
#define G_CB_OFF	(4*(MAXJSAMPLE+1))
#define B_CB_OFF	(5*(MAXJSAMPLE+1))
#define R_CR_OFF	B_CB_OFF		/* B=>Cb, R=>Cr are the same */
#define G_CR_OFF	(6*(MAXJSAMPLE+1))
#define B_CR_OFF	(7*(MAXJSAMPLE+1))

#ifndef _WIN32
static inline unsigned int pvt_rgb_gray_convert (binary_data_t *dt, register int r, register int g, register int b)
{

  register unsigned long * ctab = dt->rgb_ycc_table;
  return ((ctab[r+R_Y_OFF] + ctab[g+G_Y_OFF] + ctab[b+B_Y_OFF]) >> SCALEBITS);
}

static inline size_t image_pixels(image_data_t *ud)
{
    if(!ud || !ud->size_x || !ud->size_y) return 0;
    return ud->size_x*ud->size_y;
}
#else
unsigned int pvt_rgb_gray_convert (binary_data_t *dt, register int r, register int g, register int b);
size_t image_pixels(image_data_t *ud);
#endif //_WIN32

int image_data_init(image_data_t *id);
int image_to_bw(image_data_t *id, int level, int filtered);
void bw2gray(image_data_t *id);
int image_make_histogram(image_data_t *id, int filtered);
int image_median_filter(image_data_t *id,int bw);
int image_hist(image_data_t *id, int vertical, int filtered);
void image_data_free(image_data_t *id);
void image_data_clear(image_data_t *id);

#define UNPACKED_GIF_CAN_NOT_BE_SCALED  1
#define UNPACKED_GIF_CAN_BE_SCALED      0

int unpack_gif(binary_data_t *bd, char need_only_not_scaled);
int unpack_jpeg(binary_data_t *bd);
int unpack_png(binary_data_t *bd);
int close_gif (binary_data_t *bd);



void md5list_free(md5_list_t *m);
md5_list_t *md5list_create(void);
int md5list_put(md5_list_t *m, md5sum *sum, unsigned int type,unsigned int no, unsigned int frameno, size_t size);
void binary_free(binary_data_t **pbt);
int binary_clear(binary_data_t *bt);
int binary_clearall(binary_data_t *bt);
binary_data_t* binary_init(size_t sz);

#define BINARY_SEIALIZE_GSG_PARAMS_SAVE 1
#define BINARY_SEIALIZE_GSG_PARAMS_LOAD 0
// return size of could loaded/saved bytes, or -1 if critical errors
int binary_serialize_gsg_params(binary_data_t *bd, unsigned char *params,
                          unsigned int size, char save /*save (1) or load (0)*/);
void binary_opensigdb(binary_data_t *bd,char *filename);
void binary_setsigdb(binary_data_t *bd,md5sigarray *m);
int binary_put(binary_data_t *bd, void *data, size_t sz);
int binary_endpart(binary_data_t *bd);


#ifdef __cplusplus
}
#endif

#endif
