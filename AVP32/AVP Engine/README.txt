Short description of the AVPEngine Package.
-------------------------------------------
Alexey de Mont de Rique [mailto:graf@avp.ru]


Package consists of 3 versions of AVPEngine: DLL,SYS and VXD.

Up-to date bases you can download from
ftp://ftp.avp.ru/updates.

You should put all these files into startup folder
to test or use engine.


Contents:

  WIN32         working folder;
        there are necessary header files
        to access AVPEngine functions:

        AVPIOCTL.H - DeviceIoControl API to VXD and SYS versions of Engine.
        AVP_DLL.H  - AVPBASE.DLL exported functions
        AVP_MSG.H  - Callback interface spec.
        SCANOBJ.H  - Structure for Scan request
        TYPES  .H  - Specification of object types
        RETFLAGS.H - Specification of Return flags.

        and subfolders:

        REL_DLL (DLL version of AVPEngine)
                AVPBASE .DLL  - AVPEngine DLL
                AVPBASE .LIB  - AVPEngine Library to link with your code
                AVP_IO32.DLL  - file with 95 platform specific code
                AVP_IONT.DLL  - file with NT platform specific code
                AVP_IO.VXD    - Low level IO driver

        REL_SYS (SYS version of AVPEngine)
                FSAVP350.SYS  - AVPEngine SYS for NT 3.50 and GK 1
                FSAVP351.SYS  - AVPEngine SYS for NT 3.51 and GK 1
                FSAVP400.SYS  - AVPEngine SYS for NT 4.00 and GK 1
                FSAVP402.SYS  - AVPEngine SYS for NT 4.00 and GK 2.0
                FSAVP40G.SYS  - AVPEngine SYS for NT 4.00

        REL_VXD (VXD version of AVPEngine)
                AVP95.VXD     - AVPEngine VXD
                AVP_IO.VXD    - Low level IO driver


        SAMPDLL
                folder with MSVC++ 4.2 project.
                This is example of using DLL AVPEngine.
                Files in Project:

                AVPsamp.cpp - Nothing important, MSVC stuff.
                Avpsdlg.cpp - File that implement
                                                main dialog handlers,
                                                scan thread organization and
                                                AVP_CALLBACK Message dispatch.

                Scandir.cpp - The ditectory tree findfirst/findnext.
                                                Fill AVPScanObject structure fields
                                                and call AVP_ProcessObject().
                                                All information about object processing
                                                returns only trough AVP_CALLBACK Message.

                StdAfx.cpp  - MSVC stuff.

                *.h     - header files.
                ..\release\AvpBase.lib - Import library for AVPEngine.


                Programmers can look sources.
                I'll answer any questions.

        SAMPVXD
                folder with MSVC++ 4.2 project.
                This is example of using VXD AVPEngine.


