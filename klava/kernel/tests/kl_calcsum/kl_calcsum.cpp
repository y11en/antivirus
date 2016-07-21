
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <kl_calcsum.h>

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
	fprintf (stderr, "use kl_calcsum <filename>\n");
	return 1;
    }

    const char *fname = argv[1];

    struct stat st;
    if (stat (fname, &st) < 0)
    {
	perror (fname);
        return 1;
    }

    FILE *fd = fopen (fname, "rb");
    if (fd == 0)
    {
	perror (fname);
        return 1;
    }

    unsigned int fsz = st.st_size;

    unsigned char *buf = (unsigned char *)malloc (fsz);
    if (buf == 0)
    {
	perror ("malloc");
        return 1;
    }

    if (fread (buf, fsz, 1, fd) != 1)
    {
	fprintf (stderr, "error reading file\n");
	return 1;
    }

    uint32_t sum = klav_calcsum (buf, fsz);
    printf ("0x%08x\n", sum);

    return 0;
}


