/* -*- C -*-
 * File: file2gsg.c
 *
 * Created: Fri Feb 27 18:22:41 2004
 */
#ifdef _WIN32
#include <_unistd.h>
#include <_io.h>
#else
#include <unistd.h>
#endif /*_WIN32*/

#include <fcntl.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif /*_WIN32*/
#include <smtp-filter/libgsg2/gsg.h>

#ifdef WITH_DCC
#include <smtp-filter/libdccwrapper/dcc_wrapper.h>
#endif /*WITH_DCC*/
#ifdef _WIN32
#include <windows.h>
#include <smtp-filter/libsigs/libsigs.link.h>
#include <smtp-filter/libgsg2/libgsg2.link.h>
#include <smtp-filter/ungif/ungif.link.h>
#include <gnu/libjpeg.link.h>
#include <gnu/libpng.link.h>
#include <gnu/libmd5a.link.h>
#include <lib/pcommon.link.h>
#endif /*_WIN32*/
#include <smtp-filter/libosr/osr_version.h>
#include "fstorage.h"
#include "fstorage_id.h"

#ifdef _WIN32
  #define snprintf    _snprintf
  #define open       _open
  #define close      _close
  #define strcasecmp  stricmp
#endif /*_WIN32*/

#define MAXSZ (300*1024)
#define BUFSZ 64*1024
#define BIGSIZE 10000
static unsigned int RPT=100;
static unsigned int benchmark=0;
static unsigned int check=0;
static unsigned int printfile=0;
static unsigned int dccreport = 0;
static unsigned int targets=0;
static unsigned int frame_info=0;

void usage(void)
{
    fprintf(stderr,
	    "\n"
	    "Usage: file2gsg [options] file1 file2....\n"
	    "Options are:\n"
	    " -b      - benchmark run\n"
	    " -r N    - number of repetitions for benchmark (default %d)\n"
	    " -g file - filename of GSG-checksum file\n"
	    " -c      - check against GSG checksums in file\n"
	    " -p      - print filenames\n"
#ifdef WITH_DCC
	    " -d      - do DCC checks\n"
#endif /*WITH_DCC*/
	    " -D      - dump intenal bitmaps to RAW files\n"
#ifdef WITH_DCC
	    " -t cnt  - do dcc report with count cnt (number of many/ok/ok2/del)\n"
#endif /*WITH_DCC*/
	    " -h      - this help\n"
	    " -G      - dump \"good\" gif-file frames\n"
        " -f      - frame info\n"
	    " -v      - print version\n"
        
        , RPT);
    exit(0);

}

void print_version()
{
    printf("version: %s.%s\n", LIBGSG_VER_TXT, OSR_VER_TXT );
}

int main(int ac, char *av[]) 
{
    binary_data_t *bdata = binary_init(MAXSZ);
    unsigned int i;
    int fd=0,len;
    char *buf = malloc(BUFSZ),ch;
    char str[33];
    char *gsg=NULL;
    fstorage_section *gsg_config;
    fstorage *fs = fstorage_create();
    char *storage_name = NULL;
    char *params;


    if(!bdata||!buf) exit(1);

    while((ch = (char)getopt(ac, av, "br:g:cpdt:hDGvfF:")) != -1)
	{
	    switch (ch)
		{
		case 'b': benchmark++; break;
		case 'r': RPT=atoi(optarg); break;
		case 'g': gsg = strdup(optarg); break;
		case 'c': check++; break;
		case 'p': printfile++; break;
#ifdef WITH_DCC
		case 'd': dccreport++; break;
#endif /*WITH_DCC*/
		case 'D': bdata->generate_old_dumps=1; break;
#ifdef WITH_DCC	
		case 't':
			if(!strcasecmp(optarg,"many"))
			    targets=DCC_TGTS_TOO_MANY;
			else if(!strcasecmp(optarg,"OK"))
			    targets=DCC_TGTS_OK;
			else if(!strcasecmp(optarg,"OK2"))
			    targets=DCC_TGTS_OK2;
			else if(!strcasecmp(optarg,"DEL"))
			    targets=DCC_TGTS_DEL;
			else 
			    targets = atoi(optarg); 
			break;
#endif /*WITH_DCC*/
		case 'G': bdata->generate_gif_frames=1; break;
		case 'v': print_version(); break;
		case '?':
		case 'h': usage(); break;
		case 'f': frame_info=1; break;
                case 'F': storage_name = strdup(optarg); break;
		}
	}
    ac-=optind;
    av+=optind;
    
    if(benchmark)
	if(ac>1) ac=1;

    if(storage_name){
        if(fstorage_connect_shared(fs, storage_name, NULL)) exit(1);
        gsg_config = fstorage_find_section(fs, FSTORAGE_SECTION_GSG_CONFIG);

        if(gsg_config){
            params = fstorage_section_get_all_data(gsg_config);
            binary_serialize_gsg_params(bdata, params, BIGSIZE, 0);
        }

        fstorage_close(fs);
        fstorage_destroy(fs);
    }

    if(gsg)
	binary_opensigdb(bdata,gsg);
    for(i=0;i<(unsigned int)ac;i++)
	{
	    int open_flags = O_RDONLY;
#ifdef _WIN32
	    open_flags |= O_BINARY;
#endif /*_WIN32*/
	    if(!strcmp(av[i],"-")||(fd = open(av[i], open_flags))>=0)
		{

		    if(benchmark){
			unsigned int j;
			double usec;
			double each;
			double bytes,tput;
#ifdef _WIN32
			long tv1,tv2;
#else /*_WIN32*/
			struct timeval tv1,tv2;
#endif /*_WIN32*/

			if(!strcmp(av[i],"-"))
			    fd = 0;
			len=read(fd,buf,BUFSZ);
			if(fd>0)
			    close(fd);
#ifdef _WIN32
			tv1 = GetTickCount();
#else /*_WIN32*/
			gettimeofday(&tv1,NULL);
#endif /*_WIN32*/
			for(j=0;j<RPT;j++)
			    {
				binary_put(bdata,buf,len);
				binary_endpart(bdata);
				
			    }
#ifdef _WIN32
			tv2 = GetTickCount();
			usec = tv2-tv1;
#else	/*_WIN32*/
			gettimeofday(&tv2,NULL);
			usec = 1000000L*(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);
#endif /*_WIN32*/
			each = 1000000.0/(usec/RPT);
			bytes = len*1.0*RPT;
			tput = bytes*1000000.0/usec;
			printf ("%.0f usec for %d repeats (%d bytes)/%.0f bytes total, %6.7g/sec, %6.7g bytes/sec\n",
				usec,RPT,len,bytes,each,tput);
		    }else{
                struct stat st = {0};
			if(!strcmp(av[i],"-"))
			    fd = 0;
			if(fd>0)
			    fstat(fd,&st);
			if(fd==0 || st.st_size < MAXSZ)
			    while((len = read(fd,buf,BUFSZ))>0)
				binary_put(bdata,buf,len);
			if(fd)
			    close(fd);
            bdata->current_file_name = av[i];
			binary_endpart(bdata);
		    }
		}
	}
#ifdef WITH_DCC
    if(!benchmark && dccreport && bdata->md5list && bdata->md5list->used)
	{
	    DCC_WRAPPER_DATA dwd;
	    int dccs,i;
	    if(init_dcc_wrapper(&dwd,NULL)>=0)
		{
		    int cnt[3];
		    for(i=0;i<bdata->part_count;i++)
			{
			    if(printfile)
				printf("FILE: %s\n",strcmp(av[i],"-")?av[i]:"stdin");
			    dccs =  process_gsg_message(&dwd, bdata->md5list,NULL,i,targets,cnt);
			    if(dccs>=0)
				dump_gsg_cksums(&dwd);
			}
		    close_dcc_wrapper(&dwd);
		}
	}
#endif /*WITH_DCC*/
    if(!benchmark && !dccreport)
	{
	    int file_id = -1;
	    if(check && bdata->sigarray)
		{
		    for(i=0;i<bdata->md5list->used;i++)
			{
			    if(file_id != bdata->md5list->array[i].imageno && printfile)
				{
				    file_id = bdata->md5list->array[i].imageno;
				    printf("FILE: %s\n",strcmp(av[file_id],"-")?av[file_id]:"stdin");
				}
			    md52str(str,bdata->md5list->array[i].md5);
			    printf("GSG%d %s %c\n",bdata->md5list->array[i].type,
				   str,
				   md5sigfind(bdata->sigarray,bdata->md5list->array[i].md5)==-1?' ':'+');
			}
		    
		}
	    else
		for(i=0;i<bdata->md5list->used;i++)
		    {
			if(file_id != bdata->md5list->array[i].imageno)
			{
				file_id = bdata->md5list->array[i].imageno;
                if ( printfile)
				    printf("FILE: %s\n",strcmp(av[file_id],"-")?av[file_id]:"stdin");
                if (frame_info)
                {
                    int j = 0;
                    for (; j < bdata->images_info_count; j ++ )
                        if (bdata->images_info[j].part_no == file_id)
                        {
                            printf("frames: %d, w: %d, h: %d\n", 
                                bdata->images_info[j].frames_count,
                                bdata->images_info[j].width,
                                bdata->images_info[j].height);
                            break;
                        }

                }
			}
			md52str(str,bdata->md5list->array[i].md5);
			printf("%d GSG%d %s\n",bdata->md5list->array[i].frameno, bdata->md5list->array[i].type,str);
		    }
	    
	}
    if (bdata)
        binary_free(bdata);
    if (buf)
        free (buf);
    return 0;
}
