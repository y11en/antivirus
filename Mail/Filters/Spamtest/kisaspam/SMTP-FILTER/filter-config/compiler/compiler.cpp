/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: compiler.cpp                                                          *
 * Created: Thu Apr 11 20:32:23 2002                                           *
 * Desc: Configuration compiler (entry)                                        *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  compiler.cpp
 * \brief Configuration compiler (entry)
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "compiler.h"

#include "parse.h"
#include "samples.h"

#include "smtp-filter/common/profile_db.h"
#include "smtp-filter/common/list_db.h"
#include "smtp-filter/common/profile_db.h"
#include "smtp-filter/common/logger.h"
#include "smtp-filter/common/config.h"
#include "smtp-filter/paths/paths.h"

#include "smtp-filter/filter-config/CFBuilder/cf-messages.h"

#include "filter.h"
#include "cfdata.h"

int treat_warnings_as_errors = 0;

int compiler_warning(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("Warning: ");
    vprintf(format, args);

    va_end(args);

    return treat_warnings_as_errors;
}

int compiler_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("Error: ");
    vprintf(format, args);

    va_end(args);

    return 0;
}


static int print_samples(FILE *f, samples_node *node, size_t h)
{
    if(!node)
	return 0;

#define PRINT_SPACES 				\
    do { 					\
	for(size_t i = 0; i < h; i++)		\
	    fprintf(f, "  ");			\
    } while(0)

    PRINT_SPACES;
    fprintf(f, "<Category ID=\"%s\">\n", node->id);
    h++;
    PRINT_SPACES;
    fprintf(f, "<Terms />\n");

    if(node->files.alloc)
	{
	    PRINT_SPACES;
	    fprintf(f, "<Samples>\n");
	    
	    h++;
	    
	    for(size_t j = 0; j < node->files.alloc; j++)
		{
		    PRINT_SPACES;
		    fprintf(f, "<SampleFile fname=\"%s\" />\n", full_path_sample(node->files.names[j]));
		}
	    
	    h--;
	    
	    PRINT_SPACES;
	    fprintf(f, "</Samples>\n");
	}

    print_samples(f, node->son, h);

    h--;
    PRINT_SPACES;
    fprintf(f, "</Category>\n");

    print_samples(f, node->brother, h);

    return 0;
}

int copy_file(const char *in, const char *out)
{
    char buf[40960];

    int fd_in = open(in, O_RDONLY);
    int fd_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int res = 0, readed;

#define RETURN(x) do { res = (x); goto finish; } while(0)

    if(fd_in < 0 || fd_out < 0)
	RETURN(-1);

    for( ; (readed = read(fd_in, buf, sizeof(buf))) > 0 ; )
	if(write(fd_out, buf, readed) < 0)
	    RETURN(-1);

    if(readed < 0)
	RETURN(-1);

  finish:
    if(fd_in >= 0)
	close(fd_in);

    if(fd_out >= 0)
	close(fd_out);

    return res;
}

int create_filter_db()
{
#define COPY_EXT(ext)						\
    do {							\
	snprintf(name_buf_1, sizeof(name_buf_1), "%s.%s", 	\
		 full_name_mainset(), (ext));			\
	snprintf(name_buf_2, sizeof(name_buf_2), "%s.%s", 	\
		 full_name_cfilter(), (ext));			\
								\
	if(copy_file(name_buf_1, name_buf_2))			\
	    return 1;						\
	    							\
    } while(0)

    printf("Copying %s...\n", full_name_mainset());
  
    char name_buf_1[4096];
    char name_buf_2[4096];

    COPY_EXT("arc");
    COPY_EXT("dcs");
    COPY_EXT("idx");
    COPY_EXT("key");
    COPY_EXT("dat");
    COPY_EXT("cat");
    COPY_EXT("mph");
    COPY_EXT("men");
    COPY_EXT("mru");
    COPY_EXT("mde");
    COPY_EXT("mfr");
    COPY_EXT("mes");
    
    samples_node *sn = parse_samples(full_path_samples());

    if(!sn)
	return treat_warnings_as_errors;

    char buf[2048] = "";
    snprintf(buf, sizeof(buf)-1, "%s/tmp-samples.XXXXXXXXXX", full_path_config_bin_root());
    buf[sizeof(buf)-1] = '\0';

    int fd = mkstemp(buf);

    if(fd < 0)
	{
	    samples_free(sn);
	    return 1;
	}

    FILE* f = fdopen(fd, "w");

    fprintf(f, "<?xml version=\"1.0\" encoding=\"koi8-r\"?>\n\n"
            "<CFDoc xmlns=\"http://www.ashmanov.com/cf/1.0/\">\n\n");

    print_samples(f, sn, 1);

    fprintf(f, "</CFDoc>\n");

    cfd_file_info fi[1];
    memset(fi, 0, sizeof(fi));

    fi[0].name = buf;
    fi[0].type = CFD_FI_XML;

    fclose(f);

    printf("Compiling %s...\n", full_name_cfilter());
    
    cfd_state_config sc;
    memset(&sc, 0, sizeof(sc));
    int cf_res = cfdata_compile_append(full_name_cfilter(), &sc, fi, 1);
    if(cf_res)
      printf("Compile error: %s\n", cf_err_message(sc.error));

    samples_free(sn);
    unlink(buf);

    if(cf_res)
	return 1;
    else
	return 0;
}

struct compiler_data 
{
    list_db        *ldb;
    profile_db     *pdb;

    content_filter *cf;
};

/*******************************************************************************
 * m a i n
 */

static void print_usage(int argc, char **argv)
{
  printf("\n\nUsage:\n\n"
		 "%s -c -w -s conf_src_path -d cfdata_mainset_path -b out_bin_data_path"
         "\n\n",
		 argv[0]);
}

int main(int argc, char **argv)
{
    printf("\nMail Filter Configuration Compiler.\n\n"
     "(C) 2001-2004, Ashmanov & Partners company, Moscow, Russia.\n"
     "Contact: info@ashmanov.com, http://www.ashmanov.com\n\n");

    int ch;
    struct compiler_data cd;

    for(int i = 1; i < argc; i++)
	{
	    if(strcmp(argv[i], "--print-version") == 0)
		{
		    print_version();

		    return 0;
		}
	}

#if 0

    logger_open("compiler", LOGGER_OPEN_NORMAL);
    logger_add_syslog(LOGGER_ALL);

#endif

    memset(&cd, 0, sizeof(cd));

    char *src_path = NULL;
    char *mainset_path = NULL;
    char *bin_path = NULL;

    bool compile = false;

    if(argc == 1)
    {
      print_usage(argc, argv);
      return 1;
    }

    while((ch = getopt(argc, argv, "ws:d:b:c")) != -1)
	switch(ch)
	    {
	    case 's':
		src_path = optarg;
		path_set_conf_src_root(src_path);
		break;

	    case 'd':
		mainset_path = optarg;
		path_set_cfdata_mainset_root(mainset_path);
		break;

	    case 'b':
		bin_path = optarg;
		path_set_conf_bin_root(bin_path);
		break;

	    case 'c':
		compile = true;
		break;

	    case 'w':
		treat_warnings_as_errors = 1;
		break;

	    case '?':
	    default:

		print_usage(argc, argv);
		return 1;
	    }
    
    if(!compile && (!src_path || !mainset_path || !bin_path))
	{
	    printf("Invalid arguments\n");
	    return 1;
	}
    
    path_validate(full_path_config_bin_root(), 0744, PATH_VALIDATE_DIR);

    if(create_filter_db())
	{
	    printf("Can\'t create content filtration database: %s\n", full_name_cfilter());
	    return 1;
	}

    printf("Compiling configuration...\n");

    parse_main(full_path_main());

    cd.cf  = filter_open     (full_name_cfilter());
    cd.pdb = profile_db_open (full_path_profile_db(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    cd.ldb = list_db_open    (full_path_list_db(),    O_RDWR | O_CREAT | O_TRUNC, 0644);

    int res;
    if(cd.pdb && cd.ldb && cd.cf)
	{
	    if(parse_profile_db(cd.ldb, cd.pdb, cd.cf))
		{
		    printf("Can't compile configuration.\n");
		    res = 1;
		}
        else
            res = 0;
	}
    else
      res = 1;

    config_save(full_path_config());

    

    if(cd.ldb)
	  list_db_close(cd.ldb);
    else
      printf("Can't create %s\n", full_path_list_db());

    if(cd.pdb)
      profile_db_close(cd.pdb);
    else
      printf("Can't create %s\n", full_path_profile_db());

    if(cd.cf)
	  filter_close(cd.cf, FILTER_DEFAULTS);
    else
      printf("Can't open content filtration database %s\n", full_name_cfilter());

#if 0

    logger_close();

#endif

    if(res == 0)
      printf("OK\n");

    return res;
}

/*
 * <eof compiler.cpp>
 */
