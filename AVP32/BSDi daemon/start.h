#ifndef __AVP_H
#define __AVP_H
/*
//#define SYSLOG_NAMES
#include <sys/syslog.h>

#include <utmp.h>
#include <sys/param.h>
#include <netinet/in.h>

#define MAXUNAMES       20      // maximum number of user names
#define MAXSVLINE       120             // maximum saved line length

// This structure represents the files that will have log copies printed.
struct filed
 {
  struct        filed *f_next;          // next in linked list
  short f_type;                 // entry type, see below
  short f_file;                 // file descriptor
  time_t        f_time;                 // time this was last written
  u_char        f_pmask[LOG_NFACILITIES+1];     // priority mask
  u_char        f_pcmp[LOG_NFACILITIES+1];      // compare priority
#define PRI_LT  0x1
#define PRI_EQ  0x2
#define PRI_GT  0x4
  char  *f_program;             // program this applies to
  union
   {
    char        f_uname[MAXUNAMES][UT_NAMESIZE+1];
    struct
     {
      char      f_hname[MAXHOSTNAMELEN+1];
      struct sockaddr_in        f_addr;
     } f_forw;          // forwarding address
    char        f_fname[MAXPATHLEN];
    struct
     {
      char      f_pname[MAXPATHLEN];
      pid_t     f_pid;
     } f_pipe;
   } f_un;
  char  f_prevline[MAXSVLINE];          // last message logged
  char  f_lasttime[16];                 // time of last occurrence
  char  f_prevhost[MAXHOSTNAMELEN+1];   // host from which recd.
  int   f_prevpri;                      // pri of f_prevline
  int   f_prevlen;                      // length of f_prevline
  int   f_prevcount;                    // repetition cnt of prevline
  int   f_repeatcount;                  // number of "repeated" msgs
 };

*/
// values for f_type
#define F_UNUSED        0               // unused entry
#define F_FILE          1               // regular file
#define F_TTY           2               // terminal
#define F_CONSOLE       3               // console terminal
#define F_FORW          4               // remote machine
#define F_USERS         5               // list of users
#define F_WALL          6               // everyone logged on
#define F_PIPE          7               // pipe to program

//----------- -------------
extern BOOL blDaemon;

#define FL_LOAD_INI   0x01
#define FL_LOAD_KEY   0x02
#define FL_LOAD_PRF   0x04
#define FL_LOAD_HLP   0x08   // 1 - out help screen
#define FL_LOAD_BASE  0x10
#define FL_UNSIGN_EXE 0x20
extern ULONG fl_load;

// errorlevel
#define ERR_LEVEL_SEM_INIT	 -10 //A fatal error arising during the semaphore's init sequence
#define EXIT_OK		           0
#define ERR_LEVEL_NO_VIRUS         0 // вирусов не обнаружено
#define ERR_LEVEL_NOT_COMPLETED    1 // сканирование не закончено
#define ERR_LEVEL_WARNINGS         3 // найдены подозрительные объекты
#define ERR_LEVEL_VIRUS            4 // обнаружен вирус
#define ERR_LEVEL_ALL_DISINFECTED  5 // все обнаруженные вирусы удалены
#define ERR_LEVEL_AVP_DAMAGE       7 // файл AVP.EXE поврежден
#define ERR_LEVEL_AVP_CRASH       10 // внутренняя ошибка программы AVP.EXE

struct _cmdline
 {
  BYTE start_minumize,
       skip_mbr,
       skip_boot,
       skip_dlg,
       remote,
       abort_disable;
  char *ListFile;
 };
extern struct  _cmdline cmdline;

extern char *start_path;//[CCHMAXPATH];      // Полный путь, откуда был запущен AVP
extern char prg_path[CCHMAXPATH];        // Полный путь, где находится AVP

char *GNAM(char *_ttop);
#endif