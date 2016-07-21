----------
19.05.2001?

Added signature checking for all KAVSC modules. Some messages (mostly 
critical error messages) become localizable too.

----------
22.05.2001

Added handling of VBScript commands: Execute, ExecuteGlobal, Eval
Added handling of JScript command: eval

These commands execute string parameter as script. KAVSC examines script
code passed as command parameter, then this block together with previous 
blocks (for each scripting engine KAVSC stores a queue of code blocks).

----------
23.05.2001

Now, if script checker blocks some script, any adding code block for this 
engine instance will be bloked too.

If KAVSC blocked code during execution of command Execute, ExecuteGlobal or 
Eval (code was a parameter of that command), KAVSC stops executing of script 
(by generating exception and suppressing error message).

----------
24.05.2001

Added support for encoded scripts. If script language associated with engine 
(in system registry) contain ".encode" at the end, then KAVSC decodes each 
script code portion before checking.

----------
26.05.2001

Bugfix: "Execution of Windows Script Host failed. (Unspecified error)" error 
message after blocking some VBS files.

----------
09.06.2001

Bugfix: REGSVR32.EXE hangs in Windows 95
Bugfix: Sometimes KAVSC reports "Cannot find library 'concl.dll'", now 
        KAVSC will load all libraries from the directory where main module 
        (avpscrch.dll) exists.
Bugfix: Memory leaks during web serving
Bugfix: Memory leaks after Encoder run

Found that KAVSC cannot handle 'eval' function in JavaScript v1.0 (distributed 
with IE 3.0). Also found that statements 'Execute', 'ExecuteGlobal' and 
function 'Eval' of VBScript are present only from verion 5.0. Because of that
message "KAV Script Checker cannot find handlers for follows methods..." has 
been disabled.

A little bit changed format of temporary file that AVPM scans for viruses.

All test files are rewritten to work correctly with different versions of 
script engines.

ATTENTION! EICAR script text has been changed to avoid JavaScript error 
messages (added one more single quote char at the end). 
Now EICAR script text is:
  'X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*'

KAVSC has been tested under follows platforms:
    Windows 95 OSR2 with IE 3.0 (scripts version 1.0)
    Windows 98 with IE 4.0 (scripts version 3.0)
    Windows 2000 with IE 5.5 SP1 (scripts version 5.5)

----------
19.09.2001

Bugfix: Crash on some scripts under WinNT.
Bugfix: Crash if 'eval' parameter is NULL.

String tables in localization DLL combined altogether. Theoretically system
will use string table appropriate to current system locale (this need to be
tested!).

Added version info into all modules. Product version: 1.0.0.6
File versions:
  AVPSCRCH.DLL    1.0.2.0
  CONCL.DLL       1.0.0.2
  DINFO.DLL       1.0.0.2
  LOCSCRCH.DLL    1.0.0.2

----------
20.09.2001

System function LoadString doesn't checks current locale when gets string by
id. So, localization routine has been modified to work properly. Now it tries
to load localized string appropriate to current user locale first, if failed
it uses standard LoadString function. This method gives possibility to put
many localizations into one library.

Resources language in all libraries changed to appropriate - US English.

File versions:
  AVPSCRCH.DLL    1.0.2.2
  CONCL.DLL       1.0.0.3
  DINFO.DLL       1.0.0.3
  LOCSCRCH.DLL    1.0.0.3

