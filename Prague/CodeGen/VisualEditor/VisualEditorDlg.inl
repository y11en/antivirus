#ifndef _VisualEditorDlg_inl_included
#define _VisualEditorDlg_inl_included

//----------------------------------------------------------------------------------//
//																					//
//						CVisualEditorDlg inlines									//
//																					//
//----------------------------------------------------------------------------------//

inline BOOL  CVisualEditorDlg::IsModified() 
{
	return m_bChanged;
}
inline void CVisualEditorDlg::SetModified( BOOL bModified ) 
{
	m_bChanged = bModified;
}
inline BOOL CVisualEditorDlg::IsPrototype()
{
	return wmt_IDefinition == m_nWorkMode; 
}
inline BOOL CVisualEditorDlg::IsImplementation()
{
	return wmt_IImplementation == m_nWorkMode;
}

inline BOOL	CVisualEditorDlg::IsEditableData(HDATA i_hData, BOOL i_bLinkedToFile)
{
	return !i_bLinkedToFile && (IsPrototype() || !IsNativeData(i_hData)); 
}

inline CVisualEditorDlg::STreeNodeInfo::STreeNodeInfo
(
		CCTItemsArray&	i_newItems,
		int				i_nItemLevel,
		HDATA			i_hData, 
		int				i_nPropId, 
		BOOL			i_bEditable, 
		BOOL			i_bCanBeRemoved,
		int				i_nSpecialTypeId 
) :	m_newItems(i_newItems)
{
	m_hData				= i_hData;
	m_nPropId			= i_nPropId;
	m_nArrayPosition	= 0;
	m_nPrefixId			= 0;
	m_nSpecialTypeId	= i_nSpecialTypeId;
	m_bEditable			= i_bEditable;
	m_bCanBeRemoved		= i_bCanBeRemoved;
	m_nItemLevel		= i_nItemLevel;
	m_strTitle			= NULL;
	m_eTreeNodeType		= MD::eString;
	m_pExportedItem		= NULL;
}

#endif