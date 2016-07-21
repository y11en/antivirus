#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

#include <version\ver_avp.h>

#define BSIZE 0x2000

main(int argc,char* argv[])
{
	unsigned count;
	long len;
	int fi;
	int fo;
	int r=17;
	void* buf;
	if(argc < 2) goto ret;
	if(!stricmp(argv[1],"/LOGO")){
r=11;
		goto ret;
	}
	if(!stricmp(argv[1],"/DEL") && argc > 2){
r=8;
		cprintf("source:      %s\n\r",argv[2]);
		cprintf("deleting...");
		if(0==unlink(argv[2]))r=0;
		goto ret;
	}
	if(stricmp(argv[1],"/MOVE") || argc < 4) goto ret;
r=9;
	buf=malloc(BSIZE+10);
	if(buf==NULL)goto ret;
r=8;
	fi=open(argv[2],O_RDONLY|O_BINARY);
	if(fi==-1)goto ret;
r=7;
	fo=open(argv[3],O_RDWR|O_BINARY);
	if(fo==-1)goto cfi;
r=6;
	lseek(fi,0,SEEK_END);
	len=tell(fi);
	lseek(fi,0,SEEK_SET);

	cprintf("source:      %s\n\r",argv[2]);
	cprintf("destination: %s\n\r",argv[3]);
	cprintf("moving...");
	while((count=read(fi,buf,BSIZE)))
	{
		cprintf(".");
		if(count!=write(fo,buf,count))
			goto clo;
		len-=count;
	}
r=5;
	if(!len)
r=0;
	chsize(fo,tell(fo));

clo:	close(fo);
cfi:	close(fi);
ret:
	switch(r){
	default:
			cprintf("\n\r%s version %s",VER_PRODUCTNAME_STR,VER_PRODUCTVERSION_STR);
			cprintf("\n\r%s",VER_LEGALCOPYRIGHT_STR);
		if(r==11)
			cprintf("\n\rDOS mode startup utility.");
		else
			cprintf("\n\rThis program is 32bit Windows application.");
		break;
	case 9:	cprintf("\n\rError:  initialization failed.");
		break;
	case 8:	cprintf("\n\rError:  wrong source file %s",argv[2]);
		break;
	case 7:	cprintf("\n\rError: wrong destination file %s",argv[3]);
		break;
	case 6:	cprintf("\n\rError: I/O failed.");
		break;
	case 5:	cprintf("\n\rError: sizes mismatch.");
		break;
	case 0:	cprintf(" OK.");
		break;
	}
	cprintf("\n\r");
	if(!buf)free(buf);
	if(!r)unlink(argv[2]);
	return r;
}