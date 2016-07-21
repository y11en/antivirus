#include "../include/getupdateconfig_win32.h"

#if !defined (_WIN32)
    #error This file must be complied under Win32
#endif

#include <msxml.h>
#include <atlbase.h>
#include <comdef.h>

namespace LicensingPolicy
{

namespace UpdateConfig
{

LPCWSTR UpdateSectionName = L"UpdateFiles";
LPCWSTR UpdateDateAttrName = L"/UpdateFiles/@UpdateDate";
LPCWSTR FileDescrSectionName = L"/UpdateFiles/FileDescription";
LPCWSTR FileNameAttrName = L"./@Filename";
LPCWSTR FileSizeAttrName = L"./@FileSize";
LPCWSTR FileSignAttrName = L"./@FileSign";
LPCWSTR UpdateTypeAttrName = L"./@UpdateType";

const _TCHAR  *BlackListUpdateTypeName = _T ("blst");
const _TCHAR  *UpdateDescMasterFile     = _T ("master.xml");

const size_t DateMinLength = 8;

HRESULT readXMLFromBuffer           (IXMLDOMDocument        *pDoc,
                                     const file_image_t&    xmlImage
                                    );
HRESULT extractUpdateDate           (IXMLDOMNode            *pRootNode,
                                     date_t                 *pDate
                                    );
HRESULT extractFilesList            (IXMLDOMNode            *pRootNode,
                                     file_info_list_t       *pFilesList
                                    );
HRESULT extractFileInfo             (IXMLDOMNode            *node,
                                     file_info_t            *fileInfo
                                    );

//-----------------------------------------------------------------------------

HRESULT Win32GetUpdateConfig
    (const file_image_t&    xmlImage,
     update_config_t        *config
    )

{

   
    if (!config) 
    {
        return (E_INVALIDARG);
    }

    config->updateDate.clear ();
    config->filesList.clear ();

    HRESULT                     _hr;
    CComPtr<IXMLDOMDocument>    _pDoc;
    CComPtr<IXMLDOMNode>        _pRootNode;

    try
    {

        _hr = CoCreateInstance (CLSID_DOMDocument,/*CLSID_DOMFreeThreadedDocument,*/ 
                                0,
                                CLSCTX_INPROC_SERVER,
                                IID_IXMLDOMDocument,
                                reinterpret_cast<void **>(&_pDoc)
                               );

        if (FAILED (_hr)) 
        {
            return (LIC_E_XML_PARSE_ERROR);
        }

        _hr = readXMLFromBuffer (_pDoc, xmlImage);

        if (FAILED (_hr))
        {
            return (LIC_E_XML_PARSE_ERROR);
        }


        _hr = _pDoc->QueryInterface (IID_IXMLDOMNode, 
                                     reinterpret_cast<void**> (&_pRootNode)
                                    );

        if (FAILED (_hr)) 
        {
            return (LIC_E_XML_PARSE_ERROR);
        }

        _hr = extractUpdateDate (_pRootNode, &config->updateDate);

        if (FAILED (_hr)) 
        {
            return (LIC_E_XML_PARSE_ERROR);
        }

        _hr = extractFilesList (_pRootNode, &config->filesList);

        if (FAILED (_hr)) 
        {
            return (LIC_E_XML_PARSE_ERROR);
        }

        file_info_list_t::iterator  it (config->filesList.begin ());
        file_info_list_t::iterator  end (config->filesList.end ());

        config->blackListFileName = _T ("");

        for (; it != end; ++it)
        {
            if (it->updateType == BlackListUpdateTypeName)
            {
                config->blackListFileName = it->name;
                break;
            }
        }

    }
    catch (...) 
    {
        _hr = LIC_E_XML_PARSE_ERROR;
    }

    return (_hr);

}

//-----------------------------------------------------------------------------

HRESULT readXMLFromBuffer 
    (IXMLDOMDocument        *pDoc,
     const file_image_t&    xmlImage
    )
{

    CComPtr<IPersistStreamInit>   _pPSI;
    CComPtr<IStream>              _pStm;
    HRESULT                       _hr;
    ULONG                         _written;
    LARGE_INTEGER                 _li = {0, 0};

    _hr = CreateStreamOnHGlobal(0, TRUE, &_pStm);

    if (FAILED (_hr))
    {
        return (_hr);
    }

    _hr = _pStm->Write(xmlImage.image.c_str (), (ULONG)xmlImage.image.size (), &_written);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    _hr = pDoc->QueryInterface(IID_IPersistStreamInit, 
                               reinterpret_cast<void **> (&_pPSI)
                              );

    if (FAILED (_hr))
    {
        return (_hr);
    }

    _pStm->Seek(_li, STREAM_SEEK_SET, 0);
    _hr = _pPSI->Load(_pStm);

    if (FAILED (_hr))
    {
        return (_hr);
    }


    return (_hr);

}

//-----------------------------------------------------------------------------

HRESULT extractUpdateDate           
    (IXMLDOMNode    *pRootNode,
     date_t         *pDate
    )
{

    CComPtr<IXMLDOMNode>        _updateDateNode;
    HRESULT                     _hr;
    _bstr_t                     _updateDateStr;
    _variant_t                  _tmp;
    string_t                    _buf;

   _hr = pRootNode->selectSingleNode (_bstr_t(UpdateDateAttrName), &_updateDateNode);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    if (!_updateDateNode) 
    {
        return (E_FAIL);
    }

    _hr = _updateDateNode->get_nodeValue (&_tmp);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    // TODO
    _updateDateStr = _tmp;
    _buf = _updateDateStr;

    if (_buf.size () < DateMinLength) 
    {
        return (E_FAIL);
    }

    _buf.resize (DateMinLength);
    _buf.insert (2, _T ("."));
    _buf.insert (5, _T ("."));

    _stscanf (_buf.c_str (), _T ("%d.%d.%d"), &pDate->m_day, &pDate->m_month, &pDate->m_year);

    return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT extractFilesList            
    (IXMLDOMNode            *pRootNode,
     file_info_list_t       *pFilesList
    )
{

    CComPtr<IXMLDOMNodeList>    _filesDesc;
    IXMLDOMNode                 *_node;
    HRESULT                     _hr;
    _bstr_t                     _fileDescNodeName (L"/");

    _hr = pRootNode->selectNodes (_bstr_t(FileDescrSectionName), &_filesDesc);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    if (!_filesDesc) 
    {
        return (E_FAIL);
    }

    _hr = _filesDesc->nextNode (&_node);

    while (_node) 
    {
        file_info_t _fileInfo;
        _hr = extractFileInfo (_node, &_fileInfo);
        if (SUCCEEDED (_hr))
        {
            pFilesList->push_back (_fileInfo);
        }
        else
        {
            break;
        }

        _node->Release ();
        _filesDesc->nextNode (&_node);
    }

    return (_hr);

}

//-----------------------------------------------------------------------------

HRESULT extractFileInfo
    (IXMLDOMNode    *node,
     file_info_t    *fileInfo
    )
{

    HRESULT                 _hr;
    CComPtr<IXMLDOMNode>    _tmp;
    _bstr_t                 _name;
    _variant_t              _val;

    _hr = node->selectSingleNode (_bstr_t(FileNameAttrName), &_tmp);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    if (!_tmp) 
    {
        return (E_FAIL);
    }

    _hr = _tmp->get_nodeValue (&_val);
    _tmp.Release ();

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    fileInfo->name = static_cast<_bstr_t> (_val);
    _val.Clear ();

    /////////////////////////////////////////////////////////////
    // Update type

    _hr = node->selectSingleNode (_bstr_t(UpdateTypeAttrName), &_tmp);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    if (!_tmp) 
    {
        return (E_FAIL);
    }
    
    _hr = _tmp->get_nodeValue (&_val);
    _tmp.Release ();

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    fileInfo->updateType = static_cast<_bstr_t> (_val);
    _val.Clear ();

    ////////////////////////////////////////////////////////
    // File size

    _hr = node->selectSingleNode (_bstr_t(FileSizeAttrName), &_tmp);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    if (!_tmp) 
    {
        return (E_FAIL);
    }
    
    _hr = _tmp->get_nodeValue (&_val);
    _tmp.Release ();

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    fileInfo->size = _ttoi (static_cast<_bstr_t> (_val));
    _val.Clear ();

    _hr = node->selectSingleNode (_bstr_t(FileSignAttrName), &_tmp);

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    if (!_tmp) 
    {
        return (E_FAIL);
    }
    
    _hr = _tmp->get_nodeValue (&_val);
    _tmp.Release ();

    if (FAILED (_hr)) 
    {
        return (_hr);
    }

    fileInfo->signature = static_cast<_bstr_t> (_val);

    return (S_OK);

}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

} // namespace UpdateConfig

} // namespace LicensingPolicy
