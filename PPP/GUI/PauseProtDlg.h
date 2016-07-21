#ifndef INCLUDED_CPAUSEPROTDLG_C4ABFD99_4A82_455e_9F06_64498027776E
#define INCLUDED_CPAUSEPROTDLG_C4ABFD99_4A82_455e_9F06_64498027776E

#include "structs/s_gui.h"

// dialog shown on pause protection user request; for modal use
class CPauseProtDlg : public CDialogBindingViewT<cGuiPauseProfile>
{
public:
	CPauseProtDlg(cGuiPauseProfile* pProfile) : CDialogBindingViewT<cGuiPauseProfile>(pProfile) {};
	virtual ~CPauseProtDlg() {};

protected:
	bool OnClicked(CItemBase *pItem)
	{
		if( CDialogBindingViewT<cGuiPauseProfile>::OnClicked(pItem) )
			return true;

		sswitch(pItem->m_strItemAlias.c_str())
		scase("PauseProtForPeriod")
			m_pStruct->m_nResumeMode = cREQUEST_DELAY_MAKE_PERSISTENT;
			Close(DLG_ACTION_OK);
			sbreak;
		scase("PauseProtUntilRestart")
			m_pStruct->m_nResumeMode = 0;
			Close(DLG_ACTION_OK);
			sbreak;
		scase("PauseProtUntilRequest")
			m_pStruct->m_nResumeMode = -1;
			Close(DLG_ACTION_OK);
			sbreak;
		send
		return true;
	}
};

#endif // INCLUDED_CPAUSEPROTDLG_C4ABFD99_4A82_455e_9F06_64498027776E
