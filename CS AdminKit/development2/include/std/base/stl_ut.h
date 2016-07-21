#ifndef __KL_STL_UT_H__
#define __KL_STL_UT_H__

namespace KLSTD
{
    template<class T>
        void ReplaceToken(
                        std::basic_string<T>&   str, 
                const   std::basic_string<T>&   strToken, 
                const   std::basic_string<T>&   strValue )
    {
        std::basic_string<T>::size_type pos = str.find( strToken);
    
        while( pos != std::basic_string<T>::npos )
        {
            str.replace( pos, strToken.size(), strValue);
            pos = str.find( strToken, pos + strValue.size());
        }
    };

    template<class T>
        void ExcludeUnprintableSymbols(std::basic_string<T>& str)
    {
        const size_t c_nSize = str.size();
	    for( size_t i = 0; i < c_nSize; ++i )
	    {
		    T nCh = str[i];
		    if( unsigned(nCh) < 32u && 
                nCh != T('\r') && 
                nCh != T('\n') && 
                nCh != T('\t') )
		    {
			    str[i] = T('?');
		    };
	    };
    };
};

#endif //__KL_STL_UT_H__
