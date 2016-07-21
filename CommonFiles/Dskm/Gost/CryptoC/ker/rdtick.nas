;/*RDTSC**********************************************************
; DWORD rdtick( void )
; (Returns Pentium TimeStamp Counter's low DWORD.)
;*****************************************************************/
%ifdef  __ELF__
global           rdtick:function
%else  ;__ELF__
global          _rdtick
%endif ;__ELF__

%ifdef  __COFF__
segment         .text   class=CODE use32
%endif ;__COFF__
%ifdef  __ELF__
segment         .text   class=CODE use32
%endif ;__ELF__
%ifdef  __OBJ32__
segment         _TEXT   para public class=CODE use32
%endif ;__OBJ32__

%ifdef  __ELF__
rdtick:
%else  ;__ELF__
_rdtick:
%endif ;__ELF__
                push    edx

                rdtsc

                pop     edx
                retn

                end
