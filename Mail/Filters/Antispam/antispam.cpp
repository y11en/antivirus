// parser2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "antispam_interface_imp.h"

unsigned long g_ulErrors = 0;
unsigned long g_ulFilesCount = 0;


tERROR FilterMessageInTheFile(const char* filename, hOBJECT parent, CAntispamImplementer::CKLAntispamFilter& hFilter);
tERROR FilterMessage( hIO hMessage, CAntispamImplementer::CKLAntispamFilter& hFilter, const char* filename );
tERROR FilterIO(hIO io, CAntispamImplementer::CKLAntispamFilter& hFilter, const char* filename );

bool bIsSpam = false;
bool bFirstTrain = false;

int main(int argc, char* argv[])
{
	if ( argc < 4 ) 
	{
		std::cout << "Using sample: \r\n";
		std::cout << argv[0] << " <is the first train: 1|0> <is the spam: 1|0> <mask of files> \r\n";
		std::cout << argv[0] << " 1 1 \"c:\\spamarchive\\*.eml\"\" \r\n";
		return 0;
	}

	CAntispamImplementer::CKLAntispamFilter hFilter;
	if ( PR_SUCC(hFilter.Init(NULL, NULL)) ) 
	{
		bFirstTrain = !!atoi( argv[1] );
		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(argv[3], &FindFileData);
		
		if (hFind != INVALID_HANDLE_VALUE) 
		{
			do 
			{ 
				std::string filename = argv[3];
				filename.erase(filename.length()-1, 1);
				filename.append(FindFileData.cFileName);
				bIsSpam = ( filename.find("spam") != -1 );
				FilterMessageInTheFile( filename.c_str(), (hOBJECT)g_root, hFilter );
				OutputDebugString("Processed ");
				OutputDebugString(FindFileData.cFileName);
				OutputDebugString("\r\n");
			} 
			while(FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}
		else
			std::cout << "Cannot find any files by " << argv[3] << endl;
	}
	else
		std::cout << "Cannot initialize BayesFilter" << endl;

//	if ( !bFirstTrain )
//		std::cout 
//			<< "errors: " 
//			<< g_ulErrors 
//			<< "  from: "
//			<< g_ulFilesCount 
//			<< " ; Accuracy is: " 
//			<< 100.0-((long double)g_ulErrors/(long double)g_ulFilesCount)*100
//			<< "%"
//			<< endl;

	return 0;
}

tERROR FilterMessageInTheFile(const char* filename, hOBJECT parent, CAntispamImplementer::CKLAntispamFilter& hFilter)
{
	tERROR err = errOK;
	hIO hMyIO = 0;
	if ( PR_SUCC(err) )
		err = CALL_SYS_ObjectCreate		( parent, (hOBJECT*)&hMyIO, IID_IO, PID_NATIVE_FIO, 0);				
	if ( PR_SUCC(err) )
		err = CALL_SYS_PropertySetStr	( hMyIO, 0, pgOBJECT_FULL_NAME, (void*)(filename), strlen(filename), cCP_ANSI );
	if ( PR_SUCC(err) )
		err = CALL_SYS_PropertySetDWord	( hMyIO, pgOBJECT_ORIGIN, OID_MAIL_MSG_MIME);
	if ( PR_SUCC(err) )
		err = CALL_SYS_ObjectCreateDone ( hMyIO );

	if ( PR_SUCC(err) ) 
		err = FilterMessage(hMyIO, hFilter, filename);

	if ( hMyIO ) 
		CALL_SYS_ObjectClose		(hMyIO);
		
	return err;		
}

tERROR FilterMessage( hIO hMessage, CAntispamImplementer::CKLAntispamFilter& hFilter, const char* filename )
{
	hOS hMyOS = 0;
	cERROR err = hMessage->sysCreateObjectQuick( (hOBJECT*)&hMyOS, IID_OS, PID_MSOE );

	if ( PR_SUCC(err) ) 
	{
		hObjPtr optr;
		err = hMyOS->PtrCreate(&optr, NULL);
		if ( PR_SUCC(err) )
		{
			while ( PR_SUCC ( err = optr->Next() ) )
			{
				hIO io;
				if ( PR_SUCC( err = optr->IOCreate(&io, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
				{
					if ( io->get_pgOBJECT_ORIGIN() == OID_MAIL_MSG_BODY ) // Фильтруем только тела письма
						FilterIO(io, hFilter, filename);
					err = io->sysCloseObject();
				}
			}
			err = optr->sysCloseObject();
		}
		err = hMyOS->sysCloseObject();
	}
	else
	{
		// Check simple file
		FilterIO(hMessage, hFilter, filename);
	}
	return err;
}

tERROR FilterIO(hIO io, CAntispamImplementer::CKLAntispamFilter& hFilter, const char* filename )
{
	tERROR err = errOK;
	CAntispamCaller::CIOObjectToCheck hObjectToCheck(io);
	if ( bFirstTrain ) 
	{
		// Обучение!
		CAntispamImplementer::CAntispamFilter::TrainParams_t hTrainParams;
		hTrainParams.bIsSpam = bIsSpam;
		if ( PR_FAIL(err = hFilter.Train(&hObjectToCheck, &hTrainParams, INFINITE)) )
			std::cout << "hFilter.Train error: " << err << endl;
	}
	else
	{
		// Процессинг!
		long double S_bayes = 0;
		if ( PR_FAIL(err = hFilter.Process(&hObjectToCheck, NULL, &S_bayes, INFINITE, NULL)) )
			std::cout << "hFilter.Process error: " << err << endl;
		else
		{
			std::cout << filename << ";" ;
			if ( bIsSpam )
				std::cout << "0" << ";";
			else
				std::cout << "1" << ";";
			std::cout << S_bayes << endl;
			if ( 
				(bIsSpam && (S_bayes <= 0.5)) ||
				(!bIsSpam && (S_bayes > 0.5))
				)
			{
				g_ulErrors++;
//				std::cout << "S_bayes = " << S_bayes << endl;
//				OutputDebugString(" >> \t ");
//				OutputDebugString(filename);
			}
			g_ulFilesCount++;
		}
	}
	return err;
}

