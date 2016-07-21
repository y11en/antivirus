/* Copyright (C) 2004 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file  SpamTest.cpp
 * \author Victor V. Troitsky
 * \brief Intrepreter main functions
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 06.12.04: v 1.00 created by vvt
 *==========================================================================
 */
#include "commhdr.h"
#pragma hdrstop


//#define USE_ACTION_EXECUTE_OUTPUT


#include "LogFile.h"
#include <smtp-filter/common/logger.h>

#include "MainInterpreter.h"
#include "interpreter.h"
#include "envelope.h"

#include "profile.h"
#include "profile_db.h"
#include <smtp-filter/common/list_db.h>
#include "Filtration/API/filter.h"	

#include <client-filter/SpamTest/CFilterDll.h>

#ifndef KIS_USAGE
    #include <smtp-filter/adc/adc.link.h>
    #include <gnu/libnet.link.h>
#endif

#include "Cp2Uni.h"
#include "CodePages.h"

#include "MIMEParser/MIMEParser.hpp"

#include "smtp-filter/common/merge.h"

//Base 
static profile_db		*g_pPDB = NULL;
static list_db			*g_pLDB = NULL;


#ifndef KIS_USAGE
    extern md5sigarray *g_pMD5Array;  //!< no used in KIS
#endif


const char * actionStrNames[] = {	"HEADER_MODIFY_NONE",
									"HEADER_MODIFY_REMOVE",
									"HEADER_MODIFY_CHANGE",
									"HEADER_MODIFY_PREPEND",
									"HEADER_MODIFY_NEW",
									"HEADER_MODIFY_ADD"
								};


#ifndef MAX_GSG2_SIZE
#define MAX_GSG2_SIZE (300*1024)
#endif

//#ifdef HAVE_MD5CHECK
int check_md5(md5sum sum)
{
#ifdef KIS_USAGE
    return 0;
#else
    if(g_pMD5Array)
	{
	    if(-1==md5sigfind(g_pMD5Array,sum))
		return 0;
	    else
		return 1;
	}
    return 0;
#endif
}
//#endif

//Load profiles and local lists
HRESULT InitFormalRules(LPCSTR szPath)
{
#ifdef KIS_USAGE
    return S_OK;
#else
	char szFile[_MAX_PATH];
	static const char szPDBExt[] = ".pdb";
	
	strncpy(szFile,szPath, sizeof(szFile)-sizeof(szPDBExt));
	szFile[sizeof(szFile)-sizeof(szPDBExt)-1] = 0;
	strcat(szFile,szPDBExt);
	g_pPDB = profile_db_open(szFile, O_RDONLY | O_BINARY, 0600);

	static const char szLDBExt[] = ".ldb";
	strncpy(szFile,g_CFilterDll.MainBasePath(), sizeof(szFile)-sizeof(szLDBExt));
	szFile[sizeof(szFile)-sizeof(szLDBExt)-1] = 0;
	strcat(szFile,szLDBExt);
	g_pLDB = list_db_open(szFile, O_RDONLY | _O_BINARY, 0600);

	return ( (g_pPDB && g_pLDB) ? S_OK : E_FAIL);
#endif
}

//Unload profiles and local lists
HRESULT DoneFormalRules()
{
#ifndef KIS_USAGE
	LOG(_T("DoneFormalRules"));
	if(g_pPDB)
	{
		profile_db_close(g_pPDB);
		g_pPDB = NULL;	
	}
	if(g_pLDB)
	{
		list_db_close(g_pLDB);
		g_pLDB = NULL;
	}
#endif//KIS_USAGE
	return S_OK;
}


/*******************************************************************************
 *
 * Parse headers of letter.
 *
 *******************************************************************************/

struct EnvelopeParserConfigKASP
{
    void header_continue();
    void header_name(const char* p, size_t count);
    void header_body(const char* p, size_t count, const char* codepage, size_t cp_count);
    void part_begin(unsigned int level);
    void uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length);
    void epilogue(unsigned int level);
    void body(const char* p, size_t count, bool eol);

    void content_type(const char* p, size_t count);
    void content_subtype(const char* p, size_t count);
    void content_charset(const char* p, size_t count);
    void content_full_type(const char* p, size_t count);
    void content_name(const char* p, size_t count);
    void content_protocol(const char* p, size_t count);

    envelope *e;
    int       cur_header;
    bool      was_header_body;

    bool received_header;
    unsigned int received_counter;
    bool in_body;

    EnvelopeParserConfigKASP(envelope *env);
};

inline EnvelopeParserConfigKASP::EnvelopeParserConfigKASP(envelope *env) : e(env), cur_header(-1), was_header_body(false),
                                                                   received_header(false), received_counter(0), in_body(false)
{
}

inline void EnvelopeParserConfigKASP::header_continue()
{
}

inline void EnvelopeParserConfigKASP::header_name(const char* p, size_t count)
{
    if(in_body)
        return;

    size_t offset = envelope_heap_get_string(e, count+1);
    cur_header = envelope_new_header(e);

#ifndef KIS_USAGE
    if(received_header)
        {
            ip_extractor_end_of_line(e->ipe);
            received_header = false;
        }
#endif//KIS_USAGE
    
    memcpy(e->heap.array + offset, p, count);
    *(e->heap.array + offset + count) = 0;
    
    e->headers.array[cur_header].header = offset;
    
    was_header_body = false;
    
    if(count == sizeof("Received")-1 && cmp_nocase(p, "Received", count))
        {
            received_header = true;
            received_counter++;
        }
}

inline void EnvelopeParserConfigKASP::header_body(const char* p, size_t count, const char* codepage, size_t cp_count)
{
    if(!p || !count)
	return;

    if(in_body)
        return;

    int cp_id = CP_KOI8R;

#ifndef KIS_USAGE
    if(received_header && received_counter <= e->max_received_headers)
        ip_extractor_parse(e->ipe, p, count);
#endif//KIS_USAGE

    if(codepage)
	cp_id = check_encoding(codepage, cp_count);
    if(cp_id == CP_UNKNOWN)
	cp_id = CP_KOI8R;
    

    if(!was_header_body)
	{
	    e->headers.array[cur_header].value = envelope_heap_get_string(e, count+1);

	    memcpy(ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), p, count);
	 
	    if(cp_id != CP_KOI8R)
		ConvertData((unsigned char *)ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), count, cp_id, CP_KOI8R);		    

	    *ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value + count) = '\0';
	    was_header_body = true;
	}
    else
	{
	    envelope_heap_resize(e, count);

	    char *offset = strchr(ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), '\0');

	    memcpy(offset, p, count);
	    
	    if(cp_id != CP_KOI8R)
		ConvertData((unsigned char *)offset, count, cp_id, CP_KOI8R);

	    *(offset + count) = '\0';
	}
}

inline void EnvelopeParserConfigKASP::part_begin(unsigned int level)
{
    in_body = true;

}

inline void EnvelopeParserConfigKASP::uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length)
{
    in_body = true;
}


inline void EnvelopeParserConfigKASP::epilogue(unsigned int level)
{
    in_body = true;
}

inline void EnvelopeParserConfigKASP::body(const char* p, size_t count, bool eol)
{
#ifndef KIS_USAGE
    if(received_header)
        ip_extractor_parse_finish(e->ipe);
#endif//KIS_USAGE
    in_body = true;
}

inline void EnvelopeParserConfigKASP::content_type(const char* p, size_t count)
{
}

inline void EnvelopeParserConfigKASP::content_subtype(const char* p, size_t count)
{
}

inline void EnvelopeParserConfigKASP::content_charset(const char* p, size_t count)
{
}

inline void EnvelopeParserConfigKASP::content_full_type(const char* p, size_t count)
{
}

inline void EnvelopeParserConfigKASP::content_name(const char* p, size_t count)
{
}

inline void EnvelopeParserConfigKASP::content_protocol(const char* p, size_t count)
{
}


class CmpFunctor
{
public:
	CmpFunctor(envelope* env) : envelope_for_sort(env) {};
	CmpFunctor(const CmpFunctor &src) : envelope_for_sort(src.envelope_for_sort) {};
	CmpFunctor& operator=(const CmpFunctor &rhs)
	{
		if(this != &rhs)
		{
			envelope_for_sort = rhs.envelope_for_sort;
		}
		return *this;
	}
	
	CmpFunctor& operator*(){ return *this; };

	int operator()(const void *a, const void *b)
	{
		const msg_header *mh_a = (const msg_header *)a;
		const msg_header *mh_b = (const msg_header *)b;

		return strcasecmp(ENVELOPE_HEAP_STRING(envelope_for_sort, mh_a->header),
		      ENVELOPE_HEAP_STRING(envelope_for_sort, mh_b->header));
	}
private:
	envelope *envelope_for_sort;
};



int envelope_prepare_headers_KASP(envelope *e)
{
    EnvelopeParserConfigKASP pc(e);

    MIMEParser<EnvelopeParserConfigKASP> parser(&pc);
    
    int res = 0;
#define RETURN(x) 				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0)

    const char *line_begin = e->data.text;
    int      line_length = 0;
    size_t      i;
    

    for(i = 0; i < e->data.length; i++)
	{
            if(e->data.text[i] == 10)
                {
                    //Check for spaces and cf
                    //for(int s = i-1; (isspace(e->data.text[s]) || e->data.text[s] == 13) && line_length > 0; s--, line_length--);
                    if(parser.parse(line_begin, (i > 0 ? (e->data.text[i-1] == 13 ? line_length - 1 : line_length) : line_length)) == MIMEParserGlobal::PARSE_ERROR)
                        RETURN(-1);
                    
                    line_begin = e->data.text+i+1;
                    line_length = 0;
                }
            else 
                line_length++;
        }

    if(line_begin && line_length)
	parser.parse(line_begin, line_length);

    pc.in_body = 0;


//	local_mergesort(e->headers.array, e->headers.used, sizeof(msg_header), CmpFunctor(e));

#undef RETURN
  finish:
    
    return res;
}

int add_header(envelope *e, const char* name, const char* value)
{
	//Name
	size_t count = strlen(name);
	size_t offset = envelope_heap_get_string(e, count+1);
    int cur_header = envelope_new_header(e);

 
    memcpy(e->heap.array + offset, name, count);
    *(e->heap.array + offset + count) = 0;
    
    e->headers.array[cur_header].header = offset;
	//Value

	count = strlen(value);

    e->headers.array[cur_header].value = envelope_heap_get_string(e, count+1);

	memcpy(ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), value, count);
	 
	*ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value + count) = '\0';

	return 0;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//HEADERS DEBUG OUTPUT
	
int exec_callback_ex(void *p, unsigned int flags, const char *data, size_t length, size_t &buf_alloc, size_t &buf_used, char ** buffer)
{
    if(length)
        {
			if(buf_used + length + 1 > buf_alloc)
            {
                size_t newsize = (buf_used + length + 1 > buf_alloc + 1024 ? buf_used + length + 1024 : buf_alloc + 1024);
                //TODO: memory realloc error handling !!!
                *buffer = (char*)realloc(*buffer, newsize);
                buf_alloc = newsize;
            }
			 
			memcpy(*buffer+buf_used,data,length);
            buf_used+=length;

        }
    if(flags & ACTION_EXEC_CRLF && buf_used)
    {
        (*buffer)[buf_used] = '\x0';
		logger_printf(LOGGER_DEBUG,"%s",*buffer);			
        buf_used = 0;
    }
    return 0;
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//Get message from the pInfo structure and interpretate 
// [in]  message_with_headers *msgh - message in content_filter format
// [out] type 
// Return Values:	= 0 - Not spam
//					> 0 - Category of spam:
//					= 1 - Spam
//					= 2 - Probable spam
//					= 3 - Obscene or Suspicious 
//					= 4 - Formal
//                  = E_FAIL - if some necessary bases(ldb,pdb or cfilter) weren't opened
UINT SpamTestProfile(message_with_headers *msgh)
{
#ifndef KIS_USAGE
	logger_printf(LOGGER_DEBUG,"SpamTestProfile(): Start processing message...");

    //Check for necessary bases (listDB, profileDB and ContentFilter)
    if(g_pPDB == NULL || g_pLDB == NULL || g_CFilterDll.GetFilter() == NULL)
    {
	    logger_printf(LOGGER_DEBUG,"SpamTestProfile() == E_FAIL: Error: ListDB or ProfilesDB or ContentFilter not opened...");
        return E_FAIL;
    }
    
    //Get info about Message from pInfo and fill in envelope structure
	envelope e;				//Letter with envelope
	action_storage as;		//Action storage
	interpreter_config ic;	//
    
    bool haveHeaders = true; //If false then skip processing formal.xml profiles

#ifdef WITH_GSG
	binary_data_t *binary_processor;
	int gsg_stats[8];
#endif
    
	memset(&ic, 0, sizeof(ic));
	memset(&as, 0, sizeof(as));

	//Init interpreter_config some structures
	ic.e = &e;
	ic.as = &as;

	envelope_create(&e);

	
	//Get pointers to content filter, profilesDB and listDB
	ic.cf  = g_CFilterDll.GetFilter();
	ic.pdb = g_pPDB;
	ic.ldb = g_pLDB;
	
	//Memorize pointer to the message structure
	ic.msg = &msgh->msg;

    //Let's fill in the structures

	envelope_recycle(&e);
   
	//Memorize approximate message size
	e.msg_size = msgh->m_MsgSize;

    
    //Check MIME Headers presence 
    if( !msgh->m_pHeaders || !*(msgh->m_pHeaders) )
    {
        LOG("Warning: Message has no Internet headers!");
        haveHeaders = false;
    }
    else
    {
        envelope_set_data(&e, msgh->m_pHeaders, strlen(msgh->m_pHeaders));

        //Parse message headers (MIMEParser) if any
        envelope_prepare_headers_KASP(&e);
        
        //Check how many headers have we parsed?
        if(e.headers.used == 0)
            haveHeaders = false;
	}

    action_storage_init(&as, &e);

	//Add headers with GSG checking results
#ifdef WITH_GSG    
	bzero(&gsg_stats,sizeof(gsg_stats));
    binary_processor = binary_init(MAX_GSG2_SIZE);
    if(binary_processor)
	{
	    binary_setsigdb(binary_processor, g_pMD5Array);
	
		//Get signatures for all attachments with using binaryprocessor  
		for( size_t i = 0; i < msgh->msg.attachments_count; i++)
		{
			if(!(msgh->msg.attachments[i].ptr) || (msgh->msg.attachments[i].count < 40))
				continue;
			binary_put(binary_processor, (void*)msgh->msg.attachments[i].ptr, msgh->msg.attachments[i].count);
			binary_endpart(binary_processor);
		}
	

        if(binary_processor->part_count>1)
		{
		    gsg_stats[0]++;
		    gsg_stats[1]+=binary_processor->part_count-1;
		
		    int match[NGSG],matched;
		    unsigned int j,i;
#ifdef GSG2_DEBUG
#define  HBUFSZ (NGSG*3+1)
		    char *buf2;
		    if(gsg2_debug)
			action_header_modify(as.common_action,
					     HEADER_MODIFY_REMOVE,"X-SpamTest-GSG-Debug","");
#endif
		    matched=0;
		    int m1=0,m = 0;
		    int img=0;
		    for(i=0;i<binary_processor->part_count;i++)
			{
			    img = 0;
			    bzero(&match,sizeof(match));
			    for(j=0;j<binary_processor->md5list->used;j++)
				{
				    if(binary_processor->md5list->array[j].imageno != i)
					continue;
				    if(binary_processor->md5list->array[j].type==SIG_GSG2)//image
					{
					    gsg_stats[2]++;
					    img++;
					}
				    if(md5sigfind(g_pMD5Array, binary_processor->md5list->array[j].md5) > 0)
					{
					    match[binary_processor->md5list->array[j].type%NGSG]++;
					    if(binary_processor->md5list->array[j].type 
					       <= SIG_GSG4)
					    	m++;
					    if(binary_processor->md5list->array[j].type 
					       == SIG_GSG1)
					    	m1++;
					}
				}
				
#ifdef GSG2_DEBUG
			    if(img && gsg2_debug)
				{
				    buf2 = e.heap.array
					+envelope_heap_get_string(&e,HBUFSZ);
				    *buf2=0;
				    for(j=1;j<NGSG;j++)
					if(match[j])
					    strcat(buf2," 1");
					else
					    strcat(buf2," 0");
				    action_header_modify(as.common_action,
							 HEADER_MODIFY_NEW,"X-SpamTest-GSG-Debug",buf2);
				}
#undef HBUFSZ		    
#endif
			}
//Add headers to the message
			char header[64], value[32];
			for(i=0;i<NGSG;i++)
				if(match[i])
				{
					if( i==1 )
					{ // compatibility
						add_header(&e,"X-SpamTest-Checksum","MATCH");
                        logger_printf(LOGGER_DEBUG,"Header \"X-SpamTest-Checksum: MATCH\" added");
					}
#if 1
					snprintf(header,64,"X-SpamTest-Internal-GSG-%d",i);
					snprintf(value,32,"MATCH %u",match[i]);
					add_header(&e,header,value);
                    logger_printf(LOGGER_DEBUG,"Header \"%s: %s\" added",header,value);
					logger_printf(LOGGER_INFO|LOGGER_VERBOSE_HIGH, "GSG%d match (%d times)", i,match[i]);
#endif
				}

		    if(m1)
			gsg_stats[3]++; //message match by GSG1
		    else if(m)
			gsg_stats[4]++; //message match by GSG2
					      //but not GSG1

		}
	    binary_clearall(binary_processor);
	}
#endif

	//Sort all headers

	local_mergesort(e.headers.array, e.headers.used, sizeof(msg_header), CmpFunctor(&e));


#define VBUFSZ 60
/*	char *hbuf = e.heap.array+envelope_heap_get_string(&e,VBUFSZ);
	snprintf_short_version(hbuf, VBUFSZ-1);
	action_header_modify(as.common_action, HEADER_MODIFY_ADD,"X-SpamTest-Version",hbuf);*/
	    
	// anyway remove X-SpamTest-RPK headers
	action_header_modify(as.common_action, HEADER_MODIFY_REMOVE,"X-SpamTest-RPK-Status","");
	action_header_modify(as.common_action, HEADER_MODIFY_REMOVE,"X-SpamTest-RPK","");
	action_header_modify(as.common_action, HEADER_MODIFY_REMOVE,"X-SpamTest-RPKM","");
	action_header_modify(as.common_action, HEADER_MODIFY_REMOVE,"X-SpamTest-RPK-Loop","");


	
    //Add special formal header if there are no any MIME headers in the message, or delete them otherwise.
    if(haveHeaders)
        action_header_modify(as.common_action, HEADER_MODIFY_REMOVE,"X-SpamTest-Internal-Flag","");
    else
        add_header(&e,"X-SpamTest-Internal-Flag", "NH");
            
    //Profile interpreter
    profile_interpretate(&ic, !haveHeaders);

    //Action normalization
    action_storage_normalize(&as, ACTIONS_STORAGE_NORMALIZE_LIBSPAMTEST);
	    
	int is_bounce = 0;

	if(ACTION_IS_BOUNCE(as.common_action))
		is_bounce = 1;
	    
	/*
	 * Check BOUNCE.
	 */
	for( size_t i = 0; !is_bounce && i < as.actions.used; i++)
	{
	    if(ACTION_IS_BOUNCE(as.actions.array[i]))
			is_bounce = 1;
	}

    /*
     * Don't check BOUNCE here, because BOUNCE with REJECT not possible.
     */
    if(ACTION_GET_TYPE(as.common_action) == ACTION_REJECT)
	{
	    logger_printf(LOGGER_INFO | LOGGER_VERBOSE_INFORMATIVE, "REJECT");
	}
  

//Analyze statuses 
	DWORD dwStatus	= 0; //Not detected 
	DWORD dwFacility = STFACILITY_GENERAL;
#ifdef WITH_LOGDEBUG_LEV2
    logger_printf(LOGGER_DEBUG, "Actions used: %d", as.actions.used);
#endif //WITH_LOGDEBUG_LEV2
    for(size_t j = 0; j < as.common_action->headers.used; j++)
	{
#ifdef WITH_LOGDEBUG_LEV2
	    logger_printf(LOGGER_DEBUG, "Action %s(\"%s: %s\")", actionStrNames[as.common_action->headers.array[j].type], as.common_action->headers.array[j].header, as.common_action->headers.array[j].value);
#endif //WITH_LOGDEBUG_LEV2

		if((as.common_action->headers.array[j].type == HEADER_MODIFY_REMOVE) && (strcmp(as.common_action->headers.array[j].header, "X-SpamTest-Status") == 0)) 
        {
            dwStatus = 0; //Not detected
            continue;
        }

                if((as.common_action->headers.array[j].type == HEADER_MODIFY_REMOVE) || (as.common_action->headers.array[j].type == HEADER_MODIFY_NONE))
					continue;

				//Get message status
				if(strcmp(as.common_action->headers.array[j].header, "X-SpamTest-Status") == 0)
			    {
				    if(strcmp(as.common_action->headers.array[j].value, "SPAM") == 0)
					    dwStatus = 1; //SPAM
				    else if(stricmp(as.common_action->headers.array[j].value, "Probable spam") == 0)
					    dwStatus = 2; //Probable spam
				    else if(stricmp(as.common_action->headers.array[j].value, "OBSCENE") == 0)
					    dwStatus = 3; //Obscene
				    else if(stricmp(as.common_action->headers.array[j].value, "FORMAL") == 0)
					    dwStatus = 4; //Formal
			    }
				
				//Get facility
                if((strcmp(as.common_action->headers.array[j].header, "X-SpamTest-KPAS-Facility") == 0) && dwFacility == STFACILITY_GENERAL)
				{
				    if(strstr(as.common_action->headers.array[j].value, "GSG1"))
					    dwFacility = STFACILITY_GSG1;   //GSG
					if(strstr(as.common_action->headers.array[j].value, "GSG2"))
					    dwFacility = STFACILITY_GSG2;   //GSG2
					if(strstr(as.common_action->headers.array[j].value, "Headers"))
					    dwFacility = STFACILITY_HEADERS;//Headers
                    if(strstr(as.common_action->headers.array[j].value, "Bayes"))
                        dwFacility = STFACILITY_BAYES;  //Bayes
                    
				}
								
			}

//Debug output of all headers
//Step 1: Look through all initial headers and check whether they are still alive
	logger_printf(LOGGER_DEBUG,"Survived headers:");	    
    int headerCount;
    bool isAddAction, isPrependAction;

    //Buffer for ADD actions
    char *pBufferBefore = NULL;
    size_t BufferBeforeAlocated = 0;
    size_t BufferBeforeUsed = 0;
    //Buffer for PREPEND actions
    char *pBufferAfter = NULL;
    size_t BufferAfterAlocated = 0;
    size_t BufferAfterUsed = 0;

    char *pDebugBuffer = NULL;
    size_t buffer_allocated = 0;
    size_t buffer_used = 0;


	for(i = 0; i < e.headers.used; i++)
    {
        headerCount = 1;
        isAddAction = false;
        isPrependAction = false;
        BufferBeforeUsed = 0;
        BufferAfterUsed = 0;

        for(size_t j = 0; j < as.common_action->headers.used; j++)
	    {
	        if(strcasecmp(ENVELOPE_HEAP_STRING(&e, e.headers.array[i].header), as.common_action->headers.array[j].header) == 0)
		    {
		        if(as.common_action->headers.array[j].type == HEADER_MODIFY_CHANGE ||
		           as.common_action->headers.array[j].type == HEADER_MODIFY_NEW)
			    {
			        headerCount++;
			    }
		        else if(as.common_action->headers.array[j].type == HEADER_MODIFY_REMOVE)
			    {
			        headerCount--;
                    if(headerCount == 0)
                        break;
			    }
                else if(as.common_action->headers.array[j].type == HEADER_MODIFY_ADD)
                {
                    if(!isAddAction)
                    {   //The first action ADD
                        isAddAction = true;
                        if(headerCount == 0)
                            headerCount++;
                    }  
                    //Copy added         
                    size_t addStrLen = strlen(as.common_action->headers.array[j].value);
                    if(BufferAfterUsed + addStrLen + 1 > BufferAfterAlocated)
                    {
                        size_t newSize = BufferAfterUsed + addStrLen + 1024;
                        pBufferAfter = (char*)realloc(pBufferAfter, newSize);
                        BufferAfterAlocated = newSize;
                    }
                    memcpy(pBufferAfter+BufferAfterUsed,as.common_action->headers.array[j].value, addStrLen);
                    BufferAfterUsed += addStrLen;
                    //Put \x0 at the end of the buffer
                    *(pBufferAfter+BufferAfterUsed) = '\x0';
                }
                else if(as.common_action->headers.array[j].type == HEADER_MODIFY_PREPEND)
                {
                    if(!isPrependAction)
                    {   //The first PREPEND action
                        isPrependAction = true;
                        if(headerCount == 0)
                            headerCount++;
                    }  
                    //TODO: Should be tested ???
                    size_t prependStrLen = strlen(as.common_action->headers.array[j].value);
                    if(BufferBeforeUsed + prependStrLen + 1 > BufferBeforeAlocated)
                    {
                        size_t newSize = BufferBeforeUsed + prependStrLen + 1024;
                        pBufferBefore = (char*)realloc(pBufferBefore, newSize);
                        BufferBeforeAlocated = newSize;
                    }
                    memmove(pBufferBefore+prependStrLen,pBufferBefore,BufferBeforeUsed);
                    memcpy(pBufferBefore,as.common_action->headers.array[j].value, prependStrLen);
                    BufferBeforeUsed += prependStrLen;
                    //Put \x0 at the end of the buffer
                    *(pBufferBefore+BufferBeforeUsed) = '\x0';
                }
		    }
	    }

        if(headerCount)
        {
            if(BufferAfterUsed && BufferBeforeUsed)
                logger_printf(LOGGER_DEBUG,"%s:'%s%s%s'", ENVELOPE_HEAP_STRING(&e, e.headers.array[i].header), pBufferBefore, ENVELOPE_HEAP_STRING(&e, e.headers.array[i].value), pBufferAfter);
            else if(BufferAfterUsed)
                logger_printf(LOGGER_DEBUG,"%s:'%s%s'", ENVELOPE_HEAP_STRING(&e, e.headers.array[i].header), ENVELOPE_HEAP_STRING(&e, e.headers.array[i].value), pBufferAfter);
            else if(BufferBeforeUsed)
                logger_printf(LOGGER_DEBUG,"%s:'%s%s'", ENVELOPE_HEAP_STRING(&e, e.headers.array[i].header), pBufferBefore, ENVELOPE_HEAP_STRING(&e, e.headers.array[i].value));
            else
                logger_printf(LOGGER_DEBUG,"%s:'%s'", ENVELOPE_HEAP_STRING(&e, e.headers.array[i].header), ENVELOPE_HEAP_STRING(&e, e.headers.array[i].value));
        }
    }
//Step 2: Output all new headers
    BufferBeforeUsed = 0;
    BufferAfterUsed = 0;
    
    //Alloc memory buffer for macros expansion
    pDebugBuffer = (char*)malloc(1024);
    buffer_allocated = (pDebugBuffer ? 1024 : 0);
    if(!pDebugBuffer)
        logger_printf(LOGGER_DEBUG, "Error: allocation memory for headers debug output buffer");

    logger_printf(LOGGER_DEBUG,"Added headers:");	    
    for(j = 0; j < as.common_action->headers.used; j++)
	{
        //Find next action that creates or modify header (???: HEADER_MODIFY_CHANGE and HEADER_MODIFY_PREPEND ???)
        if((as.common_action->headers.array[j].type == HEADER_MODIFY_ADD) ||
           (as.common_action->headers.array[j].type == HEADER_MODIFY_CHANGE) ||
           (as.common_action->headers.array[j].type == HEADER_MODIFY_NEW) ||
           (as.common_action->headers.array[j].type == HEADER_MODIFY_PREPEND)) 
        {   
            //Check whether that header is still alive
            isAddAction = (as.common_action->headers.array[j].type == HEADER_MODIFY_ADD);
            isPrependAction = (as.common_action->headers.array[j].type == HEADER_MODIFY_PREPEND); 
            headerCount = 1;
            BufferBeforeUsed = 0;
            BufferAfterUsed = 0;

            for(size_t l = j + 1; l < as.common_action->headers.used; l++)
                if(strcasecmp(as.common_action->headers.array[j].header, as.common_action->headers.array[l].header) == 0)
                {
                    if(as.common_action->headers.array[l].type == HEADER_MODIFY_REMOVE)
                    {
                        headerCount--;
                        if(headerCount == 0)
                            break;
                    } 
                    else if((as.common_action->headers.array[l].type == HEADER_MODIFY_CHANGE) ||
                              (as.common_action->headers.array[l].type == HEADER_MODIFY_NEW)) 
                    {   
                        headerCount++;
                    } 
                    else if(as.common_action->headers.array[l].type == HEADER_MODIFY_ADD)
                    {
                        //ADD
                        size_t addStrLen = strlen(as.common_action->headers.array[l].value);
                        if(BufferAfterUsed + addStrLen + 1 > BufferAfterAlocated)
                        {
                            size_t newSize = BufferAfterUsed + addStrLen + 1024;
                            pBufferAfter = (char*)realloc(pBufferAfter, newSize);
                            BufferAfterAlocated = newSize;
                        }
                        memcpy(pBufferAfter+BufferAfterUsed, as.common_action->headers.array[l].value, addStrLen);
                        BufferAfterUsed += addStrLen;
                        //Put \x0 at the end of the buffer
                        *(pBufferAfter+BufferAfterUsed) = '\x0';
                    }
                    else if(as.common_action->headers.array[l].type == HEADER_MODIFY_PREPEND)
                    {
                        //TODO: Should be tested ???
                        size_t prependStrLen = strlen(as.common_action->headers.array[l].value);
                        if(BufferBeforeUsed + prependStrLen + 1 > BufferBeforeAlocated)
                        {
                            size_t newSize = BufferBeforeUsed + prependStrLen + 1024;
                            pBufferBefore = (char*)realloc(pBufferBefore, newSize);
                            BufferBeforeAlocated = newSize;
                        }
                        memmove(pBufferBefore+prependStrLen,pBufferBefore,BufferBeforeUsed);
                        memcpy(pBufferBefore,as.common_action->headers.array[l].value, prependStrLen);
                        BufferBeforeUsed += prependStrLen;
                        //Put \x0 at the end of the buffer
                        *(pBufferBefore+BufferBeforeUsed) = '\x0';
                    }

                 }
		    if(headerCount)
            {
                //Old output style
                /*if(BufferAfterUsed && BufferBeforeUsed)
                    logger_printf(LOGGER_DEBUG,"%s:'%s%s%s'", as.common_action->headers.array[j].header, pBufferBefore, as.common_action->headers.array[j].value, pBufferAfter);
                else if(BufferAfterUsed)
                    logger_printf(LOGGER_DEBUG,"%s:'%s%s'", as.common_action->headers.array[j].header, as.common_action->headers.array[j].value, pBufferAfter);
                else if(BufferBeforeUsed)
                    logger_printf(LOGGER_DEBUG,"%s:'%s%s'", as.common_action->headers.array[j].header, pBufferBefore, as.common_action->headers.array[j].value);
                else
                    logger_printf(LOGGER_DEBUG,"%s:'%s'", as.common_action->headers.array[j].header, as.common_action->headers.array[j].value);
                */

                //Output header with macros expanding(using action_add_header_value function) 

                action_add_header_value_ex(&ic, ACTION_HEADER_VALUE_NOTCRLF, as.common_action, as.common_action->headers.array[j].value, 1, &pDebugBuffer, &buffer_allocated, &buffer_used, exec_callback_ex, NULL);
                pDebugBuffer[buffer_used] = '\x0';
               
                if(BufferAfterUsed && BufferBeforeUsed)
                    logger_printf(LOGGER_DEBUG,"%s:'%s%s%s'", as.common_action->headers.array[j].header, pBufferBefore, pDebugBuffer, pBufferAfter);
                else if(BufferAfterUsed)
                    logger_printf(LOGGER_DEBUG,"%s:'%s%s'", as.common_action->headers.array[j].header, pDebugBuffer, pBufferAfter);
                else if(BufferBeforeUsed)
                    logger_printf(LOGGER_DEBUG,"%s:'%s%s'", as.common_action->headers.array[j].header, pBufferBefore, pDebugBuffer);
                else
                    logger_printf(LOGGER_DEBUG,"%s:'%s'", as.common_action->headers.array[j].header, pDebugBuffer);
                
                buffer_used = 0;
         
            }
        }
    }
    //Free buffers
    if(pBufferBefore)
        free(pBufferBefore);
    if(pBufferAfter)
        free(pBufferAfter);

#ifdef USE_ACTION_EXECUTE_OUTPUT
    //test output
  /*  if(!pDebugBuffer)
    {
	    logger_printf(LOGGER_DEBUG, "Start of test output:");
        action_execute(&ic, as.common_action, 1, exec_callback, NULL);
	    logger_printf(LOGGER_DEBUG, "End of test output.");
    }
    else
        logger_printf(LOGGER_DEBUG, "Error: allocation memory for headers debug output buffer");*/
#endif //#ifdef USE_ACTION_EXECUTE_OUTPUT

    if(buffer_allocated && pDebugBuffer)
        free(pDebugBuffer);

//End of debug output of all headers

#ifdef WITH_GSG
    if(binary_processor)
	binary_free(binary_processor);
#endif


    if(ic.fr)
		g_CFilterDll.ResultFree(ic.fr);

    if(ACTION_STORAGE_INITED(&as))
		action_storage_destroy(&as);

    envelope_destroy(&e);

	//Calculate final status (combining message status and facility code)
	dwStatus = (dwStatus > 0 ? dwStatus | dwFacility : dwStatus);
    
    logger_printf(LOGGER_DEBUG,"SpamTestProfile(): Message processing finished: Status=%d, Facility=\"%s\"", sptGetType(dwStatus), GetFacilityName(sptGetFacility(dwStatus)));
    	
    return dwStatus;
#else
    return 0;
#endif//KIS_USAGE
}
