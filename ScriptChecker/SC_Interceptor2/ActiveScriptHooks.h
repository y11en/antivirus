#ifndef __ACTIVESCRIPTHOOKS_H
#define __ACTIVESCRIPTHOOKS_H

void HookNewObjectMethods(void* _this, void* pNewObject, REFIID riid, bool bHookQueryInterface = true);

STDMETHODIMP Hook_PStoreCreateInstance(
  void** ppProvider, // IPStore** 
  void* pProviderID, // PST_PROVIDERID*
  void* pReserved,
  DWORD dwFlags
  );

#endif // __ACTIVESCRIPTHOOKS_H