////////////////////////////////////////////////////////////////////
//
//  FAutomat.h
//  A finite automaton definition file
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __FAUTOMAT_H__
#define __FAUTOMAT_H__

#include <Stuff\PArray.h>

class CFAutomaton;

// ---
typedef enum { 
  ps_und = -1,
  ps_nrm = -2,
  ps_grd = -3, 
} CFAPState;

typedef CFAPState *PCFAPState;



// ---
class CFAutoParam {
protected :
	int							 m_State;
  CFAutomaton     *m_pOwner;
public :
								 CFAutoParam() { m_State = ps_und; m_pOwner = 0; }
  virtual       ~CFAutoParam() {}
					void   SetOwner( CFAutomaton *pOwner ) { m_pOwner = pOwner; }
					int    GetState() const { return m_State; }
	virtual void   SetState(int s) { m_State = s; }
};


// ---
class CFAutoParamArray : public CPArray<CFAutoParam> {
public :
	CFAutoParamArray() :
		CPArray<CFAutoParam>( 0, 1, true ) {}
};


// ---
#define NO_EVENT     (-1)
#define NO           (-1)
#define END_STATE_TABLE (0)


// ---
struct CFAutomatonFlags {
	unsigned int faf_AutoReduce				: 1;
	unsigned int faf_Started   				: 1;
	unsigned int faf_StopOnReduce			: 1;
	unsigned int faf_EnableChainEnter	: 1;

  bool IsAutoReduce() const { return faf_AutoReduce; }
  void SetAutoReduce( bool s = true ) { faf_AutoReduce = !!s; }

  bool IsStarted() const { return faf_Started; }
  void SetStarted( bool s = true ) { faf_Started = !!s; }

  bool IsStopOnReduce() const { return faf_StopOnReduce; }
  void SetStopOnReduce( bool s = true ) { faf_StopOnReduce = !!s; }

  bool IsEnableChainEnter() const { return faf_EnableChainEnter; }
  void SetEnableChainEnter( bool s = true ) { faf_EnableChainEnter = !!s; }

	CFAutomatonFlags() :
		faf_AutoReduce( true ),
		faf_Started( false ),
		faf_StopOnReduce( true ),
		faf_EnableChainEnter( true ) {
	}
};



// ---
typedef enum {
  st_temporary   = -2, // временное
  st_reduced     = -1, // свертка
  st_normal      =  0  // нормальное
} CFAStateType;


typedef bool ( CFAutomaton::*PFLeave )( int & );
typedef bool ( CFAutomaton::*PFEnter )( int & );


// ---
struct CFAState {
  CFAStateType			m_Status;
  PFLeave           m_pLeave;
  PFEnter           m_pEnter;
  CFAPState        *m_pParamStates;
  int              *m_pJumps;

  CFAState( CFAStateType st,
						PFEnter			 fe,
						PFLeave			 fl,
						CFAPState		*ms,
						int					*mj ) :
    m_Status			( st  ),
    m_pLeave			( fl ),
    m_pEnter			( fe ),
    m_pParamStates( ms  ),
    m_pJumps			( mj  ) {
  }
};

typedef CFAState *PCFAState;

// ---
class CFAutomaton: public CFAutomatonFlags {
public:
          CFAutomaton        *prOwner;
                            CFAutomaton( PCFAState *table, bool bOwnsTable = true );
  virtual                  ~CFAutomaton();

	virtual bool              DetachParam( int nInd, DelType bDelete = defDelete );
	virtual void              AddParam( CFAutoParam *add, int atId = -1 );

	virtual void              Stop();
	virtual bool              Start();

	virtual bool              JumpAnalizer( int event );     // анализатор переходов
	virtual bool              JumpProcessor( int newState );   // реализатор переходов
	virtual bool              RunEnter( PFEnter f, int &want );
	virtual bool              RunLeave( PFLeave f, int &want );
					void							SetEvent( int event );

          int               GetCurrentStateJump( int event );
//  virtual void              SetCurrentState( int newState );  // установить новое состоние
	virtual	void							Clear() {}

  virtual int               GetResetState() { return 0; }

					bool              IsReduce();

  virtual void              Reset( bool autoRes = true );


       CFAutoParamArray     *GetParamArray() { return &m_rcPars; }

  virtual	bool              Reduce() { return true; }


protected :
          int								  m_nEvent;
          CFAutoParamArray   &m_rcPars;          // массив параметров процесса
          PCFAState					 *m_pStates;        // массив состоний процесса
          int									m_nPrevState;     // номер предыдущего состояния процесса
          int                 m_nCurrState;     // номер текущего состояния процесса
          int                 m_nResetState;    // номер состояния отката процесса
					bool                m_bOwnsTable;
					bool                m_bFirstJump;

					void							Init( PCFAState *pStates );
  virtual void              SetupState( int now );
  virtual void              JustSetupState();
};



#endif

