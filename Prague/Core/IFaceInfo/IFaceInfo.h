// Face.h: interface for the CFaceInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACE_H__72DE4CD9_CD0B_11D4_B767_00D0B7170E50__INCLUDED_)
#define AFX_FACE_H__72DE4CD9_CD0B_11D4_B767_00D0B7170E50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <property/property.h>
#include <Prague/prague.h>
#include <Prague/iface/i_iface.h>
#include <stuff/cpointer.h>
#include <avp_data.h>

class CStdTypeInfo;
class CSpecificTypeInfo;
class CBaseInfo;
class CFaceInfo;
class CPluginInfo;
class CTypeInfo;
class CConstantInfo;
class CErrInfo;
class CPropInfo;

typedef struct  _tGUID
{
  tDWORD Data1;
  tWORD  Data2;
  tWORD  Data3;
  tBYTE  Data4[8];
}	tGUID;

int operator == (const tGUID& guidOne, const tGUID& guidOther);

#define I(a) VE_PID_IF##a

// ---
class CTreeInfo {

private:
  bool  m_own;
  HDATA m_tree;
  HDATA m_first;

protected:
  HDATA m_curr;

public:
  CTreeInfo( HDATA tree, bool own=false );
  CTreeInfo( tDWORD len, const tBYTE* stream );
  virtual ~CTreeInfo();

  bool          Init            ( HDATA node, bool own=false );
  bool          Init            ( tDWORD len, const tBYTE* stream );
                operator bool   () const { return IsOK(); }
  HDATA         Data            () const { return m_curr; }
  HDATA         operator ()     () const { return m_curr; }

  bool          Own() const     { return m_own; }
  bool          Own( bool own ) { bool r=m_own; m_own = own; return r; }

  HDATA         operator =      ( HDATA tree ) { Init(tree); return m_curr; }
  virtual bool  IsOK            () const { return m_curr != 0; }
             
	HDATA         Dad             () const;
  virtual HDATA First           () const { return m_first; }
  virtual HDATA Next            () const;
  virtual bool  GoFirst         ();
  virtual bool  GoNext          ();
  virtual bool  GoPrev          ();

  HDATA         Add             ( tDWORD id, tDWORD val=0, tDWORD size=0 );
  HDATA         Copy            ( HDATA copy, tDWORD flags=0 );
  bool          Remove          ();

  bool          DelAttr         ( tDWORD aid, AVP_dword* addr=0 );
  
  tDWORD        Attr            ( tDWORD aid, void* pVal, tDWORD size, AVP_dword* addr=0 ) const;
  bool          BoolAttr        ( tDWORD aid, AVP_dword* addr=0 ) const;
  tDWORD        DWAttr          ( tDWORD aid, AVP_dword* addr=0 ) const;
	tDWORD        DWAttrAsStr     ( tDWORD aid, char* val, tDWORD size, bool hex=true, AVP_dword* addr=0 ) const;
  tDWORD        StrAttr         ( tDWORD aid, char* val, tDWORD size, AVP_dword* addr=0 ) const;

  bool          SetAttr         ( tDWORD aid, void* pVal, tDWORD size, AVP_dword* addr=0 );
  bool          SetBoolAttr     ( tDWORD aid, bool val, AVP_dword* addr=0 );
  bool          SetDWAttr       ( tDWORD aid, tDWORD val, AVP_dword* addr=0 );
  bool          SetStrAttr      ( tDWORD aid, const char* val, tUINT len = 0, AVP_dword* addr=0 );

  HPROP         MainProp        () const;
  HPROP         FindProp        ( tDWORD pid ) const;

protected:
  void          StartHere       ( HDATA start ) { m_first = m_curr = start; }
  virtual bool  ReInit          () { return true; }

private:
  CTreeInfo( const CTreeInfo& );
  void operator = ( const CTreeInfo& );
};




// ---
class CStdTypeInfo : public CTreeInfo {

public:
  typedef enum tTypeID { object, property, obligatory, predefined, last };

public:
  CStdTypeInfo( HDATA tree, bool own=false ) : CTreeInfo(tree,own) {}
  CStdTypeInfo( tDWORD len, const tBYTE* stream ) : CTreeInfo(len,stream) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }
  HDATA TypeTree( tTypeID id ) const;

  virtual HDATA Next            () const  { return 0; }
  virtual bool  GoFirst         () { return false; }
  virtual bool  GoNext          () { return false; }
  virtual bool  GoPrev          () { return false; }

  virtual bool IsOK() const;
};



// ---
class CSpecificTypeInfo : public CTreeInfo {

private:
  CStdTypeInfo::tTypeID m_type;

public:
  CSpecificTypeInfo( HDATA tree, bool own=false ) : CTreeInfo(tree,own), m_type(CStdTypeInfo::last) { ReInit(); }
  HDATA  operator = ( HDATA tree );
  virtual bool IsOK () const;
  tDWORD Id( tDWORD type ) const;
  tDWORD Id( const char* type ) const;
  tDWORD Name( tDWORD type, char* val, tDWORD size ) const;
  tDWORD MnemonicID( tDWORD type, char* val, tDWORD size ) const;

  virtual HDATA Next            () const  { return 0; }
  virtual bool  GoFirst         () { return false; }
  virtual bool  GoNext          () { return false; }
  virtual bool  GoPrev          () { return false; }

protected:
  virtual bool ReInit();

};



// ---
class CObjTypeInfo : public CSpecificTypeInfo {
public: 
  CObjTypeInfo( const CStdTypeInfo& types ) : CSpecificTypeInfo(types.TypeTree(CStdTypeInfo::object),false) {}
  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }
};


// ---
class CPropTypeInfo : public CSpecificTypeInfo {
public: 
  CPropTypeInfo( const CStdTypeInfo& types ) : CSpecificTypeInfo(types.TypeTree(CStdTypeInfo::property),false) {}
  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }
};


// ---
class CObligTypeInfo : public CSpecificTypeInfo {
public: 
  CObligTypeInfo( const CStdTypeInfo& types ) : CSpecificTypeInfo(types.TypeTree(CStdTypeInfo::obligatory),false) {}
  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }
};


// ---
class CPredefinedTypeInfo : public CSpecificTypeInfo {
public: 
  CPredefinedTypeInfo( const CStdTypeInfo& types ) : CSpecificTypeInfo(types.TypeTree(CStdTypeInfo::predefined),false) {}
  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }
};



// ---
class CBaseInfo : public CTreeInfo {

protected:
  tDWORD           m_en_type;
  CAPointer<char> m_name;

public:
  CBaseInfo( HDATA tree, tDWORD en_type, bool own=false ) : CTreeInfo(tree,own), m_en_type(en_type), m_name(0) { ReInit(); }
  CBaseInfo( tDWORD len, tBYTE* stream, tDWORD en_type ) : CTreeInfo(len,stream), m_en_type(en_type), m_name(0) { ReInit(); }

  char* StrVal( tDWORD (CBaseInfo::*func)(char*,tDWORD) const, char* buff=0, int len=0 ) const;
  char* StrVal( tDWORD (CBaseInfo::*func)(const CSpecificTypeInfo*,char*,tDWORD) const, const CSpecificTypeInfo* ti, char* buff=0, int len=0 ) const;
  
protected:
  virtual bool  ReInit();
		
public:
  static tDWORD  DetectEnType( HDATA tree );
  tDWORD         EntityType      () const { return m_curr ? m_en_type : ifAny; }
  
public:
  HDATA         operator =      ( HDATA tree ) { Init(tree); return m_curr; }

  virtual bool  IsOK            () const { return CTreeInfo::IsOK() && (m_en_type != ifAny); }

  bool          IsSeparator     () const;
  bool          SetSeparator    ( bool set );
  const char*   Name            () const;
  tDWORD         Name            ( char* val, tDWORD size ) const;
  virtual tDWORD TypeID          () const;
  virtual tDWORD TypeOfType      () const;
  virtual tDWORD TypeName        ( const CSpecificTypeInfo* ti, char* val, tDWORD size ) const;
	virtual tDWORD Value           ( char* val, tDWORD size ) const;
  tDWORD         ShortComment    ( char* val, tDWORD size ) const;
  tDWORD         ExtComment      ( char* val, tDWORD size ) const;
  tDWORD         BehaviorComment ( char* val, tDWORD size ) const;
  tDWORD         ValueComment    ( char* val, tDWORD size ) const;

  bool          SetName         ( const char* val );

  bool          SetTypeID       ( tDWORD type );
  bool          SetTypeOfType   ( tDWORD type_of_type );
  bool          SetTypeName     ( const char* val );

  bool          SetStrValue     ( const char* val );
  bool          SetDWValue      ( tDWORD val, bool hex=true );
  
  bool          SetShortComment ( const char* val );
  bool          SetExtComment   ( const char* val );
  bool          SetBehaviorComment( const char* val );
  bool          SetValueComment ( const char* val );
  
  bool          Guid            ( tGUID& val )             const;
  tDWORD        Guid            ( char* val, tDWORD size ) const;
};


// ---
class CFaceInfo : public CBaseInfo {
public:
  CFaceInfo( HDATA tree, bool own ) : CBaseInfo(tree,ifIFace,own) {}
  CFaceInfo( tDWORD len, tBYTE* stream ) : CBaseInfo(len,stream,ifIFace) {}

public:
  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

  tDWORD MnemonicID( char* val, tDWORD size ) const;
	tDWORD IFaceID() const;
	tDWORD Revision() const;
	bool  CreationDate( tDATETIME *pVal ) const;
	bool  Selected() const;																 // interface is linked (doesn't selected for codegeneration)
	bool  Linked() const;                                  // is implemented in plugin (not included)
	bool  Static() const;
  bool  System() const;
  tDWORD HeaderGUID( char* val, tDWORD size ) const;
	tDWORD HeaderName( char* val, tDWORD size ) const;
	tDWORD SourceFileName( char* val, tDWORD size ) const;
	tDWORD Author( char* val, tDWORD size ) const;
	tDWORD VendorID() const;
	tDWORD SubType() const;
	tDWORD SubTypeStr( char* val, tDWORD size ) const;
	tDWORD PluginID() const;
	bool  TransferPropUp() const;
	bool  LastCallProp() const;
	tDWORD LastCallPropRFunc( char* val, tDWORD size ) const;
	tDWORD LastCallPropWFunc( char* val, tDWORD size ) const;
	bool  UnSwapable() const;
  tDWORD Synchronized() const;
  tDWORD IncludeFile( tDWORD index, char* val, tDWORD size ) const;

	tDWORD NumberOf( tDWORD entity_type ) const;
	tDWORD NameOf( tDWORD entity_type, tDWORD index, char* val, tDWORD size ) const;
	tDWORD TypeIDOf( tDWORD entity_type, tDWORD index ) const;
	tDWORD TypeStrOf( tDWORD entity_type, tDWORD index, char* val, tDWORD size ) const;
	tDWORD ValueOf( tDWORD entity_type, tDWORD index, char* val, tDWORD size, bool hex=false ) const;

  HDATA FirstType() const;
  HDATA FirstConstant() const;
  HDATA FirstMsgClass() const;
  HDATA FirstErrCode() const;
  HDATA FirstProperty() const;
  HDATA FirstPubMethod() const;
  HDATA FirstIntMethod() const;
  HDATA FirstDataStruct() const;

public:
  typedef enum NavType { n_list, n_name, n_type_name, n_type_id, n_value, n_last };

protected:
	tDWORD EntityStr( tDWORD entity_type, tDWORD index, char* val, tDWORD size, NavType id ) const;
	HDATA EntityList( tDWORD entity_type, tDWORD* ind ) const;

};



// ---
class CPluginInfo : public CBaseInfo {
public:
  CPluginInfo( HDATA tree, bool own ) : CBaseInfo(tree,ifPlugin,own) {}
  CPluginInfo( const CFaceInfo& fi ) : CBaseInfo( ::DATA_Get_Dad(fi(),0),ifPlugin,false) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

	bool  Selected         () const;
  tDWORD PluginID         () const;
  tDWORD Version          () const;
  tDWORD MnemonicID       ( char* val, tDWORD size ) const;
  tDWORD VendorID         () const;
  tDWORD VendorMnemonicID ( char* val, tDWORD size ) const;
  tDWORD VendorName       ( char* val, tDWORD size ) const;
  tDWORD DefCodePage      () const;
  tDWORD DefDateFormatID  () const;
  tDWORD Project          ( char* val, tDWORD size ) const;
  tDWORD SubProject       ( char* val, tDWORD size ) const;
  tDWORD Author           ( char* val, tDWORD size ) const;

  HDATA FirstIFace       () const;
  HDATA StaticIFace      () const;
  
};



// ---
class CTypeInfo : public CBaseInfo {
public:
  CTypeInfo( const CFaceInfo& fi ) : CBaseInfo(fi.FirstType(),ifType,false) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }
  virtual tDWORD Value( char* v, tDWORD s, bool hex=true ) const;
  bool					IsNative() const;
};




// ---
class CConstantInfo : public CBaseInfo {
public:
  CConstantInfo( const CFaceInfo& fi ) : CBaseInfo(fi.FirstConstant(),ifConstant,false) {}
  HDATA operator = ( HDATA tree ) { Init(tree); return m_curr; }
  bool  IsNative() const;
};



// ---
class CMsgClassInfo : public CBaseInfo {
public:
  CMsgClassInfo( const CFaceInfo& fi ) : CBaseInfo(fi.FirstMsgClass(),ifMsgClass,false) {}
  HDATA operator = ( HDATA tree ) { Init(tree); return m_curr; }
	tDWORD Id()       const;
  bool  IsNative() const;
  HDATA FirstMsg() const;
};



// ---
class CMsgInfo : public CBaseInfo {
public:
  CMsgInfo( const CMsgClassInfo& mc ) : CBaseInfo(mc.FirstMsg(),ifMsg,false) {}
  HDATA operator = ( HDATA tree ) { Init(tree); return m_curr; }

	tDWORD Id()       const;
  bool  IsNative() const;
	tDWORD SendPointComment( char* val, tDWORD size );
	tDWORD ContextComment( char* val, tDWORD size );
	tDWORD DataComment( char* val, tDWORD size );
};



// ---
class CErrInfo : public CBaseInfo {
public:
  CErrInfo( const CFaceInfo& fi ) : CBaseInfo(fi.FirstErrCode(),ifErrCode,false) {}

  HDATA         operator =  ( HDATA tree ) { Init(tree); return m_curr; }
  virtual tDWORD TypeID      () const;
  virtual tDWORD TypeName    ( const CSpecificTypeInfo* ti, char* val, tDWORD size ) const;
  tDWORD         Value       () const;
  bool          IsWarning   () const;
  bool					IsNative() const;
};



// ---
class CPropInfo : public CBaseInfo {
public:
  typedef enum { local, shared } ScopeType;
  typedef enum { none, read, write, read_write } Mode;

public:
  CPropInfo( const CFaceInfo& fi ) : CBaseInfo(fi.FirstProperty(),ifProperty,false) {}
	CPropInfo( HDATA prop ) : CBaseInfo( prop, ifAny, false ) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

  tDWORD     DigitalID       () const;
  tDWORD     TypifiedID      ( const CStdTypeInfo& ti ) const;
  ScopeType Scope           () const;
  bool      Required        () const;
  Mode      MemberMode      () const;
  tDWORD     MemberName      ( char* val, tDWORD size ) const;
  Mode      FuncMode        () const;
  tDWORD     ReadFuncName    ( char* val, tDWORD size ) const;
  tDWORD     WriteFuncName   ( char* val, tDWORD size ) const;
  tDWORD     VarName         ( char* val, tDWORD size ) const;
  bool      Native          () const;
  bool      Predefined      () const;
  bool      HardPredefined  () const;
  bool      WritableOnInit  () const;
  tDWORD     BinValue        ( const CSpecificTypeInfo& ti, char* val, tDWORD size, tDWORD* type ) const;
};



// ---
class CMethodInfo : public CBaseInfo {

public:
	CMethodInfo( HDATA tree, tDWORD type, bool own=false ) : CBaseInfo(tree,type,own) {}

public:
  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

  bool  IsPublic()   const;
  bool  IsInternal() const;
  bool  IsExtended() const;
  virtual bool IsOK() const { return CBaseInfo::IsOK() && ((m_en_type == ifPubMethod) || (m_en_type == ifIntMethod)); }
  HDATA FirstParam() const;
  bool  Selected  () const;

  tDWORD MethodID()   const;
  bool  SetMethodID( tDWORD id );
};



// ---
class CPubMethodInfo : public CMethodInfo {
public:
  CPubMethodInfo( const CFaceInfo& fi ) : CMethodInfo(fi.FirstPubMethod(),ifPubMethod,false) {}
  CPubMethodInfo( HDATA tree, bool own ) : CMethodInfo(tree,ifPubMethod,own) {}
  virtual bool IsOK() const { return CBaseInfo::IsOK() && (m_en_type == ifPubMethod); }
};



// ---
class CIntMethodInfo : public CMethodInfo {
public:
  CIntMethodInfo( const CFaceInfo& fi ) : CMethodInfo(fi.FirstIntMethod(),ifIntMethod,false) {}
  CIntMethodInfo( HDATA tree, bool own ) : CMethodInfo(tree,ifIntMethod,own) {}
  virtual bool IsOK() const { return CBaseInfo::IsOK() && (m_en_type == ifIntMethod); }
};


// ---
class CMethodParamInfo : public CBaseInfo {
public:
  CMethodParamInfo( const CMethodInfo& mi ) : CBaseInfo(mi.FirstParam(),ifMethodParam,false) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

  bool  IsConst   () const;
  bool  IsPointer () const;
  bool  IsDoublePointer () const;
};



// ---
class CDataStructInfo : public CBaseInfo {
public:
  CDataStructInfo( const CFaceInfo& fi ) : CBaseInfo(fi.FirstDataStruct(),ifDataStruct,false) {}
  CDataStructInfo( const CDataStructInfo& dsi ) : CBaseInfo(dsi.FirstInnerStruct(),ifDataStruct,false) {}
  CDataStructInfo( HDATA tree, bool own ) : CBaseInfo(tree,ifDataStruct,own) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

  HDATA FirstMember() const;
  HDATA FirstInnerStruct() const;
};



// ---
class CDataMemberInfo : public CBaseInfo {
public:
  CDataMemberInfo( const CDataStructInfo& di ) : CBaseInfo(di.FirstMember(),ifDataStructMember,false) {}

  HDATA  operator = ( HDATA tree ) { Init(tree); return m_curr; }

  bool  IsArray     () const;
  tDWORD ArrayBound  ( char* val, tDWORD size ) const;
};

#endif // !defined(AFX_FACE_H__72DE4CD9_CD0B_11D4_B767_00D0B7170E50__INCLUDED_)
