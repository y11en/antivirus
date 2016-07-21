#ifndef __KL_CXREPLACE_H__
#define __KL_CXREPLACE_H__


/*!
  \brief	Acts as SettingsStorage::Replace for leaves (values of any 
            types except PARAMS_T) and like SettingsStorage::Update
            for nodes (values of type PARAMS_T).
            
            Rules for leaves.
            If leaf exists in pData it wil be taken from pData otherwise 
            it will stay unchanged.

            Rules for nodes.            
            If node exists on pData and exists in ss it wil be taken 
            from pData.
            If node exists on pData and doesn't exist in ss 'update 
            confilct' will occur.
            Otherwise node will stay unchanged.

  \param	IN szwSsPath - path to the ss
  \param	IN szwProduct - section product name 
  \param	IN szwVersion - section product version
  \param	IN szwSection - section name
  \param	IN pData - data to process
  \param	IN bIgnoreUpdateConflicts - whether to fail in case of 'update conflict'
  \param	IN lSsAccessTimeout - ss access timeout, ms. KLSTD_INFINITE means infinite timeout.

  \exception    KLPRSS::NOT_EXIST if bIgnoreUpdateConflicts is false and 
                node doesn't exist in ss but do exists in pData
*/

KLCSKCA_DECL void KLPRSS_SsMerge_ReplaceLeavesUpdateNodes(
        const wchar_t*  szwSsPath,
        const wchar_t*  szwProduct,
        const wchar_t*  szwVersion,
        const wchar_t*  szwSection,
        KLPAR::Params*  pData,
        bool            bIgnoreUpdateConflicts,
        long            lSsAccessTimeout);


#endif //__KL_CXREPLACE_H__
