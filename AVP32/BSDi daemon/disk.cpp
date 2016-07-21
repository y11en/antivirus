#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "typedef.h"
#include "avp_rc.h"
#include "contain.h"
#include "disk.h"

ULONG StrICmp(char *str1,char *str2)
;

//#include <linux/major.h>

//--------------- mount -----------
//#include "mount/mntent.h"
//#include "mount/fstab.h"
//#include "mount/sundries.h"

//#include <sys.mount.h>

//int verbose=0;
//int mount_quiet=0;

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <ufs/ufs/ufsmount.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

int   checkvfsname __P((const char *, char **));
//char    **makevfslist __P((char *));
//long      regetmntinfo __P((struct statfs **, long, char **));
int       bread __P((off_t, void *, int));
//char     *getmntpt __P((char *));
//void      prtstat __P((struct statfs *, int));
//int       ufs_df __P((char *, int));

int     iflag, nflag;
struct  ufs_args mdev;

char *getmntpt(char *name)
 {
  long mntsize, i;
  struct statfs *mntbuf;

  mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
  for (i = 0; i < mntsize; i++)
   {
    if (!strcmp(mntbuf[i].f_mntfromname, name))
            return (mntbuf[i].f_mntonname);
   }
  return (0);
 }

// * Make a pass over the filesystem info in ``mntbuf'' filtering out
// * filesystem types not in vfslist and possibly re-stating to get
// * current (not cached) info.  Returns the new count of valid statfs bufs.
long regetmntinfo(struct statfs **mntbufp,long mntsize,char **vfslist)
 {
  int i, j;
  struct statfs *mntbuf;

  if(vfslist == NULL)
     return (nflag ? mntsize : getmntinfo(mntbufp, MNT_WAIT));

  mntbuf = *mntbufp;
  for(j = 0, i = 0; i < mntsize; i++)
   {
//    if (checkvfsname(mntbuf[i].f_fstypename, vfslist)) continue;
    if (!nflag) (void)statfs(mntbuf[i].f_mntonname,&mntbuf[j]);
    else
     if (i != j) mntbuf[j] = mntbuf[i];
    j++;
   }
  return (j);
 }

// * Convert statfs returned filesystem size into BLOCKSIZE units.
// * Attempts to avoid overflow for large filesystems.
#define fsbtoblk(num, fsbs, bs) \
        (((fsbs) != 0 && (fsbs) < (bs)) ? \
                (num) / ((bs) / (fsbs)) : (num) * ((fsbs) / (bs)))

// * Print out status about a filesystem.
void prtstat(struct statfs *sfsp,int maxwidth)
 {
  static long blocksize;
  static int headerlen, timesthrough;
  static char *header;
  long used, availblks, inodes;

  if(maxwidth < 11) maxwidth = 11;
  if(++timesthrough == 1)
   {
    header = getbsize(&headerlen, &blocksize);
    (void)printf("%-*.*s %s     Used    Avail Capacity",
        maxwidth, maxwidth, "Filesystem", header);
    if (iflag)
            (void)printf(" iused   ifree  %%iused");
    (void)printf("  Mounted on\n");
   }
  (void)printf("%-*.*s", maxwidth, maxwidth, sfsp->f_mntfromname);
  used = sfsp->f_blocks - sfsp->f_bfree;
  availblks = sfsp->f_bavail + used;
  (void)printf(" %*ld %8ld %8ld", headerlen,
      fsbtoblk(sfsp->f_blocks, sfsp->f_bsize, blocksize),
      fsbtoblk(used, sfsp->f_bsize, blocksize),
      fsbtoblk(sfsp->f_bavail, sfsp->f_bsize, blocksize));
  (void)printf(" %5.0f%%",
      availblks == 0 ? 100.0 : (double)used / (double)availblks * 100.0);
  if(iflag)
   {
    inodes = sfsp->f_files;
    used = inodes - sfsp->f_ffree;
    (void)printf(" %7ld %7ld %5.0f%% ", used, sfsp->f_ffree,
       inodes == 0 ? 100.0 : (double)used / (double)inodes * 100.0);
   }
  else
    (void)printf("  ");
  (void)printf("  %s\n", sfsp->f_mntonname);
 }

// * This code constitutes the pre-system call Berkeley df code for extracting
// * information from filesystem superblocks.
#include <ufs/ufs/dinode.h>
#include <ufs/ffs/fs.h>
#include <errno.h>
#include <fstab.h>

union
 {
  struct fs iu_fs;
  char dummy[SBSIZE];
 } sb;
#define sblock sb.iu_fs

int rfd;

int ufs_df(char *file,int maxwidth)
 {
  struct statfs statfsbuf;
  struct statfs *sfsp;
  char *mntpt;
  static int synced;

  if(synced++ == 0) sync();

  if ((rfd = open(file, O_RDONLY)) < 0)
   {
    warn("%s", file);
    return (1);
   }
  if (bread((off_t)SBOFF, &sblock, SBSIZE) == 0)
   {
    (void)close(rfd);
    return (1);
   }
  sfsp = &statfsbuf;
  sfsp->f_type = 1;
  strcpy(sfsp->f_fstypename, "ufs");
  sfsp->f_flags = 0;
  sfsp->f_bsize = sblock.fs_fsize;
  sfsp->f_iosize = sblock.fs_bsize;
  sfsp->f_blocks = sblock.fs_dsize;
  sfsp->f_bfree = sblock.fs_cstotal.cs_nbfree * sblock.fs_frag +
          sblock.fs_cstotal.cs_nffree;
  sfsp->f_bavail = freespace(&sblock, sblock.fs_minfree);
  sfsp->f_files =  sblock.fs_ncg * sblock.fs_ipg;
  sfsp->f_ffree = sblock.fs_cstotal.cs_nifree;
  sfsp->f_fsid.val[0] = 0;
  sfsp->f_fsid.val[1] = 0;
  if ((mntpt = getmntpt(file)) == 0)
          mntpt = "";
  memmove(&sfsp->f_mntonname[0], mntpt, MNAMELEN);
  memmove(&sfsp->f_mntfromname[0], file, MNAMELEN);
  prtstat(sfsp, maxwidth);
  (void)close(rfd);
  return (0);
 }

int bread(off_t off,void * buf,int cnt)
 {
  int nr;

  (void)lseek(rfd, off, SEEK_SET);
  if ((nr = read(rfd, buf, cnt)) != cnt)
   {
    // Probably a dismounted disk if errno == EIO.
    if (errno != EIO)
     (void)fprintf(stderr,"\ndf: %qd: %s\n",off,strerror(nr > 0 ? EIO:errno));
    return (0);
   }
  return (1);
 }

//-- arc4random --
struct arc4_stream
 {
  u_int8_t i;
  u_int8_t j;
  u_int8_t s[256];
 } __attribute__ ((packed)) ;

static int rs_initialized;
static struct arc4_stream rs;

static inline void arc4_init(struct arc4_stream *as)
 {
  int     n;

  for (n = 0; n < 256; n++) as->s[n] = n;
  as->i = 0;
  as->j = 0;
 }

static inline void arc4_addrandom(struct arc4_stream *as,u_char *dat,int datlen)
 {
  int     n;
  u_int8_t si;

  as->i--;
  for (n = 0; n < 256; n++)
   {
    as->i = (as->i + 1);
    si = as->s[as->i];
    as->j = (as->j + si + dat[n % datlen]);
    as->s[as->i] = as->s[as->j];
    as->s[as->j] = si;
   }
 }

static void arc4_stir(struct arc4_stream *as)
 {
  int     fd;
  struct
   {
	struct timeval tv;
	pid_t pid;
	u_int8_t rnd[128 - sizeof(struct timeval) - sizeof(pid_t)];
   } __attribute__ ((packed))  rdat;

  gettimeofday(&rdat.tv, NULL);
  rdat.pid = getpid();
  fd = open("/dev/urandom", O_RDONLY, 0);
  if (fd >= 0)
   {
    (void) read(fd, rdat.rnd, sizeof(rdat.rnd));
    close(fd);
   }
  // fd < 0?  Ah, what the heck. We'll just take whatever was on the * stack... 

  arc4_addrandom(as, (void *) &rdat, sizeof(rdat));
 }

static inline u_int8_t arc4_getbyte(struct arc4_stream *as)
 {
  u_int8_t si, sj;

  as->i = (as->i + 1);
  si = as->s[as->i];
  as->j = (as->j + si);
  sj = as->s[as->j];
  as->s[as->i] = sj;
  as->s[as->j] = si;
  return (as->s[(si + sj) & 0xff]);
 }

static inline u_int32_t arc4_getword(struct arc4_stream *as)
 {
  u_int32_t val;
  val = arc4_getbyte(as) << 24;
  val |= arc4_getbyte(as) << 16;
  val |= arc4_getbyte(as) << 8;
  val |= arc4_getbyte(as);
  return val;
 }

void arc4random_stir()
 {
  if (!rs_initialized)
   {
    arc4_init(&rs);
    rs_initialized = 1;
   }
  arc4_stir(&rs);
 }

void arc4random_addrandom(u_char *dat,int datlen)
 {
  if (!rs_initialized) arc4random_stir();
  arc4_addrandom(&rs, dat, datlen);
 }

u_int32_t arc4random()
 {
  if (!rs_initialized) arc4random_stir();
  return arc4_getword(&rs);
 }

//-- end of arc4random --

static int _gettemp(char *path,register int *doopen,int domkdir)
 {
  register char *start, *trv;
  struct stat sbuf;
  int pid, rval;

  if (doopen && domkdir)
   {
    errno = EINVAL;
    return(0);
   }

  pid = getpid();
  for (trv = path; *trv; ++trv)
		;
  --trv;
  while (*trv == 'X' && pid != 0)
   {
    *trv-- = (pid % 10) + '0';
    pid /= 10;
   }
  while (*trv == 'X')
   {
    char c;

    pid = (arc4random() & 0xffff) % (26+26);
    if (pid < 26) c = pid + 'A';
    else c = (pid - 26) + 'a';
    *trv-- = c;
   }
  start = trv + 1;

  // check the target directory; if you have six X's and it
  // doesn't exist this runs for a *very* long time.
  if (doopen || domkdir)
   {
    for (;; --trv)
     {
      if (trv <= path) break;
      if (*trv == '/')
       {
	*trv = '\0';
	rval = stat(path, &sbuf);
	*trv = '/';
	if (rval != 0) return(0);
	if (!S_ISDIR(sbuf.st_mode))
	 {
	  errno = ENOTDIR;
	  return(0);
	 }
	break;
       }
     }
   }

  for (;;)
   {
    if (doopen)
     {
      if ((*doopen = open(path, O_CREAT|O_EXCL|O_RDWR, 0600)) >= 0) return(1);
      if (errno != EEXIST) return(0);
     }
    else
     if (domkdir)
      {
	if (mkdir(path, 0700) == 0) return(1);
	if (errno != EEXIST) return(0);
      }
     else
      if(lstat(path, &sbuf)) return(errno == ENOENT ? 1 : 0);

    // tricky little algorithm for backward compatibility 
    for (trv = start;;)
     {
      if (!*trv) return(0);
      if (*trv == 'Z') *trv++ = 'a';
      else
       {
        if (isdigit(*trv)) *trv = 'a';
        else
         if (*trv == 'z')	// inc from z to A 
  	  *trv = 'A';
	 else
	  ++*trv;
	break;
       }
     }
   }
  /*NOTREACHED*/
 }

char *mkdtemp(char *path)
 {
  return(_gettemp(path, (int *)NULL, 1) ? path : (char *)NULL);
 }

struct statfs *findMntPnt(char* mntname,struct statfs *statfsbuf)
 {
  struct stat stbuf;
  int err,rv,maxwidth;//,width,i,ch
  char *mntpt, *mntpath;//, **vfslist;

  if(stat(mntname,&stbuf) < 0)
   {
    err = errno;
    if((mntpt=getmntpt(mntname)) == 0)
     {
      warn("%s",mntname); // *argv);
      rv = 1;
      return NULL;//continue;
     }
   }
  else
   if ((stbuf.st_mode & S_IFMT) == S_IFCHR)
    {
     rv = ufs_df(mntname, maxwidth) || rv;
     return NULL;//continue;
    }
   else
    if ((stbuf.st_mode & S_IFMT) == S_IFBLK)
     {
      if((mntpt=getmntpt(mntname)) == 0)
       {
        mdev.fspec=mntname;// *argv;
        mntpath=strdup("/tmp/df.XXXXXX");
        if (mntpath == NULL)
         {
          warn("strdup failed");
          rv = 1;
          return NULL;//continue;
         }
        mntpt=mkdtemp(mntpath);
        if(mntpt==NULL)
         {
          warn("mkdtemp(\"%s\") failed", mntpath);
          rv = 1;
          free(mntpath);
          return NULL;//continue;
         }
        if (mount("ufs", mntpt, MNT_RDONLY,&mdev) != 0)
         {
          rv=ufs_df(mntname, maxwidth) || rv;
          (void)rmdir(mntpt);
          free(mntpath);
          return NULL;//continue;
         }
        else
         if(statfs(mntpt,statfsbuf)==0)
          {
           statfsbuf->f_mntonname[0] = '\0';
           prtstat(statfsbuf,maxwidth);
          }
         else
          {
           warn("%s",mntname);// *argv);
           rv = 1;
          }
        (void)unmount(mntpt, 0);
        (void)rmdir(mntpt);
        free(mntpath);
        return NULL;//continue;
       }
     }
    else
      mntpt=mntname;

  // Statfs does not take a `wait' flag, so we cannot
  // implement nflag here.
  if(statfs(mntpt,statfsbuf) < 0)
   {
    warn("%s", mntpt);
    rv = 1;
    return NULL;//continue;
   }
//      if(argc == 1)
//     maxwidth = strlen(statfsbuf.f_mntfromname) + 1;
  return statfsbuf;//, maxwidth);
 }
/* 
// Report on everything in mtab (of the specified types if any).
//static 
int printAllMount(void)//string_list types)
 {
//  struct stat stbuf;
  struct statfs *mntbuf;
//statfsbuf
  long mntsize;
  int i, maxwidth, rv,width;//ch,err, 
//  char *mntpt, *mntpath, **vfslist;

//  vfslist = NULL;

  mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
  maxwidth = 0;
  for (i = 0; i < mntsize; i++)
   {
    width = strlen(mntbuf[i].f_mntfromname);
    if (width > maxwidth) maxwidth = width;
   }

  rv = 0;
//    mntsize = regetmntinfo(&mntbuf, mntsize, vfslist);
//    if (vfslist != NULL)
//     {
//      maxwidth = 0;
//      for (i = 0; i < mntsize; i++)
//       {
//        width=strlen(mntbuf[i].f_mntfromname);
//        if(width>maxwidth) maxwidth = width;
//       }
//     }
    for(i=0;i<mntsize;i++)
     {
      struct statfs statfsbuf;
      if(findMntPnt(mntbuf[i].f_mntfromname,&statfsbuf)!=NULL)
       {
        prtstat(&statfsbuf, maxwidth);
//      prtstat(&mntbuf[i], maxwidth);
       }
     }

  return (rv);
//  struct mntentchn *mc;

//  for(mc=mtab_head()->nxt; mc; mc = mc->nxt)
//    if(matching_type(mc->mnt_type, types))
//     {
//      if(mount_quiet) return 0;
//      printf ("%s on %s", mc->mnt_fsname, mc->mnt_dir);
//      if(mc->mnt_type != NULL && *(mc->mnt_type) != '\0')  printf (" type %s", mc->mnt_type);
//      if(mc->mnt_opts != NULL)   printf (" (%s)", mc->mnt_opts);
//      printf ("\n");
//     }
//  return 0;
 }
*/
ULONG ExistPath(char *Path,struct stat *pstatbuf)
 {
//searchmount:
  if(lstat(Path,pstatbuf)!=-1) return 0;
  else
   {
/*    struct mntentchn *mc,*mchead=fstab_head()->nxt; //mtab_head()->nxt
    string_list types=NULL;

    for(mc=mchead;mc;mc = mc->nxt)
    if(matching_type(mc->mnt_type, types))
     {
      if(mount_quiet) return 1;
      if(mc!=mchead)
       {
        if(StrICmp(Path,mc->mnt_dir)==0)
         {
          printf("Object %s not mounted on %s.\nMount it and press any key or press ESC\n",Path,mc->mnt_dir);
          if(getchar()==27) return 1;
          else  goto searchmount;
         }
        //printf ("%s on %s", mc->mnt_fsname, mc->mnt_dir);
        //if(mc->mnt_type != NULL && *(mc->mnt_type) != '\0')   printf (" type %s", mc->mnt_type);
        //if(mc->mnt_opts != NULL)   printf (" (%s)", mc->mnt_opts);
        //printf("\n");
       }
     } */
    printf("Directory %s not exists\n",Path);
    getchar();
    return 1;
   }
 }

ULONG DiskType(PLIST pList)
 {
  struct stat statbuf;
  if(lstat(pList->szName,&statbuf)!=-1)
   {
/*    struct mntentchn *mc;
    string_list types=NULL;

    pList->szDev[0]=0;
    for(mc=mtab_head()->nxt;mc;mc = mc->nxt)
     if(matching_type(mc->mnt_type,types))
      {
       //if(mount_quiet) return 1;
//       if(!((mc->mnt_dir[0]=='/')&&(strlen(mc->mnt_dir)==1)))
         if(StrICmp(pList->szName,mc->mnt_dir)==0)
          {
           strcpy(pList->szDev,mc->mnt_fsname);
//         if(strcasecmp(mc->mnt_fsname,"/dev/fd")==0) return IDI_FLOPPY_144;
           if(mc->mnt_type!=NULL && *(mc->mnt_type)!='\0')
            {
             if(strcasecmp(mc->mnt_type,"nfs")==0) return IDI_NET_ON;
             if(strcasecmp(mc->mnt_type,"ncpfs")==0) return IDI_NET_ON;
             if(mc->mnt_opts != NULL)
              if((strcasecmp(mc->mnt_type,"iso9660")==0)&&
                (strcasecmp(mc->mnt_opts,"ro")==0)) return IDI_CD;
            }
          }
      }
    if(pList->szDev[0]==0) strcpy(pList->szDev,"/");

    struct stat bootstat;
    ULONG ret=0;
    int fd;
    if((fd=open(pList->szDev,O_RDONLY)) < 0)
      printf("You will not be able to read the disk info.\n");
    else
    if(fstat(fd,&bootstat) < 0)
     ret=0;// { scsi_disk=0; floppy=0; }
    else
     if(S_ISBLK(bootstat.st_mode) && (bootstat.st_rdev >> 8) == IDE0_MAJOR
       || (bootstat.st_rdev >> 8) == IDE1_MAJOR)
      ret=IDI_LOCAL1; //{ scsi_disk = 0; floppy = 0; }
     else
      if(S_ISBLK(bootstat.st_mode) && (bootstat.st_rdev >> 8) == 2)
       ret=IDI_FLOPPY_144;//{ scsi_disk = 0; floppy = 1; }
      else
       ret=0;//IDI_SCSI;//{ scsi_disk = 1; floppy = 0; }
    close(fd);
    //for(mc=fstab_head()->nxt;mc;mc=mc->nxt)
    // if(matching_type(mc->mnt_type,types))
    //  {
    //   //if(mount_quiet) return 1;
    //   if(!((mc->mnt_fsname[0]=='/')&&(strlen(mc->mnt_fsname)==1)))
    //    {
    //     printf("device %s is local hard disk\n",device);
    //     if(strcasecmp(device,mc->mnt_dir)==0) return IDI_LOCAL1;
    //    }
    //  }
    return ret;
*/   }
  return 0;
 }

// ----------------- df --------------
// Display a space listing for the disk device with absolute path DISK.
// If MOUNT_POINT is non-NULL, it is the path of the root of the
// filesystem on DISK.
// If FSTYPE is non-NULL, it is the type of the filesystem on DISK.
// If MOUNT_POINT is non-NULL, then DISK may be NULL -- certain systems may
// not be able to produce statistics in this case.
/*
static void show_dev (const char *disk,const char *mount_point,const char *fstype)
 {
  struct fs_usage fsu;
  long blocks_used;
  long blocks_percent_used;
  long inodes_used;
  long inodes_percent_used;
  const char *stat_file;

  // If MOUNT_POINT is NULL, then the filesystem is not mounted, and this
  // program reports on the filesystem that the special file is on.
  // It would be better to report on the unmounted filesystem,
  // but statfs doesn't do that on most systems.
  stat_file = mount_point ? mount_point : disk;

  if(get_fs_usage (stat_file, disk, &fsu))
    {
     //error (0, errno, "%s", stat_file);
     exit_status = 1;
     return;
    }

  if(kilobyte_blocks)
    {
     fsu.fsu_blocks /= 2;
     fsu.fsu_bfree /= 2;
     fsu.fsu_bavail /= 2;
    }

  if (fsu.fsu_blocks == 0)
    {
      if(!show_all_fs && !show_listed_fs) return;
      blocks_used = fsu.fsu_bavail = blocks_percent_used = 0;
    }
  else
    {
      blocks_used = fsu.fsu_blocks - fsu.fsu_bfree;
      blocks_percent_used=(long)
        (blocks_used * 100.0 / (blocks_used + fsu.fsu_bavail) + 0.5);
    }

  if (fsu.fsu_files == 0)
    {
     inodes_used = fsu.fsu_ffree = inodes_percent_used = 0;
    }
  else
    {
     inodes_used = fsu.fsu_files - fsu.fsu_ffree;
     inodes_percent_used=(long)(inodes_used * 100.0 / fsu.fsu_files + 0.5);
    }

  if (! disk)
    disk = "-";                 // unknown

  printf ((print_type ? "%-13s" : "%-20s"), disk);
  if ((int) strlen (disk) > (print_type ? 13 : 20) && !posix_format)
    printf ((print_type ? "\n%13s" : "\n%20s"), "");

  if (! fstype)
    fstype = "-";               // unknown
  if (print_type)
    printf (" %-5s ", fstype);

    printf (" %7ld %7ld  %7ld  %5ld%% ",
            fsu.fsu_blocks, blocks_used, fsu.fsu_bavail, blocks_percent_used);

  if(mount_point)
    {
#ifdef HIDE_AUTOMOUNT_PREFIX
     // Don't print the first directory name in MOUNT_POINT if it's an
     // artifact of an automounter.  This is a bit too aggressive to be
     // the default.
      if (strncmp ("/auto/", mount_point, 6) == 0)
        mount_point += 5;
      else if (strncmp ("/tmp_mnt/", mount_point, 9) == 0)
        mount_point += 8;
#endif
      printf ("  %s", mount_point);
    }
  putchar ('\n');
}
*/

// ---------------- copy -------------
#include <sys/param.h>
// Get or fake the disk device blocksize.
// Usually defined by sys/param.h (if at all).
#ifndef DEV_BSIZE
 #ifdef BSIZE
  #define DEV_BSIZE BSIZE
 #else // !BSIZE
  #define DEV_BSIZE 4096
 #endif // !BSIZE
#endif // !DEV_BSIZE

// Extract or fake data from a `struct stat'.
// ST_BLKSIZE: Optimal I/O blocksize for the file, in bytes.
// ST_NBLOCKS: Number of 512-byte blocks in the file
// (including indirect blocks).
#ifndef HAVE_ST_BLOCKS
 #define ST_BLKSIZE(statbuf) DEV_BSIZE
 #if defined(_POSIX_SOURCE) || !defined(BSIZE) // fileblocks.c uses BSIZE.
  #define ST_NBLOCKS(statbuf) (((statbuf).st_size + 512 - 1) / 512)
 #else // !_POSIX_SOURCE && BSIZE
  #define ST_NBLOCKS(statbuf) (st_blocks ((statbuf).st_size))
 #endif // !_POSIX_SOURCE && BSIZE
#else // HAVE_ST_BLOCKS
 // Some systems, like Sequents, return st_blksize of 0 on pipes.
 #define ST_BLKSIZE(statbuf) ((statbuf).st_blksize > 0 \
                               ? (statbuf).st_blksize : DEV_BSIZE)
 #if defined(hpux) || defined(__hpux__) || defined(__hpux)
 // HP-UX counts st_blocks in 1024-byte units.
 // This loses when mixing HP-UX and BSD filesystems with NFS.
  #define ST_NBLOCKS(statbuf) ((statbuf).st_blocks * 2)
 #else // !hpux
  #if defined(_AIX) && defined(_I386)
   // AIX PS/2 counts st_blocks in 4K units.
   #define ST_NBLOCKS(statbuf) ((statbuf).st_blocks * 8)
  #else // not AIX PS/2
   #if defined(_CRAY)
    #define ST_NBLOCKS(statbuf) ((statbuf).st_blocks * ST_BLKSIZE(statbuf)/512)
   #else // not AIX PS/2 nor CRAY
    #define ST_NBLOCKS(statbuf) ((statbuf).st_blocks)
   #endif // not _CRAY
  #endif // not AIX PS/2
 #endif // !hpux
#endif // HAVE_ST_BLOCKS

// Control creation of sparse files (files with holes).
enum Sparse_type
 {
  // Never create holes in DEST.
  SPARSE_NEVER,

  // This is the default.  Use a crude (and sometimes inaccurate)
  // heuristic to determine if SOURCE has holes.  If so, try to create
  // holes in DEST.
  SPARSE_AUTO,

  // For every sufficiently long sequence of bytes in SOURCE, try to
  // create a corresponding hole in DEST.  There is a performance penalty
  // here because CP has to search for holes in SRC.  But if the holes are
  // big enough, that penalty can be offset by the decrease in the amount
  // of data written to disk.
  SPARSE_ALWAYS
 };

static char const *const sparse_type_string[] =
 {
  "never", "auto", "always", 0
 };

static enum Sparse_type const sparse_type[] =
 {
  SPARSE_NEVER, SPARSE_AUTO, SPARSE_ALWAYS
 };

// Control creation of sparse files.
static int flag_sparse = SPARSE_AUTO;

#include <errno.h>
#ifndef errno
extern int errno;
#endif

// Write LEN bytes at PTR to descriptor DESC, retrying if interrupted.
// Return LEN upon success, write's (negative) error code otherwise.

int full_write(int desc,char *ptr,size_t len)
 {
  int total_written;

  total_written = 0;
  while(len>0)
    {
      int written = write (desc, ptr, len);
      if(written < 0)
        {
#ifdef EINTR
          if (errno == EINTR) continue;
#endif
          return written;
        }
      total_written += written;
      ptr += written;
      len -= written;
    }
  return total_written;
}

// A pointer to either lstat or stat, depending on whether dereferencing of symlinks is done.
//static int (*xstat)(const char*,struct stat*);

// If nonzero, copy all files except (directories and, if not dereferencing
// them, symbolic links,) as if they were regular files.
//static int flag_copy_as_regular = 1;

// If nonzero, dereference symbolic links (copy the files they point to).
//static int flag_dereference = 1;

// If nonzero, remove existing destination nondirectories.
//static int flag_force = 0;

// If nonzero, create hard links instead of copying files.
//   Create destination directories as usual.
//static int flag_hard_link = 0;

// If nonzero, query before overwriting existing destinations with regular files.
//static int flag_interactive = 0;

// If nonzero, the command "cp x/e_file e_dir" uses "e_dir/x/e_file"
//   as its destination instead of the usual "e_dir/e_file."
//static int flag_path = 0;

// If nonzero, give the copies the original files' permissions,
// ownership, and timestamps.
//static int flag_preserve = 0;

// If nonzero, copy directories recursively and copy special files
// as themselves rather than copying their contents.
//static int flag_recursive = 0;

// If nonzero, create symbolic links instead of copying files.
// Create destination directories as usual.
//static int flag_symbolic_link = 0;

// If nonzero, when copying recursively, skip any subdirectories that are
// on different filesystems from the one we started on.
//static int flag_one_file_system = 0;

// If nonzero, do not copy a nondirectory that has an existing destination
// with the same or newer modification time.
//static int flag_update = 0;

// If nonzero, display the names of the files before copying them.
//static int flag_verbose = 0;

// Add path NODE, copied from inode number INO and device number DEV,
// to the list of files we have copied.
// Return NULL if inserted, otherwise non-NULL.

char *remember_copied (const char *node, ino_t ino, dev_t dev)
{
  return NULL;//hash_insert (ino, dev, node);
}


// Copy a regular file from SRC_PATH to DST_PATH.
//   If the source file contains holes, copies holes and blocks of zeros
//   in the source file as holes in the destination file.
//   (Holes are read as zeroes by the `read' system call.)
//   Return 0 if successful, -1 if an error occurred.

int copyReg(const char *src_path,const char *dst_path)
 {
  char *buf;
  int buf_size;
  int dest_desc;
  int source_desc;
  int n_read;
  struct stat sb;
  char *cp;
  int *ip;
  int return_val = 0;
  long n_read_total = 0;
  int last_write_made_hole = 0;
  int make_holes = (flag_sparse == SPARSE_ALWAYS);

  source_desc = open (src_path, O_RDONLY);
  if(source_desc<0)
    {
     //error(0,errno,"%s",src_path);
     return -1;
    }

  // Create the new regular file with small permissions initially,
  // to not create a security hole.

  dest_desc = open (dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if(dest_desc<0)
    {
     //error(0,errno,_("cannot create regular file `%s'"), dst_path);
     return_val = -1;
     goto ret2;
    }

  // Find out the optimal buffer size.

  if(fstat (dest_desc, &sb))
    {
     //error(0,errno, "%s", dst_path);
     return_val = -1;
     goto ret;
    }

  buf_size=ST_BLKSIZE (sb);

#ifdef HAVE_ST_BLOCKS
  if(flag_sparse == SPARSE_AUTO && S_ISREG (sb.st_mode))
    {
     // Use a heuristic to determine whether SRC_PATH contains any sparse blocks.
     if (fstat (source_desc, &sb))
        {
         //error(0,errno, "%s", src_path);
         return_val = -1;
         goto ret;
        }

      // If the file has fewer blocks than would normally
      // be needed for a file of its size, then
      // at least one of the blocks in the file is a hole.
      if(S_ISREG(sb.st_mode)&& (size_t)(sb.st_size/512) > (size_t)ST_NBLOCKS(sb))
        make_holes = 1;
    }
#endif

  // Make a buffer with space for a sentinel at the end.

  buf=(char *)alloca(buf_size+sizeof (int));

  for(;;)
    {
     n_read = read (source_desc, buf, buf_size);
     if(n_read < 0)
        {
#ifdef EINTR
         if(errno==EINTR) continue;
#endif
         //error(0,errno,"%s",src_path);
         return_val = -1;
         goto ret;
        }
     if(n_read == 0) break;

     n_read_total+=n_read;

     ip = 0;
     if(make_holes)
      {
        buf[n_read] = 1;        // Sentinel to stop loop.

        // Find first nonzero *word*, or the word with the sentinel.

        ip = (int *) buf;
        while (*ip++ == 0) ;

        // Find the first nonzero *byte*, or the sentinel.

        cp = (char *) (ip - 1);
        while (*cp++ == 0) ;

        // If we found the sentinel, the whole input block was zero,
        // and we can make a hole.

        if(cp> buf+n_read)
            {
             // Make a hole.
             if(lseek (dest_desc, (off_t) n_read, SEEK_CUR) < 0L)
                {
                 //error(0,errno, "%s", dst_path);
                 return_val = -1;
                 goto ret;
                }
             last_write_made_hole = 1;
            }
          else
            // Clear to indicate that a normal write is needed.
            ip=0;
        }
      if(ip==0)
        {
         if(full_write (dest_desc, buf, n_read) < 0)
            {
             //error(0,errno, "%s", dst_path);
             return_val = -1;
             goto ret;
            }
         last_write_made_hole = 0;
        }
    }

  // If the file ends with a `hole', something needs to be written at
  // the end.  Otherwise the kernel would truncate the file at the end
  // of the last write operation.

  if(last_write_made_hole)
    {
#ifdef HAVE_FTRUNCATE
      // Write a null character and truncate it again.
     if(full_write (dest_desc, "", 1) < 0
          || ftruncate (dest_desc, n_read_total) < 0)
#else
      // Seek backwards one character and write a null.
     if(lseek (dest_desc, (off_t) -1, SEEK_CUR) < 0L
          || full_write (dest_desc, "", 1) < 0)
#endif
        {
         //error(0,errno, "%s", dst_path);
         return_val = -1;
        }
    }

ret:
  if(close(dest_desc)<0)
    {
     //error(0,errno,"%s",dst_path);
     return_val = -1;
    }
ret2:
  if(close(source_desc)<0)
    {
     //error(0,errno, "%s", src_path);
     return_val = -1;
    }

  return return_val;
 }
/*
int euidaccess ();

// For created inodes, a pointer in the search structure to this
// character identifies the inode as being new.
char new_file;

// The invocation name of this program.
char *program_name;

void initCopy(void)
 {
  // The key difference between -d (--no-dereference) and not is the version
  // of `stat' to call.

  if(flag_dereference) xstat = stat;
  else xstat = lstat;
 }

// Copy the file SRC_PATH to the file DST_PATH. The files may be of
// any type. NEW_DST should be nonzero if the file DST_PATH cannot
// exist because its parent directory was just created; NEW_DST should
// be zero if DST_PATH might already exist.  DEVICE is the device
// number of the parent directory, or 0 if the parent of this file is
// not known. ANCESTORS points to a linked, null terminated list of
// devices and inodes of parent directories of SRC_PATH.
// Return 0 if successful, 1 if an error occurs.

static int copy(const char *src_path, const char *dst_path, int new_dst,
 dev_t device,struct dir_list *ancestors)
 {
  struct stat src_sb;
  struct stat dst_sb;
  int src_mode;
  int src_type;
  char *earlier_file;
  char *dst_backup = NULL;
  int fix_mode = 0;

  initCopy();
  if((*xstat) (src_path, &src_sb))
    {
     //error (0, errno, "%s", src_path);
     return 1;
    }

  // Are we crossing a file system boundary?
  if (flag_one_file_system && device != 0 && device != src_sb.st_dev)
    return 0;

  // We wouldn't insert a node unless nlink > 1, except that we need to
  // find created files so as to not copy infinitely if a directory is
  // copied into itself.

  earlier_file=remember_copied(dst_path, src_sb.st_ino, src_sb.st_dev);

  // Did we just create this file?

  if (earlier_file == &new_file) return 0;

  src_mode = src_sb.st_mode;
  src_type = src_sb.st_mode;

  if(S_ISDIR(src_type) && !flag_recursive)
    {
     //error(0, 0, _("%s: omitting directory"), src_path);
     return 1;
    }

  if(!new_dst)
    {
     if((*xstat) (dst_path, &dst_sb))
        {
         if(errno != ENOENT)
            {
             //error (0, errno, "%s", dst_path);
             return 1;
            }
          else new_dst = 1;
        }
      else
        {
          // The file exists already.

          if(src_sb.st_ino == dst_sb.st_ino && src_sb.st_dev == dst_sb.st_dev)
            {
              if(flag_hard_link) return 0;

              //error(0,0,_("`%s' and `%s' are the same file"),src_path, dst_path);
              return 1;
            }

          if(!S_ISDIR(src_type))
            {
              if (S_ISDIR (dst_sb.st_mode))
                {
                 //error(0,0,_("%s: cannot overwrite directory with non-directory"),dst_path);
                 return 1;
                }

              if(flag_update && src_sb.st_mtime <= dst_sb.st_mtime) return 0;
            }

          if(!S_ISDIR (src_type) && !flag_force && flag_interactive)
            {
              if(euidaccess(dst_path,W_OK)!=0)
                 printf(_("%s: overwrite `%s', overriding mode %04o? "),
                   program_name,dst_path,(unsigned int)(dst_sb.st_mode & 07777));
              else
                 printf(_("%s: overwrite `%s'? "),program_name, dst_path);
              if(!yesno()) return 0;
            }

          if(backup_type!=none && !S_ISDIR (dst_sb.st_mode))
            {
             char *tmp_backup = find_backup_file_name (dst_path);
             if(tmp_backup==NULL) ;//error (1, 0, _("virtual memory exhausted"));

             // Detect (and fail) when creating the backup file would
             // destroy the source file.  Before, running the commands
             // cd /tmp; rm -f a a~; : > a; echo A > a~; cp -b -V simple a~ a
             // would leave two zero-length files: a and a~.
              if (STREQ (tmp_backup, src_path))
                {
                 //error (0, 0,_("backing up `%s' would destroy source;  `%s' not copied"),dst_path, src_path);
                 return 1;
                }
              dst_backup = (char *) alloca (strlen (tmp_backup) + 1);
              strcpy (dst_backup, tmp_backup);
              free (tmp_backup);
              if (rename (dst_path, dst_backup))
                {
                  if (errno != ENOENT)
                    {
                     //error (0, errno, _("cannot backup `%s'"), dst_path);
                     return 1;
                    }
                  else dst_backup = NULL;
                }
              new_dst = 1;
            }
          else
           if(flag_force)
            {
             if(S_ISDIR (dst_sb.st_mode))
                {
                 // Temporarily change mode to allow overwriting.
                  if (euidaccess (dst_path, W_OK | X_OK) != 0)
                    {
                      if (chmod (dst_path, 0700))
                        {
                         //error (0, errno, "%s", dst_path);
                         return 1;
                        }
                      else fix_mode = 1;
                    }
                }
              else
                {
                  if (unlink (dst_path) && errno != ENOENT)
                    {
                     //error(0,errno,_("cannot remove old link to `%s'"),dst_path);
                     return 1;
                    }
                  new_dst=1;
                }
            }
        }
    }

  // If the source is a directory, we don't always create the destination
  // directory.  So --verbose should not announce anything until we're
  // sure we'll create a directory.
  if (flag_verbose && !S_ISDIR (src_type))
    printf ("%s -> %s\n", src_path, dst_path);

  // Did we copy this inode somewhere else (in this command line argument)
  // and therefore this is a second hard link to the inode?

  if(!flag_dereference && src_sb.st_nlink > 1 && earlier_file)
    {
     if(link (earlier_file, dst_path))
        {
         //error (0, errno, "%s", dst_path);
         goto un_backup;
        }
      return 0;
    }

  //if(S_ISDIR(src_type))
  // {
  //  struct dir_list *dir;
  //
  //  //If this directory has been copied before during the
  //  //recursion, there is a symbolic link to an ancestor
  //  //directory of the symbolic link.  It is impossible to
  //  //continue to copy this, unless we've got an infinite disk.
  //
  //  if (is_ancestor (&src_sb, ancestors))
  //   {
  //    //error (0, 0, _("%s: cannot copy cyclic symbolic link"), src_path);
  //    goto un_backup;
  //   }
  //
  //  // Insert the current directory in the list of parents.
  //
  //  dir = (struct dir_list *) alloca (sizeof (struct dir_list));
  //  dir->parent = ancestors;
  //  dir->ino = src_sb.st_ino;
  //  dir->dev = src_sb.st_dev;
  //
  //  if (new_dst || !S_ISDIR (dst_sb.st_mode))
  //   {
  //    // Create the new directory writable and searchable, so
  //    // we can create new entries in it.
  //
  //    if(mkdir (dst_path, (src_mode & umask_kill) | 0700))
  //     {
  //      //error (0, errno, _("cannot create directory `%s'"), dst_path);
  //      goto un_backup;
  //     }
  //
  //    // Insert the created directory's inode and device
  //    // numbers into the search structure, so that we can
  //    // avoid copying it again.
  //
  //    if (remember_created (dst_path)) goto un_backup;
  //
  //    if (flag_verbose) printf ("%s -> %s\n", src_path, dst_path);
  //   }

  //  // Copy the contents of the directory.

  //  if(copy_dir (src_path, dst_path, new_dst, &src_sb, dir)) return 1;
  // }
#ifdef S_ISLNK
  //  else
   if (flag_symbolic_link)
    {
      if (*src_path == '/'
          || (!strncmp (dst_path, "./", 2) && strchr (dst_path + 2, '/') == 0)
          || strchr (dst_path, '/') == 0)
        {
          if (symlink (src_path, dst_path))
            {
             //error (0, errno, "%s", dst_path);
             goto un_backup;
            }

          return 0;
        }
      else
        {
          //error(0,0,_("%s: can make relative symbolic links only in current directory"),dst_path);
          goto un_backup;
        }
    }
#endif
  else
   if(flag_hard_link)
    {
     if(link (src_path, dst_path))
        {
         //error (0, errno, _("cannot create link `%s'"), dst_path);
         goto un_backup;
        }
      return 0;
    }
  else
   if(S_ISREG (src_type)|| (flag_copy_as_regular && !S_ISDIR (src_type)
#ifdef S_ISLNK
     && !S_ISLNK (src_type)
#endif
     ))
    {
     if(copyReg(src_path, dst_path))    goto un_backup;
    }
  else
#ifdef S_ISFIFO
  if(S_ISFIFO (src_type))
    {
      if (mkfifo (dst_path, src_mode & umask_kill))
        {
         //error (0, errno, _("cannot create fifo `%s'"), dst_path);
         goto un_backup;
        }
    }
  else
#endif
    if(S_ISBLK (src_type) || S_ISCHR (src_type)
#ifdef S_ISSOCK
        || S_ISSOCK (src_type)
#endif
        )
    {
      if (mknod (dst_path, src_mode & umask_kill, src_sb.st_rdev))
        {
         //error (0, errno, _("cannot create special file `%s'"), dst_path);
         goto un_backup;
        }
    }
  else
#ifdef S_ISLNK
  if (S_ISLNK (src_type))
    {
      char *link_val;
      int link_size;

      link_val = (char *) alloca (PATH_MAX + 2);
      link_size = readlink (src_path, link_val, PATH_MAX + 1);
      if (link_size < 0)
        {
         //error (0, errno, _("cannot read symbolic link `%s'"), src_path);
         goto un_backup;
        }
      link_val[link_size] = '\0';

      if (symlink (link_val, dst_path))
        {
         //error (0, errno, _("cannot create symbolic link `%s'"), dst_path);
         goto un_backup;
        }

      if(flag_preserve)
        {
          // Preserve the owner and group of the just-`copied'
          // symbolic link, if possible.
#ifdef HAVE_LCHOWN
          if (DO_CHOWN (lchown, dst_path, src_sb.st_uid, src_sb.st_gid))
            {
             //error (0, errno, "%s", dst_path);
             goto un_backup;
            }
#else
# ifdef ROOT_CHOWN_AFFECTS_SYMLINKS
          if (myeuid == 0)
            {
              if (DO_CHOWN (chown, dst_path, src_sb.st_uid, src_sb.st_gid))
                {
                  error (0, errno, "%s", dst_path);
                  goto un_backup;
                }
            }
          else
            {
             // FIXME: maybe give a diagnostic: you must be root
             // to preserve ownership and group of symlinks.
            }
# else
         // Can't preserve ownership of symlinks.
         // FIXME: maybe give a warning or even error for symlinks
         // in directories with the sticky bit set -- there, not
         // preserving owner/group is a potential security problem.
# endif
#endif
        }

      return 0;
    }
  else
#endif
    {
      error (0, 0, _("%s: unknown file type"), src_path);
      goto un_backup;
    }

  // Adjust the times (and if possible, ownership) for the copy.
  // chown turns off set[ug]id bits for non-root,
  // so do the chmod last.

  if (flag_preserve)
    {
      struct utimbuf utb;

      utb.actime = src_sb.st_atime;
      utb.modtime = src_sb.st_mtime;

      if (utime (dst_path, &utb))
        {
          error (0, errno, "%s", dst_path);
          return 1;
        }

      if (DO_CHOWN (chown, dst_path, src_sb.st_uid, src_sb.st_gid))
        {
          error (0, errno, "%s", dst_path);
          return 1;
        }
    }

  if ((flag_preserve || new_dst)
      && (flag_copy_as_regular || S_ISREG (src_type) || S_ISDIR (src_type)))
    {
      if (chmod (dst_path, src_mode & umask_kill))
        {
          error (0, errno, "%s", dst_path);
          return 1;
        }
    }
  else
   if (fix_mode)
    {
      // Reset the temporarily changed mode.
      if (chmod (dst_path, dst_sb.st_mode))
        {
          error (0, errno, "%s", dst_path);
          return 1;
        }
    }

  return 0;

un_backup:
  if (dst_backup)
    {
      if (rename (dst_backup, dst_path))
        error (0, errno, _("cannot un-backup `%s'"), dst_path);
    }
  return 1;
}
*/
/*
//--------------- fdisk ------------
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <setjmp.h>
#include <errno.h>
#include <endian.h>
#include <sys/stat.h>

#include <sys/ioctl.h>

#include <linux/types.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <scsi/scsi.h>
#include <linux/major.h>
#include <linux/fs.h>

#if 0
#include <linux/proc_fs.h>
#endif

#include "fdisk/fdisk.h"
#include "fdisk/fdisksunlabel.h"

// normally O_RDWR, -l option gives O_RDONLY
static int type_open = O_RDWR;

char    *disk_device = DEFAULT_DEVICE,  // hda, unless specified
        *line_ptr,                      // interactive input
        line_buffer[LINE_LENGTH],
        changed[MAXIMUM_PARTS],         // marks changed buffers
        buffer[SECTOR_SIZE],            // first four partitions
        *buffers[MAXIMUM_PARTS]         // pointers to buffers
                = {buffer, buffer, buffer, buffer};

int     fd,                             // the disk
        ext_index,                      // the prime extended partition
        listing = 0,                    // no aborts for fdisk -l
        dos_compatible_flag = ~0,
        partitions = 4;                 // maximum partition + 1

DEVINFO devInfo;

void initDevInfo(void)
 {
  //devInfo.heads;
  //devInfo.sectors;
  //devInfo.cylinders;
  devInfo.sector_offset=1;
  devInfo.displayFactor=1;      // in units/sector
  devInfo.unitFlag = 1;
  devInfo.fullBits = 0;         // 1024 cylinders in sectors
  devInfo.extended_offset=0;    // offset of link pointers
  for(int i=0;i<MAXIMUM_PARTS;i++) devInfo.offsets[i]=0;
 }

int other_endian=0;
int sun_label   =0;                     // if we're looking at sun disklabel
int scsi_disk   =0;
int floppy      =0;

struct  partition *part_table[MAXIMUM_PARTS]    // partitions
                = {offset(buffer, 0), offset(buffer, 1),
                offset(buffer, 2), offset(buffer, 3)},
        *ext_pointers[MAXIMUM_PARTS]            // link pointers
                = {NULL, NULL, NULL, NULL};

struct systypes sys_types[] = { // struct systypes in fdisk.h // bf
                {0, "Empty"},
                {1, "DOS 12-bit FAT"},
                {2, "XENIX root"},
                {3, "XENIX usr"},
                {4, "DOS 16-bit <32M"},
                {EXTENDED, "Extended"},
                {6, "DOS 16-bit >=32M"},
                {7, "OS/2 HPFS"},               // or QNX?
                {8, "AIX"},
                {9, "AIX bootable"},
                {10, "OS/2 Boot Manager"},
                {0x40, "Venix 80286"},
                {0x51, "Novell?"},
                {0x52, "Microport"},            // or CPM?
                {0x63, "GNU HURD"},             // or System V/386?
                {0x64, "Novell Netware 286"},
                {0x65, "Novell Netware 386"},
                {0x75, "PC/IX"},
                {0x80, "Old MINIX"},            // Minix 1.4a and earlier

                {LINUX_PARTITION, "Linux/MINIX"}, // Minix 1.4b and later
                {LINUX_SWAP, "Linux swap"},
                {LINUX_NATIVE, "Linux native"},

                {0x93, "Amoeba"},
                {0x94, "Amoeba BBT"},           // (bad block table)
                {0xa5, "BSD/386"},
                {0xb7, "BSDI fs"},
                {0xb8, "BSDI swap"},
                {0xc7, "Syrinx"},
                {0xdb, "CP/M"},                 // or Concurrent DOS?
                {0xe1, "DOS access"},
                {0xe3, "DOS R/O"},
                {0xf2, "DOS secondary"},
                {0xff, "BBT"}                   // (bad track table)
        };

#define SUNOS_SWAP 3
#define WHOLE_DISK 5
struct systypes sun_sys_types[] = { // struct systypes in fdisk.h
                {0, "Empty"},
                {1, "Boot"},
                {2, "SunOS root"},
                {SUNOS_SWAP, "SunOS swap"},
                {4, "SunOS usr"},
                {WHOLE_DISK, "Whole disk"},
                {6, "SunOS stand"},
                {7, "SunOS var"},
                {8, "SunOS home"},
                {LINUX_SWAP, "Linux swap"},
                {LINUX_NATIVE, "Linux native"},
        };

inline unsigned short __swap16(unsigned short x)
 {
  return (((__u16)(x) & 0xFF) << 8) | (((__u16)(x) & 0xFF00) >> 8);
 }

jmp_buf listingbuf;

inline __u32 __swap32(__u32 x)
 {
  return (((__u32)(x) & 0xFF) << 24) | (((__u32)(x) & 0xFF00) << 8) | (((__u32)(x) & 0xFF0000) >> 8) | (((__u32)(x) & 0xFF000000) >> 24);
 }

void fatal(enum failure why)
 {
  char  error[LINE_LENGTH],
  *message = error;

  if(listing)
   {
    close(fd);
    longjmp(listingbuf, 1);
   }

  switch (why)
   {
    case usage:
      message ="Usage: fdisk [-l] [-v] [-s /dev/hdxn] [/dev/hdx]\n";
      break;
    case unable_to_open:
      sprintf(error, "Unable to open %s\n", disk_device);
      break;
    case unable_to_read:
      sprintf(error, "Unable to read %s\n", disk_device);
      break;
    case unable_to_seek:
      sprintf(error, "Unable to seek on %s\n", disk_device);
      break;
    case unable_to_write:
      sprintf(error, "Unable to write %s\n", disk_device);
      break;
    case out_of_memory:
      message = "Unable to allocate any more memory\n";
      break;
    default: message = "Fatal error\n";
   }

  fputc('\n', stderr);
  fputs(message, stderr);
  exit(1);
 }

uint rounded(uint calcul, uint start)
 {
  uint i;
  if(!devInfo.fullBits) return calcul;
  while((i=calcul+devInfo.fullBits) <= start) calcul = i;
  return calcul;
 }

char *const str_units(void)
 {
  return devInfo.unitFlag ? "cylinder" : "sector";
 }

char *partition_type(unsigned char type)
 {
  int high;
  int low = 0, mid;
  uint tmp;
  struct systypes *types;

  if (!sun_label)
   {
    high = sizeof(sys_types) / sizeof(struct systypes);
    types = sys_types;
   }
  else
   {
    high = sizeof(sun_sys_types) / sizeof(struct systypes);
    types = sun_sys_types;
   }
  while (high >= low)
   {
    mid = (high + low) >> 1;
    if((tmp = types[mid].index) == type) return types[mid].name;
    else
     if(tmp < type) low = mid + 1;
     else high = mid - 1;
   }
  return NULL;
 }

static void long2chs(ulong ls, uint *c, uint *h, uint *s)
 {
  int   spc=devInfo.heads*devInfo.sectors;

  *c = ls / spc;
  ls = ls % spc;
  *h = ls / devInfo.sectors;
  *s = ls % devInfo.sectors + 1;        // sectors count from 1
 }

static void check_consistency(struct partition *p, int partition)
 {
  uint  pbc, pbh, pbs;          // physical beginning c, h, s
  uint  pec, peh, pes;          // physical ending c, h, s
  uint  lbc, lbh, lbs;          // logical beginning c, h, s
  uint  lec, leh, les;          // logical ending c, h, s

  if (!devInfo.heads || !devInfo.sectors || (partition >= 4))
    return;             // do not check extended partitions

  // physical beginning c, h, s
  pbc = p->cyl & 0xff | (p->sector << 2) & 0x300;
  pbh = p->head;
  pbs = p->sector & 0x3f;

  // physical ending c, h, s
  pec = p->end_cyl & 0xff | (p->end_sector << 2) & 0x300;
  peh = p->end_head;
  pes = p->end_sector & 0x3f;

  // compute logical beginning (c, h, s)
  long2chs(SWAP32(p->start_sect), &lbc, &lbh, &lbs);

  // compute logical ending (c, h, s)
  long2chs(SWAP32(p->start_sect) + SWAP32(p->nr_sects) - 1, &lec, &leh, &les);

  // Same physical / logical beginning?
  if(devInfo.cylinders <= 1024 && (pbc != lbc || pbh != lbh || pbs != lbs))
   {
    printf("Partition %d has different physical/logical beginnings (non-Linux?):\n",partition+1);
    printf("     phys=(%d, %d, %d) ", pbc, pbh, pbs);
    printf("logical=(%d, %d, %d)\n",lbc, lbh, lbs);
   }

  // Same physical / logical ending?
  if(devInfo.cylinders <= 1024 && (pec != lec || peh != leh || pes != les))
   {
    printf("Partition %d has different physical/logical endings:\n",partition+1);
    printf("     phys=(%d, %d, %d) ", pec, peh, pes);
    printf("logical=(%d, %d, %d)\n",lec, leh, les);
   }

  #if 0
  // Beginning on cylinder boundary?
  if(pbh != !pbc || pbs != 1)
   {
    printf("Partition %i does not start on cylinder boundary:\n",partition+1);
    printf("     phys=(%d, %d, %d) ", pbc, pbh, pbs);
    printf("should be (%d, %d, 1)\n", pbc, !pbc);
   }
  #endif

  // Ending on cylinder boundary?
  if (peh != (devInfo.heads - 1) || pes != devInfo.sectors)
   {
    printf("Partition %i does not end on cylinder boundary:\n",partition + 1);
    printf("     phys=(%d, %d, %d) ", pec,peh,pes);
    printf("should be (%d, %d, %d)\n",pec,devInfo.heads-1,devInfo.sectors);
   }
 }

int test_c(char **m, char *mesg)
 {
  int val = 0;
  if(!*m) fprintf(stderr, "You must set");
  else
   {
    fprintf(stderr, " %s", *m);
    val = 1;
   }
  *m=mesg;
  return val;
 }

int warn_geometry(void)
 {
  char *m = NULL;
  int prev = 0;
  if(!devInfo.heads) prev = test_c(&m, "heads");
  if(!devInfo.sectors) prev = test_c(&m, "sectors");
  if(!devInfo.cylinders) prev = test_c(&m, "cylinders");
  if(!m) return 0;
  fprintf(stderr,"%s%s.\nYou can do this from the extra functions menu.\n",
                prev ? " and " : " ", m);
  return 1;
 }

void update_units(void)
 {
  devInfo.fullBits=1024 * devInfo.heads*devInfo.sectors;
  if(devInfo.unitFlag && devInfo.fullBits) devInfo.displayFactor=devInfo.fullBits >> 10;
  else                       devInfo.displayFactor = 1;
 }

void warn_cylinders(void)
 {
  update_units();
  if(sun_label) return;
  if(devInfo.cylinders > 1024)
    fprintf(stderr,"The number of cylinders for this disk is "
                   "set to %d.\nThis is larger than 1024, and may cause "
                   "problems with:\n"
                   "1) software that runs at boot time (e.g., LILO)\n"
                   "2) booting and partitioning software form other OSs\n"
                   "   (e.g., DOS FDISK, OS/2 FDISK)\n",devInfo.cylinders);
 }

void clear_partition(struct partition *p)
 {
  p->boot_ind = 0;
  p->head = 0;
  p->sector = 0;
  p->cyl = 0;
  p->sys_ind = 0;
  p->end_head = 0;
  p->end_sector = 0;
  p->end_cyl = 0;
  p->start_sect = 0;
  p->nr_sects = 0;
 }

void readExtended(struct partition *p)
 {
  int i;
  struct partition *q;

  ext_pointers[ext_index] = part_table[ext_index];
  if (!p->start_sect)
   fprintf(stderr, "Bad offset in primary extended partition\n");
  else
   while (p->sys_ind == EXTENDED)
    {
     if (partitions >= MAXIMUM_PARTS)
      {
       fprintf(stderr,"Warning: deleting partitions after %d\n",partitions);
       clear_partition(ext_pointers[partitions - 1]);
       changed[partitions - 1] = 1;
       return;
      }
     devInfo.offsets[partitions]=devInfo.extended_offset + SWAP32(p->start_sect);
     if(!devInfo.extended_offset) devInfo.extended_offset = SWAP32(p->start_sect);
     if(ext2_llseek(fd, (ext2_loff_t)devInfo.offsets[partitions]
        * SECTOR_SIZE, SEEK_SET) < 0)
     fatal(unable_to_seek);
     if(!(buffers[partitions] = (char *) malloc(SECTOR_SIZE)))
       fatal(out_of_memory);
     if (SECTOR_SIZE != read(fd, buffers[partitions], SECTOR_SIZE))
       fatal(unable_to_read);
     part_table[partitions] = ext_pointers[partitions] = NULL;
     q = p = offset(buffers[partitions], 0);
     for(i = 0; i < 4; i++, p++)
      {
       if(p->sys_ind == EXTENDED)
         if (ext_pointers[partitions])
          fprintf(stderr, "Warning: extra link pointer in partition table "
                       "%d\n", partitions + 1);
         else ext_pointers[partitions] = p;
       else
        if(p->sys_ind)
         if(part_table[partitions])
          fprintf(stderr,"Warning: ignoring extra data in partition table %d\n",
                    partitions + 1);
         else part_table[partitions] = p;
      }
     if(!part_table[partitions])
      if(q!=ext_pointers[partitions]) part_table[partitions] = q;
      else part_table[partitions] = q + 1;
     if(!ext_pointers[partitions])
      if(q != part_table[partitions]) ext_pointers[partitions] = q;
      else ext_pointers[partitions] = q + 1;
     p=ext_pointers[partitions++];
   }
 }

int check_sun_label(void)
 {
  unsigned short *ush;
  int csum;

  if(sunlabel->magic!=SUN_LABEL_MAGIC && sunlabel->magic!=SUN_LABEL_MAGIC_SWAPPED)
   {
    sun_label=0;
    other_endian=0;
    return 0;
   }
  other_endian = (sunlabel->magic == SUN_LABEL_MAGIC_SWAPPED);
  ush = ((unsigned short *) (sunlabel + 1)) - 1;
  for(csum = 0; ush >= (unsigned short *)sunlabel;) csum ^= *ush--;
  if(csum)
    fprintf(stderr, "Detected sun disklabel with wrong checksum.\n"
                    "Probably you'll have to set all the values,\n"
                    "e.g. heads, sectors, cylinders and partitions\n"
                    "or force a fresh label (s command in main menu)\n");
  else
   {
    devInfo.heads = SSWAP16(sunlabel->ntrks);
    devInfo.cylinders = SSWAP16(sunlabel->ncyl);
    devInfo.sectors = SSWAP16(sunlabel->nsect);
   }
  update_units();
  sun_label = 1;
  partitions = 8;
  return 1;
 }

struct sun_predefined_drives
 {
  char *vendor;
  char *model;
  unsigned short sparecyl;
  unsigned short ncyl;
  unsigned short nacyl;
  unsigned short pcylcount;
  unsigned short ntrks;
  unsigned short nsect;
  unsigned short rspeed;
 } sun_drives[] = {
           {"Quantum","ProDrive 80S",1,832,2,834,6,34,3662},
           {"Quantum","ProDrive 105S",1,974,2,1019,6,35,3662},
           {"CDC","Wren IV 94171-344",3,1545,2,1549,9,46,3600},
           {"","SUN0104",1,974,2,1019,6,35,3662},
           {"","SUN0207",4,1254,2,1272,9,36,3600},
           {"","SUN0327",3,1545,2,1549,9,46,3600},
           {"","SUN0340",0,1538,2,1544,6,72,4200},
           {"","SUN0424",2,1151,2,2500,9,80,4400},
           {"","SUN0535",0,1866,2,2500,7,80,5400},
           {"","SUN0669",5,1614,2,1632,15,54,3600},
           {"","SUN1.0G",5,1703,2,1931,15,80,3597},
           {"","SUN1.05",0,2036,2,2038,14,72,5400},
           {"","SUN1.3G",6,1965,2,3500,17,80,5400},
           {"","SUN2.1G",0,2733,2,3500,19,80,5400},
           {"IOMEGA","Jaz",0,1019,2,1021,64,32,5394},
                  };

int getBoot(int action,char *device);

void createDosLabel(char *device)
 {
  int i;
  fprintf(stderr,"Building a new DOS disklabel. Changes will remain in memory only,\n"
                 "until you decide to write them. After that, of course, the previous\n"
                 "content won't be recoverable.\n\n");
  *table_check(buffer) = SWAP16(PART_TABLE_FLAG);
  for(i=0; i < 4; i++) clear_partition(part_table[i]);
  for(i=1; i < MAXIMUM_PARTS; i++) changed[i] = 0;
  changed[0] = 1;
  getBoot(3,device);
 }

int getBoot(int action,char *device)
 {
  int i;
  struct hd_geometry geometry;
  struct stat bootstat;

  initDevInfo();

  partitions = 4;
  if(action != 3)
   {
    if((fd=open(disk_device,type_open)) < 0)
     {
      if((fd=open(disk_device,O_RDONLY)) < 0) fatal(unable_to_open);
      else printf("You will not be able to write the partition table.\n");
     }
    if(fstat(fd,&bootstat)<0)
     {
      scsi_disk=0;
      floppy=0;
     }
    else
     if(S_ISBLK(bootstat.st_mode) && (bootstat.st_rdev >> 8) == IDE0_MAJOR
       || (bootstat.st_rdev >> 8) == IDE1_MAJOR)
      {
       scsi_disk = 0;
       floppy = 0;
      }
     else
      if(S_ISBLK(bootstat.st_mode) && (bootstat.st_rdev >> 8) == 2)
       {
        scsi_disk = 0;
        floppy = 1;
       }
      else
       {
        scsi_disk = 1;
        floppy = 0;
       }
    if(SECTOR_SIZE != read(fd, buffer, SECTOR_SIZE)) fatal(unable_to_read);
#ifdef HDIO_REQ
    if(!ioctl(fd, HDIO_REQ, &geometry))
#else
    if(!ioctl(fd, HDIO_GETGEO, &geometry))
#endif
     {
      devInfo.heads = geometry.heads;
      devInfo.sectors = geometry.sectors;
      devInfo.cylinders = geometry.cylinders;
      if(dos_compatible_flag) devInfo.sector_offset = devInfo.sectors;

      warn_cylinders();
     }
    update_units();
   }
  if(check_sun_label()) return 0;

  if(SWAP16(*table_check(buffer)) != PART_TABLE_FLAG)
   {
    switch (action)
     {
      case 1: return -1;
      case 2: fprintf(stderr, "Disk doesn't contain any valid partition table\n"); return -1;
      case 3: fprintf(stderr, "Internal error\n"); exit(1);
      default: break;
     }
    fprintf(stderr,"Device contains neither a valid DOS partition table, nor Sun disklabel\n");
#ifdef __sparc__
    create_sunlabel();
    return;
#else
    createDosLabel(device);
    return 0; //?
#endif
   }
  warn_cylinders();
  warn_geometry();

  for(i=0;i<4;i++)
   if(part_table[i]->sys_ind==EXTENDED)
    if(partitions != 4)
        fprintf(stderr, "Ignoring extra extended partition %d\n", i + 1);
    else readExtended(part_table[ext_index = i]);

  for(i=3;i<partitions; i++)
   if(SWAP16(*table_check(buffers[i])) != PART_TABLE_FLAG)
    {
     fprintf(stderr, "Warning: invalid flag %04x of parti"
                       "tion table %d will be corrected by w(rite)\n",
               SWAP16(*table_check(buffers[i])), i + 1);
     changed[i] = 1;
    }
  return 0;
 }

void list_table(int xtra)
 {
  struct partition *p;
  char *type;
  int i,w;

  char *diskDevice=DEFAULT_DEVICE;
  if((fd = open(DEFAULT_DEVICE, O_RDWR)) < 0) diskDevice = ALTERNATE_DEVICE;
  else close(fd);

  //  printf("Using %s as default device\n", diskDevice);
  getBoot(0,diskDevice);

  w=strlen(diskDevice);
  if(w<5) w=5;
  if(!sun_label)
   {
    printf("\nDisk %s: %d heads, %d sectors, %d cylinders\nUnits = "
              "%ss of %d * 512 bytes\n\n",
               diskDevice,devInfo.heads,devInfo.sectors,devInfo.cylinders,str_units(),devInfo.displayFactor);
    printf("%*s Boot   Begin    Start      End   Blocks   Id  System\n",
                        w+1,"Device");
    for(i=0;i<partitions;i++)
     if((p=part_table[i])->sys_ind)
      {
       printf("%*s%-2d  %c%9d%9d%9d%9d%c  %2x  %s\n", w,
// device         diskDevice, i + 1,
// boot flag      !p->boot_ind ? ' ' : p->boot_ind == ACTIVE_FLAG ? '*' : '?',
// begin          cround(rounded(calculate(p->head,p->sector,p->cyl),
                        SWAP32(p->start_sect) +devInfo.offsets[i])),
// start          cround(SWAP32(p->start_sect)+devInfo.offsets[i]),
/// end           cround(SWAP32(p->start_sect)+devInfo.offsets[i] + SWAP32(p->nr_sects)
                  - (p->nr_sects ? 1: 0)),
//odd flag on end SWAP32(p->nr_sects) / 2, SWAP32(p->nr_sects) & 1 ? '+' : ' ',
// type id        p->sys_ind,
// type name      (type = partition_type(p->sys_ind)) ? type : "Unknown");
       check_consistency(p, i);
      }
    }
   else
    {
     if(xtra)
       printf("\nDisk %s (Sun disk label): %d heads, %d sectors, %d rpm\n"
         "%d cylinders, %d alternate cylinders, %d physical cylinders\n"
         "%d extra sects/cyl, interleave %d:1\n"
         "%s\n"
         "Units = %ss of %d * 512 bytes\n\n", diskDevice,devInfo.heads,devInfo.sectors, SSWAP16(sunlabel->rspeed),
         devInfo.cylinders, SSWAP16(sunlabel->nacyl), SSWAP16(sunlabel->pcylcount),
         SSWAP16(sunlabel->sparecyl), SSWAP16(sunlabel->ilfact),
         (char *)sunlabel,
         str_units(),devInfo.displayFactor);
     else
      printf("\nDisk %s (Sun disk label): %d heads, %d sectors, %d cylinders\n"
         "Units = %ss of %d * 512 bytes\n\n", diskDevice,devInfo.heads,devInfo.sectors,
         devInfo.cylinders, str_units(),devInfo.displayFactor);
      printf("%*s Flag   Begin    Start      End   Blocks   Id  System\n",w + 1, "Device");
      for(i = 0 ; i < partitions; i++)
       if(sunlabel->partitions[i].num_sectors)
        {
         __u32 start = SSWAP32(sunlabel->partitions[i].start_cylinder) * devInfo.heads * devInfo.sectors;
         __u32 len = SSWAP32(sunlabel->partitions[i].num_sectors);
         printf("%*s%-2d %c%c%9d%9d%9d%9d%c  %2x  %s\n", w,
/ device          /    diskDevice, i + 1,
/ flags           /    (sunlabel->infos[i].flags & 0x01) ? 'u' : ' ',
                       (sunlabel->infos[i].flags & 0x10) ? 'r' : ' ',
/ begin           /    scround(start),
/ start           /    scround(start),
/ end             /    scround(start+len),
/ odd flag on end /    len / 2, len & 1 ? '+' : ' ',
/ type id         /    sunlabel->infos[i].id,
/ type name       /    (type = partition_type(sunlabel->infos[i].id)) ?
                      type : "Unknown");
        }
     }
 }
//----------- fdisk -----------------
*/
//-------------- syslinks -----------
#include <unistd.h>
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>//malloc.h>
#include <string.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <stddef.h>
#include <errno.h>

#ifndef S_ISLNK
#define S_ISLNK(mode) (((mode) & (_S_IFMT)) == (_S_IFLNK))
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define SL_VERBOSE   0x0001
#define SL_FIX_LINKS 0x0002
#define SL_DELETE    0x0008
#define SL_SHORTEN   0x0010
#define SL_TESTING   0x0020

//static int sl_regim;

// tidypath removes excess slashes and "." references from a path string

static int substr(char *s,char *old,char *news)
 {
  char *tmp=NULL;
  int oldlen=strlen(old), newlen = 0;

  if (NULL==strstr(s, old)) return 0;

  if(news)newlen=strlen(news);

  if(newlen>oldlen)
   {
    if((tmp=(char*)malloc(strlen(s)))==NULL)
     {
      fprintf(stderr, "no memory\n");
      return 1; // exit (1);
     }
   }

  while(NULL != (s = strstr(s, old)))
   {
    char *p,*old_s=s;

    if(news)
     {
      if(newlen > oldlen)
      old_s = strcpy(tmp, s);
      p=news;
      while (*p) *s++ = *p++;
     }
    p=old_s + oldlen;
    while ((*s++ = *p++));
   }
  if(tmp) free(tmp);
  return 1;
 }

int TidyPath (char *path)
 {
  int tidied = 0;
  char *s, *p;

  strcat(path,"/");     // tmp trailing slash simplifies things
  while (substr(path, "/./", "/")) tidied = 1;
  while (substr(path, "//", "/"))  tidied = 1;

  while (NULL != (p = strstr(path,"/../")))
   {
    for (s = p+3; p != path && *--p != '/';);
    if (*p != '/') break;
    if (p == path) ++p;
    while ((*p++ = *s++));
    tidied = 1;
   }
  if (!*path) strcpy(path,"/");
  p = path + strlen(path) - 1;
  if (p != path && *p == '/')
    *p-- = '\0';        /* remove tmp trailing slash */
  while (p != path && *p == '/')
   {    /* remove any others */
    *p-- = '\0';
    tidied = 1;
   }
  while (!strncmp(path,"./",2))
   {
    for (p = path, s = path+2; (*p++ = *s++););
    tidied = 1;
   }
  return tidied;
 }
/*
static int ShortenPath (char *path, char *abspath)
 {
  static char dir[PATH_MAX];
  int shortened = 0;
  char *p;

  // get rid of unnecessary "../dir" sequences
  while (abspath && strlen(abspath) > 1 && (p = strstr(path,"../")))
   {
    // find innermost occurance of "../dir", and save "dir"
    int slashes = 2;
    char *a, *s, *d = dir;
    while ((s = strstr(p+3, "../")))
     {
      ++slashes;
      p = s;
     }
    s = p+3;
    *d++ = '/';
    while (*s && *s != '/') *d++ = *s++;
    *d++ = '/';
    *d = '\0';
    if (!strcmp(dir,"//")) break;
    // note: p still points at ../dir
    if (*s != '/' || !*++s) break;
    a = abspath + strlen(abspath) - 1;
    while (slashes-- > 0)
     {
      if (a <= abspath)
      goto ughh;
      while (*--a != '/')
        if (a <= abspath) goto ughh;
     }
    if(strncmp(dir, a, strlen(dir))) break;
    while ((*p++ = *s++)); // delete the ../dir
    shortened = 1;
   }
ughh:
  return shortened;
 }
*/
char* CheckSymlink(char *path,dev_t my_dev)
 {
  static char slpath[PATH_MAX],abspath[PATH_MAX]; //,news[PATH_MAX]
  char *p;//, *lp, *tail, *msg,*np;
  struct stat stbuf;
  int c;//, fix_abs = 0, fix_messy = 0, fix_long = 0;

  if((c=readlink(path,slpath,sizeof(slpath))) == -1)
   {
    perror(path);
    return NULL;
   }
  slpath[c]='\0';       // readlink does not null terminate it

  // construct the absolute address of the link
  abspath[0]='\0';
  if(slpath[0] != '/')
   {
    strcat(abspath,path);
    if(((c=strlen(abspath)) > 0) && (abspath[c-1] == '/'))
        abspath[c-1] = '\0';
    if((p=strrchr(abspath,'/')) != NULL) *p='\0';
    strcat(abspath,"/");
   }
  strcat(abspath,slpath);
  TidyPath(abspath);

  // check for various things
  if(stat(abspath,&stbuf)==-1)
   {
    //printf("dangling: %s -> %s\n",path,slpath);
    //if(sl_regim&&SL_DELETE)
    // {
    //  if(unlink (path))       perror(path);
    //  else printf("deleted:  %s -> %s\n", path, lpath);
    // }
    return NULL;
   }
  //if(stbuf.st_dev != my_dev)  msg = "other_fs:";
  //else
  // if(slpath[0] == '/')     {  msg = "absolute:"; fix_abs = 1;  }
  // else
  //  if(sl_regim&&SL_VERBOSE) msg = "relative:";
  //  else                     msg = NULL;
  //  fix_messy =TidyPath(strcpy(news,lpath));
  //  if(sl_regim&&SL_SHORTEN) fix_long=ShortenPath(news,path);
  //  if(!fix_abs)
  //   {
  //    if (fix_messy) msg = "messy:   ";
  //    else
  //      if (fix_long) msg = "lengthy: ";
  //   }
    //if(msg != NULL) printf("%s %s -> %s\n", msg, path, lpath);
    //if(!(sl_regim&&SL_FIX_LINKS || sl_regim&&SL_TESTING) || !(fix_messy || fix_abs || fix_long))
    //  return;

    //if(fix_abs)
    // {
    //  // convert an absolute link to relative:
    //  // point tail at first part of lpath that differs from path
    //  // point p    at first part of path  that differs from lpath
    //  TidyPath(slpath);
    //  tail=lp=lpath;
    //  p=path;
    //   {
    //    if (*lp++ == '/')
    //     {
    //      tail = lp;
    //      while (*++p == '/');
    //     }
    //   }

        // now create new, with "../"s followed by tail
    //  np = news;
    //  while (*p)
    //   {
    //    if (*p++ == '/')
    //     {
    //      *np++ = '.';
    //      *np++ = '.';
    //      *np++ = '/';
    //      while (*p == '/') ++p;
    //     }
    //   }
    //  strcpy (np, tail);
    //  (void) TidyPath(news);
    // }
    //if(!sl_regim&&SL_TESTING)
    // {
    //  if (unlink (path))
    //   {
    //  perror(path);
    //  return;
    //   }
    //  if(symlink(news, path))
    //   {
    //  perror(path);
    //  return;
    //   }
    // }
    //printf("changed:  %s -> %s\n", path, news);
    return abspath;
 }

