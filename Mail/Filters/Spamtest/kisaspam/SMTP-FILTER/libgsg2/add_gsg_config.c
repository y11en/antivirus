#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#include "fstorage.h"
#include "fstorage_id.h"
#include <smtp-filter/libgsg2/gsg.h>
#include <smtp-filter/libosr/osr_version.h>
#include <smtp-filter/libosr/osr_c.h>

#define MAXSZ (300*1024)
#define BIGSIZE 10000

void usage()
{
        printf(
               "\n"
                "Usage: add_gsg_config [param1=value [param2=value..]] fstorage_base\n"
               "options:\n"
               "paramN - one of GSG-parameters\n"
               "\n"
               "Usage: -g cfbase-s.gsg fstorage_base\n"
               "options:\n"
               "-g     - append GSG-section of fstorage to the file cfbase-s.gsg\n"
               "\n"
               );
}

int main(int argc, char *argv[]){

    fstorage_section *gsg_config;
    fstorage *fs = fstorage_create();
    char *params;
    char *params1;
    int res_size, ssize, i;
    int res = 0;
    char ch;
    char *str;
    char *storage_name = NULL;
    binary_data_t *bd;
    char only_append_to_gsg = 0;
    int fs_open_mode = FSTORAGE_OPEN_READ_WRITE;
    int file_open_mode = O_RDWR;
    char *gsg_base_name = NULL;
    int gsg_file = 0;

    

    storage_name = strdup(argv[argc-1]);

    bd = binary_init(MAXSZ);


    while((ch = (char)getopt(argc, argv, "hvg")) != -1){
        switch(ch){
            case 'h': usage(); return 0; break;
    //        case 'v': print_version(); break;
            case 'g': only_append_to_gsg = 1; break;
            default:  usage(); return 1; break;
        }
    }

    params = (char*)malloc(BIGSIZE);

    if(only_append_to_gsg){
        file_open_mode = O_RDONLY;
        fs_open_mode = FSTORAGE_OPEN_READ_ONLY;
        gsg_base_name = argv[argc-2];
    }

    if(fstorage_connect(fs, storage_name, file_open_mode, 0644, fs_open_mode, NULL)){
            fprintf(stderr, "Couldn't create storage %s\n", storage_name);
            res = 1;
            goto finish;
    }

    gsg_config = fstorage_find_section(fs, FSTORAGE_SECTION_GSG_CONFIG);

    //loading if it exists. from fstorage to params. then from params to bd
    if(gsg_config){
            params1 = fstorage_section_get_all_data(gsg_config);
            binary_serialize_gsg_params(bd, params1, BIGSIZE, 0);
    }
    else{//create if it's not exists
            gsg_config = fstorage_section_create(fs, FSTORAGE_SECTION_GSG_CONFIG);
            if(!gsg_config){
                    fprintf(stderr, "Couldn't create gsg config section.\n");
                    res = 1;
                    goto end;
            }      
    }
    if (only_append_to_gsg) // append GSG-section of fstorage to the gsg-file
    {
        const unsigned int fs_size = fstorage_section_get_size(gsg_config);
        int flags = O_WRONLY|O_APPEND|O_CREAT;
#ifdef _WIN32
        flags |= O_BINARY;
#endif // _WIN32

        gsg_file = open(gsg_base_name,flags,0644);
        if (gsg_file<0)
        {
            fprintf(stderr, "Couldn't open for writing gsg-file %s\n", gsg_base_name);
            res = 1;
            goto end;
        }

        if ( GSG_PARAMS_MAGIC_SIZE != write(gsg_file, GSG_PARAMS_MAGIC, GSG_PARAMS_MAGIC_SIZE) ||
             sizeof (unsigned int) != write(gsg_file, &fs_size, sizeof (unsigned int)) ||
             fs_size != write(gsg_file, params1, fs_size) )
        {
            fprintf(stderr, "Couldn't write to gsg-file %s\n", gsg_base_name);
            res = 1;
            goto end;
        }
    }
    else
    {
        //--- DATA --------------------------------------------------------------------------
        //###################################################################################
        str = "min_animated_frame_duration_hsec="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->min_animated_frame_duration_hsec = atoi(argv[i]+ssize);

        str = "max_frames_count="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->max_frames_count = atoi(argv[i]+ssize);

        str = "max_duration_hsec="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->max_duration_hsec = atoi(argv[i]+ssize);

        str = "background_detection_mode="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->background_detection_mode = atoi(argv[i]+ssize);

        str = "min_symbol_width="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.min_symbol_width = atoi(argv[i]+ssize);

        str = "max_symbol_width="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.max_symbol_width = atoi(argv[i]+ssize);

        str = "min_symbol_height="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.min_symbol_height = atoi(argv[i]+ssize);

        str = "max_symbol_height="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.max_symbol_height = atoi(argv[i]+ssize);

        str = "max_symbol_fat_percent="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.max_symbol_fat_percent = atoi(argv[i]+ssize);

        str = "min_symbols_in_word="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.min_symbols_in_word = atoi(argv[i]+ssize);

        str = "max_symbols_in_word="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.max_symbols_in_word = atoi(argv[i]+ssize);

        str = "min_words_in_line="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.min_words_in_line = atoi(argv[i]+ssize);

        str = "min_lines_in_text="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.min_lines_in_text = atoi(argv[i]+ssize);

        str = "min_text_percentage="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->osr_params.min_text_percentage = atoi(argv[i]+ssize);

        str = "min_duration_step_hsec="; ssize = strlen(str);
        for(i=0; i<argc; i++)
            if(strncmp(argv[i], str, ssize) == 0)
            bd->min_duration_step_hsec = atoi(argv[i]+ssize);

        //saving from bd to params_array
        res_size = binary_serialize_gsg_params(bd, params, BIGSIZE, 1);

        //saving to fstorage
        if(gsg_config && res_size != -1){
                if(fstorage_section_set_data(gsg_config, params, res_size)){
                        fprintf(stderr, "Couldn't set data for section one.\n");
                        res = 1;
                        goto end;
                }
        }
    }


    end:

    if(fstorage_close(fs))
        fprintf(stderr, "Couldn't close storage %s\n", storage_name);

    if (gsg_file)
        close(gsg_file);

    finish:

    fstorage_destroy(fs);
    binary_free(bd);

    free(storage_name);

    //if(!res) printf(" ok\n");
    return res;

}

