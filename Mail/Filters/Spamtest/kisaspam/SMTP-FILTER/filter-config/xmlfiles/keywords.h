/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: keywords.h                                                            *
 * Created: Wed Apr 24 13:50:53 2002                                           *
 * Desc: Keywords in XML files                                                 *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  keywords.h
 * \brief Keywords in XML files
 */

#ifndef __keywords_h__
#define __keywords_h__

/* ref files */
#define XML_TAG_IPLISTS                               "IPLists"
#define XML_TAG_IPLISTS_REF                           "IPListRef"
#define XML_TAG_EMAILLISTS                            "EMailLists"
#define XML_TAG_EMAILLISTS_REF                        "EMailListRef"
#define XML_TAG_DNSBLACKLISTS                         "DNSBlackLists"
#define XML_TAG_DNSBLACKLISTS_REF                     "DNSBlackListRef"

#define XML_ATTR_REF_FILE                             "file"
#define XML_ATTR_REF_NAME                             "name"

/* settings.xml */
#define XML_TAG_MAIN                                  "Settings"
#define XML_TAG_REJECT                                "RejectMessage"
#define XML_TAG_BOUNCE                                "BounceMessage"
#define XML_TAG_HOSTNAME                              "HostName"

#define XML_ATTR_REJECT_TEXT                          "text"
#define XML_ATTR_HOSTNAME_TEXT                        "text"

/* list files */
#define XML_ATTR_LIST_NAME                            "name"
#define XML_ATTR_LIST_DESCR                           "description"

#define XML_TAG_IPLIST                                "IPList"
#define XML_TAG_IP                                    "IP"
#define XML_ATTR_IP_MASK                              "mask"

#define XML_TAG_EMAILLIST                             "EMailList"
#define XML_TAG_EMAIL                                 "EMail"
#define XML_ATTR_EMAIL_ADDRESS                        "address"

#define XML_TAG_DNSBLACKLIST                          "DNSBlackList"
#define XML_TAG_DNSBLACKSERVICE                       "Service"
#define XML_ATTR_DNSBLACKSERVICE_DOMAIN               "zone"
#define XML_ATTR_DNSBLACKSERVICE_WEIGHT               "weight"

/* list of profiles */
#define XML_TAG_PROFILES                              "Profiles"
#define XML_TAG_PROFILES_COMMON                       "Common"
#define XML_TAG_PROFILES_PRIVATE                      "Personal"
#define XML_TAG_PROFILES_REF                          "ProfileRef"

#define XML_ATTR_PROFILES_FILE                        "file"
#define XML_ATTR_PROFILES_NAME                        "name"
#define XML_ATTR_PROFILES_ACTIVE                      "active"
#define XML_ATTR_PROFILES_ACTIVE_YES                  "yes"
#define XML_ATTR_PROFILES_ACTIVE_NO                   "no"

/* profile */
#define XML_TAG_PROFILE                               "Profile"
#define XML_ATTR_PROFILE_NAME                         "name"
#define XML_ATTR_PROFILE_TYPE                         "type"
#define XML_ATTR_PROFILE_TYPE_COMMON                  "common"
#define XML_ATTR_PROFILE_TYPE_PRIVATE                 "personal"
#define XML_ATTR_PROFILE_RCPT                         "rcpt"
#define XML_ATTR_PROFILE_RCPTLIST                     "rcptlist"
#define XML_ATTR_PROFILE_RCPTMASK		      "rcpt_mask"
#define XML_ATTR_PROFILE_RCPTREGEXP		      "rcpt_regexp"

/*
 * supplemental attribs used by configurator
 * must be absolutely ignored by compilator
 */
#define XML_ATTR_PROFILE_RCPTTYPE                     "rcpttype"
#define XML_ATTR_PROFILE_RCPTTYPE_RCPT                "rcpt"
#define XML_ATTR_PROFILE_RCPTTYPE_LIST                "list"

#define XML_TAG_PROFILE_COMMENT                       "Comment"

/* rule */
#define XML_TAG_PROFILE_RULE                          "Rule"
#define XML_TAG_PROFILE_CONDITIONS                    "Conditions"
#define XML_TAG_PROFILE_ACTIONS                       "Actions"

/* conditions */
#define XML_TAG_PROFILE_IP_FROM                       "IPFrom"
#define XML_TAG_PROFILE_IP_FROM_LIST                  "IPFromList"
#define XML_TAG_PROFILE_IP_FROM_DNSBLACK              "IPFromDNSBlack"
#define XML_TAG_PROFILE_IP_FROM_NOTINDNS              "IPFromNotInDNS"
#define XML_TAG_PROFILE_SMTP_FROM                     "SMTPFrom"
#define XML_TAG_PROFILE_SMTP_FROM_LIST                "SMTPFromList"
#define XML_TAG_PROFILE_SMTP_TO                       "SMTPTo"
#define XML_TAG_PROFILE_SMTP_TO_LIST                  "SMTPToList"
#define XML_TAG_PROFILE_HAS_HEADER                    "HasHeader"
#define XML_TAG_PROFILE_HEADER_MATCH                  "HeaderMatch"
#define XML_TAG_PROFILE_CATEGORY_MATCH                "CategoryMatch"
#define XML_TAG_PROFILE_MSG_SIZE                      "MsgSize"
#define XML_TAG_PROFILE_REGISTER                      "Register"
#define XML_TAG_PROFILE_RATE                          "Rate"
#define XML_TAG_PROFILE_ATTRIBUTE_MATCH               "AttributeMatch"
#define XML_TAG_PROFILE_CONTENT_TYPE_MATCH            "ContentTypeMatch"
#define XML_TAG_PROFILE_FILE_NAME_MATCH               "FileNameMatch"
#define XML_TAG_PROFILE_HAS_ATTACHMENTS               "HasAttachments"
#define XML_TAG_PROFILE_UNKNOWN_CONTENT               "UnknownContent"
#define XML_TAG_PROFILE_VIRUS_MATCH                   "VirusMatch"
#define XML_TAG_PROFILE_DCC_MATCH                     "DCCMatch"
#define XML_TAG_PROFILE_BAYES_MATCH                   "BayesMatch"

#define XML_ATTR_PROFILE_BAYES_MATCH_RATE             "rate"
#define XML_ATTR_PROFILE_DCC_MATCH_BODY               "body"
#define XML_ATTR_PROFILE_DCC_MATCH_FUZ1               "fuz1"
#define XML_ATTR_PROFILE_DCC_MATCH_FUZ2               "fuz2"

#define XML_ATTR_PROFILE_REGISTER_EQUAL               "equal"
#define XML_ATTR_PROFILE_REGISTER_LESS                "less"
#define XML_ATTR_PROFILE_REGISTER_MORE                "more"

#define XML_ATTR_PROFILE_RATE_EQUAL                   "equal"
#define XML_ATTR_PROFILE_RATE_LESS                    "less"
#define XML_ATTR_PROFILE_RATE_MORE                    "more"

#define XML_ATTR_PROFILE_MASK                         "mask"
#define XML_ATTR_PROFILE_LIST                         "list"
#define XML_ATTR_PROFILE_EMAIL                        "email"
#define XML_ATTR_PROFILE_HEADER                       "header"
#define XML_ATTR_PROFILE_ATTRIBUTE                    "attribute"
#define XML_ATTR_PROFILE_REGEXP                       "regexp"
#define XML_ATTR_PROFILE_CATEGORY                     "category"
#define XML_ATTR_PROFILE_LIMIT                        "limit"

#define XML_ATTR_PROFILE_COND_NEGATIVE                "negative"
#define XML_ATTR_PROFILE_COND_NEGATIVE_YES            "yes"
#define XML_ATTR_PROFILE_COND_NEGATIVE_NO             "no"

/* supplemental attribs used by configurator
 * must be absolutely ignored by compilator
 */
#define XML_ATTR_PROFILE_COND_INVALID                 "invalid"
#define XML_ATTR_PROFILE_COND_INVALID_YES             "yes"

#define XML_ATTR_PROFILE_LIST_NAME                    "listname"
#define XML_ATTR_PROFILE_CATEGORY_TITLE               "cattitle"

/* actions */
#define XML_TAG_PROFILE_ACCEPT                        "DoAccept"
#define XML_TAG_PROFILE_BLACKHOLE                     "DoBlackHole"
#define XML_TAG_PROFILE_REJECT                        "DoReject"
#define XML_TAG_PROFILE_BOUNCE                        "DoBounce"
#define XML_TAG_PROFILE_SKIP                          "DoSkip"

#define XML_TAG_PROFILE_HEADER_REPLACE                "DoHeaderReplace"
#define XML_TAG_PROFILE_HEADER_ADD                    "DoHeaderAdd"
#define XML_TAG_PROFILE_HEADER_PREPEND                "DoHeaderPrepend"
#define XML_TAG_PROFILE_HEADER_NEW                    "DoHeaderNew"
#define XML_TAG_PROFILE_HEADER_DELETE                 "DoHeaderDelete"

#define XML_ATTR_PROFILE_ACTION_HEADER                "header"
#define XML_ATTR_PROFILE_ACTION_VALUE                 "value"

#define XML_TAG_PROFILE_RCPT_REPLACE_ALL              "DoRcptReplaceAll"
#define XML_TAG_PROFILE_RCPT_REPLACE                  "DoRcptReplace"
#define XML_TAG_PROFILE_RCPT_ADD                      "DoRcptAdd"
#define XML_TAG_PROFILE_RCPT_DELETE                   "DoRcptDelete"

#define XML_ATTR_PROFILE_RCPT_NEW                     "new"
#define XML_ATTR_PROFILE_RCPT_OLD                     "old"
#define XML_ATTR_PROFILE_RCPT_DELETE_EMAIL            "email"

#define XML_TAG_PROFILE_REGISTER_SET                  "DoRegisterSet"
#define XML_TAG_PROFILE_REGISTER_ADD                  "DoRegisterAdd"

#define XML_ATTR_PROFILE_REGISTER_NUMBER              "number"
#define XML_ATTR_PROFILE_REGISTER_VALUE               "value"

#define XML_TAG_PROFILE_RATE_SET		      "DoRateSet"
#define XML_TAG_PROFILE_RATE_ADD                      "DoRateAdd"

#define XML_ATTR_PROFILE_RATE_VALUE                   "value"

#define XML_TAG_PROFILE_SEND_NOTIFICATION             "DoSendNotification"
#define XML_ATTR_PROFILE_ACTION_ID                    "id"

#define XML_TAG_PROFILE_SEND_LOG_ENTRY                "DoSendLogEntry"
#define XML_ATTR_PROFILE_ACTION_ENTRY                 "entry"

#define XML_TAG_PROFILE_SYSLOG                        "DoSysLog"
#define XML_ATTR_PROFILE_SYSLOG_PRIORITY              "priority"

#define XML_ATTR_PROFILE_SYSLOG_PRIORITY_DEBUG         "debug"
#define XML_ATTR_PROFILE_SYSLOG_PRIORITY_INFO          "info"
#define XML_ATTR_PROFILE_SYSLOG_PRIORITY_WARNING       "warning"
#define XML_ATTR_PROFILE_SYSLOG_PRIORITY_ERROR         "error"
#define XML_ATTR_PROFILE_SYSLOG_PRIORITY_STATS         "stats"

#define XML_ATTR_PROFILE_SYSLOG_MESSAGE               "message"

#define XML_ATTR_PROFILE_SYSLOG_LEVEL                 "level"

#define XML_ATTR_PROFILE_SYSLOG_LEVEL_NORMAL          "normal"
#define XML_ATTR_PROFILE_SYSLOG_LEVEL_INFORMATIVE     "informative"
#define XML_ATTR_PROFILE_SYSLOG_LEVEL_HIGH            "high"
#define XML_ATTR_PROFILE_SYSLOG_LEVEL_EXTREME         "extreme"
#define XML_ATTR_PROFILE_SYSLOG_LEVEL_FLOOD           "flood"

#define XML_TAG_PROFILE_DCC_REPORT                    "DoDCCReport"

#define XML_ATTR_PROFILE_DCC_REPORT_COUNT             "count"

/* supplemental attribs used by configurator
 * must be absolutely ignored by compilator
 */
#define XML_ATTR_PROFILE_ACTION_INVALID               "invalid"
#define XML_ATTR_PROFILE_ACTION_INVALID_YES           "yes"

/* category list */
#define XML_TAG_CATEGORIES                            "Categories"
#define XML_TAG_CATEGORY                              "Category"
#define XML_ATTR_CATEGORY_ID                          "ID"
#define XML_ATTR_CATEGORY_TITLE                       "title"

/* list of samples */
#define XML_TAG_SAMPLES                               "Samples"
#define XML_ATTR_SAMPLES_LAST_NUM                     "last"

#define XML_TAG_SAMPLEREF                             "Sample"
#define XML_ATTR_SAMPLEREF_FILE                       "file"
#define XML_ATTR_SAMPLEREF_CATEGORY_ID                "catID"

// used only by configurator
#define XML_ATTR_SAMPLEREF_NAME                       "name"
#define XML_ATTR_SAMPLEREF_CATEGORY_TITLE             "cattitle"

/* sample */
#define XML_TAG_SAMPLE                                "Sample"
#define XML_ATTR_SAMPLE_NS                            "xmlns"
#define XML_ATTR_SAMPLE_NS_VALUE                      "http://www.ashmanov.com/cf/1.0/"
#define XML_TAG_SAMPLE_SUBJECT                        "Subject"
#define XML_TAG_SAMPLE_BODY                           "Body"

// used only by configurator
#define XML_ATTR_VIEW_MODE	                      "view"
#define XML_ATTR_DEFAULT	                      "default"
#define XML_ATTR_SYSLOG   	                      "syslog"

/* variables */

#define VARIABLE_CATEGORY                             "CATEGORY"
#define VARIABLE_CAT_ID                               "CAT_ID"
#define VARIABLE_RCPTS                                "RCPTS"
#define VARIABLE_REGISTER                             "REG"
#define VARIABLE_RATE                                 "RATE"
#define VARIABLE_TRICKS_DETECTED                      "TRICKS"
#define VARIABLE_TRICKS_DETECTED_EXPANDED             "TRICKS_EXPANDED"
#define VARIABLE_ENVELOPE_FROM                        "FROM"
#define VARIABLE_ENVELOPE_ALL_RCPTS                   "ALL_RCPTS"
#define VARIABLE_ENVELOPE_CUR_RCPTS                   "CUR_RCPTS"
#define VARIABLE_ENVELOPE_ORIG_RCPTS                  "ORIG_RCPTS"
#define VARIABLE_VIRUS_INFO                           "VIRUS_INFO"

/* profiles */

#define PROFILE_FORMAL_CHECKS                         "formal.xml"


#endif /* __keywords_h__ */

/*
 * <eof keywords.h>
 */
