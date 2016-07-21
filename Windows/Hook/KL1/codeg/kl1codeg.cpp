#include <stdio.h>
#include <kl1_api.h>

static int key[] = KL1_KEY;

int main(int argc, char* argv[])
{
    printf("KL1 Codeg (c)2006 Kaspersky Labs\n");

    int ret = 1;

    if (argc > 2)
    {
        FILE *fi = fopen(argv[1], "rb");
        if (fi)
        {
            FILE *fo = fopen(argv[2], "wb");
            if (fo)
            {
                fseek(fi, 0, SEEK_END);
                size_t size = ftell(fi);
                fseek(fi, 0, SEEK_SET);

                printf("Recoding %s to %s (%d bytes)...\n", argv[1], argv[2], size);

                char *buffer = new char [size];
                if (buffer)
                {
                    if (fread(buffer, 1, size, fi) == size)
                    {
                        for(size_t i = 0; i < size; ++i)
                            buffer[i] ^= key[i % KL1_KEY_SIZE];

                        if (fwrite(buffer, 1, size, fo) == size)
                        {
                            printf("Recoding done\n");
                            ret = 0;
                        }
                        else
                            printf("Unable to write output file %s\n", argv[2]);
                    }
                    else
                        printf("Unable to read input file %s\n", argv[1]);

                    delete [] buffer;
                }
                else
                    printf("Not enough memory\n");

                fclose(fo);
            }
            else
                printf("Unable to open output file: %s\n", argv[2]);

            fclose(fi);
        }
        else
            printf("Unable to open input file: %s\n", argv[1]);
    }
    else
        printf("Usage: kl1codeg <in.sys|.dat> <out.dat|.sys>\n");

	return ret;
}

