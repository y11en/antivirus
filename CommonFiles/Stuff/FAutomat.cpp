////////////////////////////////////////////////////////////////////
//
//  FAutomat.cpp
//  A finite automaton implementation file
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <Stuff\FAutomat.h>

// ---
CFAutomaton::CFAutomaton( PCFAState *table, bool bOwnsTable ) :
	m_rcPars( *new CFAutoParamArray ),
	m_bOwnsTable( bOwnsTable ) {
  Init( table );
}

// ---
void CFAutomaton::Init( PCFAState *pStates ) {
  m_nPrevState  = 0;        //номер предыдущего состояния процесса
  m_nCurrState  = 0;        //номер текущего состояния процесса
  m_nResetState = -1;       //номер состояния отката процесса
  m_pStates     = pStates;    //массив состояний процесса
	m_bFirstJump  = true;
}

// ---
CFAutomaton::~CFAutomaton() {
  Stop();
  delete &m_rcPars;
	if ( m_bOwnsTable && m_pStates ) {
		for ( int i=0; m_pStates[i] != END_STATE_TABLE; i++ )
			delete m_pStates[i];
		delete m_pStates;
	}
}


// ---
bool CFAutomaton::Start() {
  if ( !IsStarted() ) {
    SetStarted();
    Clear();
    JumpProcessor( m_nCurrState );
    return true;
  }
  else
    return false;
}



// ---
void CFAutomaton::Stop() {
  if ( IsStarted() ) {
    SetStarted( false );
    // для тех, кто останавливается, но не уничтожается  
    for( int i=0,c=m_rcPars.Count(); i<c; i++ )
      m_rcPars[i]->CFAutoParam::SetState( ps_und );
  }
}



// ---
void CFAutomaton::AddParam( CFAutoParam * add, int atIndex ) {
  if ( atIndex == -1 )
    m_rcPars.Add( add );
  else
    m_rcPars.Insert( atIndex, add );
  add->SetOwner( this );
}



// ---
bool CFAutomaton::DetachParam( int nInd, DelType  bDelete ) {
  m_rcPars.RemoveInd( nInd, bDelete );
  return true;
}


// ---
bool CFAutomaton::JumpAnalizer( int nEvent) {
  if ( nEvent != NO_EVENT ) {
    if ( m_pStates[m_nCurrState]->m_pJumps &&
         m_pStates[m_nCurrState]->m_pJumps[nEvent] != NO )
      return JumpProcessor( m_pStates[m_nCurrState]->m_pJumps[nEvent] );
    return false;
  }
  return true;
}


// ---
void CFAutomaton::JustSetupState() {
  for(int i=0,c=m_rcPars.Count(); i<c; i++)
    m_rcPars[i]->SetState( m_pStates[m_nCurrState]->m_pParamStates[i] );
}


// ---
void CFAutomaton::SetupState( int nState ) {
  m_nPrevState = m_nCurrState;
  m_nCurrState = nState;
  if ( m_pStates[m_nCurrState]->m_pParamStates )
    JustSetupState();
}


// ---
bool CFAutomaton::RunEnter( PFEnter f, int &want ) {
  return (this->*f)(want);
}


// ---
bool CFAutomaton::RunLeave( PFLeave f, int &want ) {
  return (this->*f)(want);
}


#define RUNENTER_ENABLE(state) (\
   ret &&                      \
   state != initWantState &&   \
   initWantState != NO &&      \
   IsEnableChainEnter() &&      \
   m_pStates[initWantState]->m_pEnter \
)                                  

// ---
bool CFAutomaton::JumpProcessor( int newState ) {
  bool ret = true;

  int initWantState = newState;

	if ( !m_bFirstJump ) {
		if ( m_pStates[m_nCurrState]->m_pLeave ) {
			ret = RunLeave( m_pStates[m_nCurrState]->m_pLeave, initWantState );
			newState = initWantState;
		}
	}

	m_bFirstJump = false;

  if ( ret && m_pStates[newState]->m_pEnter ) {

    if ( IsEnableChainEnter() ) {
      SetEnableChainEnter( false );
      int nState;
      do {
        nState = initWantState;
        ret = RunEnter( m_pStates[initWantState]->m_pEnter, initWantState );
      } while ( RUNENTER_ENABLE(nState) );
      SetEnableChainEnter( false );
    }
    else
      ret = RunEnter( m_pStates[initWantState]->m_pEnter,initWantState );

    if ( RUNENTER_ENABLE(newState) ) {
      SetEnableChainEnter( false );
      int nState;
      do {
        nState = initWantState;
        ret = RunEnter(m_pStates[initWantState]->m_pEnter,initWantState);
      } while ( RUNENTER_ENABLE(nState) );
      SetEnableChainEnter( false );
    }

    if ( m_pStates[initWantState]->m_Status != st_temporary )
      if ( ret && initWantState != NO )
        SetupState( initWantState );
  }
  else
    if ( m_pStates[newState]->m_Status != st_temporary )
      SetupState( newState );

  return ret;
}


// ---
void  CFAutomaton::SetEvent( int event ) {
  if ( JumpAnalizer(event) ) {
    if ( IsAutoReduce() && IsReduce() ) {
      if ( Reduce() )
        Reset();
    }
  }
}



// ---
bool CFAutomaton::IsReduce() {
  return m_pStates[m_nCurrState]->m_Status == st_reduced;
}



// ---
void CFAutomaton::Reset( bool autoRes ) {
  int state = autoRes
              ? (m_nResetState == -1)
                ? GetResetState()
                : m_nResetState
              : GetResetState();
  if ( state != NO )
    JumpProcessor( state );
}





