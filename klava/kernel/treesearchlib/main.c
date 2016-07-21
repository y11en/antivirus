// main.c
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "avp_tree.h"


void usage ()
{
	fprintf (stderr, "Use avp_tree <db_file> <data_files...>\n");
	exit (1);
}

void * AVP_TREE_CALL tree_heap_alloc  (void * p_heap, unsigned int size)
{
	return calloc (1, size);
}

void * AVP_TREE_CALL tree_heap_realloc (void * p_heap, void * p_mem, unsigned int new_size)
{
	if (p_mem == 0)
		return calloc (1, new_size);
	else
		return realloc (p_mem, new_size);
}

void AVP_TREE_CALL tree_heap_free (void * p_heap, void * p_mem)
{
	if (p_mem != 0)
		free (p_mem);
}


AVP_Tree_Alloc tree_heap_ctx =
{
	0,
	tree_heap_alloc,
	tree_heap_realloc,
	tree_heap_free
};

unsigned char * read_file_data (const char *fname, unsigned int *psize)
{
	struct stat st;
	unsigned char * data = 0;
	unsigned int size = 0;
	FILE * fd = 0;

	*psize = 0;

	if (stat (fname, &st) < 0)
	{
		perror (fname);
		return 0;
	}

	size = (unsigned int) st.st_size;
	if (size == 0)
	{
		fprintf (stderr, "file has zero size: %s\n", fname);
		return 0;
	}

	data = malloc (size);
	if (data == 0)
	{
		fprintf (stderr, "not enough memory\n");
		return 0;
	}

	fd = fopen (fname, "rb");
	if (fd == 0)
	{
		perror (fname);
		free (data);
		return 0;
	}

	if (fread (data, 1, size, fd) != (int) size)
	{
		fprintf (stderr, "error reading file: %s\n", fname);
		fclose (fd);
		free (data);
		return 0;
	}

	fclose (fd);

	*psize = size;
	return data;
}

int AVP_TREE_CALL tree_scan_callback (
		void *       callback_ctx,  // user context
		unsigned int sig_id,        // signature ID
		int          sig_offset,    // offset of signature in buffer
		unsigned int sig_len        // signature length
	)
{
	fprintf (stdout, "OFF:0x%08x, ID:0x%08x, LEN:0x%x\n", sig_offset, sig_id, sig_len);
	return 0;
}

int main (int argc, char *argv[])
{
	const char * db_fname = 0;
	unsigned char * cfg_data = 0;
	unsigned int cfg_size = 0;
	hAVP_TREE h_tree = 0;
	int a;

	if (argc < 3)
		usage ();

	db_fname = argv [1];

	cfg_data = read_file_data (db_fname, &cfg_size);
	if (cfg_data == 0)
	{
		return 1;
	}
		
	h_tree = AVP_Tree_Create (& tree_heap_ctx, cfg_data, cfg_size);
	if (h_tree == 0)
	{
		fprintf (stderr, "error creating tree from %s\n", db_fname);
		free (cfg_data);
		return 1;
	}

	for (a = 2; a < argc; ++a)
	{
		const char * scan_fname = argv [a];
		unsigned char *scan_data = 0;
		unsigned int scan_size = 0;

		fprintf (stdout, "scanning %s\n", scan_fname);

		scan_data = read_file_data (scan_fname, & scan_size);
		if (scan_data != 0)
		{
			AVP_Tree_Search (h_tree, scan_data, scan_size, tree_scan_callback, 0);

			free (scan_data);
		}
	}

	AVP_Tree_Destroy (h_tree);
	free (cfg_data);

	return 0;
}

