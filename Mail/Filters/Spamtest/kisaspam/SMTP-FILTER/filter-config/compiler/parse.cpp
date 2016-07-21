/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: parse.cpp                                                             *
 * Created: Mon Apr 22 21:41:17 2002                                           *
 * Desc: Implementation of parsing processes.                                  *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  parse.cpp
 * \brief Implementation of parsing processes.
 */

#include <libgen.h>
#include <ctype.h>

#include <CodePages.h>

#ifdef WITH_PCRE
#include <pcre.h>
#endif

#ifdef WITHOUT_PCRE
#include <sys/types.h>
#include <regex.h>
#endif

#include "compiler.h"
#include "parse.h"


#include "smtp-filter/filter-config/xmlfiles/XMLFiles.h"
#include "smtp-filter/filter-config/xmlfiles/keywords.h"
#include "smtp-filter/paths/paths.h"
#include "smtp-filter/common/config.h"
#include "smtp-filter/common/logger.h"
#include "smtp-filter/common/rec_types.h"
#include "filter.h"


#include "samples.h"

#define RBL_THRESHOLD 100


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
 *  Expand ? and * into regular expression (\S and \S*)  
 */

inline int expand_masks(size_t count, char **rcpts)
{
    int asterik_changes = 0;	
    char *p, *d, *buffer;
    size_t new_size;
    int ques, asterik, dots;

    for(int i = 0; i < count; i++)
    {
        //processing  rcpts[i]
	//printf("BEFORE: %s\n",rcpts[i]);
	//get size 

	ques = 0;
	asterik = 0;
	dots = 0;
	for(p = rcpts[i]; *p != '\x0'; p++)
	{
		if(*p == '*') asterik++;
		if(*p == '?') ques++;
		if(*p == '.') dots++;
	}

	if(asterik != 0 || ques != 0)
	{
		new_size = strlen(rcpts[i]) + 2*asterik + ques + dots;

		buffer = (char*)malloc(new_size);
		d = buffer;

		for(p = rcpts[i] ; *p != '\x0'; p++)
		{
			if(*p == '*')
			{
			        *d++ = '\\';
				*d++ = 'S';
				*d++ = '*';
			}
			else if (*p == '?')
			{
			        *d++ = '\\';
				*d++ = 'S';
			}
			else if (*p == '.')
			{
			        *d++ = '\\';
				*d++ = '.';
			}
			else
			{
			   	*d++ = *p;
			}
		}
		*d = '\x0';
		free(rcpts[i]);
		rcpts[i] = buffer;
		//printf("AFTER: %s\n",rcpts[i]);
	}
    }
    return asterik_changes;
}

/*
 * Creates new list
 */
list_id parse_email(const char *const_fname, list_db *ldb)
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
    res_xml = LoadXMLFile(full_path_email_list(const_fname), tag, tXMLEMailList, &s, 0, CP_KOI8R);

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
list_id parse_ip_list(const char *const_fname, list_db *ldb)
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
    res_xml = LoadXMLFile(full_path_ip_list(const_fname), tag, tXMLIPList, &s, 0, CP_KOI8R);

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
dns_checker_check_id_t parse_rbl_list(const char *const_fname, profile_db *pdb)
{
    XMLTag tag;
    XMLTag::SubtagsIterator sti;

    int res_xml, err, res = 0; 
    size_t i;
    dns_checker_check_id_t dns_cid = DNS_CHECKER_CHECK_ID_NONE;
    dns_checker_rbl_data_t *rbls = NULL;
    const char *weight;

#define RETURN(x)				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0);


    std::string s;

    res = 0;
    res_xml = LoadXMLFile(full_path_dns_list(const_fname), tag, tXMLDNSList, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_error("rbl list (%s) parse error: %s\n", const_fname, s.c_str());
	    RETURN(DNS_CHECKER_CHECK_ID_NONE);
	}

    rbls = (dns_checker_rbl_data_t*)malloc(sizeof(dns_checker_rbl_data_t) * tag.SubtagsNumber());
    
    if(rbls == NULL)
	{
	    compiler_error("can\'t create new rbl list (%s): memory not enought.\n", const_fname);
	    RETURN(DNS_CHECKER_CHECK_ID_NONE);
	}
    memset(rbls, 0, sizeof(dns_checker_rbl_data_t) * tag.SubtagsNumber());

    i = 0;

    for(sti = tag.SubtagsBegin(); sti != tag.SubtagsEnd(); sti++)
	{

	    rbls[i].domain = (char *)sti->AttrValue(XML_ATTR_DNSBLACKSERVICE_DOMAIN);

            weight = sti->AttrValue(XML_ATTR_DNSBLACKSERVICE_WEIGHT);
            if(weight)
                {
                    if(sscanf(weight, "%u", &rbls[i].weight) < 1)
                        rbls[i].weight = RBL_THRESHOLD;
                }
            else
		rbls[i].weight = RBL_THRESHOLD;
           

	    // lre.description = (char *)sti->AttrValue(XML_ATTR_LIST_DESCR);

	    i++;
	}

    dns_cid = dns_checker_add_rbl(profile_db_get_dns_checker(pdb), rbls, i, &err);
//    printf("DNS_CID: %u\n", dns_cid);
    if(err)
    {
	    compiler_error("couldn't construct rbl request template from list (%s), error(%d): %s\n", const_fname, err, dns_checker_strerror(err));
	    RETURN(DNS_CHECKER_CHECK_ID_NONE);
    }
     
    
#undef RETURN 

  finish:
    if(rbls)
        free(rbls);

    return dns_cid;
}

/*
 * Loads profile from XML.
 */
profile *parse_profile(const char *const_fname, list_db *ldb, profile_db *pdb, content_filter *cf, unsigned int needed_type, unsigned int flags)
{
    printf("Parsing %s\n",const_fname);
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

    
    res_xml = LoadXMLFile(const_fname[0] == '/' ? const_fname : full_path_profile(const_fname), tag, tXMLProfile, &s, 0, CP_KOI8R);

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
	const char *profile_mask  = tag.AttrValue(XML_ATTR_PROFILE_RCPTMASK);
	const char *profile_regexp = tag.AttrValue(XML_ATTR_PROFILE_RCPTREGEXP);
	list_id     lid = LIST_ID_NONE;
	
	
	if(profile_type == PROFILE_COMMON)
	    {
		if(profile_rcpts || profile_lid || profile_mask || profile_regexp)
		    RETURN_NULL;
	    }
	else if(profile_type == PROFILE_PRIVATE)
	    {
		if(profile_rcpts)
		{
		    parse_rcpts(profile_rcpts, rcpts_count, rcpts);
		    printf(" rcpts= %d\n",rcpts_count);
		}
		else if(profile_mask)
		{
		    parse_rcpts(profile_mask, rcpts_count, rcpts);
		    printf(" rcpts_masks= %d\n",rcpts_count);
		    if(expand_masks(rcpts_count, rcpts) > rcpts_count)
			compiler_warning("profile %s: too many * in recipients masks\n", const_fname);
		    profile_type |= PROFILE_USE_REGEX;
		}
		else if(profile_regexp)
		{
		    //check regex
#ifdef WITH_PCRE
		    pcre *re;
		    const char *error;
		    int erroffset;
		    re = pcre_compile(profile_regexp, 0, &error, &erroffset, NULL);
		    if(re == NULL)
		    {
		 	compiler_error("profile %s: wrong Perl regular expression in recipients mask: offset %d, error: %s\n", const_fname, erroffset, error);
			RETURN_NULL;
		    }
		    pcre_free(re);
#endif
#ifdef WITHOUT_PCRE
		    regex_t regex;
		    if(regcomp(&regex, profile_regexp, REG_EXTENDED | REG_NOSUB))
		    {
		 	compiler_error("profile %s: wrong regex in recipients mask\n", const_fname);
			RETURN_NULL;
	            }
		    regfree(&regex);
#endif

		    rcpts_count = 1;
	    	    rcpts = (char **)malloc(rcpts_count*sizeof(char *));
	    	    memset(rcpts, 0, rcpts_count*sizeof(char *));

		    rcpts[0] = strdup(profile_regexp);

		    profile_type |= PROFILE_USE_REGEX;

		    printf(" rcpts_regex= %s\n",rcpts[0]);

		}
		else
		{   if(profile_lid)
		    {
		         lid = list_find(ldb, LIST_TYPE_EMAIL, profile_lid);
		         if(lid == LIST_ID_NONE)
		        	lid = parse_email(profile_lid, ldb);
			
		         if(lid == LIST_ID_NONE)
		         {
		 		compiler_error("profile %s: can\'t get get list of recipients\n", const_fname);
				RETURN_NULL;
			 }
		    }
		    else
		    {
			compiler_error("profile %s: error only one attribute (%s, %s or %s) must be!\n", const_fname, XML_ATTR_PROFILE_RCPT, XML_ATTR_PROFILE_RCPTMASK, XML_ATTR_PROFILE_RCPTREGEXP);
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
				lid = parse_email(list_name, ldb);

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
				lid = parse_email(list_name, ldb);

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
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_HAS_ATTRIBUTE) == 0)
			{
			    attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_ATTRIBUTE);
			    if(rule_add_condition_has_attribute(r, cond_flag, attr))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add HasAttribute condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
                            if(strcmp(attr, MAIL_ATTR_IP_FROM_NAME) == 0)
				dns_checker_add_ip_from_resolve(profile_db_get_dns_checker(pdb));
			    else if(strcmp(attr, MAIL_ATTR_SPF_STATUS) == 0)
				dns_checker_add_spf_resolve(profile_db_get_dns_checker(pdb));
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_ATTRIBUTE_MATCH) == 0)
			{
			    attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_ATTRIBUTE);
			    if(rule_add_condition_attribute_match(r, cond_flag, attr, 
                                                                  conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGEXP)))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add AttributeMatch condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
                            if(strcmp(attr, MAIL_ATTR_IP_FROM_NAME) == 0)
				dns_checker_add_ip_from_resolve(profile_db_get_dns_checker(pdb));
			    else if(strcmp(attr, MAIL_ATTR_SPF_STATUS) == 0)
				dns_checker_add_spf_resolve(profile_db_get_dns_checker(pdb));
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
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_HELO) == 0)
			{
			    attr = conditions_iterator->AttrValue(XML_ATTR_PROFILE_MASK);
			    if(!attr) 
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPHelo condition, mask not specified.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_ip_helo(r, cond_flag, attr))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add IPHelo condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    dns_checker_add_helo_resolve(profile_db_get_dns_checker(pdb));

			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_FROM_NOTINDNS) == 0)
			{
			    dns_checker_add_ip_from_resolve(profile_db_get_dns_checker(pdb));

			    if(rule_add_condition_ip_from_not_in_dns(r, cond_flag))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromNotInDNS condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_MAIL_FROM_ROUTABLE) == 0)
			{
			    if(rule_add_condition_mail_from_routable(r, cond_flag))
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add MailFromRoutable condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    dns_checker_add_mail_from_resolve(profile_db_get_dns_checker(pdb));
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
				lid = parse_ip_list(list_name, ldb);

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
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_HELO_LIST) == 0)
			{
			    const char *list_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIST);
			    list_id lid = LIST_ID_NONE;

			    if(!list_name)
				{
				    if(compiler_warning("profile %s, rule %u: can\'t add IPHeloList condition, bad list name.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    
			    lid = list_find(ldb, LIST_TYPE_IP, list_name);
			    if(lid == LIST_ID_NONE)
				lid = parse_ip_list(list_name, ldb);

			    if(lid == LIST_ID_NONE)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t get corresponding list for IPHeloList condition.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_ip_helo_list(r, cond_flag, lid))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPHeloList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			    dns_checker_add_helo_resolve(profile_db_get_dns_checker(pdb));		    
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_IP_FROM_DNSBLACK) == 0)
			{
			    const char *list_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_LIST);
                            dns_checker_check_id_t cid = DNS_CHECKER_CHECK_ID_NONE;  

			    if(!list_name)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromDNSBlackList condition, bad list name.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
						     
			    cid = profile_db_rbl_list_find(pdb, list_name);

           
			    if(cid == DNS_CHECKER_CHECK_ID_NONE)
			        {
				    cid = parse_rbl_list(list_name, pdb);
				    //printf("CID: %u\n", cid);
                                    if(cid != DNS_CHECKER_CHECK_ID_NONE)
                                    	cid = profile_db_rbl_list_add(pdb, list_name, cid);
                                }
				
			
			    if(cid == DNS_CHECKER_CHECK_ID_NONE)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t get corresponding rbl template for IPFromDNSBlackList condition.\n", 
							const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(rule_add_condition_ip_from_rbl(r, cond_flag, cid))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add IPFromDNSBlackList condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_CATEGORY_MATCH) == 0)
			{
			    const rubricator *rub = filter_get_rubricator(cf);
			    const char *cat_name = conditions_iterator->AttrValue(XML_ATTR_PROFILE_CATEGORY);
			    const char *ptr, *id;

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
		    else if(strcmp(conditions_iterator->Name(), XML_TAG_PROFILE_RATE) == 0)
			{
			    const char *equal  = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_EQUAL);
			    const char *less   = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_LESS);
			    const char *more   = conditions_iterator->AttrValue(XML_ATTR_PROFILE_REGISTER_MORE);

			    unsigned int type    = 0;
			    int          value   = 0;

			    if(!equal && !less && !more)
				{
				    if(compiler_warning("Profile %s, rule %u: not specified what to check in rate register condition: "
							"equal, less or more.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if((equal && (less || more)) ||
			       (less && (equal || more)) ||
			       (more && (equal || less)))
				{
				    if(compiler_warning("Profile %s, rule %u: to many checks specified for rate register condition.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    if(equal)
				{
				    type = CONDITION_RATE_EQUAL;
				    sscanf(equal, "%i", &value);
				}

			    if(less)
				{
				    type = CONDITION_RATE_LESS;
				    sscanf(less, "%i", &value);
				}

			    if(more)
				{
				    type = CONDITION_RATE_MORE;
				    sscanf(more, "%i", &value);
				}
				

			    if(rule_add_condition_rate(r, cond_flag, type, value))
				{
				    if(compiler_warning("Profile %s, rule %u: Can\'t add Rate register condition.\n", const_fname, rule_counter))
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
				    else if (strcmp(priority, XML_ATTR_PROFILE_SYSLOG_PRIORITY_STATS) == 0)
					mask |= LOGGER_STATS;
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
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_RATE_SET) == 0)
			{
			    const char *value  = actions_iterator->AttrValue(XML_ATTR_PROFILE_RATE_VALUE);

			    int          reg_val = 0;

			    if(!value)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RateSet action, no register value.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    sscanf(value, "%i", &reg_val);

			    if(rule_add_action_rate_set(r, reg_val))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RateSet action.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}
			}
		    else if(strcmp(actions_iterator->Name(), XML_TAG_PROFILE_RATE_ADD) == 0)
			{
			    const char *value  = actions_iterator->AttrValue(XML_ATTR_PROFILE_RATE_VALUE);

			    int          reg_val = 0;

			    if(!value)
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RateAdd action, no register value.\n", const_fname, rule_counter))
					RETURN_NULL;
				    else
					SKIP_RULE;
				}

			    sscanf(value, "%i", &reg_val);

			    if(rule_add_action_rate_add(r, reg_val))
				{
 				    if(compiler_warning("profile %s, rule %u: can\'t add RateAdd action.\n", const_fname, rule_counter))
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

int parse_profile_db(list_db *ldb, profile_db *pdb, content_filter *cf)
{
    XMLTag tag;
    XMLTag::SubtagsIterator refs_iterator;
    
    const char *const_fname = full_path_profiles();
    
    int res_xml;

    std::string s;
    
    res_xml = LoadXMLFile(const_fname, tag, tXMLProfilesRefs, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_error("profiles (%s) parse error: %s\n", const_fname, s.c_str());
	    return 1;
	}

    const XMLTag *common_profiles  = tag.Subtag(XML_TAG_PROFILES_COMMON); 
    const XMLTag *private_profiles = tag.Subtag(XML_TAG_PROFILES_PRIVATE); 

    /*
     * Parse hidden profiles.
     */
    
    profile *p = parse_profile(full_path_hidden_profile(PROFILE_FORMAL_CHECKS), 
			       ldb, pdb, cf, PROFILE_COMMON, PARSE_PROFILE_CAN_BE_OMITTED);
    if(p)
	profile_db_add(pdb, p);


    /*
     * Parse ordinary profiles.
     */


    if(common_profiles)
	for(refs_iterator = common_profiles->SubtagsBegin(); refs_iterator != common_profiles->SubtagsEnd(); refs_iterator++)
	    {
		if(strcmp(refs_iterator->AttrValue(XML_ATTR_PROFILES_ACTIVE), XML_ATTR_PROFILES_ACTIVE_YES) == 0)
		    {
			
			profile *p = parse_profile(refs_iterator->AttrValue(XML_ATTR_PROFILES_FILE), 
						   ldb, pdb, cf, PROFILE_COMMON, PARSE_PROFILE_DEFAULTS);
			if(p)
			    profile_db_add(pdb, p);
			else
			    {
				return 1;
			    }
		    }
	    }

    if(private_profiles)
	for(refs_iterator = private_profiles->SubtagsBegin(); refs_iterator != private_profiles->SubtagsEnd(); refs_iterator++)
	    {
		if(strcmp(refs_iterator->AttrValue(XML_ATTR_PROFILES_ACTIVE), XML_ATTR_PROFILES_ACTIVE_YES) == 0)
		    {
			profile *p = parse_profile(refs_iterator->AttrValue(XML_ATTR_PROFILES_FILE), 
						   ldb, pdb, cf, PROFILE_PRIVATE, PARSE_PROFILE_DEFAULTS);
			if(p)
			    profile_db_add(pdb, p);
			else
			    return 1;
		    }
	    }
    
    
    return 0;
}

/*
 * Configuration parse.
 */
int parse_main(const char *fname)
{
    XMLTag tag;
    
    int res_xml;

    std::string s;
    
    res_xml = LoadXMLFile(fname, tag, tXMLMain, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_warning("can\'t parse setting file (%s): %s\n", fname, s.c_str());
	    return 1;
	}

    const XMLTag *bounce = tag.Subtag(XML_TAG_BOUNCE); 
    const XMLTag *reject = tag.Subtag(XML_TAG_REJECT); 
    const XMLTag *hostname = tag.Subtag(XML_TAG_HOSTNAME); 

    if(bounce && bounce->Data())
	config_set_bounce_text(bounce->Data());

    if(reject && reject->AttrValue(XML_ATTR_REJECT_TEXT))
	config_set_reject_text(reject->AttrValue(XML_ATTR_REJECT_TEXT));

    if(hostname && hostname->AttrValue(XML_ATTR_HOSTNAME_TEXT))
	config_set_hostname(reject->AttrValue(XML_ATTR_HOSTNAME_TEXT));

    return 0;
}

samples_node *parse_samples(const char *fname)
{
    XMLTag tag;
    
    int res_xml;

    std::string s;
    
    res_xml = LoadXMLFile(fname, tag, tXMLSampleList, &s, 0, CP_KOI8R);

    if(res_xml)
	{
	    compiler_warning("can\'t parse list of samples (%s): %s\n", fname, s.c_str());
	    return NULL;
	}

    XMLTag::SubtagsIterator sti;
    samples_node *res = NULL;


    for(sti = tag.SubtagsBegin(); sti != tag.SubtagsEnd(); sti++)
	{
	    res = samples_add(res, sti->AttrValue(XML_ATTR_SAMPLEREF_CATEGORY_ID), sti->AttrValue(XML_ATTR_SAMPLEREF_FILE));

	    if(!res)
		break;
	}

    return res;
}

/*
 * <eof parse.cpp>
 */
