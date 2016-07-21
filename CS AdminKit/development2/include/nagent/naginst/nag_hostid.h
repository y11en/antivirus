/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	nag_hostid.h
 * \author	Andrew Kazachkov
 * \date	12.04.2004 11:11:32
 * \brief	
 * 
 */

#ifndef __KLNAG_HOSTID_H__
#define __KLNAG_HOSTID_H__

namespace KLNAG
{
    void StoreHostId(const std::wstring& wstrHostId);
    
    bool LoadHostId(std::wstring& wstrHostId);

    /*!
      \brief	Checks for duplication

      \return	true if duplication was detected
    */
    bool CheckDuplication();

    /*!
      \brief	Resets data required for fixing duplication
    */
    void ResetFixDuplicationInfo();


    /*!
      \brief	Makes decision if the host was duplicated.

      \param	vecMacsNew
      \param	vecHdIdsNew
      \param	vecMacsOld
      \param	vecHdIdsOld
      \return	true if the host was replicated
    */
    bool IfHostReplicated(
            std::vector<std::wstring>& vecMacsNew,
            std::vector<std::wstring>& vecHdIdsNew,
            std::vector<std::wstring>& vecMacsOld,
            std::vector<std::wstring>& vecHdIdsOld);


    /*!
      \brief	Returns hd ids

      \param	vecHdIds
    */
    void GetHardDriveIds(std::vector<std::wstring>& vecHdIds);
};

#endif //__KLNAG_HOSTID_H__
