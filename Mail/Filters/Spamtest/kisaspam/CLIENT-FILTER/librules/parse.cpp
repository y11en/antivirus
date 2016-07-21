/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file   parse.cpp
 * \author Victor V. Troitsky
 * \brief  Filtration Profiles Compiler
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 06.01.05: v 1.00 created by VVT from ALK source
 *==========================================================================
 */
#include "commhdr.h"
#pragma hdrstop

#include "LogFile.h"

#include <_include/nix_types.h>

#include <ctype.h>

#include <CodePages.h>



#include "parse.h"

#include <smtp-filter/filter-config/xmlfiles/XMLFiles.h>
#include <smtp-filter/filter-config/xmlfiles/keywords.h>
#include <smtp-filter/paths/paths.h>
#include <smtp-filter/common/config.h>
#include <smtp-filter/common/logger.h>
#include "filter.h"

#include "samples.h"


#include <client-filter/SpamTest/CFilterDll.h>

#include "ProfileCompile.h"

static bool profile_warnings = false;
static bool profile_errors = false;

void init_errors()
{
	profile_errors = false;
	profile_warnings = false;
}

int is_warnings()
{
	return profile_warnings;
}

int is_errors()
{
	return profile_errors;
}


int compiler_warning(const char *format, ...)
{
	profile_warnings = true;

    va_list args;
    va_start(args, format);

    //sprintf("Warning: ");
    
	TCHAR buf[1024]; 
	buf[sizeof(buf)-1] = 0;
	_snprintf(buf, sizeof(buf)-2, "%s", "Warning: ");
	_vsnprintf(buf+strlen(buf), sizeof(buf)-2 - strlen(buf), format, args);

	LOG(buf);
    va_end(args);

    return 0;
}

int compiler_error(const char *format, ...)
{
	profile_errors = true;

    va_list args;
    va_start(args, format);

	TCHAR buf[1024]; 
	buf[sizeof(buf)-1] = 0;
	_snprintf(buf, sizeof(buf)-2, "%s", "Error: ");
	_vsnprintf(buf+strlen(buf), sizeof(buf)-2 - strlen(buf), format, args);

	LOG(buf);
    va_end(args);

    return 0;
}


#pragma warning (disable:4127)
#pragma warning (disable:4706)





inline int parse_rcpts(const char *rcpt_analyze, size_t &count, char **&rcpts)
{
    if(*rcpt_analyze == '\0')
	{
	    count = 0;
	}
    else
	{
	    const char *p, *p_colon, *nonspace;
	    int was_nonspaces = 0;
	    size_t i = 0;
	    
	    p = rcpt_analyze;

	    count = 0;
	    for(p_colon = p; ; p_colon++)
		{
		    if(*p_colon == ';' || *p_colon == ',' || *p_colon == '\0')
			{
			    if(was_nonspaces)
				count++;
			    was_nonspaces = 0;

			    if(*p_colon == '\0')
				break;
			}
		    else
			{
			    if(!(*p_colon == ' ' || *p_colon == '\t' || *p_colon == '\r' || *p_colon == '\n'))
				{
				    was_nonspaces = 1;
				}
			    
			}
		}

	    rcpts = (char **)malloc(count*sizeof(char *));
	    memset(rcpts, 0, count*sizeof(char *));
    
	    for(p = rcpt_analyze, i = 0; *p; )
		{
		    for(p_colon = p; *p_colon != ';' && *p_colon != ',' && *p_colon; p_colon++)
			;

		    if(*p_colon)
			{
			    for(nonspace = p; nonspace <= p_colon && isspace(*nonspace); nonspace++)
				;

			    if(nonspace < p_colon)
				{
				    rcpts[i] = (char *)malloc(p_colon - nonspace + 1);
				    memset(rcpts[i], 0, p_colon - nonspace + 1);
			    
				    memcpy(rcpts[i], nonspace, p_colon-nonspace);

				    nonspace = strchr(rcpts[i], '\0')-1;
				    for( ; isspace(*nonspace); nonspace--)
					;
				    *((char *)(nonspace+1)) = '\0';

				    ++i;
				}
			}
		    else
			{
			    for(nonspace = p; *nonspace && isspace(*nonspace); nonspace++)
				;

			    if(*nonspace)
				{
				    rcpts[i] = strdup(nonspace);
				    
				    nonspace = strchr(rcpts[i], '\0')-1;
				    for( ; isspace(*nonspace); nonspace--)
					;
				    *((char *)(nonspace+1)) = '\0';
				}

			    break;
			}
		    
		    
		    p = p_colon+1;
		}
	}

    return count;
}


/*
 * Creates new list
 */
list_id parse_email(const char *const_fname, XmlFilesPool *xmlPool, list_db *ldb)
{
    XMLTag tag;
    XMLTag::SubtagsIterator sti;

    int res_xml, res = 0; 
    list_id          lid = LIST_ID_NONE;
    list_email_entry lee;

    memset(&lee, sizeof(lee), 0);

#define RETURN(x)				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0);


    std::string s;

    res = 0;
    res_xml = LoadXMLFileFromPool(const_fname, xmlPool, tag, tXMLEMailList, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_error("email list parse error: %s: %s\n", const_fname, s.c_str());
	    RETURN(-1);
	}
    
    lid = list_create(ldb, LIST_TYPE_EMAIL, const_fname, tag.AttrValue(XML_ATTR_LIST_DESCR));

    if(lid == LIST_ID_NONE)
	{
	    compiler_error("can\'t create new list (%s) in output database.\n", const_fname);
	    RETURN(-1);
	}

    for(sti = tag.SubtagsBegin(); sti != tag.SubtagsEnd(); sti++)
	{
	    char **rcpts = NULL;
	    size_t rcpts_count = 0;
	    const char *addr = sti->AttrValue(XML_ATTR_EMAIL_ADDRESS);

	    if(!addr) continue;
	    parse_rcpts(addr, rcpts_count, rcpts);
	    lee.description = sti->AttrValue(XML_ATTR_LIST_DESCR);

	    for(size_t i = 0; i < rcpts_count; i++)
		{
		    lee.address     = rcpts[i];
		    
		    if(list_email_add(ldb, lid, &lee))
			{
			    compiler_error("can\'t add entry to email list (%s) in output database.\n", const_fname);

			    RETURN(-1);
			}
		}

	    if(rcpts)
		{
		    for(size_t i = 0; i < rcpts_count; i++)
			free(rcpts[i]);
		    free(rcpts);
		}
	}
    
#undef RETURN 

  finish:

    if(res == 0)
	return lid;
    else
	return LIST_ID_NONE;

}

/*
 * Parsing of ip list.
 */
list_id parse_ip_list(const char *const_fname, XmlFilesPool *xmlPool, list_db *ldb)
{
    XMLTag tag;
    XMLTag::SubtagsIterator sti;

    int res_xml, res = 0; 
    list_id          lid = LIST_ID_NONE;

#define RETURN(x)				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0);


    std::string s;

    res = 0;
    res_xml = LoadXMLFileFromPool(const_fname, xmlPool, tag, tXMLIPList, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_error("ip list (%s) parse error: %s\n", const_fname, s.c_str());
	    RETURN(-1);
	}
    
    lid = list_create(ldb, LIST_TYPE_IP, const_fname, tag.AttrValue(XML_ATTR_LIST_DESCR));

    if(lid == LIST_ID_NONE)
	{
	    compiler_error("can\'t create new ip list (%s) in database.\n", const_fname);
	    RETURN(-1);
	}

    for(sti = tag.SubtagsBegin(); sti != tag.SubtagsEnd(); sti++)
	{
	    const char *mask = sti->AttrValue(XML_ATTR_IP_MASK);

	    if(!mask) continue;

	    if(list_ip_add(ldb, lid, mask))
		{
		    compiler_error("can\'t add entry to ip list (%s) in database.\n", const_fname);
		    RETURN(-1);
		}

	}
    
#undef RETURN 

  finish:

    if(res == 0)
	return lid;
    else
	return LIST_ID_NONE;

}

/*
 * Parsing of RBL list.
 */
list_id parse_rbl_list(const char *const_fname, XmlFilesPool *xmlPool, list_db *ldb)
{
    XMLTag tag;
    XMLTag::SubtagsIterator sti;

    int res_xml, res = 0; 
    list_id          lid = LIST_ID_NONE;
    list_rbl_entry lre;
    const char *weight;

#define RETURN(x)				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0);


    std::string s;

    res = 0;
    res_xml = LoadXMLFileFromPool(const_fname, xmlPool, tag, tXMLDNSList, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_error("rbl list (%s) parse error: %s\n", const_fname, s.c_str());
	    RETURN(-1);
	}
    
    lid = list_create(ldb, LIST_TYPE_RBL, const_fname, tag.AttrValue(XML_ATTR_LIST_DESCR));

    if(lid == LIST_ID_NONE)
	{
	    compiler_error("can\'t create new rbl list (%s) in list database.\n", const_fname);
	    RETURN(-1);
	}

    for(sti = tag.SubtagsBegin(); sti != tag.SubtagsEnd(); sti++)
	{
            memset(&lre, 0, sizeof(lre));

	    lre.domain = (char *)sti->AttrValue(XML_ATTR_DNSBLACKSERVICE_DOMAIN);

            weight = sti->AttrValue(XML_ATTR_DNSBLACKSERVICE_WEIGHT);
            if(weight)
                {
                    if(sscanf(weight, "%u", &lre.weight) < 1)
                        lre.weight = 0;
                }

	    lre.description = (char *)sti->AttrValue(XML_ATTR_LIST_DESCR);

	    if(list_rbl_add(ldb, lid, &lre))
		{
		    compiler_error("can\'t add entry to rbl list (%s) in database.\n", const_fname);
		    RETURN(-1);
		}

	}
    
#undef RETURN 

  finish:

    if(res == 0)
	return lid;
    else
	return LIST_ID_NONE;

}


/*
 * Loads profile from XML.
 */
profile *parse_profile(const char *const_fname, XmlFilesPool *xmlPool, list_db *ldb, content_filter *cf, unsigned int needed_type, unsigned int flags)
{
    XMLTag tag;
    XMLTag::SubtagsIterator rules_iterator, conditions_iterator, actions_iterator;

    profile *result = NULL;
    unsigned int profile_type = PROFILE_NONE;
    int res_xml;
    unsigned int rule_counter = 1;

#define RETURN_NULL				\
    do {					\
	goto error;				\
    } while(0)

    std::string s;


	res_xml = LoadXMLFileFromPool(const_fname, xmlPool, tag, tXMLProfile, &s, 0, CP_KOI8R);


    if(res_xml)
	{
	    if(!(flags & PARSE_PROFILE_CAN_BE_OMITTED))
		compiler_error("profile (%s) parse error: %s\n", const_fname, s.c_str());
	    RETURN_NULL;
	}

    if(strcmp(tag.AttrValue(XML_ATTR_PROFILE_TYPE), XML_ATTR_PROFILE_TYPE_COMMON) == 0)
	profile_type = PROFILE_COMMON;
    else if(strcmp(tag.AttrValue(XML_ATTR_PROFILE_TYPE), XML_ATTR_PROFILE_TYPE_PRIVATE) == 0)
	profile_type = PROFILE_PRIVATE;
    else
	RETURN_NULL;

    if(needed_type != PROFILE_NONE && profile_type != needed_type)
	RETURN_NULL;


    {
	size_t   rcpts_count = 0;
	char   **rcpts = NULL;
	size_t   i = 0;
	const char *profile_rcpts = tag.AttrValue(XML_ATTR_PROFILE_RCPT);
	const char *profile_lid   = tag.AttrValue(XML_ATTR_PROFILE_RCPTLIST);
	list_id     lid = LIST_ID_NONE;
	
	
	if(profile_type == PROFILE_COMMON)
	    {
		if(profile_rcpts || profile_lid)
		    RETURN_NULL;
	    }
	else if(profile_type == PROFILE_PRIVATE)
	    {
		if(profile_rcpts)
		    parse_rcpts(profile_rcpts, rcpts_count, rcpts);
		else
		    {
			lid = list_find(ldb, LIST_TYPE_EMAIL, profile_lid);
			if(lid == LIST_ID_NONE)
			    lid = parse_email(profile_lid, xmlPool, ldb);
			
			if(lid == LIST_ID_NONE)
			    {
				compiler_error("profile %s: can\'t get get list of recipients\n");
				RETURN_NULL;
			    }
		    }
	    }

	if(lid != LIST_ID_NONE)
	    result = profile_create(tag.AttrValue(XML_ATTR_PROFILE_NAME), NULL, 
				    /* tag.AttrValue(XML_ATTR_PROFILE_DESCR), */ 
				    profile_type, lid, NULL);
	else
	    {
		result = profile_create(tag.AttrValue(XML_ATTR_PROFILE_NAME), NULL, 
					/* tag.AttrValue(XML_ATTR_PROFILE_DESCR), */ 
					profile_type, rcpts_count, rcpts);
	    }

	if(rcpts)
	    {
		for(i = 0; i < rcpts_count; i++)
		    free(rcpts[i]);
		
		free(rcpts);
	    }
    }

    if(!result)
	RETURN_NULL;


    for(rules_iterator = tag.SubtagsBegin(); rules_iterator != tag.SubtagsEnd(); rules_iterator++, rule_counter++)
	{
	    const XMLTag *conditions = rules_iterator->Subtag(XML_TAG_PROFILE_CONDITIONS); 
	    const XMLTag *actions    = rules_iterator->Subtag(XML_TAG_PROFILE_ACTIONS); 

	    rule   *r          = profile_create_rule(result);

#define SKIP_RULE				\
	    do {				\
		goto bad_rule;			\
	    } while(0)

	    for(conditions_iterator = conditions->SubtagsBegin(); 
		conditions_iterator != conditions->SubtagsEnd();
		conditions_iterator++)
		{
		    unsigned int   cond_flag = CONDITION_NORMAL;
		    const char    *attr      = NULL;
		    
		    /*
		     * Common attributes.
		     */

		    if((attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_COND_NEGATIVE)))
			{
			    if(strcmp(attr, XML_ATTR_PROFILE_COND_NEGATIVE_YES) == 0)
				cond_flag |= CONDITION_NOT;
			}

		    /*
		     * Specific conditions.
		     */

		    if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_SMTP_FROM) == 0)
			{
			    if(rule_add_condition_smtp_from(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_EMAIL)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SMTPFrom condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_SMTP_FROM_LIST) == 0)
			{
			    const char *list_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIST);
			    list_id lid = LIST_ID_NONE;

			    if(!list_name)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SMTPFromList condition, bad list name.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    
			    lid = list_find(ldb, LIST_TYPE_EMAIL, list_name);
			    if(lid == LIST_ID_NONE)
				lid = parse_email(list_name, xmlPool, ldb);

			    if(lid == LIST_ID_NONE)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t get corresponding list for SMTPFromList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_smtp_from_list(r, cond_flag, lid))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SMTPFromList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_SMTP_TO) == 0)
			{
			    if(rule_add_condition_smtp_to(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_EMAIL)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SMTPTo condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_SMTP_TO_LIST) == 0)
			{
			    const char *list_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIST);
			    list_id lid = LIST_ID_NONE;

			    if(!list_name)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SMTPToList condition, bad list name.\n", 
							 const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    
			    lid = list_find(ldb, LIST_TYPE_EMAIL, list_name);
			    if(lid == LIST_ID_NONE)
				lid = parse_email(list_name, xmlPool, ldb);

			    if(rule_add_condition_smtp_to_list(r, cond_flag, lid))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SMTPToList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_HAS_HEADER) == 0)
			{
			    if(rule_add_condition_has_header(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_HEADER)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HasHeader condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_HEADER_MATCH) == 0)
			{
			    if(rule_add_condition_header_match(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_HEADER), 
							       conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGEXP)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HeaderMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_ATTRIBUTE_MATCH) == 0)
			{
			    if(rule_add_condition_attribute_match(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_ATTRIBUTE), 
                                                                  conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGEXP)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add AttributeMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_DCC_MATCH) == 0)
			{
			    if(rule_add_condition_dcc_match(r, cond_flag, 
                                                            conditions_iterator->AttrValue(XML_ATTR_PROFILE_DCC_MATCH_BODY), 
                                                            conditions_iterator->AttrValue(XML_ATTR_PROFILE_DCC_MATCH_FUZ1), 
                                                            conditions_iterator->AttrValue(XML_ATTR_PROFILE_DCC_MATCH_FUZ2)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add DCCMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_CONTENT_TYPE_MATCH) == 0)
			{
			    if(rule_add_condition_content_type_match(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGEXP)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add ContentTypeMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_FILE_NAME_MATCH) == 0)
			{
                            /* XXX 
                             * Filenames translated to 1251 in content-filtration library, 
                             * but here all data in koi8-r. Hence we must convert data to 
                             * 1251.
                             */

                            const char *source = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGEXP);
                            char cp1251_buf[50*1024];
                            size_t length;

                            length = strlen(source);
                            if(length >= sizeof(cp1251_buf))
                                length = sizeof(cp1251_buf)-1;

                            memcpy(cp1251_buf, source, length);
                            cp1251_buf[length] = '\0';

                            ConvertString(cp1251_buf, length, CP_KOI8R, CP_CYRILLIC);

			    if(rule_add_condition_file_name_match(r, cond_flag, cp1251_buf))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add FileNameMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_MSG_SIZE) == 0)
			{
			    unsigned int limit = 0;

			    attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIMIT);
			    if(!attr) 
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add MsgSize condition, because limit field not specified.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(sscanf(attr, "%u", &limit) == 0)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add MsgSize condition, bad format of limit field.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_msg_size(r, cond_flag, limit))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add MsgSize condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_FROM) == 0)
			{
			    attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_MASK);
			    if(!attr) 
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPFrom condition, mask not specified.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_ip_from(r, cond_flag, attr))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add IPFrom condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_FROM_NOTINDNS) == 0)
			{
			    if(rule_add_condition_ip_from_not_in_dns(r, cond_flag))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromNotInDNS condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_HAS_ATTACHMENTS) == 0)
			{
			    if(rule_add_condition_has_attachments(r, cond_flag))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add HasAttachments condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_UNKNOWN_CONTENT) == 0)
			{
			    if(rule_add_condition_unknown_content(r, cond_flag))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add UnknownContent condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_VIRUS_MATCH) == 0)
			{
			    if(rule_add_condition_virus_match(r, cond_flag, conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGEXP)))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add VirusMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_FROM_LIST) == 0)
			{
			    const char *list_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIST);
			    list_id lid = LIST_ID_NONE;

			    if(!list_name)
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromList condition, bad list name.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    
			    lid = list_find(ldb, LIST_TYPE_IP, list_name);
			    if(lid == LIST_ID_NONE)
				lid = parse_ip_list(list_name, xmlPool, ldb);

			    if(lid == LIST_ID_NONE)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t get corresponding list for IPFromList condition.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_ip_from_list(r, cond_flag, lid))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_FROM_DNSBLACK) == 0)
			{
			    const char *list_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIST);
			    list_id lid = LIST_ID_NONE;

			    if(!list_name)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromDNSBlackList condition, bad list name.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    
			    lid = list_find(ldb, LIST_TYPE_RBL, list_name);
			    if(lid == LIST_ID_NONE)
				lid = parse_rbl_list(list_name, xmlPool, ldb);

			    if(lid == LIST_ID_NONE)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t get corresponding list for IPFromDNSBlackList condition.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_ip_from_rbl(r, cond_flag, lid))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromDNSBlackList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_CATEGORY_MATCH) == 0)
			{
			    
				//const rubricator *rub = filter_get_rubricator(cf);
				const rubricator *rub = g_CFilterDll.GetRubricator();

			    const char *cat_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_CATEGORY);
			    const char /**ptr,*/ *id;

			    if(!cat_name || cat_name[0] == '\0')
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add CategoryMatch condition, invalid category name.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    id = cat_name;
			    const category *cur_cat = NULL;

			    for( ; ; )
				{
				    const char *ptr = strchr(id, '/');
				    
				    if(cur_cat)
					{
					    for(size_t i = 0; i < cur_cat->sub.count; i++)
						{
						    if(ptr)
							{
							    if(strlen(cur_cat->sub.categories[i]->textID) == ptr-id &&
							       strncmp(cur_cat->sub.categories[i]->textID, id, ptr-id) == 0)
								{
								    cur_cat = cur_cat->sub.categories[i];
								    break;
								}
							}
						    else
							{
							    if(strcmp(cur_cat->sub.categories[i]->textID, id) == 0)
								{
								    cur_cat = cur_cat->sub.categories[i];
								    break;
								}
							}
								   
						}
					}
				    else
					{
					    /* 
					     * Root of rubricator.
					     */
					    for(size_t i = 0; i < rub->top.count; i++)
						{
						    if(ptr)
							{
							    if(strlen(rub->top.categories[i]->textID) == ptr-id &&
							       strncmp(rub->top.categories[i]->textID, id, ptr-id) == 0)
								{
								    cur_cat = rub->top.categories[i];
								    break;
								}
							}
						    else
							{
							    if(strcmp(rub->top.categories[i]->textID, id) == 0)
								{
								    cur_cat = rub->top.categories[i];
								    break;
								}
							}
								   
						}
					}

				    if(ptr == NULL)
					break;
				    else
					id = ptr+1;
				}

			    if(cur_cat == NULL)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add CategoryMatch condition, can\'t find category.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    unsigned int cat_id = cur_cat->CID;

			    if(rule_add_condition_category_match(r, cond_flag, cat_id))
				{
				    if(compiler_warning("Profile %s, rule %u: Can\'t add CategoryMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_REGISTER) == 0)
			{
			    const char *equal  = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_EQUAL);
			    const char *less   = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_LESS);
			    const char *more   = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_MORE);

			    const char *number = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_NUMBER);

			    unsigned int type    = 0;
			    unsigned int reg_num = 0;
			    int          value   = 0;

			    if(!number)
				{
				    if(compiler_warning("Profile %s, rule %u: not specified number of register in register condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    sscanf(number, "%u", &reg_num);

			    if(!equal && !less && !more)
				{
				    if(compiler_warning("Profile %s, rule %u: not specified what to check in register condition: "
							"equal, less or more.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if((equal && (less || more)) ||
			       (less && (equal || more)) ||
			       (more && (equal || less)))
				{
				    if(compiler_warning("Profile %s, rule %u: to many checks specified for register condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(equal)
				{
				    type = CONDITION_REGISTER_EQUAL;
				    sscanf(equal, "%i", &value);
				}

			    if(less)
				{
				    type = CONDITION_REGISTER_LESS;
				    sscanf(less, "%i", &value);
				}

			    if(more)
				{
				    type = CONDITION_REGISTER_MORE;
				    sscanf(more, "%i", &value);
				}
				

			    if(rule_add_condition_register(r, cond_flag, reg_num, type, value))
				{
				    if(compiler_warning("Profile %s, rule %u: Can\'t add Register condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
  		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_BAYES_MATCH) == 0)
			{
			    unsigned int rate = 0;

			    attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_BAYES_MATCH_RATE);
			    if(!attr) 
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add BayesMatch condition, because rate field not specified.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(sscanf(attr, "%u", &rate) == 0)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add BayesMatch condition, bad format of rate field.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_bayes_match(r, cond_flag, rate))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add BayesMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else
			{
			    /*
			     * XXX Here must be code for returning error at unknown tag.
			     */
			    //printf("Very strange: %s\n", conditions_iterator->Name());
			}
		}

	    for(actions_iterator = actions->SubtagsBegin(); 
		actions_iterator != actions->SubtagsEnd();
		actions_iterator++)
		{
		    if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_ACCEPT) == 0)
			{
			    if(rule_add_action_accept(r))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add Accept action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_BLACKHOLE) == 0)
			{
			    if(rule_add_action_blackhole(r))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add BlackHole action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_REJECT) == 0)
			{
			    if(rule_add_action_reject(r))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add Reject action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_BOUNCE) == 0)
			{
			    if(rule_add_action_bounce(r))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add Reject action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_SKIP) == 0)
			{
			    if(rule_add_action_skip(r))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add Skip action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_HEADER_ADD) == 0)
			{
			    if(rule_add_action_header_add(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_HEADER),
							  actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_VALUE)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HeaderAdd action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_HEADER_PREPEND) == 0)
			{
			    if(rule_add_action_header_prepend(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_HEADER),
							  actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_VALUE)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HeaderPrepend action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_HEADER_NEW) == 0)
			{
			    if(rule_add_action_header_new(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_HEADER),
							  actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_VALUE)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HeaderNew action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_HEADER_REPLACE) == 0)
			{
			    if(rule_add_action_header_change(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_HEADER),
							     actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_VALUE)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HeaderReplace action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_HEADER_DELETE) == 0)
			{
			    if(rule_add_action_header_remove(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_HEADER)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HeaderDelete action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_RCPT_REPLACE_ALL) == 0)
			{
			    const char *rcpt_analyze = actions_iterator->AttrValue(XML_ATTR_PROFILE_RCPT_NEW);
			    char **rcpts = NULL;
			    size_t count = 0, i;

			    if(!rcpt_analyze)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t parse recipient lst in RcptReplace action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    parse_rcpts(rcpt_analyze, count, rcpts);

			    int res = rule_add_action_send(r, ACTION_SEND_NEW, count, rcpts);

			    if(rcpts)
				{
				    for(i = 0; i < count; i++)
					if(rcpts[i]) free(rcpts[i]);
				    free(rcpts);
				}

			    if(res)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RcptReplaceAll action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_RCPT_ADD) == 0)
			{
			    const char *rcpt_analyze = actions_iterator->AttrValue(XML_ATTR_PROFILE_RCPT_NEW);
			    char **rcpts = NULL;
			    size_t count = 0, i;

			    if(!rcpt_analyze)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t parse recipient lst in RcptAdd action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    parse_rcpts(rcpt_analyze, count, rcpts);

			    int res = rule_add_action_send(r, ACTION_SEND_ADD, count, rcpts);

			    if(rcpts)
				{
				    for(i = 0; i < count; i++)
					if(rcpts[i]) free(rcpts[i]);
				    free(rcpts);
				}

			    if(res)
				{
 				    if(compiler_warning("Profile %s, rule %u: Can\'t add RcptAdd action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_RCPT_ADD) == 0)
			{
			    const char *rcpt_analyze = actions_iterator->AttrValue(XML_ATTR_PROFILE_RCPT_NEW);
			    char **rcpts = NULL;
			    size_t count = 0, i;

			    if(!rcpt_analyze)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t parse recipient lst in RcptAdd action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    parse_rcpts(rcpt_analyze, count, rcpts);

			    int res = rule_add_action_send(r, ACTION_SEND_ADD, count, rcpts);

			    if(rcpts)
				{
				    for(i = 0; i < count; i++)
					if(rcpts[i]) free(rcpts[i]);
				    free(rcpts);
				}

			    if(res)
				{
 				    if(compiler_warning("Profile %s, rule %u: Can\'t add RcptAdd action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_SYSLOG) == 0)
			{
			    const char *message  = actions_iterator->AttrValue(XML_ATTR_PROFILE_SYSLOG_MESSAGE);
			    const char *priority = actions_iterator->AttrValue(XML_ATTR_PROFILE_SYSLOG_PRIORITY);
			    const char *level    = actions_iterator->AttrValue(XML_ATTR_PROFILE_SYSLOG_LEVEL);

			    unsigned int mask = 0;

			    if(!message)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add DoSysLog action without any message.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(priority)
				{
				    if(strcmp(priority, XML_ATTR_PROFILE_SYSLOG_PRIORITY_DEBUG) == 0)
					mask |= LOGGER_DEBUG;
				    else if (strcmp(priority, XML_ATTR_PROFILE_SYSLOG_PRIORITY_INFO) == 0)
					mask |= LOGGER_INFO;
				    else if (strcmp(priority, XML_ATTR_PROFILE_SYSLOG_PRIORITY_WARNING) == 0)
					mask |= LOGGER_WARNING;
				    else if (strcmp(priority, XML_ATTR_PROFILE_SYSLOG_PRIORITY_ERROR) == 0)
					mask |= LOGGER_ERROR;
				    else
					{
					    if(compiler_warning("profile %s, rule %u: can\'t add DoSysLog action: unknown priority (%s) name.\n", 
								const_fname, rule_counter, priority))
						RETURN_NULL;
					    else
						SKIP_RULE;
					}
				}
			    else
				mask |= LOGGER_INFO;

			    if(level)
				{
				    if(strcmp(level, XML_ATTR_PROFILE_SYSLOG_LEVEL_NORMAL) == 0)
					mask |= LOGGER_VERBOSE_NORMAL;
				    else if (strcmp(level, XML_ATTR_PROFILE_SYSLOG_LEVEL_INFORMATIVE) == 0)
					mask |= LOGGER_VERBOSE_INFORMATIVE;
				    else if (strcmp(level, XML_ATTR_PROFILE_SYSLOG_LEVEL_HIGH) == 0)
					mask |= LOGGER_VERBOSE_HIGH;
				    else if (strcmp(level, XML_ATTR_PROFILE_SYSLOG_LEVEL_EXTREME) == 0)
					mask |= LOGGER_VERBOSE_EXTREME;
				    else if (strcmp(level, XML_ATTR_PROFILE_SYSLOG_LEVEL_FLOOD) == 0)
					mask |= LOGGER_VERBOSE_FLOOD;
				    else
					{
					    if(compiler_warning("profile %s, rule %u: can\'t add DoSysLog action: unknown level (%s) name.\n", 
								const_fname, rule_counter, level))
						RETURN_NULL;
					    else
						SKIP_RULE;
					}
				}
			    else
				mask |= LOGGER_VERBOSE_INFORMATIVE;

			    int res = rule_add_action_syslog(r, mask, message);

			    if(res)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add DoSysLog action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_REGISTER_SET) == 0)
			{
			    const char *number = actions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_NUMBER);
			    const char *value  = actions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_VALUE);

			    unsigned int reg_num = 0;
			    int          reg_val = 0;

			    if(!number || !value)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RegisterSet action, no register number or value.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    sscanf(number, "%u", &reg_num);
			    sscanf(value, "%i", &reg_val);

			    if(rule_add_action_register_set(r, reg_num, reg_val))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RegisterSet action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_REGISTER_ADD) == 0)
			{
			    const char *number = actions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_NUMBER);
			    const char *value  = actions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_VALUE);

			    unsigned int reg_num = 0;
			    int          reg_val = 0;

			    if(!number || !value)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RegisterAdd action, no register number or value.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    sscanf(number, "%u", &reg_num);
			    sscanf(value, "%i", &reg_val);

			    if(rule_add_action_register_add(r, reg_num, reg_val))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RegisterAdd action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_SEND_NOTIFICATION) == 0)
			{
			    if(rule_add_action_send_notification(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_ENTRY)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SendLogEntry action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_SEND_LOG_ENTRY) == 0)
			{
			    if(rule_add_action_send_log_entry(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_ACTION_ENTRY)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add SendLogEntry action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_DCC_REPORT) == 0)
			{
			    if(rule_add_action_dcc_report(r, actions_iterator->AttrValue(XML_ATTR_PROFILE_DCC_REPORT_COUNT)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add DccReport action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else
			{
			    /* XXX Nothing. */
			} 
		}

	    continue;

	    /*
	     * Error handling.
	     */
	  bad_rule:
	    
	    if(r)
		{
		    if(compiler_warning("profile %s, rule %u: there is an errors in this rule, skipping it.\n", const_fname, rule_counter))
			RETURN_NULL;

		    if(profile_remove_last_rule(result))
			{
			    compiler_error("internal error: can\'t remove last rule from profile.\n");
			    RETURN_NULL;
			}
		}

#undef SKIP_RULE
	}


    return result;

#undef RETURN_NULL

  error:
    
    if(result)
	profile_destroy(result);

    return NULL;
}

int parse_profile_db(list_db *ldb, profile_db *pdb, content_filter *cf, XmlFilesPool *xmlPool)
{
    
    /*
     * Parse hidden profiles.
     */
    
    profile *p = parse_profile(PROFILE_FORMAL_CHECKS, xmlPool, 
			       ldb, cf, PROFILE_COMMON, PARSE_PROFILE_CAN_BE_OMITTED);
    if(p)
		profile_db_add(pdb, p);


	/*
     * Parse DETECT-KPAS profile.
     */
    
    p = parse_profile(PROFILE_KPAS_DETECT, xmlPool, 
			       ldb, cf, PROFILE_COMMON, PARSE_PROFILE_DEFAULTS);
    if(p)
		profile_db_add(pdb, p);
	else
		return 1;

   	/*
     * Parse DETECT-KPAS-SUPPL profile.
     */
    
    p = parse_profile(PROFILE_KPAS_DETECT_SUPPL, xmlPool, 
			       ldb, cf, PROFILE_COMMON, PARSE_PROFILE_DEFAULTS);
    if(p)
		profile_db_add(pdb, p);

   
    return 0;

	//?????: Who is an owner of profile structure? Who should free that memory?
}

/*
 * <eof parse.cpp>
 */
