// ##########################################################################################
// ##########################################################################################

#if defined (__unix__)
#define REG_NONE (-1)
#define REG_DWORD 1
#define REG_DWORD_BIG_ENDIAN 2
#define REG_QWORD 3
#define REG_SZ 4
#define REG_EXPAND_SZ 5 
#define REG_MULTI_SZ 6
#endif // __unix__

static void Reg_Bin2Str(CRootItem * pRoot, tDWORD nRegType, tBYTE * pData, tDWORD nSize, tString& strRes);

// ##########################################################################################

bool Product_IsSerializableHolder(CRootItem * pRoot, const cSerDescriptor * pDesc, const cSerDescriptorField * pFld, LPCSTR strType, CustomHolderData ** ppHolderData)
{
	sswitch(strType)
	stcase(notification)
		return true;
		sbreak;

	stcase(reg_data)
		if( (pFld->m_flags & SDT_FLAG_VECTOR) && pFld->m_id == tid_BYTE )
			return true;
		sbreak;
	send;

	return false;
}

bool Product_ResolveSerializable(CRootItem * pRoot, bool bToStruct, const cSerDescriptorField * pFld, LPCSTR strType, CustomHolderData * pHolderData, cSerializable * pStruct, tString& strResult, CBindProps * pProps)
{
	sswitch(strType)
	stcase(reg_data)
		if( bToStruct )
			return false;
		
		cVector<tBYTE> * pValData = NULL;
		tDWORD           nValueType = REG_NONE;

// 		if( pStruct->isBasedOn(cAskObjectAction_PDM_BASE::eIID) )
// 		{
// 			cAskObjectAction_PDM_BASE * pPdmBase = (cAskObjectAction_PDM_BASE *)pStruct;
// 			if( pFld == pPdmBase->getField(&pPdmBase->m_Value) )
// 			{
// 				pValData = &pPdmBase->m_Value;
// 				nValueType = pPdmBase->m_ValueType;
// 			}
// 			else if( pFld == pPdmBase->getField(&pPdmBase->m_ValueOld) )
// 			{
// 				pValData = &pPdmBase->m_ValueOld;
// 				nValueType = pPdmBase->m_ValueTypeOld;
// 			}
// 		}
// 		if( pStruct->isBasedOn(CAskRegVerdict::eIID) )
// 		{
// 			CAskRegVerdict * pPdmBase = (CAskRegVerdict *)pStruct;
// 			if( pFld == pPdmBase->getField(&pPdmBase->m_Value) )
// 			{
// 				pValData = &pPdmBase->m_Value;
// 				nValueType = pPdmBase->m_ValueType;
// 			}
// 			else if( pFld == pPdmBase->getField(&pPdmBase->m_ValueOld) )
// 			{
// 				pValData = &pPdmBase->m_ValueOld;
// 				nValueType = pPdmBase->m_ValueTypeOld;
// 			}
// 		}

		if( !pValData )
			return false;

		Reg_Bin2Str(pRoot, nValueType, &pValData->at(0), pValData->size(), strResult);
		sbreak;
	send;

	return false;
}

// ##########################################################################################

void Reg_Bin2Str(CRootItem * pRoot, tDWORD nRegType, tBYTE * pData, tDWORD nSize, tString& strRes)
{
	if( !nSize )
	{
		strRes = "";
		return;
	}

	cStrObj res;

	bool bBin = true;
	switch( nRegType )
	{
	case REG_DWORD:
	case REG_DWORD_BIG_ENDIAN:
		if( nSize == sizeof(DWORD) )
		{
			res.format(cCP_UNICODE, L"0x%08x (%d)", *(DWORD *)pData, *(DWORD *)pData);
			bBin = false;
		}
		break;

	case REG_QWORD:
		if( nSize == sizeof(__int64) )
		{
			res.format(cCP_UNICODE, L"0x%016I64x (%I64d)", *(__int64 *)pData, *(__int64 *)pData);
			bBin = false;
		}
		break;

	case REG_SZ:
	case REG_EXPAND_SZ:
		if( !(nSize%2) )
		{
			LPCWSTR pChars = (LPCWSTR)pData;
			int     nChars = nSize/2;

			if( !pChars[nChars - 1] &&
				(wcslen(pChars) == (nChars - 1)) )
			{
				res = pChars;
				bBin = false;
			}
		}
		break;

	case REG_MULTI_SZ:
		if( !(nSize%2) )
		{
			LPCWSTR pChars = (LPCWSTR)pData;
			int     nChars = nSize/2;

			if( !pChars[nChars - 1] )
			{
				tWCHAR szChar[] = {L" "};
				for(int i = 0; i < nChars; i++)
				{
					szChar[0] = pChars[i];
					if( (i + 2 < nChars) && !szChar[0] )
						szChar[0] = L'\n';

					res += szChar;
				}

				bBin = false;
			}

		}
		break;
	}

	if( bBin )
	{
		for( tDWORD i = 0; i < nSize; i++ )
		{
			tWCHAR buf[4];
			int wPos = 0;
			if( i )
				buf[wPos++] = L' ';

			_snwprintf(&buf[wPos], countof(buf) - wPos, L"%02x", (unsigned char)pData[i]);
			res.append(buf);
		}
	}

	pRoot->LocalizeStr(strRes, res.data(), res.size());
}

// ##########################################################################################
// ##########################################################################################
