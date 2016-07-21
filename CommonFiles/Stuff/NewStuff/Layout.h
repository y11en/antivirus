/******************************************************************************
Module name: Layout.h
Written by: Jonathan W. Locke
Notices: Copyright (c) 1995 Jeffrey Richter
Purpose: Header file with external interface of layout algorithm.
******************************************************************************/


////////////////////////// Special Parent Identifier //////////////////////////


#define lPARENT 0x10000           // Special CHILD id for parent - this id is
                                  // guaranteed not to be used because you
                                  // cannot have control ids above 0xffff.


/////////////////////////////// Layout Actions ////////////////////////////////


typedef enum {
   lSTRETCH,                      // Metric should be stretched
   lMOVE,                         // Control should be moved
   lVCENTER,                      // Vertically center control/group
   lHCENTER,                      // Horizontally center control/group
	 lFLAGS, 												// Special flag for setting flags
   lEND,                          // Special flag for end of list
} lACTION;


////////////////////////////// Layout Part Types //////////////////////////////


typedef enum {
   lpLEFT   = 0,                  // Left side of control
   lpTOP    = 1,                  // Top side of control
   lpRIGHT  = 2,                  // Right side of control
   lpBOTTOM = 3,                  // Bottom side of control
   lpWIDTH  = 4,                  // Width of control
   lpHEIGHT = 5,                  // Height of control
   lpGROUP  = 6,                  // Group of one or more controls
   lpFLAGS  = 7,                  // Control flags                 
} lPART;


////////////////////////////// Control flags //////////////////////////////////
#define cfNOTINVALIDATE  0x0001


////////////////////////// Macros for Defining Rules //////////////////////////


//                                  Part,       Control id    Percentage
//                                  metric      or first id   of control or 
//      Macro                       or side     in group      last id in group
//      -----                       ----        -------       ---------
#define lLEFT(idc)                  lpLEFT,     idc,          0
#define lTOP(idc)                   lpTOP,      idc,          0
#define lRIGHT(idc)                 lpRIGHT,    idc,          0
#define lBOTTOM(idc)                lpBOTTOM,   idc,          0
#define lWIDTH(idc)                 lpWIDTH,    idc,          100
#define lHEIGHT(idc)                lpHEIGHT,   idc,          100
#define lWIDTHOF(idc, percent)      lpWIDTH,    idc,          percent
#define lHEIGHTOF(idc, percent)     lpHEIGHT,   idc,          percent
#define lCHILD(idc)                 lpGROUP,    idc,          idc
#define lGROUP(idcFirst, idcLast)   lpGROUP,    idcFirst,     idcLast
#define lCFLAGS(idc, flags)         lpFLAGS,    idc,          flags   


/////////////////////////// Definition of an Action ///////////////////////////


typedef struct {
   union {
      int nPart;                  // Name used only for group oriented actions
      int nMetric;                // Name used only for metric oriented actions
      int nSide;                  // Name used only for side oriented actions
   };
   union { 
      int idc;                    // Used for all actions except group actions
      int idcFirst;               // Used for group actions
   };
   union {
      int nPercent;               // Used only for width/height actions
      int idcLast;                // Used for group actions
      int nFlags;                 // Used for flags actions
   };
} lACTIONINFO;


//////////////////////////// Definition of a Rule /////////////////////////////


typedef struct {
   lACTION Action;                // Layout action to take 
   lACTIONINFO ActOn;             // Data describing the part to act on
   lACTIONINFO RelTo;             // Data for the part to position relative to
   int nOffset;                   // Additional offset (in dialog units)
   UINT fState;                   // INTERNAL USE: Rule application state flag
   int nPixelOffset;              // INTERNAL USE: nOffset converted to pixels
} RULE;


//////////////////////////// Prototype for Layout  ////////////////////////////

#ifdef LAYOUT_C

BOOL Layout_ComputeLayout (HWND hwndDlg, RULE* pRules);

#else

extern "C" BOOL Layout_ComputeLayout (HWND hwndDlg, RULE* pRules);

#endif
///////////////////////////////// End of File /////////////////////////////////
