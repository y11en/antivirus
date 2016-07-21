// Editor.cpp: implementation of the editor
//

#include "StdAfx.h"

#include <ProductCore/ItemRoot.h>
#include "../../Prague/IniLib/IniRead.h"
#include "SysSink.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void _Str_TruncateSpaces(tString& str)
{
	bool bSpace = true;
	for(tDWORD i = 0; i < str.size(); i++)
	{
		if( str[i] == ' ' )
		{
			if( bSpace )
				str.erase(i, 1), i--;
			else
				bSpace = true;
		}
		else
			bSpace = false;
	}

	if( bSpace && str.size() )
		str.erase(str.size() - 1);
}

//////////////////////////////////////////////////////////////////////

static tERROR _Edit_CleanSection(LPCSTR strSection, sIni * pIni, bool bErase)
{
	sIniSection * pSecPrev;
	sIniSection * pSec = IniGetSection(pIni->head, strSection, &pSecPrev);
	if( !pSec || (!bErase && !pSec->line_count) )
		return warnFALSE;
	
	if( bErase )
		return pSec->line_count ? warnFALSE : IniDelSectionEx(pIni, pSec, pSecPrev);
	
	// Зачистка содержимого секции, исключая макросы
	sIniLine * p = NULL;
	for(sIniLine * i = pSec->line_head; i; )
	{
		sIniLine * n = i->next;

		char * name = i->name;
		if( *name != '$' && *name != '#' )
		{
			tERROR err = IniDelLineEx(pSec, i, p);
			if( PR_FAIL(err) )
				return err;
			i = n;
		}
		else
		{
			p = i;
			i = n;
		}
	}

	return errOK;
}

static tERROR _Edit_PrepareSection(CItemBase * pItem, sIni * pIni, sIni * pIniNew)
{
	if( pItem->m_strSection.empty() )
		return warnFALSE;
	
	tERROR err = _Edit_CleanSection(pItem->m_strSection.c_str(), pIni, false);
	if( PR_FAIL(err) )
		return err;
	
	if( pItem->m_pEditData->m_strSection == pItem->m_strSection && !pIniNew )
		return errOK;
	
	sIniSection * pSecPrev;
	sIniSection * pSec = IniGetSection(pIni->head, pItem->m_strSection.c_str(), &pSecPrev);
	if( !pSec )
		return warnFALSE;
	
	if( !pIniNew )
		return IniSetSectionName(pSec, pItem->m_pEditData->m_strSection.c_str());

	sIniSection * pSecNew = IniCreateSection(pIniNew, pItem->m_pEditData->m_strSection.c_str());
	if( !pSecNew )
		return errNOT_ENOUGH_MEMORY;

	for(sIniLine * i = pSec->line_head; i; i = i->next)
		if( PR_FAIL(err = IniSetValue(IniCreateLine(pSecNew, i->name), IniGetValueEx(i, cTRUE))) )
			return err;
	
	return IniDelSectionEx(pIni, pSec, pSecPrev);
}

static void _Edit_StoreDeletedSections(CItemBase * pTop, CItemBase * pItem)
{
	if( !pItem->m_pEditData || pItem->m_strSection.empty() )
		return;
	if( pItem->m_pEditData->IsLinked() )
		return;

	if( pItem->m_pEditData->IsOverridable() &&
		pItem->m_pEditData->m_strOverridableOrigSection == pItem->m_strSection )
		return;

	tDWORD i;
	
	for(i = 0; i < pTop->m_pEditData->m_aDelSects.size(); i++)
		if( pTop->m_pEditData->m_aDelSects[i] == pItem->m_strSection )
			break;
	
	if( i == pTop->m_pEditData->m_aDelSects.size() )
		pTop->m_pEditData->m_aDelSects.push_back(pItem->m_strSection);

	for(i = 0; i < pItem->m_aItems.size(); i++)
		_Edit_StoreDeletedSections(pTop, pItem->m_aItems[i]);
}

struct CRootItem::CSaveCtx
{
	CSaveCtx() : ini(NULL), loc(NULL), last_sec(NULL), last_sec_loc(NULL) {}
	
	struct _A
	{
		_A() : ini(NULL) {}
		~_A() { if( ini ) IniFileFree(ini), delete ini; }
		
		cStrObj			name;
		cAutoObj<cIO>	pIO;
		sIni *			ini;
	};
	
	cVector<_A, cSimpleMover<_A> > m_a;
	cVector<cStrObj, cSimpleMover<cStrObj> > m_groups;

	sIni *			ini;
	sIni *			loc;
	sIniSection *	last_sec;
	sIniSection *	last_sec_loc;
};

tERROR CRootItem::_Edit_GetIniEx(CSaveCtx& _ctx, sIni ** ppIni, cStrObj& strPath)
{
	for(tDWORD i = 0; i < _ctx.m_a.size(); i++)
	{
		CSaveCtx::_A& _a = _ctx.m_a[i];
		if( _a.name == strPath )
			return *ppIni = _a.ini, errOK;
	}
	
	CSaveCtx::_A& _a = _ctx.m_a.push_back();
	_a.name = strPath;
	_a.ini = new sIni();
	if( !_a.ini )
		return errNOT_ENOUGH_MEMORY;
	memset(_a.ini, 0, sizeof(sIni));

	tERROR err = errNOT_OK;
	while( PR_FAIL(err) )
	{
		err = IniIOCreate(&_a.pIO, strPath, fACCESS_READ);
		if( err == errNOT_OK /*OTHER CODE !!!!!!!!!!!!!!!!!!!!!!!!!!*/ )
			err = errOK;
		if( !!_a.pIO )
			err = IniFileLoad(_a.ini, _a.pIO, INI_LOAD_MULTILINEVALS);

		if( PR_SUCC(err) || !m_pSink )
			break;
		
		tERROR nAction = ((CRootSink *)m_pSink)->OnFileSaveError(strPath, err);
		if( PR_FAIL(nAction) )
			break;
	}

	if( PR_FAIL(err) )
	{
		_ctx.m_a.remove(_ctx.m_a.size() - 1);
		return *ppIni = NULL, err;
	}

	return *ppIni = _a.ini, errOK;
}

tERROR CRootItem::_Edit_GetIni(CSaveCtx& _ctx, struct tagIni ** ppIni, LPCWSTR strGroup, bool bLocalize)
{
	tDWORD i;
	for(i = 0; i < _ctx.m_groups.size(); i++)
		if( _ctx.m_groups[i] == strGroup )
			break;

	if( i == _ctx.m_groups.size() )
		_ctx.m_groups.push_back() = strGroup;

	cStrObj strPath;
	if( bLocalize )
	{
		strPath = INI_DIR_LOCALIZE;
		strPath.add_path_sect(m_strLocName.c_str(), cCP_ANSI);
	}
	else
		strPath = INI_DIR_LAYOUT;
	strPath.add_path_sect(strGroup);
	strPath += bLocalize ? ".loc" : ".ini";
	return _Edit_GetIniEx(_ctx, ppIni, strPath);
}

tERROR CRootItem::_Edit_Commit(CSaveCtx& _ctx)
{
	tERROR err;

	// Обновление перечня файлов скина
	{
		sIni * pIni = NULL;
		if( PR_FAIL(err = _Edit_GetIniEx(_ctx, &pIni, TOR(cStrObj, (INI_FILE_SKINROOT)))) )
			return err;

		sIniSection * pSecLayout = IniCreateSection(pIni, PROFILE_SID_LAYOUT);
		sIniSection * pSecLoc = IniCreateSection(pIni, PROFILE_SID_LOCALIZE);
		
		tString strGroup, strFile;
		for(tDWORD i = 0; i < _ctx.m_groups.size(); i++)
		{
			strGroup = _ctx.m_groups[i].c_str(cCP_ANSI);

			strFile = strGroup; strFile += ".ini";
			IniCreateLine(pSecLayout, strFile.c_str());
			
			strFile = strGroup; strFile += ".loc";
			IniCreateLine(pSecLoc, strFile.c_str());
		}
	}

	// Сохранение измененных файлов
	sIniSaveParams _prm; memset(&_prm, 0, sizeof(_prm));
	_prm.flags = INI_SAVE_SECCOMMENTSATBOTTOM|INI_SAVE_ACCURATELOOK|INI_SAVE_VOIDVALSNOASSIGN;
	_prm.tab_spaces = 4;

	for(tDWORD i = 0; i < _ctx.m_a.size(); i++)
	{
		CSaveCtx::_A& _a = _ctx.m_a[i];

		// Получаем старое содержимое
		tDWORD nPrevSize = 0;
		tCHAR * pPrev = NULL;
		if( !!_a.pIO )
		{
			tQWORD qsize;
			if( PR_FAIL(err = _a.pIO->GetSize(&qsize, IO_SIZE_TYPE_EXPLICIT)) )
				return err;
			if( PR_FAIL(err = g_root->heapAlloc((tPTR *)&pPrev, (tDWORD)qsize)) )
				return err;
			if( PR_FAIL(err = _a.pIO->SeekRead(&nPrevSize, 0, pPrev, (tDWORD)qsize)) )
				return err;
			if( nPrevSize != (tDWORD)qsize )
				return errNOT_OK;
		}

		// Получаем новое содержимое
		tDWORD nNewSize = 0;
		tCHAR * pNew = NULL;
		if( PR_FAIL(err = IniFileCreateBuf(_a.ini, (hOBJECT)g_root, &pNew, &nNewSize, &_prm)) )
		{
			if( pPrev )
				g_root->heapFree(pPrev);
			return err;
		}

		// Сравниваем старое и новое содержимые
		if( nPrevSize == nNewSize && pPrev && pNew && !memcmp(pPrev, pNew, nNewSize) )
		{
			g_root->heapFree(pPrev);
			g_root->heapFree(pNew);
			continue;
		}
		
		// Если данные поменялись, пытаемся записать в файл
		err = errNOT_OK; bool bForceRo = false;
		while( PR_FAIL(err) )
		{
			err = IniIOCreate(&_a.pIO, _a.name, fACCESS_READ|fACCESS_WRITE, bForceRo);
			if( PR_SUCC(err) )
			{
				tDWORD w_size = 0;
				tCHAR empty [] = "";
				if( PR_SUCC(err = _a.pIO->SeekWrite(&w_size, 0, nNewSize ? pNew : empty, nNewSize)) )
					if( w_size != nNewSize )
						err = errBAD_SIZE;

				if( PR_SUCC(err) )
					err = _a.pIO->SetSize(nNewSize);
			}

			if( PR_SUCC(err) || !m_pSink )
				break;
			
			tERROR nAction = ((CRootSink *)m_pSink)->OnFileSaveError(_a.name, err);
			if( PR_FAIL(nAction) )
				break;
			
			if( nAction == warnFALSE )
				bForceRo = true;
		}
	
		if( pPrev )
			g_root->heapFree(pPrev);
		if( pNew )
			g_root->heapFree(pNew);

		if( PR_FAIL(err) )
			return err;

		if( PR_FAIL(err = _a.pIO.clean()) )
			return err;
	}

	return errOK;
}

tERROR CRootItem::SaveToSkin(CItemBase * pItem)
{
	tERROR err;
	CSaveCtx _ctx;

	// Подчистка содержимого ненужных секций
	_Edit_DelSections(_ctx, pItem);
	
	if( PR_FAIL(err = _Save_Item(_ctx, pItem, pItem)) )
		return err;

	// Удаление пустых секций
	_Edit_DelSections(_ctx, pItem, true);
	_Edit_CleanItemsSection(_ctx, pItem);

	if( PR_FAIL(err = _Edit_Commit(_ctx)) )
		return err;
	
	pItem->m_pEditData->m_aDelSects.clear();
	_Save_Finalize(pItem, pItem);

	Refresh(REFRESH_SKIN_DATA);
	return errOK;
}

tERROR CRootItem::_Edit_CleanItemsSection(CSaveCtx& _ctx, CItemBase * pItem)
{
	if( !pItem->m_pEditData )
		return errOK;
	if( pItem->m_pEditData->IsLinked() )
		return errOK;

	if( pItem->m_pEditData->m_strSection.empty() )
		return errOK;
	
	sIni * pIni = NULL;
	sIni * pLoc = NULL;

	tERROR err;
	if( PR_FAIL(err = _Edit_GetIni(_ctx, &pIni, pItem->m_pEditData->m_strGroup.data())) )
		return err;
	if( PR_FAIL(err = _Edit_GetIni(_ctx, &pLoc, pItem->m_pEditData->m_strGroup.data(), true)) )
		return err;

	if( PR_FAIL(err = _Edit_CleanSection(pItem->m_pEditData->m_strSection.c_str(), pIni, true)) )
		return err;
	if( PR_FAIL(err = _Edit_CleanSection(pItem->m_pEditData->m_strSection.c_str(), pLoc, true)) )
		return err;

	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
		if( PR_FAIL(err = _Edit_CleanItemsSection(_ctx, pItem->m_aItems[i])) )
			return err;

	return errOK;
}

tERROR CRootItem::DeleteFromSkin(LPCSTR strSection)
{
	tString strProps; GetString(strProps, PROFILE_LAYOUT, strSection, STR_SELF);
	if( strProps.empty() )
		return warnFALSE;
	
	bool bExt = false;
	{
		CItemProps _Props(strProps.c_str());
		if( _Props.Get(STR_PID_TYPE).Get() != GUI_ITEMT_DIALOG )
		{
			if( !_Props.Get(STR_PID_EXT).GetBool() )
				return warnFALSE;
		
			bExt = true;
		}
	}

	CItemBase * pItem = NULL;
	CItemBase * pContainer = NULL;
	if( bExt )
	{
		tString strContProps;
		{
			CItemProps _Props; _Props.Get(STR_PID_TYPE, true).Set(0, GUI_ITEMT_DIALOG);
			_Props.DeParse(strContProps);
		}
		
		LoadItem(NULL, pContainer, strContProps.c_str(), "_cont", LOAD_FLAG_INFO);
		if( !pContainer )
			return errNOT_OK;
		
		pContainer->LoadItem(NULL, pItem, strSection, NULL, LOAD_FLAG_PRELOAD|LOAD_FLAG_EDIT);
	}
	else
		LoadItem(NULL, pItem, strSection, NULL, LOAD_FLAG_PRELOAD|LOAD_FLAG_EDIT);

	if( !pItem )
		return errNOT_OK;

	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
		_Edit_StoreDeletedSections(pItem, pItem->m_aItems[i]);
	pItem->m_pEditData->m_aDelSects.push_back(pItem->m_strSection);
	
	CSaveCtx _ctx;
	_Edit_DelSections(_ctx, pItem);
	_Edit_DelSections(_ctx, pItem, true);
	tERROR err = _Edit_Commit(_ctx);

	((CDialogItem *)(pContainer ? pContainer : pItem))->Close();
	
	if( PR_SUCC(err) )
		Refresh(REFRESH_SKIN_DATA);
	return err;
}

tERROR CRootItem::_Save_Item(CSaveCtx& _ctx, CItemBase * pTop, CItemBase * pItem)
{
	if( !pItem->m_pEditData )
		return errOK;
	
	tERROR err;

	if( pItem->m_pEditData->IsLinked() )
		return errOK;

	// Автогенерация имен для дочерних внутренних секций
	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
	{
		CItemBase * p = pItem->m_aItems[i];
		if( p->m_strSection.empty() || !p->m_pEditData )
			continue;

		if( (p->m_nFlags & STYLE_EXT) /*&& p->m_pEditData->m_bExtProp*/ )
			continue;
		
		p->m_pEditData->m_strSection = pItem->m_pEditData->m_strSection;
		p->m_pEditData->m_strSection += "_";
		p->m_pEditData->m_strSection += p->m_pEditData->m_strItemId;

		p->m_pEditData->m_strGroup = pItem->m_pEditData->m_strGroup;
	}
	
	if( PR_FAIL(err = _Save_SectItem(_ctx, pTop, pItem)) )
		return err;
	
	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
	{
		CItemBase * pChild = pItem->m_aItems[i];
		if( pChild->m_strSection.empty() )
			continue;
		
		if( PR_FAIL(err = _Save_Item(_ctx, pTop, pChild)) )
			return err;
	}

	return errOK;
}

sIniSection * CRootItem::_Edit_GetSection(CSaveCtx& _ctx, sIni* pIni, const tCHAR* section_name, bool bLocalize)
{
	sIniSection * pSec = IniCreateSection(pIni, section_name);
	if( !pSec )
		return NULL;

	sIni *&			ini = bLocalize ? _ctx.loc : _ctx.ini;
	sIniSection *&	last_sec = bLocalize ? _ctx.last_sec_loc : _ctx.last_sec;

	if( ini )
	{
		if( pIni == ini )
		{
			pSec = IniMoveSection(pIni, pSec, last_sec);
			last_sec = pSec;
			if( !pSec )
				return NULL;
		}
	}
	else
	{
		ini = pIni;
		last_sec = pSec;
	}

	return pSec;
}

tERROR	CRootItem::_Save_SectItem(CSaveCtx& _ctx, CItemBase * pTop, CItemBase * pItem)
{
	tERROR err;
	
	sIni * pIni = NULL;
	sIni * pLoc = NULL;

	{
		if( PR_FAIL(err = _Edit_GetIni(_ctx, &pIni, pItem->m_pEditData->m_strGroup.data())) )
			return err;
		if( PR_FAIL(err = _Edit_GetIni(_ctx, &pLoc, pItem->m_pEditData->m_strGroup.data(), true)) )
			return err;

		sIni * pOldIni = NULL;
		sIni * pOldLoc = NULL;
		if( pItem->m_pEditData->m_strOldGroup.size() && pItem->m_pEditData->m_strOldGroup != pItem->m_pEditData->m_strGroup )
		{
			if( PR_FAIL(err = _Edit_GetIni(_ctx, &pOldIni, pItem->m_pEditData->m_strOldGroup.data())) )
				return err;
			if( PR_FAIL(err = _Edit_GetIni(_ctx, &pOldLoc, pItem->m_pEditData->m_strOldGroup.data(), true)) )
				return err;
		}

		// Удаление старых элементов в файле
		_Edit_PrepareSection(pItem, (pOldIni ? pOldIni : pIni), (pOldIni ? pIni : NULL));
		_Edit_PrepareSection(pItem, (pOldLoc ? pOldLoc : pLoc), (pOldLoc ? pLoc : NULL));
	}
	
	sIniSection * pIniSec = _Edit_GetSection(_ctx, pIni, pItem->m_pEditData->m_strSection.c_str());
	if( !pIniSec )
		return errNOT_ENOUGH_MEMORY;

	// Запись новой строки свойств для секции
	if( pItem->m_pEditData->m_strProps.size() )
		IniSetValue(IniCreateLine(pIniSec, STR_SELF), pItem->m_pEditData->m_strProps.c_str());
	
	if( !(pItem->m_nFlags & STYLE_EXT) && pItem->m_pEditData->m_bExtProp )
	{
		pItem->m_nFlags |= STYLE_EXT;
		pItem->m_pEditData->m_strProps.erase();
		Edit_UpdateItemProps(pTop, pItem);
	}
	
	// Запись локализации для внешнего элемента
	if( (pItem->m_nFlags & STYLE_EXT_ITEM) &&
		(pItem->m_pEditData->m_strText.size() || pItem->m_pEditData->m_strTipText.size()) )
	{
		sIniSection * pLocSec = _Edit_GetSection(_ctx, pLoc, pItem->m_pEditData->m_strSection.c_str(), true);
		if( !pLocSec )
			return errNOT_ENOUGH_MEMORY;

		if( pItem->m_pEditData->m_strText.size() )
			IniSetValue(IniCreateLine(pLocSec, STR_SELF), pItem->m_pEditData->m_strText.c_str());
	
		if( pItem->m_pEditData->m_strTipText.size() )
			IniSetValue(IniCreateLine(pLocSec, STR_SELF GUI_STR_AUTOSECT_SEP_STR STR_PID_TIPTEXT), pItem->m_pEditData->m_strTipText.c_str());
	}
	
	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
	{
		CItemBase * pChild = pItem->m_aItems[i];
		if( !pChild->m_pEditData )
			continue;
			
		// Сброс признака "External"
		if( (pChild->m_nFlags & STYLE_EXT) && !pChild->m_pEditData->m_bExtProp )
		{
			pChild->m_nFlags &= ~STYLE_EXT;
			pChild->m_pEditData->m_bLinked = 0;
			
			tString strOwnProps; GetString(strOwnProps, PROFILE_LAYOUT, pChild->m_strSection.c_str(), STR_SELF, "", GETSTRING_RAW);
			pChild->m_pEditData->m_strProps.insert(0, strOwnProps.c_str());

			if( pChild->m_pEditData->m_strText.empty() )
				GetString(pChild->m_pEditData->m_strText, PROFILE_LOCALIZE, pChild->m_strSection.c_str(), STR_SELF, "", GETSTRING_RAW);

			Edit_UpdateItemProps(pTop, pChild);

			sIni * pIni = NULL; sIni * pLoc = NULL;
			if( PR_SUCC(err = _Edit_GetIni(_ctx, &pIni, pChild->m_pEditData->m_strGroup.data())) )
				err = _Edit_GetIni(_ctx, &pLoc, pChild->m_pEditData->m_strGroup.data(), true);
			
			if( PR_SUCC(err) )
			{
				IniSetValue(IniCreateLine(IniCreateSection(pIni, pChild->m_pEditData->m_strSection.c_str()), STR_SELF), pChild->m_pEditData->m_strProps.c_str());
				IniDelLine(IniGetSection(pLoc->head, pChild->m_pEditData->m_strSection.c_str()), STR_SELF);
			}

			pChild->m_nFlags |= STYLE_EXT;
			pChild->m_pEditData->m_bLinked = 1;

			if( PR_FAIL(err) )
				return err;
		}

		// Запись новой строки для дочернего элемента
		tString strItemVal;
		if( pChild->m_strSection.size() )
		{
			// Добавление нового имени секции в начало строки свойств
			strItemVal += "[";
			strItemVal += pChild->m_pEditData->m_strSection;
			strItemVal += "]";

			if( pChild->m_nFlags & STYLE_EXT )
			{
				if( pChild->m_pEditData->m_strProps.size() )
				{
					strItemVal += " ";
					strItemVal += pChild->m_pEditData->m_strProps;
				}
				
				_Save_ExtOverrides(pChild, strItemVal, pLoc);
			}
		}
		else
			strItemVal = pChild->m_pEditData->m_strProps;
		
		IniSetValue(IniCreateLine(pIniSec, pChild->m_pEditData->m_strItemId.c_str()), strItemVal.c_str());

		// Запись локализации для элемента
		if( pChild->m_pEditData->m_strText.size() || pChild->m_pEditData->m_strTipText.size() )
		{
			sIniSection * pLocSec = _Edit_GetSection(_ctx, pLoc, pItem->m_pEditData->m_strSection.c_str(), true);
			if( !pLocSec )
				return errNOT_ENOUGH_MEMORY;

			if( pChild->m_pEditData->m_strText.size() )
				IniSetValue(IniCreateLine(pLocSec, pChild->m_pEditData->m_strItemId.c_str()), pChild->m_pEditData->m_strText.c_str());

			if( pChild->m_pEditData->m_strTipText.size() )
				IniSetValue(IniCreateLine(pLocSec, (pChild->m_pEditData->m_strItemId + GUI_STR_AUTOSECT_SEP_STR STR_PID_TIPTEXT).c_str()), pChild->m_pEditData->m_strTipText.c_str());
		}
	}
	
	return errOK;
}

tERROR CRootItem::_Save_ExtOverrides(CItemBase * pExtItem, tString& strProps, sIni * pLoc)
{
	Overrides _ovrds;
	
	CItemBase * pOver = NULL;
	while( pOver = pExtItem->EnumNextItem(pOver) )
	{
		if( !pOver->m_pEditData || !pOver->m_pEditData->IsOverridable() )
			continue;
		
		// Генерация строки свойств
		tString strExtProps = pOver->m_pEditData->m_strProps;
		if( pOver->m_pEditData->m_strOverridableOrigSection != pOver->m_pEditData->m_strSection )
		{
			if( strExtProps.size() )
				strExtProps += " ";
			strExtProps += "[";
			strExtProps += pOver->m_pEditData->m_strSection;
			strExtProps += "]";
		}
		
		if( strExtProps.empty() && pOver->m_pEditData->m_strText.empty() )
			continue;
		
		Override * pOvrd = __GetOverrideItem(_ovrds, &pOver->m_pEditData->m_strOverrideId[0], true);
		if( !pOvrd )
			continue;

		pOvrd->m_strProps = strExtProps;
		
		// Сохранение локализации
		if( pOver->m_pEditData->m_strText.size() )
		{
			if( pLoc )
			{
				tString strLocId;
				strLocId += pExtItem->m_pEditData->m_strItemId;
				strLocId += GUI_STR_AUTOSECT_SEP_STR;
				strLocId += pOver->m_pEditData->m_strOverrideId;
				
				IniSetValue(IniCreateLine(IniCreateSection(pLoc, pExtItem->m_pParent->m_pEditData->m_strSection.c_str()), strLocId.c_str()), pOver->m_pEditData->m_strText.c_str());
			}
			else
				pOvrd->m_strText = pOver->m_pEditData->m_strText;
		}
	}
	
	_GetOverridesPropString(_ovrds, strProps, !pLoc);
	return errOK;
}

void CRootItem::_Edit_DelSections(CSaveCtx& _ctx, CItemBase * pItem, bool bErase)
{
	for(tDWORD i = 0; i < pItem->m_pEditData->m_aDelSects.size(); i++)
	{
		tString& strSection = pItem->m_pEditData->m_aDelSects[i];
		LPCWSTR strGroup = GetSectionGroup(PROFILE_LAYOUT, strSection.c_str());
		if( !strGroup )
			continue;

		sIni * pIni = NULL; _Edit_GetIni(_ctx, &pIni, strGroup);
		sIni * pLoc = NULL; _Edit_GetIni(_ctx, &pLoc, strGroup, true);

		_Edit_CleanSection(strSection.c_str(), pIni, bErase);
		_Edit_CleanSection(strSection.c_str(), pLoc, bErase);
	}
}

void CRootItem::_Save_Finalize(CItemBase * pTop, CItemBase * pItem)
{
	if( pItem->m_pEditData )
	{
		pItem->m_pEditData->m_strOldGroup = pItem->m_pEditData->m_strGroup;
		pItem->m_strSection = pItem->m_pEditData->m_strSection;
		pItem->m_strItemId = pItem->m_pEditData->m_strItemId;
	}

	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
		_Save_Finalize(pTop, pItem->m_aItems[i]);
}

//////////////////////////////////////////////////////////////////////

void CRootItem::Edit_UpdateItemProps(CItemBase * pTop, CItemBase * pItem, enGuiEditPropsMode eMode)
{
	if( !pItem || !pItem->m_pEditData )
		return;
	
	if( eMode == guiepmUpdateEditable )
	{
		tCHAR szNum[20];

		// Size
		{
			CItemPropVals& prp = pItem->m_pEditData->m_Props.Get(STR_PID_SIZE, true);
			
			if( pItem->m_nFlags & AUTOSIZE_X )
				prp.Set(0, "a");
			else if( pItem->m_nFlags & AUTOSIZE_PARENT_X )
				prp.Set(0, "p");
			else
				prp.Set(0, _ltoa(pItem->m_szSize.cx, szNum, 10));
			
			if( pItem->m_nFlags & AUTOSIZE_Y )
				prp.Set(1, "a");
			else if( pItem->m_nFlags & AUTOSIZE_PARENT_Y )
				prp.Set(1, "p");
			else
				prp.Set(1, _ltoa(pItem->m_szSize.cy, szNum, 10));
		}

		// Align
		{
			CItemPropVals& prp = pItem->m_pEditData->m_Props.Get(STR_PID_ALIGN, true);
			tString v;
			
			if( pItem->m_nFlags & ALIGN_RIGHT )
				v += "r";
			else if( pItem->m_nFlags & ALIGN_HCENTER )
				v += "c";
			else if( pItem->m_nFlags & ALIGN_HBEFORE )
				v += "p";
			else if( pItem->m_nFlags & ALIGN_HAFTER )
				v += "a";
			else if( pItem->m_nFlags & ALIGN_HSAME )
				v += "s";
			else
				v += "l";

			if( pItem->m_nFlags & ALIGN_BOTTOM )
				v += "b";
			else if( pItem->m_nFlags & ALIGN_VCENTER )
				v += "c";
			else if( pItem->m_nFlags & ALIGN_VBEFORE )
				v += "p";
			else if( pItem->m_nFlags & ALIGN_VAFTER )
				v += "a";
			else if( pItem->m_nFlags & ALIGN_VSAME )
				v += "s";
			else
				v += "t";

			prp.Set(0, v.c_str());
		}

		return;
	}
	
	CItemProps _PropsUpdate;
	{
		CItemProps _Props;
		CItemProps _RawProps;
		CItemProps _BaseFixedProps;

		// Получение сырых предыдущих свойств элемента
		tString strMacroses;
		_RawProps.Parse(pItem->m_pEditData->m_strProps.c_str(), strMacroses);
		_Str_TruncateSpaces(strMacroses);

		// Получение реальных предыдущих свойств элемента
		_Props.Parse(_GetString(TOR(tString,(pItem->m_pEditData->m_strProps)), PROFILE_LAYOUT, (!(pItem->m_nFlags & STYLE_EXT) && pItem->m_strSection.empty() && pItem->m_pParent) ? pItem->m_pParent->m_strSection.c_str() : pItem->m_strSection.c_str(), "", pItem));

		// Перенос новых свойств к старым
		{
			CItemProps _NewProps;
			_NewProps.Get(STR_PID_EXT, true).Set(0, pItem->m_pEditData->m_bExtProp ? "1" : "");
			_NewProps.Get(STR_PID_EXTPRM, true).Set(0, (pItem->m_nFlags & STYLE_EXTPRM) ? "1" : "");
			if( eMode != guiepmUpdateFromString )
			{
				_NewProps.Get(STR_PID_TYPE, true).Get(0, true) = pItem->m_strItemType;
				_NewProps.Assign(pItem->m_pEditData->m_Props, IPV_F_APPEND);
			}

			for(int i = _NewProps.Count(); i > 0; i--)
			{
				CItemPropVals& prp = _NewProps.GetByIdx(i - 1);
				if( _Props.Get(prp.GetName()).IsEqual(prp) )
					_NewProps.DelByIdx(i - 1);
			}

			_RawProps.Assign(_NewProps, IPV_F_APPEND);
			_Props.Assign(_NewProps, IPV_F_APPEND);

			if( !_Props.Get(STR_PID_TEXT).IsValid() && pItem->m_pEditData->m_strText.size() )
				_Props.Get(STR_PID_TEXT, true).Get(0, true) = _GetString(TOR(tString,(pItem->m_pEditData->m_strText)), PROFILE_LOCALIZE, pItem->m_pEditData->m_pAttrOwnerParent->m_strSection.c_str(), "", pItem->m_pEditData->m_pAttrOwnerParent);
		}

		// Удаление новых свойств, эквивалентных с собственными "External" или "Overridable" элемента и которые в макросах
		{
			// Взятие свойств собственных "External" или "Overridable" элемента
			pItem->m_pEditData->GetBaseInfo(_BaseFixedProps);
			
			// Взятие свойств, которые в макросах
			_BaseFixedProps.Parse(_GetString(TOR(tString, (strMacroses)), PROFILE_LAYOUT, (!(pItem->m_nFlags & STYLE_EXT) && pItem->m_strSection.empty() && pItem->m_pParent) ? pItem->m_pParent->m_strSection.c_str() : pItem->m_strSection.c_str(), "", pItem), TOR_tString, NULL, IPV_F_APPEND);

			for(int i = 0; i < _BaseFixedProps.Count(); i++)
			{
				CItemPropVals& prp = _BaseFixedProps.GetByIdx(i);
				if( _Props.Get(prp.GetName()).IsEqual(prp) )
				{
					_RawProps.Del(prp.GetName());
					_Props.Del(prp.GetName());
				}
			}
		}

		// Создание новой строки свойств
		{
			pItem->m_pEditData->m_strProps = strMacroses;
			_RawProps.DeParse(pItem->m_pEditData->m_strProps);
		}

		// Создание набора свойств для обновления
		{
			_PropsUpdate.Assign(_BaseFixedProps, IPV_F_APPEND);
			_PropsUpdate.Assign(_Props, IPV_F_APPEND);

			// Если нет текста, то принудительно ставим пустой текст, чтобы при применении свойств текст не читался из скина
			if( !_PropsUpdate.Get(STR_PID_TEXT).IsValid() )
				_PropsUpdate.Get(STR_PID_TEXT, true).Get(0, true) = "\"\"";
		}
	}
	
	// Применение новых свойств
	{
		if( pTop == pItem )
			_PropsUpdate.Del(STR_PID_OFFSET);
		// !!!!!! pItem->ApplyStyle(NULL);
		pItem->_ApplyAttrs(_PropsUpdate);
		pItem->InitProps(_PropsUpdate, &CSinkCreateCtx(pTop));

		CItemPropVals& l_prpSize = _PropsUpdate.Get(STR_PID_SIZE);
		if( l_prpSize.IsValid() )
		{
			POINT ptRs =
			{
				(pItem->m_nFlags & (AUTOSIZE_X|AUTOSIZE_PARENT_X)) ? 0 : (pItem->m_szSizeMin.cx - pItem->m_szSize.cx),
				(pItem->m_nFlags & (AUTOSIZE_Y|AUTOSIZE_PARENT_Y)) ? 0 : (pItem->m_szSizeMin.cy - pItem->m_szSize.cy)
			};
			
			pItem->Resize(ptRs.x, ptRs.y);
		}
		
		pItem->RecalcLayout();
		if( (pItem->m_nFlags & AUTOSIZE) && pItem->m_strSection.size() )
			pItem->RecalcParentSize();
	}

	// Обновление редактируемых свойств
	{
		pItem->m_pEditData->m_Props.Get(STR_PID_ALIAS, true).Assign(_PropsUpdate.Get(STR_PID_ALIAS));
		pItem->m_pEditData->m_Props.Get(STR_PID_FONT, true).Assign(_PropsUpdate.Get(STR_PID_FONT));
		pItem->m_pEditData->m_Props.Get(STR_PID_ICON, true).Assign(_PropsUpdate.Get(STR_PID_ICON));
		pItem->m_pEditData->m_Props.Get(STR_PID_BACKGND, true).Assign(_PropsUpdate.Get(STR_PID_BACKGND));
		pItem->m_pEditData->m_Props.Get(STR_PID_BORDER, true).Assign(_PropsUpdate.Get(STR_PID_BORDER));
		pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS, true).Assign(_PropsUpdate.Get(STR_PID_MARGINS));
		if( pTop != pItem )
			pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true).Assign(_PropsUpdate.Get(STR_PID_OFFSET));
		
		Edit_UpdateItemProps(pTop, pItem, guiepmUpdateEditable);
		pItem->UpdateData(true);
	}
}

CItemBase * CRootItem::Edit_CreateExtContainer(HWND hParent)
{
	CItemBase * pExtCont = NULL;
	CItemBase * pTab = CreateItem("", GUI_ITEMT_TAB);
	if( pTab )
	{
		pExtCont = pTab->CreateChild("", "");
		pTab->Destroy();
	}

	if( !pExtCont )
		return NULL;
	
	AddItem(pExtCont);
	
	pExtCont->m_szSize.cx = pExtCont->m_szSize.cy = 50;
	
	{
		class _Sink : public CItemSink
		{
		public:
			HWND OnGetParent() { return m_hParent; }
			HWND m_hParent;
		} _sink;
		
		_sink.m_hParent = hParent;
		pExtCont->AttachSink(&_sink, false);
		pExtCont->InitItem("");

		pExtCont->AttachSink(NULL, false);
	}
	
	pExtCont->m_nState |= ISTATE_PREINITED;
	
	pExtCont->PreInit();
	pExtCont->PostInit(pExtCont);
	return pExtCont;
}

//////////////////////////////////////////////////////////////////////

static bool _IsQuotedPropSimple(tString& strProp)
{
	tDWORD i = 1, n = strProp.size();
	if( n > 2 && strProp[0] == '\"' && strProp[n - 1] == '\"' )
	{
		n--;
		for(; i < n; i++)
			if( strProp[i] == '\"' && !(i && strProp[i - 1] == '\\') )
				break;

		return i == n;
	}
	
	return !n;
}

void CItemEditData::Init(CItemBase * pItem, CItemProps& _Props, const tString& strItemOrigSection)
{
	if( pItem->m_strItemId == STR_SELF )
		return;

	/*
	if( strId == STR_SELF )
	{
		p_pItem->m_nFlags |= STYLE_EXTPRM;
		
		CItemBase * pParent = this;
		for(; pParent; pParent = pParent->m_pParent)
			if( pParent->m_strItemId != STR_SELF )
				break;

		pParent->m_pParent->AddItem(p_pItem);
		//strItemOrigSection = ;
		p_pItem->m_strSection = pParent->m_strSection;
		p_pItem->m_strItemId = pParent->m_strItemId;
	}
	*/

	CRootItem * pRoot = pItem->m_pRoot;

	CItemBase * pTop = pItem;
	{
		for(CItemBase * p = pItem; p->m_pParent; p = p->m_pParent)
			if( (p->m_nFlags & STYLE_EXT_ITEM) && p->m_pEditData )
				pTop = p;
	}
	
	if( !pTop )
		return;

	CItemBase * pExtOwner = pItem->GetByFlagsAndState(STYLE_EXT, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
	if( !pExtOwner )
		pExtOwner = pTop;
	
	CItemBase * pExtOwnerOneUpLevel = (pExtOwner && pExtOwner->m_pParent) ? pExtOwner->m_pParent->GetByFlagsAndState(STYLE_EXT, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent) : NULL;
	if( pExtOwnerOneUpLevel == pTop )
		pExtOwnerOneUpLevel = NULL;

	CItemBase * pExtPrmOwner = pItem->GetByFlagsAndState(STYLE_EXTPRM, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
	if( pExtPrmOwner && !pExtPrmOwner->IsParent(pExtOwner) && pExtPrmOwner != pExtOwner )
		pExtPrmOwner = NULL;

	bool bEdit = false;
	bool bExt = false;
	bool bExtOverridable = false;
	
	// Все внутри верхнего контейнера
	if( pExtOwner == pTop )
		bEdit = true;
	else
	{
		// Если это не первично вложенный контейнер, только если он является еще и параметризуемым узлом
		if( pExtOwnerOneUpLevel )
		{
			if( pExtOwner->m_nFlags & STYLE_EXTPRM )
				if( pItem->m_nFlags & (STYLE_EXT|STYLE_EXTPRM) )
					bEdit = true;
		}
		// Если это первично вложенный контейнер, то редактируем только его и параметры
		else if( pItem->m_nFlags & (STYLE_EXT|STYLE_EXTPRM) )
			bEdit = true;

		bExt = !!(pItem->m_nFlags & STYLE_EXT);
		bExtOverridable = !!(pItem->m_nFlags & STYLE_EXTPRM);
	}

	if( !bEdit )
		return;

	pItem->m_pEditData = new CItemEditData();
	pItem->m_pEditData->m_pItem = pItem;
	pItem->m_pEditData->m_strSection = pItem->m_strSection;
	pItem->m_pEditData->m_strItemId = pItem->m_strItemId;
	pItem->m_pEditData->m_bLinked = bExt;
	pItem->m_pEditData->m_bOverridable = bExtOverridable;
	pItem->m_pEditData->m_bTextSimple = 0;

	if( bExtOverridable )
		pItem->m_pEditData->m_strOverridableOrigSection = strItemOrigSection;
	
	pItem->m_pEditData->m_pTop = pTop;
	
	// Определение первого вложенного внешнего элемента
	{
		CItemBase * pExtPrev = NULL;
		for(CItemBase * p = pItem; p; p = p->m_pParent)
		{
			if( (p->m_nFlags & STYLE_EXT_ITEM) && p != pTop )
				pExtPrev = p;
		}

		pItem->m_pEditData->m_pOverridableOwner = pExtPrev;
	}

	// Определение идентификатора для переопределения вложенного элемента
	if( pItem->m_nFlags & STYLE_EXTPRM )
	{
		pItem->m_pEditData->m_strOverrideId = pItem->m_strItemId;

		for(CItemBase * p = pItem->m_pParent; p && p != pItem->m_pEditData->m_pOverridableOwner; p = p->m_pParent)
		{
			if( p->m_nFlags & STYLE_EXT_ITEM )
			{
				pItem->m_pEditData->m_strOverrideId.insert(0, GUI_STR_AUTOSECT_SEP_STR);
				pItem->m_pEditData->m_strOverrideId.insert(0, p->m_strItemId);
			}
		}
	}

	pItem->m_pEditData->m_pAttrOwner = pTop;
	if( bExt || bExtOverridable )
		pItem->m_pEditData->m_pAttrOwner = pExtOwner;
	
	pItem->m_pEditData->m_pAttrOwnerParent = pTop;
	if( bExt )
		pItem->m_pEditData->m_pAttrOwnerParent = pExtOwnerOneUpLevel ? pExtOwnerOneUpLevel : pTop;
	else if( bExtOverridable )
		pItem->m_pEditData->m_pAttrOwnerParent = pExtOwner;
	
	pItem->m_pEditData->m_bExtProp = !!(pItem->m_nFlags & STYLE_EXT);
	
	LPCWSTR strFile = !pItem->m_strSection.empty() ? pRoot->GetSectionGroup(PROFILE_LAYOUT, pItem->m_strSection.c_str()) : NULL;
	if( strFile )
		pItem->m_pEditData->m_strGroup = strFile;
	else if( pItem->m_pParent && pItem->m_pParent->m_pEditData )
		pItem->m_pEditData->m_strGroup = pItem->m_pParent->m_pEditData->m_strGroup;
	pItem->m_pEditData->m_strOldGroup = pItem->m_pEditData->m_strGroup;

	pItem->m_pEditData->GetOverridedRawInfo(pItem->m_pEditData->m_strProps, pItem->m_pEditData->m_strText);

	pItem->m_pEditData->m_Props.Get(STR_PID_ALIAS, true).Assign(_Props.Get(STR_PID_ALIAS));
	
	{
		CItemPropVals& prp = _Props.Get(STR_PID_FONT);
		if( _IsQuotedPropSimple(prp.Get()) )
			pItem->m_pEditData->m_Props.Get(STR_PID_FONT, true).Assign(prp);
	}
	
	{
		CItemPropVals& prp = _Props.Get(STR_PID_ICON);
		if( _IsQuotedPropSimple(prp.Get()) )
			pItem->m_pEditData->m_Props.Get(STR_PID_ICON, true).Assign(prp);
	}
	{
		CItemPropVals& prp = _Props.Get(STR_PID_BACKGND);
		if( _IsQuotedPropSimple(prp.Get()) )
			pItem->m_pEditData->m_Props.Get(STR_PID_BACKGND, true).Assign(prp);
	}
	
	pItem->m_pEditData->m_Props.Get(STR_PID_BORDER, true).Assign(_Props.Get(STR_PID_BORDER));
	pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS, true).Assign(_Props.Get(STR_PID_MARGINS));
	pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true).Assign(_Props.Get(STR_PID_OFFSET));

	// Определение простого текста
	pItem->m_pEditData->m_bTextSimple = _IsQuotedPropSimple(pItem->m_pEditData->m_strText);
}

//////////////////////////////////////////////////////////////////////

void CItemEditData::Destroy() { delete this; }

CItemProps&	CItemEditData::GetOverrideOriginalInfo(CItemProps& _Props, tString& strText)
{
	if( !m_pItem->m_pEditData->IsOverridable() )
		return _Props;

	if( m_pItem->m_pEditData->m_strOverridableOrigSection.size() )
		_Props.Parse(m_pItem->m_pRoot->GetString(TOR(tString,()), PROFILE_LAYOUT, m_pItem->m_pEditData->m_strOverridableOrigSection.c_str(), STR_SELF, "", m_pItem), TOR_tString, NULL, IPV_F_APPEND);
	else if( m_pItem->m_pParent )
		_Props.Parse(m_pItem->m_pRoot->GetString(TOR_tString, PROFILE_LAYOUT, m_pItem->m_pParent->m_strSection.c_str(), m_pItem->m_strItemId.c_str(), "", m_pItem->m_pParent), TOR_tString, NULL, IPV_F_APPEND);

	return _Props;
}

CItemProps& CItemEditData::GetBaseInfo(CItemProps& _Props)
{
	if( !m_pItem->m_pEditData )
		return _Props;

	// Свойства, которые есть по умолчанию у любого элемента
	_Props.Parse("a(lt)", TOR_tString, NULL, IPV_F_APPEND);

	if( m_pItem->m_pEditData->IsOverridable() )
	{
		if( m_pItem->m_pEditData->m_strOverridableOrigSection.size() )
		{
			_Props.Parse(m_pItem->m_pRoot->GetString(TOR(tString,()), PROFILE_LAYOUT, m_pItem->m_pEditData->m_strOverridableOrigSection.c_str(), STR_SELF, "", m_pItem), TOR_tString, NULL, IPV_F_APPEND);
			if( m_pItem->m_pEditData->IsLinked() && !_Props.Get(STR_PID_TEXT).IsValid() )
				m_pItem->m_pRoot->GetString(_Props.Get(STR_PID_TEXT, true).Get(0, true), PROFILE_LOCALIZE, m_pItem->m_pEditData->m_strOverridableOrigSection.c_str(), STR_SELF, "", m_pItem);
		}
		else if( m_pItem->m_pParent )
			_Props.Parse(m_pItem->m_pRoot->GetString(TOR_tString, PROFILE_LAYOUT, m_pItem->m_pParent->m_strSection.c_str(), m_pItem->m_strItemId.c_str(), "", m_pItem->m_pParent), TOR_tString, NULL, IPV_F_APPEND);
	}
	
	if( m_pItem->m_pEditData->IsLinked() )
	{
		_Props.Parse(m_pItem->m_pRoot->GetString(TOR(tString,()), PROFILE_LAYOUT, m_pItem->m_strSection.c_str(), STR_SELF, "", m_pItem), TOR_tString, NULL, IPV_F_APPEND);
		tString strText; m_pItem->m_pRoot->GetString(strText, PROFILE_LOCALIZE, m_pItem->m_strSection.c_str(), STR_SELF, "", m_pItem);

		if( !_Props.Get(STR_PID_TEXT).IsValid() && strText.size() )
			_Props.Get(STR_PID_TEXT, true).Get(0, true) = strText;
	}
	else if( m_pItem->m_pEditData->IsOverridable() )
	{
		tString strLocId; m_pItem->GetOwnerRelativePath(strLocId, m_pItem->m_pEditData->m_pAttrOwnerParent, false);
		tString strText; m_pItem->m_pRoot->GetString(strText, PROFILE_LOCALIZE, m_pItem->m_pEditData->m_pAttrOwnerParent->m_strSection.c_str(), strLocId.c_str(), "", m_pItem->m_pEditData->m_pAttrOwnerParent);

		if( !_Props.Get(STR_PID_TEXT).IsValid() && strText.size() )
			_Props.Get(STR_PID_TEXT, true).Get(0, true) = strText;
	}

	if( m_pItem->m_pEditData->IsOverridable() )
	{
		if( CItemBase::Override * pOver = m_pItem->m_pEditData->m_pOverridableOwner->_GetOverrideItem(m_pItem->m_pEditData->m_strOverrideId) )
		{
			tString str = pOver->m_strId; pOver->m_strId = "";
			m_pItem->_GetOverride(m_pItem->m_pEditData->m_pAttrOwnerParent, TOR(tString,(m_pItem->m_strItemId)), _Props, TOR_tString);
			pOver->m_strId = str;
		}
	}

	return _Props;
}

void CItemEditData::GetOverridedRawInfo(tString& strProps, tString& strText)
{
	if( m_pItem->m_pEditData->IsOverridable() )
	{
		CItemBase::Override * pOver = m_pItem->m_pEditData->m_pOverridableOwner->_GetOverrideItem(m_pItem->m_pEditData->m_strOverrideId);
		if( pOver )
		{
			strProps = pOver->m_strProps;
			CItemBase::_ExtractSection(strProps, TOR_tString);

			strText = pOver->m_strText;
		}
	}
	else if( m_pItem->m_pEditData->IsLinked() )
	{
		if( m_pItem->m_pParent )
		{
			m_pItem->m_pRoot->GetString(strProps, PROFILE_LAYOUT, m_pItem->m_pParent->m_strSection.c_str(), m_pItem->m_strItemId.c_str(), "", GETSTRING_RAW);
			CItemBase::_ExtractSection(strProps, TOR_tString);

			m_pItem->m_pRoot->GetString(strText, PROFILE_LOCALIZE, m_pItem->m_pParent->m_strSection.c_str(), m_pItem->m_strItemId.c_str(), "", GETSTRING_RAW);
		}
	}
	else
	{
		if( m_pItem->m_strSection.size() )
			m_pItem->m_pRoot->GetString(strProps, PROFILE_LAYOUT, m_pItem->m_strSection.c_str(), STR_SELF, "", GETSTRING_RAW);
		else if( m_pItem->m_pParent )
			m_pItem->m_pRoot->GetString(strProps, PROFILE_LAYOUT, m_pItem->m_pParent->m_strSection.c_str(), m_pItem->m_strItemId.c_str(), "", GETSTRING_RAW);

		if( m_pItem->m_nFlags & STYLE_EXT_ITEM )
			m_pItem->m_pRoot->GetString(strText, PROFILE_LOCALIZE, m_pItem->m_strSection.c_str(), STR_SELF, "", GETSTRING_RAW);
		else if( m_pItem->m_pParent )
			m_pItem->m_pRoot->GetString(strText, PROFILE_LOCALIZE, m_pItem->m_pParent->m_strSection.c_str(), m_pItem->m_strItemId.c_str(), "", GETSTRING_RAW);
	}

	{
		LPCSTR strSubstPrp[] = {STR_PID_SUBST, NULL};
		tString strNoSubst; CItemProps(strProps.c_str(), strNoSubst, strSubstPrp);
		strProps = strNoSubst;
	}
	
	_Str_TruncateSpaces(strProps);
}

CItemBase * CRootItem::Edit_CreateItem(CItemBase * pTop, CItemBase * pWhere, LPCSTR strId, LPCSTR strLoadInfo, LPCSTR strText, tDWORD nFlags)
{
	if( !pWhere )
		pWhere = this;

	if( !strLoadInfo )
		strLoadInfo = "";

	CItemBase * pItem = NULL;
	if( pWhere && pWhere->m_pEditData && pWhere->m_pEditData->IsOverridable() )
	{
		if( pWhere == pTop )
			return NULL;
		
		pItem = pWhere;
		pWhere = pItem->m_pParent;
		if( !pWhere || pItem->m_strItemId.empty() )
			return NULL;

		// Подставляем для соответствующего элемента новую строку свойств для переопределения
		if( CItemBase::Override * pOver = pItem->m_pEditData->m_pOverridableOwner->_GetOverrideItem(pItem->m_pEditData->m_strOverrideId, true) )
		{
			pOver->clear();
			pOver->m_strProps = strLoadInfo;
			
			tString strProps; pItem->m_pEditData->GetOverrideOriginalInfo(TOR(CItemProps,()), TOR_tString).DeParse(strProps);
			if( pItem->m_pEditData->m_strOverridableOrigSection.size() )
			{
				strProps += " [";
				strProps += pItem->m_pEditData->m_strOverridableOrigSection;
				strProps += "]";
			}
			pWhere->LoadItem(NULL, pItem, strProps.c_str(), NULL, LOAD_FLAG_INFO|LOAD_FLAG_REPLACE|LOAD_FLAG_EDIT|nFlags);
		}
	}
	else
		pWhere->LoadItem(NULL, pItem, strLoadInfo, strId, LOAD_FLAG_INFO|LOAD_FLAG_EDIT|nFlags);

	if( !pItem )
		return NULL;

	if( !pItem->m_pEditData )
	{
		pItem->Destroy();
		return NULL;
	}
	
	pItem->m_pEditData->m_strProps = strLoadInfo;
	CItemBase::_ExtractSection(pItem->m_pEditData->m_strProps, TOR_tString);
	
	if( strText )
		pItem->m_pEditData->m_strText = strText;

	if( pItem->m_strSection.size() )
		pItem->m_pEditData->m_strSection = pItem->m_strSection;
	
	if( !(nFlags & LOAD_FLAG_PRELOAD) )
	{
		Edit_UpdateItemProps(pTop, pItem, guiepmUpdateEditable);
		Edit_UpdateItemProps(pTop, pItem);
	}
	
	return pItem;
}

CItemBase * CItemEditData::DeleteItem(CItemBase * pItem)
{
	if( !pItem || !pItem->m_pEditData || pItem == m_pItem )
		return NULL;
	
	CItemBase * pParent = pItem->m_pParent;
	
	_Edit_StoreDeletedSections(m_pItem, pItem);

	if( pItem->m_pEditData->IsOverridable() )
	{
		// Стираем для соответствующего элемента строку свойств для переопределения
		if( CItemBase::Override * pOver = pItem->m_pEditData->m_pOverridableOwner->_GetOverrideItem(pItem->m_pEditData->m_strOverrideId, true) )
		{
			pOver->clear();

			tString strProps; pItem->m_pEditData->GetOverrideOriginalInfo(TOR(CItemProps,()), TOR_tString).DeParse(strProps);
			if( pItem->m_pEditData->m_strOverridableOrigSection.size() )
			{
				strProps += " [";
				strProps += pItem->m_pEditData->m_strOverridableOrigSection;
				strProps += "]";
			}
			pParent->LoadItem(NULL, pItem, strProps.c_str(), NULL, LOAD_FLAG_INFO|LOAD_FLAG_REPLACE|LOAD_FLAG_EDIT);

			m_pItem->m_pRoot->Edit_UpdateItemProps(m_pItem, pItem);
		}
	}
	else
	{
		pItem->Show(false);
		pItem->Destroy();
		pItem = pParent;
	}

	if( pParent && pParent->m_nFlags & STYLE_SHEET_ITEM )
		pParent->OnUpdateProps(UPDATE_FLAG_TEXT);

	return pItem;
}

void CItemEditData::CopyData(CItemBase* pItem, Cd& _data)
{
	if( !pItem || !pItem->m_pEditData )
		return;
	
	_data.m_strId = pItem->m_pEditData->m_strItemId;
	_data.m_strText = pItem->m_pEditData->m_strText;
	_data.m_strProps.erase();
	_data.m_aItems.clear();

	if( pItem->m_pEditData->IsLinked() )
	{
		_data.m_strProps += "[";
		_data.m_strProps += pItem->m_strSection;
		_data.m_strProps += "]";
	}
	else if( pItem->m_strSection.size() )
		_data.m_strProps += "[_]";
	
	if( _data.m_strProps.size() )
		_data.m_strProps += " ";
	_data.m_strProps += pItem->m_pEditData->m_strProps;

	if( pItem->m_pEditData->IsLinked() )
	{
		m_pItem->m_pRoot->_Save_ExtOverrides(pItem, _data.m_strProps, NULL);
		return;
	}

	if( pItem->m_nFlags & STYLE_EXT )
	{
		tString strProps; LPCSTR strIncl[] = {STR_PID_EXT, NULL};
		CItemProps _prps(_data.m_strProps.c_str(), strProps, strIncl);
		_data.m_strProps = strProps;
	}

	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
		CopyData(pItem->m_aItems[i], _data.m_aItems.push_back());
}

CItemBase * CRootItem::Edit_CreateFromData(CItemBase * pTop, CItemBase * pWhere, const CItemEditData::Cd& _data, LPCSTR strId)
{
	tString strProps;
	{
		// Оставляем переопределения с макросами
		LPCSTR strOvrd[] = {STR_PID_SUBST, NULL}; CItemProps _SubstProps(_data.m_strProps.c_str(), strProps, strOvrd);
		_GetString(strProps, PROFILE_LAYOUT, pTop ? pTop->m_strSection.c_str() : "", "", pTop);
		_SubstProps.DeParse(strProps);
	}
	
	CItemBase * pItem = Edit_CreateItem(pTop, pWhere, strId, strProps.c_str(), _data.m_strText.c_str(), LOAD_FLAG_PRELOAD);
	if( !pItem )
		return NULL;
	
	_Edit_CreateFromData(pTop, pItem, _data);

	pItem->PostInit(pItem);
	pItem->PostShow();
	
	_Edit_PostCreateFromData(pItem, pItem);
	return pItem;
}

//////////////////////////////////////////////////////////////////////

void CRootItem::_Edit_CreateFromData(CItemBase * pTop, CItemBase * pParent, const CItemEditData::Cd& _data)
{
	if( !pParent )
		return;
	
	pParent->m_pEditData->m_strText = _data.m_strText;
	if( !pParent->m_pEditData->IsOverridable() )
	{
		pParent->m_pEditData->m_strProps.erase();
		CItemProps _Props(_data.m_strProps.c_str(), pParent->m_pEditData->m_strProps);
		CItemBase::_ExtractSection(pParent->m_pEditData->m_strProps, TOR_tString);
		_Props.Del(STR_PID_SUBST);
		_Str_TruncateSpaces(pParent->m_pEditData->m_strProps);
		
		_Props.DeParse(pParent->m_pEditData->m_strProps);
	}

	for(tDWORD i = 0; i < _data.m_aItems.size(); i++)
	{
		const CItemEditData::Cd& _cd = _data.m_aItems[i];
		
		CItemBase * pItem = NULL;
		tString strProps = _cd.m_strProps; _GetString(strProps, PROFILE_LAYOUT, pTop ? pTop->m_strSection.c_str() : "", "", pTop);
		pParent->LoadItem(NULL, pItem, strProps.c_str(), _cd.m_strId.c_str(), LOAD_FLAG_INFO|LOAD_FLAG_EDIT|LOAD_FLAG_PRELOAD);

		_Edit_CreateFromData(pTop, pItem, _cd);
	}
}

void CRootItem::_Edit_PostCreateFromData(CItemBase * pTop, CItemBase * pParent)
{
	Edit_UpdateItemProps(pTop, pParent, guiepmUpdateEditable);
	Edit_UpdateItemProps(pTop, pParent);
	
	for(tDWORD i = 0; i < pParent->m_aItems.size(); i++)
		_Edit_PostCreateFromData(pTop, pParent->m_aItems[i]);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
