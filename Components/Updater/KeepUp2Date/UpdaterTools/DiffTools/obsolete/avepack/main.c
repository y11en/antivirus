#include <stdio.h>
#include "AvcPacker.h"
#include "fakeoop.h"

int main(int argc, char **argv)
{
  DWORD dwBufSize=0;
  BYTE *buf=0;
  FILE *fp=0;
  int ret=0;

  printf("Start avc packer!\n");
  /************************************************************/
  ret=UnpackAvc("o:\\bases\\test\\extract.avc",&buf,&dwBufSize);
  if (ret!=0)
  {
    printf("Error %d occured during 1st unpacking!\n",ret);
    return ret;
  }

  fp=fopen("o:\\bases\\test\\00000001.av2","wb");
  if(fp)
  {
    fwrite(buf,dwBufSize,1,fp);
    fclose(fp);
  }
  printf("Creating ave2 passed.\n");

  ret = PackInAvc(buf,dwBufSize,"o:\\bases\\test\\00000002.avc");
  FreeBuffer(buf);
  dwBufSize=0;
  if(ret!=0)
  {
    printf("Error %d occured during 1st packing!\n",ret);
  }
  printf("Creating avc passed.\n");
  /************************************************************/
  printf("Start avc packer again!\n");

  ret=UnpackAvc("o:\\bases\\test\\00000002.avc",&buf,&dwBufSize);
  if (ret!=0)
  {
    printf("Error %d occured during 2nd unpacking!\n",ret);
    return ret;
  }

  fp=fopen("o:\\bases\\test\\00000003.av2","wb");
  if(fp)
  {
    fwrite(buf,dwBufSize,1,fp);
    fclose(fp);
  }
  printf("Creating ave2 passed.\n");

  ret = PackInAvc(buf,dwBufSize,"o:\\bases\\test\\00000004.avc");
  FreeBuffer(buf);
  dwBufSize=0;
  if(ret!=0)
  {
    printf("Error %d occured during 2nd packing!\n",ret);
  }
  printf("Creating avc passed.\n");
  /************************************************************/

  printf("Passed!!!\n");
  return 0;
}
