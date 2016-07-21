// ------------------------------------------------
#include <prague.h>
#include "tm.h"
// ------------------------------------------------

#define CS_COPY_MANDATORIED     1
#define CS_COPY_UNMANDATORIED   2
#define CS_MANDATORIED_IN_DST   4

// ------------------------------------------------

tERROR CopyField(tPTR pDstData, tPTR pSrcData, const cSerDescriptorField *pField)
{
	if( pField->m_flags & SDT_FLAG_VECTOR )
	{
		cVectorHnd aDst, aSrc;
		aDst.attach((tMemChunk *)pDstData, (tTYPE_ID)pField->m_id, pField->m_flags);
		aSrc.attach((tMemChunk *)pSrcData, (tTYPE_ID)pField->m_id, pField->m_flags);
		aDst.assign(aSrc);
		return errOK;
	}

	if( IS_SERIALIZABLE(pField->m_id) )
		return errOBJECT_INCOMPATIBLE;

	switch(pField->m_id)
	{
//	case tid_STRING:
//	case tid_WSTRING:
	case tid_STRING_OBJ:
		*(cStringObj *)pDstData = *(cStringObj *)pSrcData;
		break;
	
	default:
		memcpy(pDstData, pSrcData, pr_sizeometer(pField->m_id, NULL));
	}
	
	return errOK;
}

tERROR ApplyPolicyStruct(cSerializable *pDst, cSerializable *pSrc, tDWORD nUnique = cSerializable::eIID, tDWORD nFlags = CS_COPY_MANDATORIED)
{
	if( !pDst || !pSrc )
		return errPARAMETER_INVALID;

	if( nUnique == cSerializable::eIID )
	{
		if( pDst->getIID() != pSrc->getIID() )
			return errPARAMETER_INVALID;

		nUnique = pSrc->getIID();
	}

	tERROR err = errOK;
	cSerDescriptor *pDesc;
	if( PR_FAIL(err = g_root->FindSerializableDescriptor(&pDesc, nUnique)) )
		return err;

	cPolicySettings *pStr = (cPolicySettings *)(nFlags & CS_MANDATORIED_IN_DST ? pDst : pSrc);
	if( !pStr->isBasedOn(cPolicySettings::eIID) )
		pStr = NULL;
	
	for(const cSerDescriptorField *pField = pDesc->m_fields; pField->m_flags != SDT_FLAG_ENDPOINT; pField = pField++)
	{
		tPTR pSrcData = SADDR(pSrc, pField);
		tPTR pDstData = SADDR(pDst, pField);

		if( pField->m_flags & SDT_FLAG_BASE )
		{
			if( pField->m_flags & SDT_FLAG_VECTOR )
				continue;
			
			ApplyPolicyStruct((cSerializable *)pDstData, (cSerializable *)pSrcData, pField->m_id, nFlags);
			continue;
		}

		if( IS_SERIALIZABLE(pField->m_id) && !(pField->m_flags & SDT_FLAG_VECTOR) )
		{
			if( pField->m_flags & SDT_FLAG_POINTER )
			{
				pSrcData = *(tPTR *)pSrcData;
				pDstData = *(tPTR *)pDstData;
			}

			bool bCopied = false;
			if( pStr && pSrcData && pDstData )
			{
				tDWORD nPolicyBit = pStr->FindFieldBit(pField->m_name);
				bool bMandatoried = pStr->m_dwMandatoriedFields.IsBitSet(nPolicyBit);
				if( bMandatoried )
					((cPolicySettings *)pDst)->m_dwMandatoriedFields.SetBit(nPolicyBit, true);
				if( (bMandatoried && (nFlags & CS_COPY_MANDATORIED)) || (!bMandatoried && (nFlags & CS_COPY_UNMANDATORIED)) )
					bCopied = PR_SUCC(((cSerializable *)pDstData)->assign(*(cSerializable *)pSrcData, false));
			}
			
			if( !bCopied )
				ApplyPolicyStruct((cSerializable *)pDstData, (cSerializable *)pSrcData, pField->m_id, nFlags);
			
			continue;
		}

		if( pStr && pField->m_name && *pField->m_name )
		{
			tDWORD nPolicyBit = pStr->FindFieldBit(pField->m_name);
			bool bMandatoried = pStr->m_dwMandatoriedFields.IsBitSet(nPolicyBit);
			if( bMandatoried )
				((cPolicySettings *)pDst)->m_dwMandatoriedFields.SetBit(nPolicyBit, true);
			if( (bMandatoried && (nFlags & CS_COPY_MANDATORIED)) || (!bMandatoried && (nFlags & CS_COPY_UNMANDATORIED)) )
				CopyField(pDstData, pSrcData, pField);
		}
	}
	return errOK;
}

tERROR CheckMandatoyField(tPTR obj, const cSerDescriptorField* field, bool bMandatoried, bool bPolicy)
{
	if( !field || !obj )
		return warnDEFAULT;

	if( bMandatoried )
		if( field->m_id == cTaskRunAs::eIID ||
			field->m_id == cTaskSchedule::eIID /*||
			field->m_id == cTaskSleepingMode::eIID*/ )
			return warnFALSE;

	if( bPolicy )
		return warnDEFAULT;

	cPolicySettings* settings = NULL;
	if( field->m_id < tid_LAST_TYPE || (field->m_flags & SDT_FLAG_VECTOR) )
		settings = (cPolicySettings*)((tBYTE*)obj - field->m_offset);

	if( !settings || !settings->isBasedOn(cPolicySettings::eIID) )
		return warnDEFAULT;

	if( !strcmp(field->m_name, "MandatoriedFields") ||
		!strcmp(field->m_name, "LockedFields") )
		return warnFALSE;

	if( settings->m_dwMandatoriedFields.IsZero() )
		return bMandatoried ? warnFALSE : warnDEFAULT;

	if( settings->IsMandatoried(field->m_name) == bMandatoried )
		return warnDEFAULT;

	return warnFALSE;
}

tERROR pr_call PolicySerializeCallback(tPTR context, tSerializeAction action, tPTR obj,
									   const cSerDescriptorField* field, tVOID* data)
{
	return (action == sa_serialize_field) ? CheckMandatoyField(obj, field, true, true) : warnDEFAULT;
}

// ------------------------------------------------

tERROR TMImpl::InitPolicy()
{
	cSerializable* pData = &m_policy;
	return g_root->RegDeserialize(&pData, m_config, "policy", SERID_UNKNOWN);
}

tERROR TMImpl::ApplyPolicy(cSerializable* data, cTmProfile* profile)
{
	if( !profile || !data || profile->m_sType.empty() || profile->m_bClone )
		return errOK;

	cAutoCS locker(m_pcs, true);

	cCfgEx* policy = m_policy.find(profile->m_sType);
	if( !policy )
		return errNOT_FOUND;

	if( data->isBasedOn(cCfg::eIID) )
	{
		cCfg& cfg = *(cCfg*)data;

		if( cfg.m_nFlags & cCfg::fWithoutPolicy )
		{
			cfg.m_dwMandatoriedFields = policy->m_dwMandatoriedFields;
			if( cfg.m_settings && policy->m_settings )
			{
				cPolicySettings* settings = (cPolicySettings*)cfg.m_settings.ptr_ref();
				cPolicySettings* policy_sett = (cPolicySettings*)policy->m_settings.ptr_ref();

				settings->m_dwMandatoriedFields = policy_sett->m_dwMandatoriedFields;
			}

			profile->SaveCfg(cfg, dfCfg|dfSettings, false);
		}

		return ApplyPolicyStruct(data, policy, cCfg::eIID);
	}

	return ApplyPolicyStruct(data, policy->settings());
}

tERROR TMImpl::ApplyPolicyEx(cTmProfile* profile, cCfgEx* policy)
{
	ApplyPolicy(profile, policy);

	tDWORD i, c = policy->m_aChildren.size();
	for(i = 0; i < c; i++)
		ApplyPolicyEx(profile, &policy->m_aChildren.at(i));

	return errOK;
}

tERROR TMImpl::ApplyPolicy(cTmProfile* profile, cCfgEx* policy, bool bDelete)
{
	if( !policy )
		return errPARAMETER_INVALID;

	if( !profile )
	{
		cCfgEx* found = NULL;

		cAutoCS locker(m_pcs, true);

		tDWORD i, c = m_policy.m_aChildren.size();
		for( i = 0; i < c; i++ )
			if( m_policy.m_aChildren.at(i).m_sType == policy->m_sType )
			{
				found = &m_policy.m_aChildren.at(i);
				break;
			}

		if( !bDelete )
		{
			if( !found )
				found = &m_policy.m_aChildren.push_back();
			else if( found->isEqual(policy) )
				return errOK;

			found->assign(*policy, false);
			if( found->m_sName.empty() )
				found->m_sName = found->m_sType;

			g_root->RegSerializeEx(found, SERID_UNKNOWN, m_config,
				"policy\\subItems", PolicySerializeCallback, NULL);
		}
		else
		{
			if( !found )
				return errNOT_FOUND;

			policy->m_aChildren = found->m_aChildren;

			m_policy.m_aChildren.remove(i);

			cStrObj strKey("policy\\subItems\\");
			strKey += found->m_sName;
			m_config->DeleteKey(cRegRoot, strKey.c_str(cCP_ANSI));
		}

		cCfgEx policy_copy = m_policy;

		locker.unlock();

		tBOOL make_persistent = cFALSE;
		m_bl->CustomizeSettings(NULL, &policy_copy, SETTINGS_LEVEL_DEFAULT, csdFROM_POLICY_TO_DATA, &make_persistent);

		return ApplyPolicyEx(this, policy);
	}

	cTmProfileEnum child(profile);
	while( child.next() )
		ApplyPolicy(child, policy);

	if( profile->m_sType != policy->m_sType || profile->m_bClone )
		return errNOT_MATCHED;

	cCfg cfg;
	tERROR error = profile->LoadCfg(&cfg, true);
	if( PR_SUCC(error) )
		error = profile->SetInfoRequest(&cfg, 0, REQUEST_FLAG_APPLY_POLICY);
	return error;
}

tERROR TMImpl::GetPolicy(cCfgEx* policy)
{
	cAutoCS locker(m_pcs, true);

	cCfgEx* found = m_policy.find(policy->m_sType);
	if( !found )
		return errNOT_FOUND;

	return policy->assign(*found, false);
}

//////////////////////////////////////////////////////////////////////////
