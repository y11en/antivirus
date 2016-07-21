#if !defined (__APPINFO_STRUCT_H__)
#define __APPINFO_STRUCT_H__

#define _uint unsigned int

//  Component description
typedef struct
{
  _uint  componentId;                     // component id
  _uint  funcLevel;                       // component functionality level
} _component_info_t;

// Components list
typedef struct
{
  _uint                componentsNumber;   // number of list elements
  _component_info_t  *componentList;      // pointer to components info vector
} _component_list_t;

// Product description
typedef struct
{
  _uint               prodId;             // product id
  char              *prodName;           // product name
  char              *prodMajorVer;       // product major version
  _uint               publicKeyId;        // publick key id
} _product_info_t;

// Products list
typedef struct
{
  _uint                productsNumber;     // number of list elements
  _product_info_t    *productsList;       // pointer to products info vector
} _product_list_t;

// Public key GUID description
typedef struct
{
  _uint               ID;
  char              *account;            // developer account
  char              *devGUID;            // developer id
  char              *pubStr1;
  char              *pubStr2;
  char              *pubStr3;
  char              *pubStr4;
} _pub_key_info_t;

// GUIDs list
typedef struct
{
  _uint                GUID_number;        // number of list elements
  _pub_key_info_t      *GUID_list;          // pointer to GUIDs info vector
} _pub_key_info_list_t;

// Old application compatibility information
typedef struct
{
  _uint               appId;              // application id
  char              *appName;            // application name
} _app_compat_info_t;

// Old application compatibility list
typedef struct
{
  _uint                appsNumber;         // number of list elements
  _app_compat_info_t *appsList;           // Compatible applications list
} _app_compat_list_t;

typedef struct
{
  unsigned int year;
  unsigned int month;
  unsigned int day;
} _date_t;

typedef enum
{
  aitUnknown = 0,
  aitApplicationInfo,
  aitComponentInfo
} _app_info_type_t;

// Component functionality level bit mask in particular app context
typedef struct
{
  _uint     appId;                         // id of application, which can contain component
  _uint     funcLevelMask;                 // component functionality level bit mask for this app
} _app_component_info_t;

typedef struct
{
  _uint                    appsNumber;      // number of list elements
  _app_component_info_t  *list;
} _app_component_list_t;

//  Application info

typedef struct
{
  char                  *description;     // "Kaspersky lab application info"
  _uint                   appInfoVer;      // application info structure version
  _date_t                appInfoDate;     // application info creation date
  _app_info_type_t       infoType;        // application info type (app or component)
  _uint                   id   ;           // app or component id
  char                  *name;         // app or componet name
  char                  *ver;          // app or componet version
  int                    isRelease;       // 0 - beta, not 0 - release
  _component_list_t      compList;        // components list (app only)
  _app_component_list_t  appList;         // applications list (component only)
  _product_list_t        productList;     // products list
  _pub_key_info_list_t   GUID_list;       // public key GUIDs list
  _app_compat_list_t     appCompatList;   // old applications compatibility info
} _app_info_t;

#endif

