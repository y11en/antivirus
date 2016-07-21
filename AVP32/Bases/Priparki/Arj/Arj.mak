#////////////////////////////////////////////////////////////////////
#//
#//  ARJ.MAK
#//  AVP 3.0 Database internal O16 O32 makefile for ARJ extractor
#//  Project AVP
#//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
#//  Copyright (c) Kaspersky Labs.
#//
#////////////////////////////////////////////////////////////////////

TargetName=ARJ
DEP=arj.c io.c decode.c arj.h unarj.h seek.h

CommonFiles=..\..\..\..\CommonFiles
!INCLUDE $(CommonFiles)\Bases\obj.mak 