#if !defined (__UPDATECONFIG_H__)
#define __UPDATECONFIG_H__

#if defined (_MSC_VER)
    #pragma warning (disable : 4786)
#endif

#include <algorithm>
#include <functional>
#include "lic_defs.h"

//-----------------------------------------------------------------------------

namespace LicensingPolicy
{

    namespace UpdateConfig
    {

        typedef struct 
        {
            string_t    name;
            string_t    updateType;
            size_t      size;
            std::string signature;
        } file_info_t;

        typedef std::vector<file_info_t>    file_info_list_t;

		struct FindFileByName : public std::unary_function <const file_info_t&, bool>
		{
			FindFileByName (const string_t& name) : m_name (name) {}

			bool operator () (const file_info_t& fileInfo) const
			{
				return (m_name.compare (fileInfo.name) == 0);
			}
			const string_t&	m_name;
		};

        typedef struct 
        {
			file_info_t* FindFileInfo (const string_t& name)
			{
				file_info_list_t::iterator it = std::find_if (filesList.begin (),
					filesList.end (), FindFileByName (name));
				return (it != filesList.end () ? &(*it) : 0);
			}

            date_t              updateDate;
            string_t            blackListFileName;
            file_info_list_t    filesList;
        } update_config_t;


        HRESULT getUpdateConfig (const file_image_t&,
                                 update_config_t *
                                );

        typedef HRESULT (*getUpdateConfig_t) (const file_image_t&, update_config_t *);


    } // namespace UpdateConfig

} // namespace LicensingPolicy

#endif
