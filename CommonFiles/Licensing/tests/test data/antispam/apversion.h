#ifndef  _VERSION_H
#define  _VERSION_H 1

/*!
 * (C) 2003 "Company name"
 *
 * \file 
 * \author DISCo
 * \date 2003
 * \brief Version and project dependent names and constants
 */


#define REG_KEY TEXT("Ashmanov & Partners\\SpamTest")	//!< Registry key for options
#define APPDATA_KEY TEXT("\\SpamTest")	                //!< Application data subdirectory
#define DEF_EXAMPLES_EMAIL_SPAM TEXT("spam@spam.test")       //!< Email address for spam examples
#define DEF_EXAMPLES_EMAIL_NOT_SPAM TEXT("notspam@spam.test")   //!< Email address for not spam examples

#define OLEXT_SUBKEY TEXT("SpamTest")                 //!< OL Extention regisry subkey
#define OLADDIN_SUBKEY TEXT("SpamTestOutlookAddIn")   //!< OL Addin OLE ProgID
#define OLADDIN_CLSID TEXT("{74487093-550B-4974-BC70-39A2ACBA6B26}") //!< OL Addin OLE CLSID



#define OLEXTRULE_SUBKEY TEXT("SpamTest")                 //!< OL Extention custom rule action regisry subkey

//  License

#define LIC_PROD_NUM                          138 //<! Product Number
#define LIC_PROD_STRING                       "Kaspersky Personal Anti-Spam" //!< Product Name
#define LIC_PROD_MAJORVERSION_STRING          ""   //<! Product major version

#define LIC_APP_DESCRIPTION_STRING            "Kaspersky lab application info file" //!< Application description
#define LIC_APP_CREATE_DATE                   { 2003, 10,  9 } //!< Date create


//  RC defines

#define RC_COMPANYNAME "Ashmanov & Partners\0"		//!< RC vesion info Company name
#define RC_COPYRIGHT   "Copyright © 2003 Ashmanov & Partners\0" //!< RC vesion info Copyright
#define RC_TRADEMARKS  "\0"	//!< RC vesion info Trade marks

#define RC_PRODUCTNAME "Kaspersky Personal Anti-Spam\0"   //!< RC vesion info Product name. Common components
#define RC_OL_PRODUCTNAME "Kaspersky Personal Anti-Spam for Outlook\0"   //!< RC vesion info Product name. OL Components
#define RC_OE_PRODUCTNAME "Kaspersky Personal Anti-Spam for Outlook Express\0" //!< RC vesion info Product name. OE Components

#define IS_RELEASE() 1 //!< 0 - beta, not 0 - release
#define RC_PRODUCTVERSION_STR "0.8.0 (Beta)\0"  //!< RC vesion info Product version (string)
#define RC_PRODUCTVERSION 0,8,0,0        //!< RC vesion info Product version (number)

#endif

