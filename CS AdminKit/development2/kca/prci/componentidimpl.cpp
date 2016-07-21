#include <std/base/klstd.h>
#include <kca/prci/componentid.h>

namespace KLPRCI
{
	////////////////////////////////////////////////////////////////////////
	// ComponentId

	const wchar_t c_ConnNameDelimer = L';';

	std::wstring ComponentId::PutToString() const
	{
		std::wstring rc;
        rc.reserve( productName.size() + 
                    componentName.size() + 
                    version.size() + 
                    instanceId.size() + 4);
		rc.append( productName ); rc.append( 1, c_ConnNameDelimer );
		rc.append( componentName ); rc.append( 1, c_ConnNameDelimer );
		rc.append( version ); rc.append( 1, c_ConnNameDelimer );
		rc.append( instanceId );

		return rc;
	}
	
	void ComponentId::GetFromString(const std::wstring& s)
	{
		int prevDelimer = 0;
		int delimerPos = s.find( c_ConnNameDelimer );
		if ( delimerPos!=(int)std::wstring::npos ) 
		{
			productName = s.substr( prevDelimer, delimerPos );			
			prevDelimer = ++delimerPos;
			delimerPos = s.find( c_ConnNameDelimer, prevDelimer );			
		} 
		else productName.erase();

		if ( delimerPos!=(int)std::wstring::npos ) 
		{
			componentName = s.substr( prevDelimer, delimerPos-prevDelimer );
			prevDelimer = ++delimerPos;
			delimerPos = s.find( c_ConnNameDelimer, prevDelimer );
		} 
		else componentName.erase();

		if ( delimerPos!=(int)std::wstring::npos ) 
		{
			version = s.substr( prevDelimer, delimerPos-prevDelimer );
			prevDelimer = ++delimerPos;
			delimerPos = s.find( c_ConnNameDelimer, prevDelimer );
		} 
		else version.erase();
		
		instanceId = s.substr( prevDelimer );
	}
};
