/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_soapapi.h
 * \author	Andrew Kazachkov
 * \date	05.05.2003 11:52:40
 * \brief	
 * 
 */

class klsssrv_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

int klsssrv_Close(
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrProxyId,
                    struct klsssrv_CloseResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_CreateSettingsStorage(
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrLocation,
					xsd__int        nCreationFlags,
					xsd__int        nAccessFlags,
                    param__params   options,
                    struct klsssrv_CreateSettingsStorageResponse
                    {
                        xsd__wstring        wstrProxyId;
					    struct param_error	error;
                    }& r);


int klsssrv_Read(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    param__params  extra,
                    struct klsssrv_ReadResponse
                    {
                        param__params       result;
                        struct param_error	error;                 
                    }& r);

int klsssrv_Write(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    xsd__int       nFlags,
                    param__params  data,
                    struct klsssrv_WriteResponse
                    {
                        struct param_error	error;
                    }& r);


int klsssrv_CreateSection(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    struct klsssrv_CreateSectionResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_DeleteSection(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    struct klsssrv_DeleteSectionResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_SetTimeout(
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrProxyId,
                    xsd__int        lTimeout,
                    struct klsssrv_SetTimeoutResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_GetNames(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    struct klsssrv_GetNamesResponse
                    {
                        klsssrv_wstrings names;
                        struct param_error	error;
                    }& r);

int klsssrv_AttrRead(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    xsd__wstring   wstrAttr,
                    struct klsssrv_AttrReadResponse
                    {
                        param__value        value;
                        struct param_error	error;
                    }& r);
