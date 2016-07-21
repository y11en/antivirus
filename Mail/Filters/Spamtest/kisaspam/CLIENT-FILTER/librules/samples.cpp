/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: samples.cpp                                                           *
 * Created: Thu Jul 11 22:26:57 2002                                           *
 * Desc: Samples tree building.                                                *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  samples.cpp
 * \brief Samples tree building.
 */

#include <string.h>
#include <stdio.h>

#include "samples.h"

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif
 
samples_node *samples_add(samples_node *root, const char *path, const char *filename)
{
    if(!(path && path[0] && filename && filename[0]))
	{
	    samples_free(root);
	    return NULL;
	}

    size_t id_size = 0;
    
    for(id_size = 0; path[id_size] && path[id_size] != '/'; id_size++)
	;

    if(id_size == 0)
	{
	    samples_free(root);
	    return NULL;
	}

#define ADD_FILE_NAME do { goto add_file_name; } while(0)

    if(!root)
	{
	    root = (samples_node *)malloc(sizeof(samples_node));
	    memset(root, 0, sizeof(samples_node));

	    root->id = (char *)malloc(id_size+1);
	    memcpy(root->id, path, id_size);
	    root->id[id_size] = '\0';

	    if(path[id_size] == '\0')
		ADD_FILE_NAME;
	    else 
		{
		    root->son = samples_add(root->son, path+id_size+1, filename);
		    
		    if(!root->son)
			{
			    samples_free(root);
			    return NULL;
			}
		    else
			return root;
		}
	}
    
    if(strlen(root->id) == id_size && memcmp(root->id, path, id_size) == 0)
	{
	    if(path[id_size] == '\0')
		ADD_FILE_NAME;

	    root->son = samples_add(root->son, path+id_size+1, filename);
	    if(!root->son)
		{
		    samples_free(root);
		    return NULL;
		}
	    else
		return root;
	}
    else
	{
	    root->brother = samples_add(root->brother, path, filename);
	    
	    if(!root->brother)
		{
		    samples_free(root);
		    return NULL;
		}
	    else
		return root;
	}
    

#undef ADD_FILE_NAME

  add_file_name:


    if(root->files.names)
	{
	    root->files.names = (char **)realloc(root->files.names, sizeof(char *)*(root->files.alloc + 1));
	    memset(root->files.names + root->files.alloc, 0, sizeof(char *));
	}
    else
	{
	    root->files.names = (char **)malloc(sizeof(char *));
	    memset(root->files.names, 0, sizeof(char *));
	}

    root->files.names[root->files.alloc] = strdup(filename);
    root->files.alloc++;

    return root;
}

int samples_free(samples_node *root)
{
    if(!root) return 0;

    if(samples_free(root->brother))
	return 1;

    if(samples_free(root->son))
	return 1;

    if(root->id)
	free(root->id);

    for(size_t i = 0; i < root->files.alloc; i++)
	if(root->files.names[i]) free(root->files.names[i]);

    if(root->files.names)
	free(root->files.names);

    free(root);

    return 0;
}



/*
 * <eof samples.cpp>
 */
