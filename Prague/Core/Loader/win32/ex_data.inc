;typedef struct ExceptionData { /*
  ExceptionData STRUCT
    reg_ptr   DWORD              ?  ; */ struct tag_ExceptionReg_i* reg_ptr;      /*
    next      DWORD              ?  ; */ struct ExceptionData*      next;         /*
    happened  DWORD              ?  ; */ int                        happened;     /*
    frame     DWORD              ?  ; */ void*                      frame;        /*
    ep        DWORD       2  DUP(?) ; */ EXCEPTION_POINTERS         ep;           /*
    er        BYTE       80  DUP(?) ; */ EXCEPTION_RECORD           er;           /*
    ctx       BYTE      716  DUP(?) ; */ CONTEXT                    ctx;          /*
    ret_addr  DWORD              ?  ; */ DWORD                      ret_addr;     /*
    jump      BYTE       64  DUP(?) ; */ jmp_buf                    jump;         /*
  ExceptionData ENDS ; */ };
