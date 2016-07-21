#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"

#include "TestSuite.h"
#include "TestSuiteSiteInfo.h"
#include "main.h"

#include "helper/sites_info.h"


class SiteInfoWr: public SiteInfo
{
public:
	SiteInfoWr	(const SiteInfo::Region &region = SiteInfo::Region(), const wchar_t *address = L"", const size_t weight = 0,
				 const wchar_t *differenceFolder = L"diffs/", SiteInfo::RegionList regs = SiteInfo::RegionList()):
				 SiteInfo (region, address, weight, differenceFolder)
				  {
						m_regions = regs;
				  }
    
};

bool operator==(const SiteInfo& v1, const SiteInfoWr& v2)
{
	//std::wcout<<v1.toString().c_str()<<L"\n";
	//std::wcout<<v2.toString().c_str()<<L"\n";
	return (
				v1.m_address == v2.m_address &&
				v1.m_differenceFolder == v2.m_differenceFolder &&
				v1.m_region == v2.m_region &&
				v1.m_regions == v2.m_regions &&
				v1.m_weight == v2.m_weight
			);

}

void TestSuiteSiteInfo::TestWeight ()
{
	using namespace KLUPD;
	using namespace std;
	std::wcout<<L"TestWeight\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWeight");


	{
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test1.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
														  &testLog, false, L""),
														  L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}

	{//symbolic weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 1, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}
	{//symbolic weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 1, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}
	{//symbolic weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2a.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 1, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}
	{//symbolic weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2b.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 1, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}

	{//max/min weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2c.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 1, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 1, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 1, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}


/*	QC_BOOST_CHECK_MESSAGE (false, L"Если происходит переполнение, то все последующие вызовы будут выдавать неверный результат, бага записана\n");

	{//max/min weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2d.xml");
		vector<SiteInfo> sites;

		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 1, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 2, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 1, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}
*/	
	{//missing weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2e.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 1, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}
	
	{//missing tag weight
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2f.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 1, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}

	}
	
	{//floating weight values
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2g.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 1, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 1, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}
	
	{//double weight tag
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test2h.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 15, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}
}

void TestSuiteSiteInfo::TestUrl ()
{//можно переделать, теперь при любой ошибке возвращается false и пустой вектор
	using namespace KLUPD;
	using namespace std;
	std::wcout<<L"TestUrl\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUrl");
/*
	{//empty value
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test3.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}
*/
	{//empty value
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test3.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE (! SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");

		bool bCorrSize = sites.size() == 0;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");	
	}

	{//missing value
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test3a.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}

	{//missing value
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test3a.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");

		bool bCorrSize = sites.size() == 0;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
	}

	{//very long value
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test3b.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*",
L"ftp://tl-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv\
-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k\
-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k-srv-w2k"
, 20, L"diffs/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}
	

}

void TestSuiteSiteInfo::TestDiff ()
{
	using namespace KLUPD;
	using namespace std;
	std::wcout<<L"TestDiff\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestDiff");

	{
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test4.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 4;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w2k", 20, L"diffs2/", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"", reglist), L"Incorrect site information 3\n");
			QC_BOOST_CHECK_MESSAGE (sites[3] == SiteInfoWr (L"*", L"ftp://tl-srv-w04", 40, L"diffs/", reglist), L"Incorrect site information 3\n");
		}
	}	









}


void TestSuiteSiteInfo::TestRegion ()
{
	using namespace KLUPD;
	using namespace std;
	std::wcout<<L"TestRegion\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRegion");

	{//normal use
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test5.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			SiteInfo::RegionList reglist2;
			reglist2.push_back (L"ab");
			reglist2.push_back (L"cd");
			reglist2.push_back (L"ef");
			reglist2.push_back (L"gh");
			SiteInfo::RegionList reglist3;
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"ab,cd,ef,gh", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist2), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"", L"ftp://tl-srv-w03", 30, L"diffs/", reglist3), L"Incorrect site information 3\n");
		}
	}	

	{//normal use
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test5.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			SiteInfo::RegionList reglist2;
			reglist2.push_back (L"ab");
			reglist2.push_back (L"cd");
			reglist2.push_back (L"ef");
			reglist2.push_back (L"gh");
			SiteInfo::RegionList reglist3;
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"ab,cd,ef,gh", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist2), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"", L"ftp://tl-srv-w03", 30, L"diffs/", reglist3), L"Incorrect site information 3\n");
		}
	}

	{//normal use
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test5a.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 3;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
			SiteInfo::RegionList reglist2;
			//reglist2.push_back (L"ab");
			SiteInfo::RegionList reglist3;
			reglist3.push_back (L"a");
			reglist3.push_back (L"b");
			reglist3.push_back (L"c");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\
abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",L"ftp://tl-srv-wxp", 10, L"diffs/", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L",,", L"ftp://tl-srv-w2k", 20, L"diffs/", reglist2), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"a,,b,,,,,,,,,,,c,", L"ftp://tl-srv-w03", 30, L"diffs/", reglist3), L"Incorrect site information 3\n");
		}
	}


}

void TestSuiteSiteInfo::TestTags ()
{
	using namespace KLUPD;
	using namespace std;
	std::wcout<<L"TestTags\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestTags");

	{//unknown attribute at different positions
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test6.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 5;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-w01", 10, L"difffs", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w02", 20, L"difffs", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"difffs", reglist), L"Incorrect site information 3\n");
			QC_BOOST_CHECK_MESSAGE (sites[3] == SiteInfoWr (L"*", L"ftp://tl-srv-w04", 40, L"difffs", reglist), L"Incorrect site information 4\n");
			QC_BOOST_CHECK_MESSAGE (sites[4] == SiteInfoWr (L"*", L"ftp://tl-srv-w05", 50, L"difffs", reglist), L"Incorrect site information 5\n");
			
		}
	}

	{//unclosed tag site2
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test6a.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE (! SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 0;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");

	}

	{//unclosed tag sites_list
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test6b.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE (! SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 0;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");

	}

	{//unknown tag
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test6c.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 5;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		if (bCorrSize)
		{
			SiteInfo::RegionList reglist;
			reglist.push_back (L"*");
			QC_BOOST_CHECK_MESSAGE (sites[0] == SiteInfoWr (L"*", L"ftp://tl-srv-w01", 10, L"difffs", reglist), L"Incorrect site information 1\n");
			QC_BOOST_CHECK_MESSAGE (sites[1] == SiteInfoWr (L"*", L"ftp://tl-srv-w02", 20, L"difffs", reglist), L"Incorrect site information 2\n");
			QC_BOOST_CHECK_MESSAGE (sites[2] == SiteInfoWr (L"*", L"ftp://tl-srv-w03", 30, L"difffs", reglist), L"Incorrect site information 3\n");
			QC_BOOST_CHECK_MESSAGE (sites[3] == SiteInfoWr (L"*", L"ftp://tl-srv-w04", 40, L"difffs", reglist), L"Incorrect site information 4\n");
			QC_BOOST_CHECK_MESSAGE (sites[4] == SiteInfoWr (L"*", L"ftp://tl-srv-w05", 50, L"difffs", reglist), L"Incorrect site information 5\n");
			
		}
	}




}


void TestSuiteSiteInfo::TestSorting ()
{
	using namespace KLUPD;
	using namespace std;
	std::wcout<<L"TestSorting\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSorting");


	{
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test7.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 7;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		vector<Path> sites_sorted;
		if (bCorrSize) SiteInfo().sortSites (sites, L"*", sites_sorted, &testLog);
		bCorrSize = sites_sorted.size() == 7;
		if (bCorrSize)
		{
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w07") != sites_sorted.end(), L"Incorrect site 1\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w06") != sites_sorted.end(), L"Incorrect site 2\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w05") != sites_sorted.end(), L"Incorrect site 3\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w04") != sites_sorted.end(), L"Incorrect site 4\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w03") != sites_sorted.end(), L"Incorrect site 5\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w02") != sites_sorted.end(), L"Incorrect site 6\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w01") != sites_sorted.end(), L"Incorrect site 7\n");

		}

	}

	{//preferred region
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test7a.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 7;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		vector<Path> sites_sorted;
		if (bCorrSize) SiteInfo().sortSites (sites, L"ab", sites_sorted, &testLog);
		bCorrSize = sites_sorted.size() == 7;
		if (bCorrSize)
		{
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w07") != sites_sorted.end(), L"Incorrect site 1\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w06") != sites_sorted.end(), L"Incorrect site 2\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w05") != sites_sorted.end(), L"Incorrect site 3\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w04") != sites_sorted.end(), L"Incorrect site 4\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w03") != sites_sorted.end(), L"Incorrect site 5\n");
			QC_BOOST_CHECK_MESSAGE (sites_sorted[0] == L"ftp://tl-srv-w02", L"Incorrect site 6\n");
			QC_BOOST_CHECK_MESSAGE (sites_sorted[1] == L"ftp://tl-srv-w01", L"Incorrect site 7\n");

		}

	}

	{//no preferred region
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test7b.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 7;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		vector<Path> sites_sorted;
		if (bCorrSize) SiteInfo().sortSites (sites, L"gh", sites_sorted, &testLog);
		bCorrSize = sites_sorted.size() == 7;
		if (bCorrSize)
		{
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w07") != sites_sorted.end(), L"Incorrect site 1\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w06") != sites_sorted.end(), L"Incorrect site 2\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w05") != sites_sorted.end(), L"Incorrect site 3\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w04") != sites_sorted.end(), L"Incorrect site 4\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w03") != sites_sorted.end(), L"Incorrect site 5\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w02") != sites_sorted.end(), L"Incorrect site 6\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w01") != sites_sorted.end(), L"Incorrect site 7\n");
			
			QC_BOOST_CHECK_MESSAGE (sites_sorted[3] == L"ftp://tl-srv-w02", L"Incorrect site w02\n");
			QC_BOOST_CHECK_MESSAGE (sites_sorted[6] == L"ftp://tl-srv-w01", L"Incorrect site w01\n");

		}

	}

	{//empty region
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test7c.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 7;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		vector<Path> sites_sorted;
		if (bCorrSize) SiteInfo().sortSites (sites, L"ab", sites_sorted, &testLog);
		bCorrSize = sites_sorted.size() == 7;
		if (bCorrSize)
		{
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w07") != sites_sorted.end(), L"Incorrect site 1\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w06") != sites_sorted.end(), L"Incorrect site 2\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w05") != sites_sorted.end(), L"Incorrect site 3\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w04") != sites_sorted.end(), L"Incorrect site 4\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w03") != sites_sorted.end(), L"Incorrect site 5\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w02") != sites_sorted.end(), L"Incorrect site 6\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w01") != sites_sorted.end(), L"Incorrect site 7\n");
			
			QC_BOOST_CHECK_MESSAGE (sites_sorted[0] == L"ftp://tl-srv-w01", L"Incorrect site w01\n");
			QC_BOOST_CHECK_MESSAGE (sites_sorted[1] == L"ftp://tl-srv-w02", L"Incorrect site w02\n");

		}

	}

	{//empty region
		wstring fileName = wstring (COLL_ROOT).append(L"\\Updcfgs\\upd_test7d.xml");
		vector<SiteInfo> sites;
		KLUPD::SiteInfo::Description description;
		QC_BOOST_CHECK_MESSAGE ( SiteInfo().getSitesInfo (sites, description, fileName.c_str(), false,
			&testLog, false, L""),
			L"Incorrect parsing result\n");
		
		bool bCorrSize = sites.size() == 7;
		QC_BOOST_CHECK_MESSAGE (bCorrSize, L"Incorrect size\n");
		vector<Path> sites_sorted;
		if (bCorrSize) SiteInfo().sortSites (sites, L"", sites_sorted, &testLog);
		bCorrSize = sites_sorted.size() == 7;
		if (bCorrSize)
		{
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w07") != sites_sorted.end(), L"Incorrect site 1\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w06") != sites_sorted.end(), L"Incorrect site 2\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w05") != sites_sorted.end(), L"Incorrect site 3\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w04") != sites_sorted.end(), L"Incorrect site 4\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w03") != sites_sorted.end(), L"Incorrect site 5\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w02") != sites_sorted.end(), L"Incorrect site 6\n");
			QC_BOOST_CHECK_MESSAGE (find (sites_sorted.begin(), sites_sorted.end(), L"ftp://tl-srv-w01") != sites_sorted.end(), L"Incorrect site 7\n");
			
			QC_BOOST_CHECK_MESSAGE (sites_sorted[1] == L"ftp://tl-srv-w05", L"Incorrect site w05\n");
			QC_BOOST_CHECK_MESSAGE (sites_sorted[0] == L"ftp://tl-srv-w02", L"Incorrect site w02\n");

		}

	}

}