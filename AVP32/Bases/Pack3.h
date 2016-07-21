////////////////////////////////////////////////////////////////////
//
//  PACK3.H
//  Engine Dynamic Linkage API, for decode() proc in pack/extr records
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __PACK3_H
#define __PACK3_H

#include "dlink3.h"

#ifndef SEEK_SET
#	define SEEK_SET 0
#	define SEEK_CUR 1
#	define SEEK_END 2
#endif

#define P_OK	0
#define P_ERR	1
#define P_END	2
#define P_DISK	3
#define P_VOL	4

#ifdef	_WIN32
#define AO	void*Area_Owner,
#define AO_	void*Area_Owner
#else
#define AO
#define AO_
#endif

kernel int	NEAR	P_N;
	//parameter for decode proc:
	//if P_N==0, decode() return P_OK if packed, P_END othervise.
	//if P_N==-1 decode() must free all allocated resources.(New/Delete)
	//if P_N>0 , decode() returns:
	//	P_OK if inpacked successfully,
	//	P_END if this member not exist
	//	P_ERR if error in format or algoritms
	//	P_DISK if I/O error
	//	P_VOL if next volume present

kernel BYTE NEAR		Zip_Name[NAME_SIZE];

kernel BYTE	FAR* FAR	New(long size);						//memory management
kernel void FAR			Delete(BYTE FAR* ptr);				//memory management

#ifdef	_WIN32

kernel int		_Copy_P_File(AO_ );						//copy handles
kernel long 	_LSeek(AO long lOff, int from);
kernel int		_P_Read(AO char NEAR* buff,WORD size);  //read from packed handle
kernel long		_P_Seek(AO long offset,int from);       //seek on paked file
kernel int		_V_Read(AO char NEAR* buff,WORD size);  //read from tmp handle
kernel int		_V_Write(AO char NEAR* buff,WORD size); //write to tmp handle
kernel long		_V_Seek(AO long offset,int from);       //seek on tmp file
kernel long		_S_Save(AO char FAR* buff, long size);  //save solid data
kernel long		_S_Load(AO char FAR* buff);             //load solid data
kernel int 		_CheckExt(AO_);							//access trough File_Name link
kernel int 		_NextVolume(AO_);						//open next arch volume, does not dispatches file

#endif

// in next AVP version fix differences with other function - 0,1, etc

kern16 int	FAR	Copy_P_File();           			 //copy handles; returns P_OK, P_ERR
kern16 long	FAR	LSeek(long Offset,int From);
kern16 int	FAR	P_Read(char NEAR* buff,WORD size);  //read from packed handle, returns as Read
kern16 long	FAR	P_Seek(long offset,int from);       //seek on paked file, returns as Seek
kern16 int	FAR	V_Read(char NEAR* buff,WORD size);  //read from tmp handle, returns as Read
kern16 int	FAR	V_Write(char NEAR* buff,WORD size); //write to tmp handle, returns as Write
kern16 long	FAR	V_Seek(long offset,int from);       //seek on tmp file, returns as Seek
kern16 long	FAR	S_Save(char FAR* buff, long size);  //save solid data, returns nBytes, 0 - err
													// limit for DOS - 20 sequented saves
kern16 long	FAR	S_Load(char FAR* buff);             //load solid data, returns nBytes, 0 - err
kern16 int 	FAR	CheckExt();				//access trough File_Name link, ret 0 - false, 1 - true
kernel int 	FAR	NextVolume();			//open next arch volume, does not dispatches file, ret 0 - err, 1 - ok

#ifndef	_WIN32

kern16 void	FAR	setDgroup(void);					 //set DS back to Data

#endif

#undef	AO
#undef	AO_

extern WORD NEAR		AFlags;		// Archive flags by name of Vad (vAd)
									// bit 0 set - solid

/* Tech notes
   ----------

V_Handle is temporary unpack file by the name of Vadim Bogdanov.

        if RFlags.Unpack {
	        is created by Kernel before ScanStart,
		closed and deleted after ScanEnd
		Seek0,Write0 after Unpacking
	}

P_Handle is SourcePackedFile or SourceZipFile

	if PackedFile or ZipFile	{
		P_Handle=Handle
	}

        if PackedFile: unpack,close
        if ZipFile:    unpack,save P_Handle in stack without Close
                       closed after processing whole Zip

S_Handle is SolidArchiveData

        if RFlags.Unzip {
	        is created by Kernel before ScanStart,
		closed and deleted after ScanEnd
		Seek0,Write0 after Unzip if ZipLevel=0
	}

Handle is destination handle
       it is created if PackedFile or ZipFile in ExclusiveMode

*/

#endif//__PACK3_H