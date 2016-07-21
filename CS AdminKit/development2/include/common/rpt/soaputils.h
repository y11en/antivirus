/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	soaputils.h
 * \author	Mikhail Karmazine
 * \date	11.06.2003 19:36:57
 * \brief	файл для работы с объектами модуля KLPRT, передающимися через SOAP
 * 
 */


#ifndef __KLRPT_SOAPUTILS_H__
#define __KLRPT_SOAPUTILS_H__

#include <soapH.h>
#include <srvp/rpt/reports.h>

using namespace KLRPT;

void ReportTypeDetailedDataForSoap(
    struct soap* soap,
    const ReportTypeDetailedData & details,
    struct klrpt_type_details   & soapDetails );

void ReportTypeDetailedDataFromSoap(
    const struct klrpt_type_details & soapDetails,
    ReportTypeDetailedData & details );

void ReportCommonDataForSoap(
    struct soap* soap,
    const ReportCommonData & data,
    struct klrpt_common & soapData );

void ReportCommonDataFromSoap(
    const struct klrpt_common & soapData,
    ReportCommonData & data );

void ReportDetailedDataForSoap(
    struct soap* soap,
    const ReportDetailedData & data,
    struct klrpt_details & soapData );

void ReportDetailedDataFromSoap(
    const struct klrpt_details & soapData,
    ReportDetailedData & data );

void HostVectorForSoap(
    struct soap* soap,
    const HostVector  & vectHosts,
    struct klrpt_host_vector     & vectHostsSoap );

void HostVectorFromSoap(
    const struct klrpt_host_vector & vectHostsSoap,
    HostVector & vectHosts );


#endif //__KLRPT_SOAPUTILS_H__
