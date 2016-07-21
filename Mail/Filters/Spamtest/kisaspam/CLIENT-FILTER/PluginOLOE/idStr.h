#ifndef  _IDSTR_H
#define  _IDSTR_H

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief Strings IDs
 *
 */


#define IDS_SUPPORT_PAGE           4980
#define IDS_SUPPORT_NOTAVAILABLE           4981
#define IDS_DLL_LANG_NAME                  4990
#define IDS_LANG_CHANGE_HEADER             4991
#define IDS_LANG_CHANGE_TEXT               4992


#define IDS_COLOR                          5001


#define IDS_ADD_FRIEND_BUTTON              32001
#define IDS_ADD_ENEMY_BUTTON               32002
#define IDS_FRIENDS_LIST_TITLE             32003
#define IDS_ENEMIES_LIST_TITLE             32004
#define IDS_FRIENDS_LIST                   32005
#define IDS_ENEMIES_LIST                   32006
#define IDS_ADDING_TO_FRIENDS_LIST         32007
#define IDS_ADDING_TO_ENEMIES_LIST         32008
#define IDS_ADD_TO_FRIENDS_LIST_HEADER     32009
#define IDS_ADD_TO_ENEMIES_LIST_HEADER     32010
#define IDS_ADD_TO_FRIENDS_LIST            32011
#define IDS_ADD_TO_ENEMIES_LIST            32012

#define IDS_NEED_RELAUNCH_HEADER           32013     
#define IDS_NEED_RELAUNCH                  32014
#define IDS_SOURCE_NOT_AVAILABLE           32015     
#define IDS_OE_SPAM                        32016 
#define IDS_OL_SPAM                        32017 
#define IDS_CONTINUE                       32018
#define IDS_ADD_SENDER_TO_ENEMIES          32023
#define IDS_ADD_SENDER_TO_FRIENDS          32024
#define IDS_SEND_BACK_TO_SPAMER            32025    
#define IDS_ADD_TO_SPAM_DATABASE           32026
#define IDS_BOUNCE_OK                      32029 
#define IDS_SPAM_MESSAGE_LOST              32030 
#define IDS_ERROR_SAVING_SAMPLE            32031
#define IDS_SPAMTEST_ERROR                 32032 
#define IDS_EMPTY_SMTPSERVER               32033
#define IDS_SMTPSERVER_ERROR               32034
#define IDS_SMTPSERVER_ERROR_HEADER        32035
#define IDS_SMTPSERVER_ERROR_TEXT          32036
#define IDS_NO_SENDER_ADDRESS_TO_BOUNCE    32038
#define IDS_OPTIONS_DIALOG                 32039


#define IDS_BOUNCE_MULTIPLE_MESSAGES       32040

#define IDS_DELETE_KEY_HEADER              32041
#define IDS_DELETE_KEY_MESSAGE             32042
#define IDS_DELETE_KEY_ERROR_HEADER        32043
#define IDS_ADD_KEY_ERROR_HEADER           32044
#define IDS_LICENSE_WARNING_HEADER         32045
#define IDS_LICENSE_WARNING_TEXT           32046
#define IDS_LICENSE_WARNING_LASTDAY_TEXT   32047
#define IDS_LICENSE_WARNING_DATE_TEXT      32048
#define IDS_LICENSE_WARNING_TEXT_TRIAL     32049
#define IDS_LICENSE_WARNING_LASTDAY_TEXT_TRIAL   32050
#define IDS_LICENSE_WARNING_DATE_TEXT_TRIAL 32051
#define IDS_LICENSE_ERROR_HEADER           32052
#define IDS_LICENSE_ERROR_TEXT             32053
#define IDS_LICENSE_ERROR_TEXT_TRIAL       32054
#define IDS_LICENSE_ERROR_TEXT_WRONGKEY    32055
#define IDS_LICENSE_CANNOT_UPGRADE_HEADER  32056
#define IDS_LICENSE_CANNOT_UPGRADE         32057

#define IDS_UPDATE_HEADER                  32060
#define IDS_UPDATE_START_QUESTION          32061 
#define IDS_UPDATE_CLOSE_OUTLOOK           32062
#define IDS_UPDATE_NO_UPDATE_FOUND         32063
#define IDS_UPDATE_UPDATE_DONE             32064
#define IDS_UPDATE_ALREADY_RUNNING         32065

#define IDS_TOTAL                          32078

#define IDS_RULE_MODIFIED                  32080 // OE
#define IDS_RULE_DELETED                   32081 // OE
#define IDS_RULE_DISABLED                  32082 // OE
#define IDS_RULE_NOT_MODIFIED              32083 // OE


#define IDS_ADDBUTTON                      32090
#define IDS_ADDADDRESSFROMFOLDER           32091

  
#define IDS_BOUNCE_MESSAGE_HEADER          32095
#define IDS_BOUNCE_MESSAGE_PROGRESS        32096

#define IDS_PROCESS_MESSAGE_PROGRESS       32098

#define IDS_ERROR_CLEARING_FOLDER          32100 
#define IDS_ERROR_CREATING_FOLDER          32101

#define IDS_ERROR_ACCESS_EMAIL             32102


#define IDS_CLEAR_EXAMPLES_HEADER          32113     
#define IDS_CLEAR_EXAMPLES                 32114

#define IDS_PSPHELP                        32115

#define IDS_INBOX_NAME                     32232 // OE
#define IDS_SPAMTEST_FOLDER_NAME           32233 // OE

#define IDS_OL_OPTIONS_INFO                32234 // OL
#define IDS_OL_OPTIONS_INFO_IMAP4          32235 // OL


#define IDS_SPAMTEST_TOOLBAR_NAME          32300
#define IDS_SPAMTEST_POPUP_NAME            32301
#define IDS_SPAMTEST_WARNING               32303
#define IDS_MENUSTR_FIRST                  32305
#define IDS_IS_SPAM                           (IDS_MENUSTR_FIRST+0)
#define IDS_IS_NOT_SPAM                       (IDS_MENUSTR_FIRST+1)
#define IDS_ADD_FRIEND                        (IDS_MENUSTR_FIRST+2)
#define IDS_ADD_ENEMY                         (IDS_MENUSTR_FIRST+3)
//#define IDS_ADD_SPAM_EXAMPLE                  (IDS_MENUSTR_FIRST+4)
#define IDS_BOUNCE_MESSAGE                    (IDS_MENUSTR_FIRST+4)
#define IDS_PROCESS_FOLDER                    (IDS_MENUSTR_FIRST+5)
#define IDS_FOLDER_ADDRESSES                  (IDS_MENUSTR_FIRST+6)
#define IDS_MANAGE_FRIENDS                    (IDS_MENUSTR_FIRST+7)
#define IDS_MANAGE_ENEMIES                    (IDS_MENUSTR_FIRST+8)
#define IDS_EMPTY_SPAM_FOLDERS                (IDS_MENUSTR_FIRST+9)
#define IDS_OPTIONS                           (IDS_MENUSTR_FIRST+10)
#define IDS_HELP                              (IDS_MENUSTR_FIRST+11)
#define IDS_ABOUT                             (IDS_MENUSTR_FIRST+12)

#define MIDS_STR_FIRST 35000
#define MIDS_Building_the_list_of_files_to_download (MIDS_STR_FIRST+1)
#define MIDS_Building_the_list_of_files_to_remove   (MIDS_STR_FIRST+2)
#define MIDS_Checking_black_list_integrity          (MIDS_STR_FIRST+3)
#define MIDS_Checking_license_keys                  (MIDS_STR_FIRST+4)
#define MIDS_Checking_product_license_keys          (MIDS_STR_FIRST+5)
#define MIDS_Checking_reserved_files                (MIDS_STR_FIRST+6)
#define MIDS_Checking_reserved_update_description_file (MIDS_STR_FIRST+7)
#define MIDS_Checking_update_description_file_integrity (MIDS_STR_FIRST+8)
#define MIDS_Clearing_reserve_dir                   (MIDS_STR_FIRST+9)
#define MIDS_Comparing_update_description_files     (MIDS_STR_FIRST+10)
#define MIDS_Copying_files                          (MIDS_STR_FIRST+11)
#define MIDS_Copying_new_black_list_file            (MIDS_STR_FIRST+12)
#define MIDS_Copying_update_description_file        (MIDS_STR_FIRST+13)
#define MIDS_Downloading_remote_file                (MIDS_STR_FIRST+14)
#define MIDS_Getting_product_configuration          (MIDS_STR_FIRST+15)
#define MIDS_Getting_updater_configuration          (MIDS_STR_FIRST+16)
#define MIDS_Making_reserve_copy_of_replaced_files  (MIDS_STR_FIRST+17)
#define MIDS_Parsing_update_description_file        (MIDS_STR_FIRST+18)
#define MIDS_Reading_reserved_description_file      (MIDS_STR_FIRST+19)
#define MIDS_Removing_extra_files                   (MIDS_STR_FIRST+20)
#define MIDS_Replacing_files                        (MIDS_STR_FIRST+21)
#define MIDS_Removing_files                         (MIDS_STR_FIRST+22)
#define MIDS_Rebooting_os                           (MIDS_STR_FIRST+23)
#define MIDS_Delaying_os_reboot                     (MIDS_STR_FIRST+24)
#define MIDS_Checking_lock_status                   (MIDS_STR_FIRST+25)
#define MIDS_Saving_updater_settings                (MIDS_STR_FIRST+26)
#define MIDS_Reading_list_of_products               (MIDS_STR_FIRST+27)
#define MIDS_Getting_generic_settings               (MIDS_STR_FIRST+28)
#define MIDS_Waiting_user_input                     (MIDS_STR_FIRST+29)
#define MIDS_Stopping_product_components            (MIDS_STR_FIRST+30)
#define MIDS_Restarting_product_components          (MIDS_STR_FIRST+31)
#define MIDS_Updating_all_products                  (MIDS_STR_FIRST+32)
#define MIDS_Checking_product_setings               (MIDS_STR_FIRST+33)
#define MIDS_Initialising                           (MIDS_STR_FIRST+34)
#define MIDS_Applying_patch                         (MIDS_STR_FIRST+35)
#define MIDS_Locking_directory                      (MIDS_STR_FIRST+36)
#define MIDS_Unlocking_directory                    (MIDS_STR_FIRST+37)
#define MIDS_Waiting_dialup_to_restore              (MIDS_STR_FIRST+38)
#define MIDS_Delaying_operation                     (MIDS_STR_FIRST+39)
#define MIDS_Exiting                                (MIDS_STR_FIRST+40)

#define MIDS_List_of_URLS_Exhausted       (MIDS_STR_FIRST+41)
#define MIDS_Current_version_is_newer     (MIDS_STR_FIRST+42)
#define MIDS_Current_version_is_the_same  (MIDS_STR_FIRST+43)
#define MIDS_Nothing_to_update            (MIDS_STR_FIRST+44)
#define MIDS_File_operation_failed        (MIDS_STR_FIRST+45)
#define MIDS_Local_filesystem_operations_failed (MIDS_STR_FIRST+46)
#define MIDS_Product_integrity_broken (MIDS_STR_FIRST+47)
#define MIDS_No_valid_keys (MIDS_STR_FIRST+48)
#define MIDS_Cant_rollback (MIDS_STR_FIRST+49)
#define MIDS_Cant_rollback_to_fresher_or_the_same_version (MIDS_STR_FIRST+50)
#define MIDS_Inconsistent_reserved_files (MIDS_STR_FIRST+51)
#define MIDS_Admin_kit_request_failure (MIDS_STR_FIRST+52)
#define MIDS_License_check_failure (MIDS_STR_FIRST+53)
#define MIDS_No_such_product_installed (MIDS_STR_FIRST+54)
#define MIDS_Not_updatable_product (MIDS_STR_FIRST+55)
#define MIDS_Internal_error (MIDS_STR_FIRST+56)
#define MIDS_Network_module_error (MIDS_STR_FIRST+57)
#define MIDS_Wrong_signature (MIDS_STR_FIRST+58)
#define MIDS_Invalid_update_description_file (MIDS_STR_FIRST+59)
#define MIDS_Download_failure (MIDS_STR_FIRST+60)
#define MIDS_Cancelled (MIDS_STR_FIRST+61)
#define MIDS_Done (MIDS_STR_FIRST+62)
#define MIDS_CompFromFld (MIDS_STR_FIRST+63)
#define MIDS_CompFromInet (MIDS_STR_FIRST+64)
#define MIDS_Proxy_auth_failure (MIDS_STR_FIRST+65)
#define MIDS_Downloading_from_url (MIDS_STR_FIRST+66)
#define MIDS_Server_connected (MIDS_STR_FIRST+67)
#define MIDS_Collecting_secondary_indices (MIDS_STR_FIRST+68)
#define MIDS_SystemError (MIDS_STR_FIRST+69)

#define CFIDS_STR_FIRST 36000
#define CFIDS_UnknownMessage (CFIDS_STR_FIRST)
#define CFIDS_Complete  (CFIDS_STR_FIRST+1)
#define CFIDS_Database  (CFIDS_STR_FIRST+2)
#define CFIDS_Compiling (CFIDS_STR_FIRST+3)
#define CFIDS_Unpacking (CFIDS_STR_FIRST+4)

#define CFIDS_PE_NOERROR (CFIDS_STR_FIRST+5)
#define CFIDS_PE_UNKNOWNTAG (CFIDS_STR_FIRST+6)
#define CFIDS_PE_UNEXPECTEDTAG (CFIDS_STR_FIRST+7)
#define CFIDS_PE_BADATTRIBUTE (CFIDS_STR_FIRST+8)
#define CFIDS_PE_BADQUANT (CFIDS_STR_FIRST+9)
#define CFIDS_PE_REFONLY (CFIDS_STR_FIRST+10)
#define CFIDS_PE_REDEFINE (CFIDS_STR_FIRST+11)
#define CFIDS_PE_CATID (CFIDS_STR_FIRST+12)
#define CFIDS_PE_NOTEXT (CFIDS_STR_FIRST+13)
#define CFIDS_PE_NEWTERMS (CFIDS_STR_FIRST+14)
#define CFIDS_PE_CPCONVERT (CFIDS_STR_FIRST+15)
#define CFIDS_PE_SAMPLEFILE (CFIDS_STR_FIRST+16)
#define CFIDS_PE_NOTUNIQCATID (CFIDS_STR_FIRST+17)
#define CFIDS_PE_DECODE (CFIDS_STR_FIRST+18)
#define CFIDS_PE_UNKNOWN (CFIDS_STR_FIRST+19)

#define CFIDS_ERROR_OK (CFIDS_STR_FIRST+20)
#define CFIDS_ERROR_PARSE (CFIDS_STR_FIRST+21)
#define CFIDS_ERROR_STRUCT (CFIDS_STR_FIRST+22)
#define CFIDS_ERROR_SYS (CFIDS_STR_FIRST+23)
#define CFIDS_ERROR_PAT (CFIDS_STR_FIRST+24)
#define CFIDS_ERROR_BIN (CFIDS_STR_FIRST+25)
#define CFIDS_ERROR_CREATE (CFIDS_STR_FIRST+26)
#define CFIDS_ERROR_MERGE (CFIDS_STR_FIRST+27)
#define CFIDS_ERROR_UNKNOWN (CFIDS_STR_FIRST+28)

#define CFIDS_PW_DUPTERMS (CFIDS_STR_FIRST+29)
#define CFIDS_PW_CANTOPENFILE (CFIDS_STR_FIRST+30)
#define CFIDS_PW_UNSUPPORTEDMSG (CFIDS_STR_FIRST+31)
#define CFIDS_PW_BADCHARS (CFIDS_STR_FIRST+32)
#define CFIDS_PW_CAT_EMPTY (CFIDS_STR_FIRST+33)
#define CFIDS_PW_UNKNOWN (CFIDS_STR_FIRST+34)

#endif // _IDSTR_H
