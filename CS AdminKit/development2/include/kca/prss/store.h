/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file store.h
 * \author Андрей Казачков
 * \date 2002
 * \brief INTERNAL
 *
 */

#ifndef STORE_H_5842BF9D_1897_4d6d_B64F_F58D82FFA454
#define STORE_H_5842BF9D_1897_4d6d_B64F_F58D82FFA454

#include "std/thr/sync.h"
#include "settingsstorage.h"
#include "prssconst.h"



#define KLPRSS_MAKE_PATH3(_name, _x1, _x2, _x3)	\
	const wchar_t* _name[]={_x1, _x2, _x3, NULL};

#define KLPRSS_MAKE_PATH2(_name, _x1, _x2)	\
	const wchar_t* _name[]={_x1, _x2, NULL};

#define KLPRSS_MAKE_PATH1(_name, _x1)	\
	const wchar_t* _name[]={_x1, NULL};


namespace KLPRSS
{
    typedef enum
    {
        STGLT_DEFAULT   = 0,    // former false
        STGLT_EXCLUSIVE = 1,    // former true
        STGLT_NOLOCK    = 2     // don't lock SS (in read-mode only !!!)
    }
    STG_LOCK_TYPE;

	class KLSTD_NOVTABLE Storage: public KLSTD::KLBase{
	public:
		virtual void trans_begin(bool bForChange, long nTimeout, AVP_byte nLockType = STGLT_DEFAULT)=0;
		virtual void trans_end(bool bCommit=true)=0;
		virtual bool region_load(const wchar_t** pszAbsPath, bool bForChange, bool bExceptIfNotExist=true)=0;
		virtual void region_unload()=0;
		virtual bool entry_create(const wchar_t*	szEntry, bool bExceptIfExist=true)=0;
		virtual bool entry_destroy(const wchar_t*	szEntry, bool bExceptIfNotExist=true)=0;
		virtual void entry_read(KLPAR::Params** ppParams)=0;
		virtual void entry_write(AVP_dword dwFlags, KLPAR::Params*	pParams)=0;

		virtual void entry_readall(KLPAR::Params** ppParams)=0;
		virtual void entry_writeall(KLPAR::Params*	pParams)=0;

		virtual bool attr_get(const wchar_t* szwName, KLPAR::Value** ppAttribute)=0;
		virtual void attr_put(const wchar_t* szwName, KLPAR::Value* pAttribute)=0;
		virtual void entry_enumerate(std::vector<std::wstring>& names)=0;
        
        virtual void remove(long lTimeout) = 0;
        virtual bool get_changed() = 0;
        
        virtual void entry_readall2(KLPAR::Params** ppParams, bool bSupportsSemiPacked)=0;
	};

	void KLCSKCA_DECL CreateStorage(
						const std::wstring&	wstrFileName,
						AVP_dword			dwCreation,
						AVP_dword			dwAccess,
						Storage**			ppStore,
                        const ss_format_t*  pFormat = NULL);

    enum SYNC_SSF
    {
        SYNC_SSF_COMPATIBLE = 0,
        SYNC_SSF_ALLOW_SEMIPACKED = 1,
        SYNC_SSF_FORCE_SEMIPACKED = 2
    };

	class KLSTD_NOVTABLE IStoreSync: public KLSTD::KLBase
	{
	public:
		virtual void sync_lock(long lTimeout=KLSTD_INFINITE)=0;
		virtual void sync_unlock(bool bCommit=true)=0;
		virtual void sync_enum(const wchar_t** pszAbsPath, std::vector<std::wstring>& names)=0;
		virtual void sync_read(const wchar_t** pszAbsPath, KLPAR::Params** ppParams)=0;
		virtual void sync_write(const wchar_t** pszAbsPath, KLPAR::Params* pParams)=0;
		virtual void sync_create(const wchar_t** pszAbsPath)=0;
		virtual void sync_delete(const wchar_t** pszAbsPath, bool bThrow=false)=0;
		virtual void sync_changed(const wchar_t** pszAbsPath, bool bChangedValue)=0;
        virtual void sync_new(const wchar_t** pszAbsPath, bool bNewValue)=0;
        virtual void sync_get_store(Storage** ppStore) = 0;
        virtual void sync_read2(const wchar_t** pszAbsPath, KLPAR::Params** ppParams, SYNC_SSF nFlags)=0;
	};

	void CreateStoreSync(
                        const std::wstring& wstrFileName,
                        IStoreSync**        ppSync,
                        const ss_format_t*  pFormat = NULL);
	
	void KLCSKCA_DECL getData(KLPAR::Params* pCurrent, KLPAR::Params** ppNewParams);
	void KLCSKCA_DECL getChanged(KLPAR::Params* pCurrent, KLPAR::BoolValue** ppBoolValue);

/*


	class KLSTD_NOVTABLE IStore: public KLSTD::KLBase{
	public:
		virtual void region_lock(const wchar_t** pszAbsPath, bool bForChange, long nTimeout)=0;
		virtual void region_unlock(bool bCommit=true)=0;
		virtual void entry_create(const wchar_t*	szEntry)=0;
		virtual void entry_destroy(const wchar_t*	szEntry)=0;
		virtual void entry_read(KLPAR::Params** ppParams)=0;
		virtual void entry_readi(KLPAR::Params** ppParams)=0;
		virtual void entry_write(AVP_dword dwFlags, KLPAR::Params*	pParams)=0;
		virtual void entry_enumerate(std::vector<std::wstring>& names)=0;		
	};

	void CreateStore(
				const std::wstring&	wstrFileName,
				AVP_dword			dwCreation,
				AVP_dword			dwAccess,
				IStore**			ppStore);



	void CreateStore2(
				const std::wstring&	wstrFileName,
				AVP_dword			dwCreation,
				AVP_dword			dwAccess,
				IStore**			ppStore);
*/
};

/*!
  \brief	Turn on caching

  \param	true if should use caching.
*/
void KLCSKCA_DECL KLPRSS_UseCaching(bool bUse);


/*!
  \brief	Returns caching flag

  \return	bool KLCSKCA_DECL 
*/
bool KLCSKCA_DECL KLPRSS_IfUseCaching();

#endif
