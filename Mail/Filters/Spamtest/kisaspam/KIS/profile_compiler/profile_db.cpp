/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: profile_db.cpp                                                        *
 * Created: Mon Apr 29 21:14:25 2002                                           *
 * Desc: Profile database (implementation).                                    *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  profile_db.cpp
 * \brief Profile database (implementation).
 */
#include "StdAfx.h"
//#include "commhdr.h"
#pragma hdrstop

#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/param.h>
#include <netinet/in.h>
#include <unistd.h>
#endif // _WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <_include/nix_types.h>
#include <io.h>
#include <winsock.h>
#endif // WIN32

#include <stdio.h>
/* #include "gnu/bdb1.h" */

#include "profile_db.h"

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif

///#include "ProfileCompile.h"


#define PDB_CHANGED 0x00000001

struct __profile_db
{
    struct {
	profile **array;
	size_t    used;
	size_t    alloc;
    } profiles;

    int          fd;
    unsigned int flags;
};

profile_db *profile_db_open(const char *name, int flags, int mode)
{
    profile_db *pdb = (profile_db *)malloc(sizeof(profile_db));
    memset(pdb, 0, sizeof(profile_db));

    pdb->fd = open(name, flags, mode);

    off_t size = 0;
    struct stat fd_stat;
    
    memset(&fd_stat, 0, sizeof(fd_stat));

    if(pdb->fd < 0)
	goto error;

    //stat(name, &fd_stat);

    size = lseek(pdb->fd, 0, SEEK_END);
    //size = fd_stat.st_size;

    if(size > 0)
	{
	    /*
	     * Read profile data from database.
	     */

	    u_long profiles_count, profile_size;

	    char   *buf       = NULL;
	    size_t  buf_alloc = 0;

	    lseek(pdb->fd, 0, SEEK_SET);

	    read(pdb->fd, &profiles_count, sizeof(u_long));
	    pdb->profiles.alloc = ntohl(profiles_count);
	    pdb->profiles.array = (profile **)malloc(sizeof(profile *)*pdb->profiles.alloc);
	    memset(pdb->profiles.array, 0, sizeof(profile *)*pdb->profiles.alloc);

	    for(pdb->profiles.used = 0; pdb->profiles.used < pdb->profiles.alloc; pdb->profiles.used++)
		{
		    read(pdb->fd, &profile_size, sizeof(u_long));
		    profile_size = ntohl(profile_size);

		    if(buf_alloc < profile_size)
			{
			    if(buf) free(buf);
			    buf = (char *)malloc(buf_alloc = profile_size);
			}

		    if(profile_size)
			{
			    read(pdb->fd, buf, profile_size);
			    pdb->profiles.array[pdb->profiles.used] = profile_load(buf, profile_size);
			}
		}

	    if(buf)
		free(buf);
	}

    return pdb;

  error:
    
    if(pdb)
	profile_db_close(pdb);

    return NULL;
    
}

void profile_db_close(profile_db *pdb)
{
    if(pdb)
	{
	    if(pdb->flags & PDB_CHANGED)
		{
		    char   *buf = NULL;
		    size_t  buf_alloc = 0;
		    
		    size_t  profile_size;
		    u_long  tmp;
            short   shortTmp;

		    lseek(pdb->fd, 0, SEEK_SET);

            // { write header (pdb signature, major version, minor version)
		    tmp = htonl(0x20050901UL); // some pdb signature  
            write(pdb->fd, &tmp, sizeof(tmp));
            shortTmp = htons(0x0001);   // major version
            write(pdb->fd, &shortTmp, sizeof(shortTmp));
            shortTmp = htons(0x0000);   // minor version
            write(pdb->fd, &shortTmp, sizeof(shortTmp));
            // }

		    tmp = htonl(pdb->profiles.used);
		    write(pdb->fd, &tmp, sizeof(tmp));

		    for(size_t i = 0; i < pdb->profiles.used; i++)
			{
			    if(!pdb->profiles.array[i]) continue;

			    profile_size = profile_get_size(pdb->profiles.array[i]);

			    if(profile_size == 0) continue;
			    if(profile_size > buf_alloc)
				{
				    if(buf) free(buf);
				    buf = (char *)malloc(buf_alloc = profile_size);
				}

			    memset(buf, '?', profile_size);

			    tmp = htonl(profile_size);
			    write(pdb->fd, &tmp, sizeof(tmp));

			    profile_save(pdb->profiles.array[i], buf, buf_alloc);

			    write(pdb->fd, buf, profile_size);
			}

		    if(buf)
			free(buf);
		}

	    
	    for(size_t i = 0; i < pdb->profiles.used; i++)
		{
		    if(pdb->profiles.array[i]) 
			profile_destroy(pdb->profiles.array[i]);
		}
	    
	    if(pdb->profiles.array)
		free(pdb->profiles.array);

		close(pdb->fd);

	    free(pdb);
	}
}

int profile_db_add(profile_db *pdb, profile *p)
{
    if(!pdb || !p)
	return -1;

    if(pdb->profiles.used == pdb->profiles.alloc)
	{
	    if(pdb->profiles.array)
		pdb->profiles.array = (profile **)realloc(pdb->profiles.array, (pdb->profiles.alloc += 10)*sizeof(profile *));
	    else
		pdb->profiles.array = (profile **)malloc((pdb->profiles.alloc = 20)*sizeof(profile *));

	    memset(pdb->profiles.array + pdb->profiles.used, 0, (pdb->profiles.alloc - pdb->profiles.used)*sizeof(profile *));
	}
    
    pdb->profiles.array[pdb->profiles.used++] = p;
    pdb->flags |= PDB_CHANGED;
    

    return 0;
}

#if 0

int profile_db_iterate(profile_db *pdb, int (* callback)(void *user_data, profile *p), void *user_data, bool skipFormal)
{
    if(!pdb || !callback)
	return -1;

    for(size_t i = 0; i < pdb->profiles.used; i++)
        //Logic for skipping formal.xml profile (used heuristics: 1) First position in file and 2) Profile name)
        if(i != 0 || !skipFormal /*|| stricmp(pdb->profiles.array[i]->name, PROFILE_FORMAL_NAME) != 0*/)
	    {
	        if(callback(user_data, pdb->profiles.array[i]))
		    break;
	    }

    return 0;
}


profile *profile_search(profile_db *pdb, const char *name)
{
    if(!pdb || !name)
	return NULL;

    DBT key, data;

    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    if(pdb->db->get(pdb->db, &key, &data, 0) == 0)
	{
	    return profile_load(data.data, data.size);
	    
	}
    else
	{
	    return NULL;
	}
}

#endif /* 0 */


/*
 * <eof profile_db.cpp>
 */
