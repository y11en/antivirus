#pragma once


#include <list>

//using namespace std;

typedef struct _RUL_ITEM 
{
	WCHAR	symbolLink[260];
	WCHAR	deviceName[260];
	WCHAR	ValueName [260];
	int		w_3st;
	int		r_3st; 
	int		e_3st; 
	ULONG	blockID;
	BOOLEAN	logFlag;
}RUL_ITEM, *PRUL_ITEM;

typedef std::list <RUL_ITEM> RUL_ARRAY;

typedef struct _APPL_ITEM 
{
	WCHAR	path[260];
	BOOL	hash_ch; 
	BOOL	path_ch;
}APPL_ITEM, *PAPPL_ITEM;

typedef struct _APPL_RUL 
{
	APPL_ITEM apl_item;
	RUL_ARRAY *prul_array;

}APPL_RUL,*PAPPL_RUL;

typedef std::list <APPL_RUL> APPL_RUL_ARRAY;




class CCont
{
public:
	CCont(void);
	~CCont(void);
	HRESULT	CCont::Init();

	void CCont::AddAppl(PAPPL_ITEM pappl_item);
		void CCont::FillRulArray(RUL_ARRAY *pra);
	BOOL CCont::AddRulItem(PRUL_ITEM prul_item, int ApplNum);
	void CCont::DelAppl(int ApplNum);
	
	RUL_ARRAY* CCont::GetRulArray(int ApplNum);
	void CCont::ChangeApplItem(PAPPL_ITEM pappl_item, int ApplNum);
	void CCont::ChangeRulItem(PRUL_ITEM prul_item, int ApplNum, int RulNum);

	HRESULT ResetApplRulesFromDriver();
	HRESULT InsertApplRulesToDriver();
	

	void CCont::Convert_MaskToWRE( ULONG mask, int *pw_3st, int *pr_3st,int *pe_3st );
	ULONG CCont::WREToMask( int w_3st, int r_3st,int e_3st );
	//BOOL ResetApplRules();


private:
	APPL_RUL_ARRAY conteiner;

	char* pClientContext;
	ULONG ApiVersion;
	ULONG AppId;

};
