#if !defined(__tm_util_h)
#define __tm_util_h

// ------------------------------------------------

#ifdef __ASM_REF
	#define RefAddRef(ref)    { __asm mov ecx,ref __asm mov eax,1 __asm lock xadd dword ptr[ecx],eax }
	#define RefRelease(ref)   { __asm mov ecx,ref __asm mov eax,0xFFFFFFFF __asm lock xadd dword ptr[ecx],eax }
#else
	#define RefAddRef(ref)    { PrInterlockedIncrement(&ref); }
	#define RefRelease(ref)   { PrInterlockedDecrement(&ref); }
#endif

struct cRefLocker
{
	cRefLocker() : m_refRead(0), m_refWrite(0){}

	void lock(bool bRead=true)
	{
		tLONG& ref = bRead ? m_refRead : m_refWrite;
		RefAddRef(ref);
		while( bRead ? (m_refWrite) : (m_refWrite > 1 || m_refRead) )
		{
			RefRelease(ref);
			PrSleep(50);
			RefAddRef(ref);
		}
	}

	void unlock(bool bRead=true)
	{
		tLONG& ref = bRead ? m_refRead : m_refWrite;
		RefRelease(ref);
	}

	tLONG  m_refRead;
	tLONG  m_refWrite;
};

struct cEnvStrObj : public cStringObj
{
	cEnvStrObj(const tCHAR* env) : cStringObj(env)
	{
		g_root->sysSendMsg( pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hSTRING)cAutoString(*this), NULL, 0);
	}
};

#define IS_TIME_EMPTY(_tm)  ((_tm)== -1 || (_tm)== 0)

// ------------------------------------------------

#endif // __tm_util_h
