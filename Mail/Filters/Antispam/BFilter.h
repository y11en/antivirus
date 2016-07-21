

#ifndef __BOGOFILTER_H__
#define __BOGOFILTER_H__


#pragma warning(disable : 4786)

#include "bayestable.h"
#include "textfilter.h"
#include <structs/s_antispam.h>


#define BOGO_X     0.5
#define BOGO_S     1.0
#define MIN_DEV    0.375
#define BOGO_Y     1.0
#define BOGO_Z     1.0
#define USE_YZ     false

#define CBayesFilter CBFilter

class CBFilter
{
public:
   
   typedef vector<CTextFilter::token_t> tokens_t;

   typedef struct __FLOAT {
      double m_dbMant ;
      long   m_lExp ;
   } FLOAT_t ;

public:

   CBFilter() ;

   virtual ~CBFilter() ;

   long double Process(const tokens_t& p_tokens) ;

   tERROR Init(const hOBJECT p_parent, 
               tCHAR* p_sfdb_path, 
               tCHAR* p_sfdb_save_to,
               double p_dbX = BOGO_X,
               double p_dbS = BOGO_S,
               double p_dbMinDev = MIN_DEV,
               bool p_blUseYZ = USE_YZ,
               double p_dbY = BOGO_Y,
               double p_dbZ = BOGO_Z) ;
   
	tERROR Update(IN const tokens_t& tokens, IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action);

public:
   static double Bogo_f(char* p_token,
						long p_lBad, 
                        long p_lGood, 
                        long p_lMsgBad, 
                        long p_lMsgGood, 
                        double p_s, 
                        double p_x) ;

   static double GetSpamicity(long p_lTokenCount, 
                              FLOAT_t p_P, 
                              FLOAT_t p_Q, 
                              double p_x,
                              bool p_blUseYZ,
                              double p_y, 
                              double p_z) ;
   
	inline tERROR ClearDB()
	{ return m_pBayesTable->ClearDB(); };

	inline tERROR Merge(CBayesFilter* pFilter)
	{ return m_pBayesTable->Merge(pFilter->GetBayesTable()); };

	bool GetData(
		IN char* pszRecordName,
		OUT CSFDBRecordData& data, 
		OUT OPTIONAL unsigned __int64* phKey = NULL
		);
	static bool CBayesFilter::GetData(
		IN char* pszRecordName,
		OUT CSFDBRecordData& data, 
		IN OPTIONAL CBayesTable* pBayesTable, 
		IN OPTIONAL hSECUREFILEDB hDataBase,
		OUT OPTIONAL unsigned __int64* phKey = NULL
		);

	inline CBayesTable* GetBayesTable() const { return m_pBayesTable; }
	
private:
	tERROR UpdateSpamHamLetters(IN /*bool bUpdateAsSpamt*/ cAntispamTrainParams::Action_t update_action);
	tERROR UpdateSpamHam(IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action, unsigned long ulCount);

private:
   double         m_dbX ;
   double         m_dbS ;
   double         m_dbMinDev ;
   double         m_dbY ;
   double         m_dbZ ;
   bool           m_blUseYZ ;
   CBayesTable*   m_pBayesTable ;
};

#endif // __BOGOFILTER_H__