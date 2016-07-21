#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

/*!
  \brief
	MySplitString - выдел€ет из строки wstr элементы, разделенные строкой wstrDelim,
        и заполн€ет ими вектор vectValues, в том же пор€дке, в каком они встречались в исходной строке.
        ѕри этом wstrDelim не добавл€етс€ в качестве элемента, а пробелы равнозначны с другими символами, 
        т.е. пробелы могут быть как частью элементов, так и частью wstrDelim.
        ≈сли wstrDelim отсутсвует, то возвращаетс€ вс€ строка wstr. ≈сли после финального wstrDelim
        ничего нет, то в конце списка будет пуста€ строка. ≈сли два wstrDelim следуют
        друг за другом, то в соответсующем месте будет пуста€ строка.
        ¬ начале работы функци€ очищает vectValues.

  \param	 const std::wstring & wstr - [in] исходна€ строка
  \param	const std::wstring & wstrDelim - [in] разделитель
  \param	std::vector<std::wstring> & vectValues - [out] результат
*/
void MySplitString( const std::string & str,
                        const std::string & strDelim,
                        std::vector<std::string> & vectValues ) {
	using namespace std;

    vectValues.clear();

    if( str.size() && strDelim.size() )
    {
        std::string::size_type pos = 0;
        std::string::size_type posStart = 0;
        while( pos != string::npos )
        {
            pos = str.find( strDelim, posStart );
            std::string::const_pointer pStart = str.c_str() + posStart;
            if( pos == string::npos )
            {
                vectValues.push_back( string( pStart ) );
            } else
            {
                vectValues.push_back( string( pStart, pos - posStart ) );
                posStart = pos + strDelim.size();
            }
        }
    }
}
