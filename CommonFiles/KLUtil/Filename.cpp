#include "stdafx.h"
#include "Filename.h"

namespace KLUTIL
{
	CFilename::CFilename()
	{
	}
	
	CFilename::CFilename(const CStdStringW& path)
	{
		SplitPath(path);
	}
	
	void CFilename::SplitPath(const CStdStringW& path)
	{
		_wsplitpath(path.c_str(),
			drive.GetBuffer(MAX_PATH),
			dir.GetBuffer(MAX_PATH),
			fname.GetBuffer(MAX_PATH),
			ext.GetBuffer(MAX_PATH));
		drive.ReleaseBuffer();
		dir.ReleaseBuffer();
		fname.ReleaseBuffer();
		ext.ReleaseBuffer();
	}
	
	CStdStringW CFilename::MakePath()
	{
		CStdStringW res;
		_wmakepath(res.GetBuffer(MAX_PATH),
			drive.c_str(), dir.c_str(), fname.c_str(), ext.c_str());
		res.ReleaseBuffer();
		return res;
	}
	
} // namespace KLUTIL
