/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klids.h
 * \author	Andrew Kazachkov
 * \date	25.02.2003 19:58:41
 * \brief	CS AdminKit identifiers
 * 
 */
 
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __KLIDS_H__
#define __KLIDS_H__

/*!
    Applications
*/

//! mmc gui
#define KLCS_PRODUCT_KAVMSNAP   L"1102"
#define KLCS_VERSION_KAVMSNAP   L"1.0.0.0"

//! nagent
#define KLCS_PRODUCT_NAGENT     L"1103"
#define KLCS_VERSION_NAGENT     L"1.0.0.0"

//! administration server
#define KLCS_PRODUCT_ADMSERVER	L"1093"		//! Product CS Admin kit
#define KLCS_VERSION_ADMSERVER	L"1.0.0.0"	//! Product CS Admin kit

//! nagent
#define KLCS_PRODUCT_KLMOBILE     L"KLMOBILE"
#define KLCS_VERSION_KLMOBILE     L"1.0.0.0"

/*!
    Components	
*/
	#define KLCS_COMPONENT_AGENT	L"85"	//! Local Product Agent
	#define KLCS_COMPONENT_NAGENT	L"86"	//! Network Agent
	#define KLCS_COMPONENT_SERVER	L"87"	//! CS Administration Server
	#define KLCS_COMPONENT_CONSOLE	L"88"	//! CS Administration Console

/*KLCSAK_BEGIN_PRIVATE*/
//! Well-knowns
	#define KLCS_MAKEWELLKNOWN_AGENT(wstrProd)		\
								(wstrProd + L"-" + KLCS_COMPONENT_AGENT)


	#define KLCS_WELLKNOWN_SERVER	\
							KLCS_PRODUCT_ADMINKIT L"-" KLCS_COMPONENT_SERVER


//!misc
    #define KLCS_TEST_COMPONENT_PREFIX L"~"

/*KLCSAK_END_PRIVATE*/

//!obsolete
#define KLCS_PRODUCT_ADMINKIT	KLCS_PRODUCT_ADMSERVER
#define KLCS_VERSION_ADMINKIT	KLCS_VERSION_ADMSERVER

#endif //__KLIDS_H__

// Local Variables:
// mode: C++
// End:
