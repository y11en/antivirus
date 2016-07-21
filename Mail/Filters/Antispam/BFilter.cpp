#define _DMP_TOKENS
#ifdef _DMP_TOKENS
#include <iostream>
using namespace std;
#endif//_DMP_TOKENS


#include <math.h>
#include <float.h>

#include "BFilter.h"


#define EVEN_ODDS   0.5
#define MANT_MIN    DBL_EPSILON//1.0e-200


extern double prbx(double p_x, double p_df) ;


CBFilter::CBFilter()
   : m_dbX(BOGO_X),
     m_dbS(BOGO_S),
     m_dbMinDev(MIN_DEV),
     m_dbY(BOGO_Y),
     m_dbZ(BOGO_Z),
     m_blUseYZ(USE_YZ),
     m_pBayesTable(NULL)
{
}

CBFilter::~CBFilter()
{
   if (m_pBayesTable)
   {
      delete m_pBayesTable ;
      m_pBayesTable = NULL ;
   }
}

tERROR CBFilter::Init(const hOBJECT p_parent, 
                         tCHAR* p_sfdb_path, 
                         tCHAR* p_sfdb_save_to, 
                         double p_dbX /*= BOGO_X*/, 
                         double p_dbS /*= BOGO_S*/, 
                         double p_dbMinDev /*= MIN_DEV*/, 
                         bool p_blUseYZ /*= USE_YZ*/, 
                         double p_dbY /*= BOGO_Y*/, 
                         double p_dbZ /*= BOGO_Z*/)
{
   tERROR error = errOK ;

   m_dbX      = p_dbX ;
   m_dbS      = p_dbS ;
   m_dbMinDev = p_dbMinDev ;
   m_blUseYZ  = p_blUseYZ ;
   m_dbY      = p_dbY ;
   m_dbZ      = p_dbZ ;

   if (m_pBayesTable)
   {
      return error ;
   }

   m_pBayesTable = new CBayesTable() ;
   if (!m_pBayesTable)
   {
      return errUNEXPECTED ;
   }

   error = m_pBayesTable->Init((hOBJECT)p_parent, p_sfdb_path, p_sfdb_save_to) ;
   if (PR_FAIL(error))
   {
      delete m_pBayesTable ;
      m_pBayesTable = NULL ;
   }

   return error ;
}

bool CBFilter::GetData(
	IN char* pszRecordName,
	OUT CSFDBRecordData& data, 
	OUT OPTIONAL unsigned __int64* phKey 
	)
{
	return GetData( pszRecordName, data, m_pBayesTable, NULL, phKey );
}
bool CBFilter::GetData(
	IN char* pszRecordName,
	OUT CSFDBRecordData& data, 
	IN OPTIONAL CBayesTable* pBayesTable, 
	IN OPTIONAL hSECUREFILEDB hDataBase,
	OUT OPTIONAL unsigned __int64* phKey
	)
{
	bool bRes = false;
	if ( !(pBayesTable||hDataBase) )
		return bRes;
	
	unsigned __int64 hKey = 0L;
	unsigned __int64* _phKey = phKey ? phKey : &hKey;
	CSFDBRecordKey key;
	key.pToken1 = (tBYTE*)pszRecordName;
	key.dwToken1Size = strlen(pszRecordName);
	if ( CBayesTable::HashKey(key, *_phKey) )
	{
		if ( pBayesTable )
			bRes = pBayesTable->GetData(*_phKey, data);
		else if ( hDataBase )
			bRes = CBayesTable::GetData(hDataBase, *_phKey, data);
	}
	return bRes;
}

double CBFilter::Bogo_f(char* token,
						long p_lBad, 
                        long p_lGood, 
                        long p_lMsgBad, 
                        long p_lMsgGood, 
                        double p_s, 
                        double p_x)
{
   int n = p_lBad + p_lGood ;
   if (n == 0)
   {
      return p_x ;
   }
   double pw = p_lBad * (double)p_lMsgGood / (p_lBad* (double)p_lMsgGood + p_lGood * (double)p_lMsgBad) ;
   double result = ( (p_s * p_x + n * pw) / (p_s + n) );

#ifdef _DMP_TOKENS
	{
		cout << token;
		cout << "; ";
		cout << p_lBad;
		cout << "; ";
		cout << p_lGood;
		cout << "; ";
		cout << result;
		cout << "\n";
	}
#endif // _DEBUG

	return result;
}

double CBFilter::GetSpamicity(long p_lTokenCount, 
                                 FLOAT_t p_P, 
                                 FLOAT_t p_Q, 
                                 double p_x, 
                                 bool p_blUseYZ,
                                 double p_y, 
                                 double p_z)
{
   if (p_lTokenCount == 0)
   {
      return p_x ;
   }

   double dbP_df   = 2.0 * p_lTokenCount * p_y ;
   double dbQ_df   = 2.0 * p_lTokenCount * p_z ;
   double dbLn2    = log(2.0) ;
   double dbP_Ln   = (log(p_P.m_dbMant) + p_P.m_lExp * dbLn2) * p_y ;
   double dbQ_Ln   = (log(p_Q.m_dbMant) + p_Q.m_lExp * dbLn2) * p_z ;
   double dbP_prbf = prbx(-2.0 * dbP_Ln, dbP_df) ;
   double dbQ_prbf = prbx(-2.0 * dbQ_Ln, dbQ_df) ;

   double dbResult = 0.0 ;
   if (!p_blUseYZ && p_y >= 1.0 && p_z >= 1.0)
   {
      dbResult = (1.0 + dbQ_prbf - dbP_prbf) / 2.0 ;
   }
   else if (dbQ_prbf < DBL_EPSILON && dbP_prbf < DBL_EPSILON)
   {
      dbResult = 0.5 ;
   }
   else
   {
      dbResult = dbQ_prbf / (dbQ_prbf + dbP_prbf) ;
   }

   return dbResult ;
}

long double CBFilter::Process(const tokens_t& p_tokens)
{
   FLOAT_t P = { 1.0, 0 } ;
   FLOAT_t Q = { 1.0, 0 } ;

   CSFDBRecordData dataCountAll ;
   GetData(">> SPAMHAM >>", dataCountAll);

   long lBadMsgCount  = dataCountAll.ulSpamCount ;
   long lGoodMsgCount = dataCountAll.ulHamCount ;

   long lTokenCount = 0 ;
   for (tokens_t::const_iterator iTerator = p_tokens.begin(); iTerator != p_tokens.end(); ++iTerator)
   {
      unsigned __int64 hKey = 0L ;
      CSFDBRecordKey key ;
      key.pToken1 = (tBYTE*)(iTerator->token1) ;
      key.dwToken1Size = stl_port::strlen(iTerator->token1) ;
      key.pToken2 = (tBYTE*)(iTerator->token2) ;
      key.dwToken2Size = stl_port::strlen(iTerator->token2) ;

      if (!m_pBayesTable->HashKey(key, hKey))
      {
         continue ;
      }

      CSFDBRecordData data ;
      if (!m_pBayesTable->GetData(hKey, data))
      {
         continue ;
      }

      double ft = Bogo_f(
						(char*)key.pToken1,
						data.ulSpamCount, 
                        data.ulHamCount, 
                        lBadMsgCount,
                        lGoodMsgCount,
                        m_dbS,
                        m_dbX) ;

      if (fabs(EVEN_ODDS - ft) - m_dbMinDev >= DBL_EPSILON) 
      {
         int exp = 0 ;
         P.m_dbMant *= 1 - ft ;
         if (P.m_dbMant < MANT_MIN)
         {
            P.m_dbMant = frexp(P.m_dbMant, &exp) ;
            P.m_lExp += exp ;
         }

         Q.m_dbMant *= ft ;
         if (Q.m_dbMant < MANT_MIN) 
         {
            Q.m_dbMant = frexp(Q.m_dbMant, &exp) ;
            Q.m_lExp += exp ;
         }

         ++lTokenCount ;
      }
   }

   double S = GetSpamicity(lTokenCount, P, Q, m_dbX, m_blUseYZ, m_dbY, m_dbZ) ;

   return S ;
}

tERROR CBFilter::Update(IN const tokens_t& tokens, IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action)
{
	cERROR err = errOK;
	if ( m_pBayesTable )
	{
		unsigned long i = 0;
		for (tokens_t::const_iterator 
			iTerator = tokens.begin();
			iTerator != tokens.end();
			++iTerator
			)
			{
				unsigned __int64 hKey = 0L;
				CSFDBRecordKey key;
				key.pToken1 = (tBYTE*)(iTerator->token1);
				key.dwToken1Size = stl_port::strlen(iTerator->token1);
				if ( m_pBayesTable->HashKey(key, hKey) )
				{
					CSFDBRecordData data;
					m_pBayesTable->GetData(hKey, data);
					switch (update_action) 
					{
					case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
					case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
					case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
					case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
					}
					if ( PR_FAIL(err = m_pBayesTable->Update(hKey, data)) )
						break;
					i++;
				}
			}
		if ( i )
		{
			UpdateSpamHam(update_action, i);
			UpdateSpamHamLetters(update_action);
		}
	}
	return err;
}

tERROR CBayesFilter::UpdateSpamHam(IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action, unsigned long ulCount)
{
	tERROR err = errNOT_OK;
	char spamham[] = ">> SPAMHAM >>";
	unsigned __int64 hKey = 0L;
	CSFDBRecordKey key;
	key.pToken1 = (tBYTE*)spamham;
	key.dwToken1Size = strlen(spamham);
	if ( m_pBayesTable->HashKey(key, hKey) )
	{
		CSFDBRecordData data;
		m_pBayesTable->GetData(hKey, data);
		switch (update_action) 
		{
		case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
		case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
		case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
		case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
		}
		err = m_pBayesTable->Update(hKey, data);
	}
	return err;
}

tERROR CBayesFilter::UpdateSpamHamLetters(IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action)
{
	tERROR err = errNOT_OK;
	char spamham[] = ">> SPAMHAM_LETTERS >>";
	unsigned __int64 hKey = 0L;
	CSFDBRecordKey key;
	key.pToken1 = (tBYTE*)spamham;
	key.dwToken1Size = strlen(spamham);
	if ( m_pBayesTable->HashKey(key, hKey) )
	{
		CSFDBRecordData data;
		m_pBayesTable->GetData(hKey, data);
		switch (update_action) 
		{
		case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
		case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
		case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
		case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
		}
		err = m_pBayesTable->Update(hKey, data);
	}
	return err;
}