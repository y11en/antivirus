// MethDlg.cpp : implementation file
//

#include "stdafx.h"
#include "visualeditor.h"
#include "visualeditordlg.h"
#include "MethDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char g_buff[0x4000];


/////////////////////////////////////////////////////////////////////////////
// CMethDlg dialog
// ---
const char* _findp( const char* c, const char* fp, uint* pref_len ) {
  uint fpl;
  if ( fp )
    fpl = strlen( fp );

  for( ; *c && *(c+1); c++ ) {
    const char* n;
    if ( (*c == '/') && (*(c+1) == '/') && (0!=(n=strchr(c+2,':'))) && (n>(c+2)) ) {
      if ( !fp || !strncmp(fp,c+2,fpl) ) {
        if ( pref_len )
          *pref_len = n - c + 1;
        return c;
      }
    }
  }
  if ( pref_len )
    *pref_len = 0;
  return 0;
}



// ---
bool iseng( const char* c ) {
  for( ; *c; c++ ) {
    if ( !isprint(*c) )
      return false;
  }
  return true;
}



// ---
char* extract_lang( tLangId lang, const char* comm ) {
  
  if ( (lang <= li_all) || (lang >= li_last) )
    return 0;

  uint        fsl;
  const char* fsp = _findp( comm, gLangId[lang], &fsl );

  if ( !fsp ) {
    if ( lang == li_rus ) {
      fsl = 0;
      fsp = comm;
    }
    else
      return 0;
  }

  fsp += fsl;

  uint        len;
  const char* ssp = _findp( fsp, 0, 0 );

  if ( !ssp ) {
    len = strlen( fsp );
    ssp = fsp + len; 
  }
  else
    len = ssp - fsp;

  char* ret = (char*)memcpy( new char[len+1], fsp, len );
  *(ret + len) = 0;
  return ret;
}



// ---
char* put_lang( char* comm, tLangId lang, const char* body ) {
  uint l, b;
  
  strcpy( comm+0, "//" );
  l = strlen( gLangId[lang] );

  memcpy( comm+2, gLangId[lang], l );
  l += 2;

  *(comm+l) = ':'; l++;
  if ( body ) {
    b = strlen( body );
    memcpy( comm+l, body, b+1 );
  }
  else {
    *(comm+l) = 0;
    b = 0;
  }
  return comm + b + l;
}



// ---
bool correct_lang( char* buff ) {
  CAPointer<char> rus = extract_lang( li_rus, buff );
  CAPointer<char> eng = extract_lang( li_eng, buff );
  put_lang( put_lang(buff,li_rus,rus), li_eng, eng );
  return true;
}



// ---
bool correct_lang( CBaseInfo& node ) {
  
  if ( 1 < node.ShortComment(g_buff,sizeof(g_buff)) ) {
    correct_lang( g_buff );
    node.SetShortComment( g_buff );
  }

  if ( 1 < node.ExtComment(g_buff,sizeof(g_buff)) ) {
    correct_lang( g_buff );
    node.SetExtComment( g_buff );
  }
  
  if ( 1 < node.BehaviorComment(g_buff,sizeof(g_buff)) ) {
    correct_lang( g_buff );
    node.SetBehaviorComment( g_buff );
  }
  
  if ( 1 < node.ValueComment(g_buff,sizeof(g_buff)) ) {
    correct_lang( g_buff );
    node.SetValueComment( g_buff );
  }

  if ( node.EntityType() == ifPubMethod ) {
    CMethodParamInfo pi( *(CMethodInfo*)&node );
    while( pi ) {
      correct_lang( pi );
      pi.GoNext();
    }
  }
  return true;
}




// ---
bool extract_curr_lang( CString& r, CBaseInfo& n, tDWORD (CBaseInfo::*func)(char*,tDWORD) const ) {
  n.StrVal( func, ::g_buff, sizeof(::g_buff) );
  if ( ::gLang == li_all )
    r = ::g_buff;
  else {
    CAPointer<char> comm = extract_lang( ::gLang, ::g_buff );
    r = comm;
  }
  return true;
}



// ---
bool set_curr_lang( CBaseInfo& n, tDWORD (CBaseInfo::*get)(char*,tDWORD) const, bool (CBaseInfo::*set)(const char*), CString& r ) {
  if ( ::gLang == li_all )
    return (n.*set)( r );
  else {
    char* b;
    n.StrVal( get, ::g_buff, sizeof(::g_buff) );
    CAPointer<char> rus = extract_lang( li_rus, ::g_buff );
    CAPointer<char> eng = extract_lang( li_eng, ::g_buff );
    if ( ::gLang == li_rus ) {
      b = put_lang( ::g_buff, li_rus, r );
      put_lang( b, li_eng, eng );
    }
    else {
      b = put_lang( ::g_buff, li_rus, eng );
      put_lang( b, li_eng, r );
    }
    return (n.*set)( ::g_buff );
  }
}



// ---
CMethDlg::CMethDlg( CWnd* pParent, HDATA meth )
	: CDialog(CMethDlg::IDD, pParent), 
    m_method(0,ifPubMethod), 
    m_param(m_method), 
    m_param_curr(-1),
    m_dad( dynamic_cast<CVisualEditorDlg*>(pParent) ),
    m_prop_types( m_dad->m_types ),
    m_obj_types( m_dad->m_types )
{

  if ( meth ) {
    meth = ::DATA_Copy( 0, 0, meth, 0 );
    m_method = meth;
    m_param = meth;
    m_type = m_method.StrVal( &CBaseInfo::TypeName, 0, ::g_buff, sizeof(::g_buff) );
  }
  
  if ( m_method.IsOK() )
    correct_lang( m_method );

  //{{AFX_DATA_INIT(CMethDlg)
  m_par_comm_behav = _T("");
  m_par_comm_large = _T("");
  m_par_comm_short = _T("");
	//}}AFX_DATA_INIT

}


void CMethDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMethDlg)
	DDX_Control(pDX, IDC_CB_LANG, m_cb_lang);
  DDX_Control(pDX, IDC_PAR_LIST, m_par_list);
  DDX_Control(pDX, IDC_CB_RET_VAL, m_ret_val_cb);
  DDX_Text(pDX, IDC_ED_COMM_BEHAV, m_comm_behav);
  DDX_Text(pDX, IDC_ED_COMM_LARGE, m_comm_large);
  DDX_Text(pDX, IDC_ED_COMM_SHORT, m_comm_short);
  DDX_Text(pDX, IDC_ED_NAME, m_name);
  DDX_Text(pDX, IDC_ED_COMM_VAL, m_comm_val);
	DDX_Text(pDX, IDC_ED_PAR_COMM_BEHAV, m_par_comm_behav);
	DDX_Text(pDX, IDC_ED_PAR_COMM_LARGE, m_par_comm_large);
	DDX_Text(pDX, IDC_ED_PAR_COMM_SHORT, m_par_comm_short);
	DDX_Text(pDX, IDC_RET_DEF_VAL, m_ret_def_val);
	DDX_CBString(pDX, IDC_CB_RET_VAL, m_ret_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMethDlg, CDialog)
//{{AFX_MSG_MAP(CMethDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PAR_LIST, OnItemchangedParList)
	ON_BN_CLICKED(IDC_BT_PAR_ADD, OnBtParAdd)
	ON_BN_CLICKED(IDC_BT_PAR_CHANGE, OnBtParChange)
	ON_BN_CLICKED(IDC_BT_PAR_DEL, OnBtParDel)
	ON_CBN_SELCHANGE(IDC_CB_LANG, OnSelchangeCbLang)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMethDlg message handlers

BOOL CMethDlg::OnInitDialog() 
{
  char  tn[MAX_PATH];
  CDialog::OnInitDialog();

  DWORD dwStyle = ListView_GetExtendedListViewStyle( m_par_list );
  dwStyle |= LVS_EX_FULLROWSELECT;// | LVS_EX_GRIDLINES;
  ListView_SetExtendedListViewStyle( m_par_list, dwStyle );

  CRect rect;
  m_par_list.GetWindowRect( &rect );
  int width = rect.Width();
  m_par_list.InsertColumn( 0, "type", LVCFMT_LEFT, width/3-2 );
  m_par_list.InsertColumn( 1, "name", LVCFMT_LEFT, 2*width/3-2 );

  int ind = 0, c = 0;
  m_param_count = 0;
  while( m_param ) {
    if ( m_param.IsSeparator() ) {
      ind = m_par_list.InsertItem( ind+1, "separator" );
      m_par_list.SetItemText( ind, 1, m_param.Name() );
    }
    else {
      int offs;
      if ( m_param.IsConst() ) {
        lstrcpy( tn, "const " );
        offs = 6;
      }
      else
        offs = 0;
      m_param.TypeName( &m_prop_types, tn+offs, sizeof(tn)-offs );
      if ( m_param.IsPointer() )
        lstrcat( tn, "*" );
      
      ind = m_par_list.InsertItem( ind+1, tn );
      m_par_list.SetItemText( ind, 1, m_param.Name() );
      m_param_count++;
    }
    m_param.GoNext();
  }
  m_param.GoFirst();
  
  method2str();

  if ( m_param_count )
    m_par_list.SetItemState( 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
  
  for ( ind=0,c=gTypes.Count(); ind<c; ind++ )
    m_ret_val_cb.AddString( gTypes[ind]->m_pName );

  CIObjectContainer& tbl = m_dad->m_IOTable;
  for ( ind=0,c=tbl.Count(); ind<c; ind++ )
    m_ret_val_cb.AddString( tbl[ind]->m_rcObjectName );
  m_ret_val_cb.SelectString( 0, m_ret_type );

  m_cb_lang.SelectString( 0, ::gLangId[::gLang] );
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}


#define IS_CHECKED(v,f) ( ((v) & (f)) == (f) )

// ---
void CMethDlg::OnItemchangedParList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  
  if ( m_param_count && IS_CHECKED(pNMListView->uNewState,LVIS_FOCUSED) ) {
    if ( m_param_curr != -1 ) {
      UpdateData( TRUE );
      str2param();
    }
    select_param();
    param2str();
    UpdateData( FALSE );
  }
  
  *pResult = 0;
}



// ---
bool CMethDlg::update( CBaseInfo& dst, CBaseInfo& src ) {
  CString pdst = dst.StrVal( &CBaseInfo::TypeName, &m_obj_types, ::g_buff, sizeof(::g_buff) );
  CString psrc = src.StrVal( &CBaseInfo::TypeName, &m_obj_types, ::g_buff, sizeof(::g_buff) );

  dst.SetName( src.Name() );
  if ( pdst.Compare(psrc) ) {
    m_dad->RefuseUsedObject( dst.Data(), VE_PID_IFM_RESULTTYPENAME );
    
    AVP_dword dwTypeType = 0;
    AVP_dword nNewType = m_dad->AcceptUsedObject( psrc, dwTypeType );
    
    dst.SetTypeName( psrc );
    dst.SetTypeID( nNewType );
    dst.SetTypeOfType( dwTypeType );
  }
  dst.SetShortComment( src.StrVal(&CBaseInfo::ShortComment,::g_buff,sizeof(::g_buff)) );
  dst.SetExtComment( src.StrVal(&CBaseInfo::ExtComment,::g_buff,sizeof(::g_buff)) );
  dst.SetBehaviorComment( src.StrVal(&CBaseInfo::BehaviorComment,::g_buff,sizeof(::g_buff)) );
  return true;
}



// ---
bool CMethDlg::update( HDATA data ) {
  CPubMethodInfo dst( data, false );

  str2method();
  update( dst, m_method );
  dst.SetStrValue( m_ret_def_val );
  dst.SetValueComment( m_comm_val );

  CMethodParamInfo par( dst );
  m_param.GoFirst();
  bool the_same = true;
  while ( m_param ) {
    if ( the_same ) {
      tGUID dstg;
      tGUID srcg;				   
      par.Guid( dstg );
      m_param.Guid( srcg );
      if ( dstg == srcg )
        update( par, m_param );
      else {
        the_same = false;
        while ( par )
          par.Remove();
      }
      par.GoNext();
    }
    else {
      DWORD type;
      m_dad->AcceptUsedObject( m_param.Name(), type );
      dst.Copy( m_param.Data() );
    }
    m_param.GoNext();
  }

  return true;
}




void CMethDlg::OnBtParAdd() 
{
  // TODO: Add your control notification handler code here
  
}

void CMethDlg::OnBtParChange() 
{
  // TODO: Add your control notification handler code here
  
}

void CMethDlg::OnBtParDel() 
{
  // TODO: Add your control notification handler code here
  
}


// ---
int CMethDlg::select_param() {
  POSITION pos = m_par_list.GetFirstSelectedItemPosition();
  m_param_curr = m_par_list.GetNextSelectedItem( pos );

  m_param.GoFirst();
  for( int i=0; (i<m_param_count) && (i!=m_param_curr) && m_param; i++,m_param.GoNext() )
    ;
  return m_param_curr;
}


// ---
void CMethDlg::OnSelchangeCbLang() {
  tLangId index = (tLangId)m_cb_lang.GetCurSel();
  
  UpdateData( TRUE );
  str2param();
  
  if ( (index >= li_all) && (index < li_last) )
    ::gLang = index;
  else
    ::gLang = li_all;
  
  param2str();

  extract_curr_lang( m_comm_val,   m_method, &CBaseInfo::ValueComment );
  extract_curr_lang( m_comm_short, m_method, &CBaseInfo::ShortComment );
  extract_curr_lang( m_comm_large, m_method, &CBaseInfo::ExtComment  );
  extract_curr_lang( m_comm_behav, m_method, &CBaseInfo::BehaviorComment );

  UpdateData( FALSE );
}



// ---
bool CMethDlg::str2param() {
  set_curr_lang( m_param, &CBaseInfo::ShortComment, &CBaseInfo::SetShortComment, m_par_comm_short );
  set_curr_lang( m_param, &CBaseInfo::ExtComment, &CBaseInfo::SetExtComment, m_par_comm_large );
  set_curr_lang( m_param, &CBaseInfo::BehaviorComment, &CBaseInfo::SetBehaviorComment, m_par_comm_behav );
  return true;
}



// ---
bool CMethDlg::param2str() {
  if ( m_param ) {
    extract_curr_lang( m_par_comm_short, m_param, &CBaseInfo::ShortComment );
    extract_curr_lang( m_par_comm_large, m_param, &CBaseInfo::ExtComment );
    extract_curr_lang( m_par_comm_large, m_param, &CBaseInfo::BehaviorComment );
  }
  return true;
}



// ---
bool CMethDlg::str2method() {
  m_method.SetName( m_name );
  m_method.SetTypeName( m_ret_type );
  m_method.SetStrValue( m_ret_def_val );
  
  set_curr_lang( m_method, &CBaseInfo::ValueComment,     &CBaseInfo::SetValueComment,     m_comm_val   );
  set_curr_lang( m_method, &CBaseInfo::ShortComment,     &CBaseInfo::SetShortComment,     m_comm_short );
  set_curr_lang( m_method, &CBaseInfo::ExtComment,       &CBaseInfo::SetExtComment,       m_comm_large );
  set_curr_lang( m_method, &CBaseInfo::BehaviorComment,  &CBaseInfo::SetBehaviorComment,  m_comm_behav );
  return true;
}



// ---
bool CMethDlg::method2str() {
  if ( m_method ) {
    m_name        = m_method.Name();
    m_ret_type    = m_method.StrVal( &CBaseInfo::TypeName, &m_obj_types,  ::g_buff, sizeof(::g_buff) );
    m_ret_def_val = m_method.StrVal( &CBaseInfo::Value,                   ::g_buff, sizeof(::g_buff) );
  
    extract_curr_lang( m_comm_val,   m_method, &CBaseInfo::ValueComment );
    extract_curr_lang( m_comm_short, m_method, &CBaseInfo::ShortComment );
    extract_curr_lang( m_comm_large, m_method, &CBaseInfo::ExtComment  );
    extract_curr_lang( m_comm_behav, m_method, &CBaseInfo::BehaviorComment );
  }
  else {
    m_name        = _T("method");
    m_ret_type    = _T("tERROR");
    m_ret_def_val = _T("errOK");
    m_comm_val    = _T("");
    m_comm_short  = _T("");
    m_comm_large  = _T("");
    m_comm_behav  = _T("");
  }
  return true;
}
