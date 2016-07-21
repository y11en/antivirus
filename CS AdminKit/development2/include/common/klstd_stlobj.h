/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klstd_stlobj.h
 * \author	Andrew Kazachkov
 * \date	27.10.2005 11:27:57
 * \brief	
 * 
 */

#include <hash_map>

namespace KLSTD
{
    typedef std::hash_map<long, std::wstring>   hmap_long2str_t;
    typedef hmap_long2str_t::iterator           it_hmap_long2str_t;
    typedef hmap_long2str_t::value_type         val_hmap_long2str_t;

    typedef std::hash_map<std::wstring, long>   hmap_str2long_t;
    typedef hmap_str2long_t::iterator           it_hmap_str2long_t;
    typedef hmap_str2long_t::value_type         val_hmap_str2long_t;
};
