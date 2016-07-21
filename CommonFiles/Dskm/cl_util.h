#ifndef __CL_UTIL_H__
#define __CL_UTIL_H__

#include "../Stuff/TEXT2BIN.h"
#include "dskmi.h"

#if defined (_WIN32)
#define SECURE_CALL
#endif //_WIN32

#define SIGN_TXT_BEG        ( 4 )
#define SIGN_TXT_END        ( 2 )

#define DSKM_SIGN_TAG_CRIPT    0xf0dfed41   /*is a crc32 for "Kaspersky Lab DSKM 2005"*/

#define DSKM_SIGN_CHAR_0       ((unsigned char)0x0d)
#define DSKM_SIGN_CHAR_1       ((unsigned char)0x0a)
#define DSKM_SIGN_CHAR_2       ((unsigned char)';')
#define DSKM_SIGN_CHAR_3       ((unsigned char)':')
#define DSKM_SIGN_DWORD        ((unsigned long)( (DSKM_SIGN_CHAR_0<<0) | (DSKM_SIGN_CHAR_1<<8) | (DSKM_SIGN_CHAR_2<<16) | (DSKM_SIGN_CHAR_3<<24) ))

#define DSKM_SIGN_CHAR_0_CR    0x4c						//DSKM_SIGN_CHAR_0 ^ (DSKM_SIGN_TAG_CRIPT & 0xff)
#define DSKM_SIGN_CHAR_1_CR    0xe7						//DSKM_SIGN_CHAR_1 ^ (DSKM_SIGN_TAG_CRIPT & 0xff00)
#define DSKM_SIGN_CHAR_2_CR    0xe4						//DSKM_SIGN_CHAR_2 ^ (DSKM_SIGN_TAG_CRIPT & 0xff0000)
#define DSKM_SIGN_CHAR_3_CR    0xca						//DSKM_SIGN_CHAR_3 ^ (DSKM_SIGN_TAG_CRIPT & 0xff000000)
#define DSKM_SIGN_DWORD_CR		 0xcae4e74c			//DSKM_SIGN_DWORD ^ DSKM_SIGN_TAG_CRIPT  

#define DSKM_SIGN_CHAR_P       ((unsigned char)'%')
#define DSKM_SIGN_CHAR_L       ((unsigned char)'%')
#define DSKM_SIGN_WORD				 ((unsigned short)( (DSKM_SIGN_CHAR_P<<0) | (DSKM_SIGN_CHAR_L<<8) ))

#define DSKM_SIGN_CHAR_P_CR    0x64      	  //DSKM_SIGN_WORD ^ (DSKM_SIGN_TAG_CRIPT & 0xff) 
#define DSKM_SIGN_CHAR_L_CR    0xc8      	  //DSKM_SIGN_WORD ^ (DSKM_SIGN_TAG_CRIPT & 0xff00) 
#define DSKM_SIGN_WORD_CR			 0xc864      	//DSKM_SIGN_WORD ^ (DSKM_SIGN_TAG_CRIPT & 0xffff) 

#if !defined (MACHINE_IS_BIG_ENDIAN)
#define DSKM_CHECK_WORD_TAG
#endif

#define DSKM_SECURE_CHECK_TAG

#if defined (DSKM_CHECK_WORD_TAG)

#define SET_BEG_OF_SIGN( p )   ( (*((unsigned long*)(p))) = DSKM_SIGN_DWORD )
#define SET_END_OF_SIGN( p )   ( (*((unsigned short*)(p))) = DSKM_SIGN_WORD )

#if defined(DSKM_SECURE_CHECK_TAG)

#define IS_BEG_OF_SIGN( p )    ( (*((unsigned long *)(p)) ^ (unsigned long )DSKM_SIGN_TAG_CRIPT) == DSKM_SIGN_DWORD_CR )
#define IS_END_OF_SIGN( p )    ( (*((unsigned short*)(p)) ^ (unsigned short)DSKM_SIGN_TAG_CRIPT) == DSKM_SIGN_WORD_CR )

#else /*DSKM_SECURE_CHECK_TAG*/

#define IS_BEG_OF_SIGN( p )    ( (*((unsigned long *)(p))) == DSKM_SIGN_DWORD )
#define IS_END_OF_SIGN( p )    ( (*((unsigned short*)(p))) == DSKM_SIGN_WORD  )

#endif /*DSKM_SECURE_CHECK_TAG*/


#else	/*DSKM_CHECK_WORD_TAG*/

#define SET_BEG_OF_SIGN( p ) \
		( \
		*((unsigned char*)(p)+0) = DSKM_SIGN_CHAR_0, \
		*((unsigned char*)(p)+1) = DSKM_SIGN_CHAR_1, \
		*((unsigned char*)(p)+2) = DSKM_SIGN_CHAR_2, \
		*((unsigned char*)(p)+3) = DSKM_SIGN_CHAR_3 )

#define SET_END_OF_SIGN( p ) \
		( \
		*((unsigned char*)(p)+0) = DSKM_SIGN_CHAR_P, \
		*((unsigned char*)(p)+1) = DSKM_SIGN_CHAR_L )


#if defined(DSKM_SECURE_CHECK_TAG)

#define IS_BEG_OF_SIGN( p ) \
		( \
		( (*((unsigned char*)(p)+0) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>0 )) == DSKM_SIGN_CHAR_0_CR ) && \
		( (*((unsigned char*)(p)+1) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>8 )) == DSKM_SIGN_CHAR_1_CR ) && \
		( (*((unsigned char*)(p)+2) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>16)) == DSKM_SIGN_CHAR_2_CR ) && \
		( (*((unsigned char*)(p)+3) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>24)) == DSKM_SIGN_CHAR_3_CR ) )

#define IS_END_OF_SIGN( p ) \
		( \
		( (*((unsigned char*)(p)+0) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>0)) == DSKM_SIGN_CHAR_P_CR ) && \
		( (*((unsigned char*)(p)+1) ^ (unsigned char)(DSKM_SIGN_TAG_CRIPT>>8)) == DSKM_SIGN_CHAR_L_CR ) )

#else /*DSKM_SECURE_CHECK_TAG*/

#define IS_BEG_OF_SIGN( p ) \
		( \
		( *((unsigned char*)(p)+0) == DSKM_SIGN_CHAR_0 ) && \
		( *((unsigned char*)(p)+1) == DSKM_SIGN_CHAR_1 ) && \
		( *((unsigned char*)(p)+2) == DSKM_SIGN_CHAR_2 ) && \
		( *((unsigned char*)(p)+3) == DSKM_SIGN_CHAR_3 ) )

#define IS_END_OF_SIGN( p ) \
		( \
		( *((unsigned char*)(p)+0) == DSKM_SIGN_CHAR_P ) && \
		( *((unsigned char*)(p)+1) == DSKM_SIGN_CHAR_L ) )

#endif /*DSKM_SECURE_CHECK_TAG*/



#endif /* DSKM_CHECK_WORD_TAG */


//#define DSKM_EMUL

#ifndef DSKM_EMUL
#include "Gost/CrC_User.h"
#endif


typedef struct tagDSKMHash {
#ifdef DSKM_EMUL
	unsigned char hash[100];
#else
	struct   R34_11_1994 HContext;
	struct	 COM_State   PContext;
	unsigned char				 hash[LNQ*4];	// 32 bytes
#endif
} DSKMHash;



#define SIGN_VERSION (1)

#ifdef DSKM_EMUL
#define HASH_SIZE (50)
#define SIGN_BIN_LEN  (100)
#define SIGN_FULL_LEN  	(1 + 1 + SIGN_BIN_LEN)  // version + size + sign(64b) = 66
#define SIGN_AUX_SIZE		2
#else
#define HASH_SIZE (LNQ*4)
#define SIGN_BIN_LEN  	(LNQ*8)				      	// (64b) 
#define SIGN_FULL_LEN  	(1 + 1 + SIGN_BIN_LEN)  // version + size + sign(64b) = 66
#define SIGN_AUX_SIZE		2
#endif

#define SIGN_TXT_LEN  (B2T_LENGTH(SIGN_FULL_LEN))	 // 88 bytes


#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
# if defined( __cplusplus )
#pragma pack(1)
# endif
# define PACKSTRUCT __attribute ((packed))
#else
#pragma pack(push,1)
# define PACKSTRUCT 
#endif


typedef struct tagDSKMSign {
	unsigned long  begin;
	unsigned char  sign[SIGN_TXT_LEN];
	unsigned short end;
}	PACKSTRUCT DSKMSign;	 // 96 bytes


#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(pop)
#else
# if defined( __cplusplus )
#pragma pack()
# endif
#endif



#ifndef DSCLAPI
#if defined (__unix__)
#define DSCLAPI
#else
#define DSCLAPI 
#endif /* __unix__ */
#endif /* DSCLAPI */


#if defined (SECURE_CALL)
static 
#endif
AVP_dword DSCLAPI DSKMHashInit( HDSKM hDSKM, DSKMHash *pHash, void *pvInitialHash );
#if defined (SECURE_CALL)
static 
#endif
AVP_dword DSCLAPI DSKMHashBuffer( HDSKM hDSKM, DSKMHash *pHash, void *pBuffer, AVP_dword dwSize );
#if defined (SECURE_CALL)
static 
#endif
AVP_dword DSCLAPI DSKMHashEnd( HDSKM hDSKM, DSKMHash *pHash );

#if defined (SECURE_CALL)
static 
#endif
AVP_dword DSCLAPI DSKMFindSign( void *pBuffer, AVP_dword dwSize, void **ppvSign );
#if defined (SECURE_CALL)
static 
#endif
AVP_dword DSCLAPI DSKMCheckSign( HDSKM hDSKM, DSKMSign *pSign, DSKMHash *pHash, void *pKey, AVP_dword dwKeySize );


typedef AVP_dword (DSCLAPI *pfnDSKMUtil)( AVP_size_t p1, ... );
extern void *g_dwFTable[9];

#define DSKM_FTABLE				g_dwFTable
#define DSKM_FTABLE_SIZE	(sizeof(g_dwFTable) + sizeof(AVP_dword))


// Метод №1
/*
// Не забуть cl_check::DSKM_CheckSignByKeysTree - пропуск первого узла дерева ключей (в нем лежит таблица по методу №2)
#define DSKM_FTABLE_ADD(a)																																		\
{																																															\
if ( !DATA_Find_Prop((HDATA)a, 0, DSKM_CLFUNC_TABLE_ID) )																			\
DATA_Add_Prop( (HDATA)a, 0, DSKM_CLFUNC_TABLE_ID, (AVP_size_t)DSKM_FTABLE, DSKM_FTABLE_SIZE ); \
}																																															\
*/						

// Метод №2
#define DSKM_FTABLE_ADD(a)																																 	                         \
	{																																														                       \
		MAKE_ADDR1(a##addr,DSKM_CLFUNC_TABLE_ID)																									                         \
		if( !DATA_Find((HDATA)a,a##addr) ) {																												                       \
			HDATA hData = DATA_Get_First((HDATA)a,0);																																				 \
			if ( !hData )																																																		 \
				hData = DATA_Add( (HDATA)a, 0, DSKM_CLFUNC_TABLE_ID, (AVP_size_t)DSKM_FTABLE, DSKM_FTABLE_SIZE );               \
			else																																																						 \
				hData = DATA_Insert(hData, 0, DATA_Add(0, 0, DSKM_CLFUNC_TABLE_ID, (AVP_size_t)DSKM_FTABLE, DSKM_FTABLE_SIZE ));\
			DSKM_DeserializeRegBuffer( 0, g_pDSKMEData, g_dwDSKMEDataSize, 0, 0, 0, &hData );                                \
		}                                                                                                                \
	}																																														                       \

#define DSKM_FTABLE_REMOVE(a)																																 	                         \
	{																																														                       \
		MAKE_ADDR1(a##addr,DSKM_CLFUNC_TABLE_ID)																									                        \
		HDATA hData;																																																			\
		if( (hData=DATA_Find((HDATA)a,a##addr)) != 0 ) {																												          \
			DATA_Remove(hData,0);																																														\
		}                                                                                                                \
	}																																														                       \

#if defined( _WIN32 ) && !defined( _WIN64 )
#define ASM_CALL
#endif

#if defined( SECURE_CALL )
#define DSKM_HASHINIT_IND  (2)
#define DSKM_HASHBUFF_IND	 (3)
#define DSKM_HASHEND_IND   (5)
#define DSKM_FINDSIGN_IND  (6)
#define DSKM_CHECKSIGN_IND (7)

#if defined( ASM_CALL )
// Метод №1
// Не забуть cl_check::DSKM_CheckSignByKeysTree - пропуск первого узла дерева ключей (в нем лежит таблица по методу №2)
//#define DSKM_FTABLE_GET_PROP(a)																																\
//a=(AVP_dword)DATA_Find_Prop(hKeysData, 0, DSKM_CLFUNC_TABLE_ID);															\

// Метод №2
#define DSKM_FTABLE_GET_PROP(a)																															\
a=(AVP_dword)DATA_Get_First(hKeysData, 0);																									\
a=(AVP_dword)DATA_Find_Prop((HDATA)a, 0, 0);																								\

/*
#define DSCL_CALL(a,b,c,d,e,f,g)																												\
{																																												\
	DSKM_FTABLE_GET_PROP(a)																															 \
	PROP_Arr_Get_Items((HPROP)a,b,&a,sizeof(AVP_dword));																	\
	_asm { push eax																																			}\
	_asm { push ecx																																			}\
	_asm { push edx																																			}\
	_asm { mov eax, dword ptr[g] 																												}\
	_asm { push eax																																			}\
	_asm { mov ecx, dword ptr[f] 																												}\
	_asm { push ecx																																			}\
	_asm { mov edx, dword ptr[e] 																												}\
	_asm { push edx																																			}\
	_asm { mov ecx, dword ptr[d] 																												}\
	_asm { push ecx																																			}\
	_asm { mov eax, dword ptr[c] 																												}\
	_asm { push eax																																			}\
	_asm { call $+5h								 																										}\
	_asm { pop eax																																			}\
	_asm { add eax, 10h								    																							}\
	_asm { push eax																																			}\
	_asm { mov eax, dword ptr[a]																												}\
	_asm { push ebp																																			}\
	_asm { mov ebp, esp																																	}\
	_asm { add eax, 3h																																	}\
	_asm { push eax																																			}\
	_asm { ret 																																					}\
	_asm { add esp,14h 																																	}\
	_asm { mov dword ptr[a], eax 																												}\
	_asm { pop edx																																			}\
	_asm { pop ecx																																			}\
	_asm { pop eax																																			}\
}																																												\
*/
#define DSCL_CALL(a,b,c,d,e,f,g)																											 \
{																																											 \
	_asm { push eax																																			}\
	_asm { sub esp,14h																																	}\
	_asm { mov eax, dword ptr[g] 																												}\
	_asm { mov [esp+10h], eax 																													}\
	_asm { mov eax, dword ptr[f] 																												}\
	_asm { mov [esp+0Ch], eax 																													}\
	_asm { mov eax, dword ptr[e] 																												}\
	_asm { mov [esp+08h], eax 																													}\
	_asm { mov eax, dword ptr[d] 																												}\
	_asm { mov [esp+04h], eax 																													}\
	_asm { mov eax, dword ptr[c] 																												}\
	_asm { mov [esp+00h], eax 																													}\
	DSKM_FTABLE_GET_PROP(a)																															 \
	PROP_Arr_Get_Items((HPROP)a,b,&a,sizeof(void *));																 \
	_asm { call $+5h								 																										}\
	_asm { pop eax																																			}\
	_asm { add eax, 0Ah								    																							}\
	_asm { push eax																																			}\
	_asm { mov eax, dword ptr[a]																												}\
	_asm { push eax																																			}\
	_asm { ret 																																					}\
	_asm { add esp,14h 																																	}\
	_asm { mov dword ptr[a], eax 																												}\
	_asm { pop eax																																			}\
}																																										 \

#else

#define DSKM_FTABLE_GET_PROP(hp)																													\
{																																												\
HDATA hd=DATA_Get_First(hKeysData, 0);																									\
hp=DATA_Find_Prop(hd, 0, 0);																														\
}																																												\


#define DSCL_CALL(a,b,c,d,e,f,g)																												\
{																																												\
	HPROP hp;																																							\
	void *fp;																																							\
	DSKM_FTABLE_GET_PROP(hp)																															\
	PROP_Arr_Get_Items(hp,b,&fp,sizeof(void *));																	\
	a=((pfnDSKMUtil)fp)((AVP_size_t)c,(AVP_size_t)d,(AVP_size_t)e,(AVP_size_t)f,(AVP_size_t)g);	\
}																																												\

#endif

#define DSCL_CALL_HASH_INIT(a,b,c,d)			DSCL_CALL(a,DSKM_HASHINIT_IND,b,c,d,0,0)
#define DSCL_CALL_HASH_BUFFER(a,b,c,d,e)	DSCL_CALL(a,DSKM_HASHBUFF_IND,b,c,d,e,0)
#define DSCL_CALL_HASH_END(a,b,c)					DSCL_CALL(a,DSKM_HASHEND_IND,b,c,0,0,0)
#define DSCL_CALL_FIND_SIGN(a,b,c,d)	    DSCL_CALL(a,DSKM_FINDSIGN_IND,b,c,d,0,0)
#define DSCL_CALL_CHECK_SIGN(a,b,c,d,e,f)	DSCL_CALL(a,DSKM_CHECKSIGN_IND,b,c,d,e,f)

#define DSCL_SIGN_CALL_HASH_INIT(b,c,d)				((pfnDSKMUtil)(g_dwFTable[DSKM_HASHINIT_IND]))(b,c,d,0,0)
#define DSCL_SIGN_CALL_HASH_BUFFER(b,c,d,e)	  ((pfnDSKMUtil)(g_dwFTable[DSKM_HASHBUFF_IND]))(b,c,d,e,0)
#define DSCL_SIGN_CALL_HASH_END(b,c)				  ((pfnDSKMUtil)(g_dwFTable[DSKM_HASHEND_IND]))(b,c,0,0,0)
#else
#define DSCL_CALL_HASH_INIT(a,b,c,d)			a=DSKMHashInit(b,(DSKMHash *)c,(void*)d)
#define DSCL_CALL_HASH_BUFFER(a,b,c,d,e)  a=DSKMHashBuffer(b,(DSKMHash *)c,(void *)d,e)
#define DSCL_CALL_HASH_END(a,b,c)         a=DSKMHashEnd(b,(DSKMHash *)c)
#define DSCL_CALL_FIND_SIGN(a,b,c,d)			a=DSKMFindSign((void *)b,c,(void **)d)
#define DSCL_CALL_CHECK_SIGN(a,b,c,d,e,f) a=DSKMCheckSign(b,c,d,e,f)

#define DSCL_SIGN_CALL_HASH_INIT(b,c,d)				DSKMHashInit((HDSKM)b,(DSKMHash *)c,(void*)d)
#define DSCL_SIGN_CALL_HASH_BUFFER(b,c,d,e)	  DSKMHashBuffer((HDSKM)b,(DSKMHash *)c,(void *)d,e)
#define DSCL_SIGN_CALL_HASH_END(b,c)				  DSKMHashEnd((HDSKM)b,(DSKMHash *)c)
#endif

#endif //__CL_UTIL_H__
