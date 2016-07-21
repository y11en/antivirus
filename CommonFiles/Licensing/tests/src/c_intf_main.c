#include <stdio.h>
#include <malloc.h>
#include <licensing_policy_c_intf.h>

//#include <1110r.h>
//#include <appinfo1108r.h>
#include <compinfo99r.h>

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

const char LicStorgaeFile[] = "lic_storage.dat";

int    writeLicData     (const char *s, unsigned int size);
int    readLicData      (char **s, unsigned int *size);

void   regKey           (const char * fileName);
void   checkActive_Key  ();
void   getActive_KeyInfo();
void   viewKeys         ();
void   checkAndInstall  (const char *fileName);
int    loadFileImage    (const char * fileName, void **buf, unsigned int *size);

void   printMsg                 (HRESULT hr);
void   printKeyInfo             (const _key_info_t *ki);
void   printCheckInfo           (const _check_info_t *ci);
void   printKeyType             (const _key_type_t kt);
void   printFuncLevel           (const _functionality_level_t fl);
void   printInvalidReason       (const _key_invalid_reason_t kir);
void   printDate                (const _date_t *d); 

ILicenseStorage licStorage = {readLicData, writeLicData};

int main ()
{

  HRESULT hr;
  int     i;

  hr = init (&licStorage,
             &AppInfo,
             0,
             cmKeyOnly
            );

  if (FAILED (hr)) 
  {
    printf ("Initialization failed, error code %x \n", hr);
    return (0);
  }


  for (i = 0; i < 1000; i++)
  {
    revokeActiveKey ();

    checkActive_Key ();

    checkAndInstall ("sys:mynlms\\00052930.key");

    checkActive_Key ();

    getActive_KeyInfo ();
  }

  return (0);

}

//-----------------------------------------------------------------------------

void regKey (const char *fileName)
{

  void             *fileImage;
  unsigned int      fsize;
  HRESULT           hr;

  if (loadFileImage (fileName, &fileImage, &fsize) < 0)
  {
    printf ("Cannot load file %s \n", fileName);
    return;
  }

  hr = addKey (fileName, fileImage, fsize);

  free (fileImage);

  printf ("Reg key: ");
  printMsg (hr);
  printf ("\n");

}

//-----------------------------------------------------------------------------

void checkActive_Key ()
{

  HRESULT             hr;
  _check_info_t       ci;

  hr = checkActiveKey (&ci);

  printf ("checkActive_Key ");
  printMsg (hr);
  printf ("\n");

  if (SUCCEEDED (hr)) 
  {
    printCheckInfo (&ci);
  }


  freeCheckInfo (&ci);

}

//-----------------------------------------------------------------------------

void getActive_KeyInfo()
{

  HRESULT         hr;
  _key_info_t     ki;

  hr = getActiveKeyInfo (&ki);

  printf ("getActive_KeyInfo");
  printMsg (hr);
  printf ("\n");

  if (SUCCEEDED (hr)) 
  {
    printKeyInfo (&ki);
    freeKeyInfo (&ki);
  }

}

//-----------------------------------------------------------------------------

void viewKeys ()
{

}

//-----------------------------------------------------------------------------

void checkAndInstall  (const char *fileName)
{

  void           *fileImage;
  unsigned int    fsize;
  HRESULT         hr;
  _check_info_t   ci;
  
  if (loadFileImage (fileName, &fileImage, &fsize) < 0) 
  {
    printf ("Cannot load file %s", fileName);
    return;
  }

  hr = checkKeyAndInstall ("sys:mynlms\\00052930.key", fileImage, fsize, &ci);

  free (fileImage);

  printf ("CheckAndInstall : ");
  printMsg (hr);
  printf ("\n");
  printCheckInfo (&ci);

  freeCheckInfo (&ci);

}

//-----------------------------------------------------------------------------

int loadFileImage  (const char * fileName, void **buf, unsigned int *fsize)
{

  FILE           *file;
  fpos_t          size;
  
  if (!buf || !fsize) 
  {
    return (-1);
  }

  *buf = 0;
  *fsize = 0;
  
  file = fopen (fileName, "rb");

  if (!file) 
  {
    return (-1);
  }

  if (fseek (file, 0, SEEK_END))
  {
    fclose (file);
    return (-1);
  }

  if (fgetpos (file, &size))
  {
    fclose (file);
    return (-1);
  }

  *fsize = (unsigned int)size;

  *buf = malloc (*fsize);

  if (!*buf) 
  {
    *fsize = 0;
    fclose (file);
    return (-1);
  }


  fseek (file, 0, SEEK_SET);

  if (!fread (*buf, *fsize, 1, file)) 
  {
    free (*buf);
    *buf = 0;
    *fsize = 0;
    fclose (file);
    return (-1);
  }

  fclose (file);

  return (0);

}

//-----------------------------------------------------------------------------

int writeLicData (const char *s, unsigned int size)
{

  FILE         *file;
  unsigned int  r;

  if (!s || !size) 
  {
    return (0);
  }

  file = fopen (LicStorgaeFile, "wb");

  if (!file) 
  {
    return (0);
  }

  r = fwrite (s, size, 1, file);

  fclose (file);

  return ((int)r);

}

//-----------------------------------------------------------------------------

int readLicData  (char **s, unsigned int *size)
{

  FILE             *file;
  fpos_t            f_size;
  unsigned int      r;

  if (!s || !size) 
  {
    return (0);
  }


  file = fopen (LicStorgaeFile, "rb");

  if (!file) 
  {
    return (0);
  }

  fseek (file, 0, SEEK_END);

  
  fgetpos (file, &f_size);

  if (!f_size) 
  {
    fclose (file);
    return (0);
  }

  *s = (char*) malloc ((unsigned int)f_size);

  if (!*s) 
  {
    return (0);
  }

  *size = (unsigned int) f_size;

  fseek (file, 0, SEEK_SET);

  r = fread (*s, *size, 1, file);

  fclose (file);

  return ((int)(r ? *size : 0));

}

//-----------------------------------------------------------------------------

void printMsg (HRESULT hr)
{

  char *msg;

  switch (hr)
  {
    case S_OK               : msg = "S_OK"; break;
    case S_FALSE            : msg = "S_FALSE"; break;
    case E_FAIL             : msg = "E_FAIL"; break;
    case E_INVALIDARG       : msg = "E_INVALIDARG"; break;
    case E_OUTOFMEMORY      : msg = "E_OUTOFMEMORY"; break;
    case E_NOTIMPL          : msg = "E_NOTIMPL"; break;
    default                 : msg = getLicErrMsg (hr); break;
  }

  printf ("%s (%X)", msg, hr);

}

//-----------------------------------------------------------------------------

void printKeyInfo (const _key_info_t *ki)
{
  printf ("%x-%x-%x\n", ki->keySerNum);
  
  printf ("Key creation date : ");
  printDate (&ki->keyCreationDate);
  printf ("\n");

  printf ("Key limit life date : ");
  printDate (&ki->keyExpDate);
  printf ("\n");

  printf ("Key subscription period : %d\n", ki->keyLifeSpan);
  
  printf ("Key type : ");
  printKeyType (ki->keyType);
  printf ("\n");


  printf ("Key structure version : %d\n", ki->keyVer);

  printf ("Product id : %d\n", ki->productId);
  printf ("Product name : %s\n", ki->productName);
  printf ("Product major version : %s\n", ki->productVer);
  printf ("Application id : %d\n", ki->appId);
  printf ("License count : %d\n", ki->licenseCount);

  printf ("Restrictions :\n");
  printf ("            nodes number : %d\n", ki->nodesNumber);
  printf ("           emails number : %d\n", ki->emailsNumber);
  printf ("           daily traffic : %d\n", ki->dailyTraffic);
  printf ("           total traffic : %d\n", ki->totalTraffic);
  printf ("      connections number : %d\n", ki->connectionsNumber);
  printf (" checking threads number : %d\n", ki->checkThreadsNumber);

  printf ("Customer info : %s\n", ki->customer_info);
  printf ("Support info  : %s\n", ki->supportInfo);

  printf ("\n");

}

//-----------------------------------------------------------------------------

void printCheckInfo (const _check_info_t *ci)
{

  printf ("Key file name : %s\n", ci->keyFileName);

  printf ("Functionality level : ");
  printFuncLevel (ci->funcLevel);
  printf ("\n");

  if (kirValid != ci->invalidReason) 
  {
    printf ("Key invalidity reason : ");
    printInvalidReason (ci->invalidReason);
    printf ("\n");
  }

  printf ("Key expiration date : ");
  printDate (&ci->keyExpirationDate);
  printf ("\n");

  printf ("App license expiration date : ");
  printDate (&ci->appLicenseExpDate);
  printf ("\n");

  printf ("Days to app license expiration %d\n ", ci->daysTillExpiration);

  printf ("\nKey info:\n");
  printKeyInfo (ci->keyInfo);

}

//-----------------------------------------------------------------------------

void printInvalidReason (const _key_invalid_reason_t kir)
{

  char *s;

  switch (kir)
  {
    case kirValid              : s = "kirValid"; break;
    case kirExpired            : s = "kirExpired"; break;
    case kirCorrupted          : s = "kirCorrupted"; break;
    case kirNotSigned          : s = "kirNotSigned"; break;
    case kirWrongProduct       : s = "kirWrongProduct"; break;
    case kirNotFound           : s = "kirNotFound"; break;
    case kirBlackListed        : s = "kirBlackListed"; break;
    case kirTrialAlredyUsed    : s = "kirTrialAlredyUsed"; break;
    case kirIllegalBaseUpdate  : s = "kirIllegalBaseUpdate"; break;
    default                    : s = "kirUnknown"; break;
  }

  printf ("%s", s);

}

//-----------------------------------------------------------------------------

void printFuncLevel (const _functionality_level_t fl)
{

  char *s;

  switch (fl)
  {
    case flNoFeatures        : s = "flNoFeatures"; break;
    case flNoUpdates         : s = "flNoUpdates"; break;
    case flFullFunctionality : s = "flFullFunctionality"; break;
    default                  : s = "flUnknown"; break;
  }

  printf ("%s", s);

}

//-----------------------------------------------------------------------------

void printKeyType (const _key_type_t kt)
{

  char *s;

  switch (kt)
  {
    case ktBeta      : s = "ktBeta"; break;
    case ktTrial     : s = "ktTrial"; break;
    case ktTest      : s = "ktTest"; break;
    case ktOEM       : s = "ktOEM"; break;
    case ktCommercial: s = "ktCommercial"; break;
    default          : s = "ktUnknown"; break;
  }

  printf ("%s", s);

}

//-----------------------------------------------------------------------------

void printDate (const _date_t *d)
{
  printf ("%d.%d.%d", d->day, d->month, d->year);
}

//-----------------------------------------------------------------------------
