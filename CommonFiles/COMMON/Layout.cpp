/******************************************************************************
Module name: Layout.c
Written by: Jonathan W. Locke
Notices: Copyright (c) 1995 Jeffrey Richter
Purpose: Implementation of a layout algorithm which positions controls
         in a window based on a set of rules.
******************************************************************************/
#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#pragma warning(disable: 4001)    // Single line comment 

#include "Layout.h"


//////////////////////// ID for Child List Terminator /////////////////////////


#define IDC_LASTCHILD 0x10001     // Special child id for last child in list.
                                  // Guaranteed not to be used because you
                                  // cannot have control ids above 0xffff.


/////////////////////////////////// Limits ////////////////////////////////////


#define NUMPARTS     7            // There are 7 parts: left, top, right,
                                  // bottom, width, height and group
#define NUMMETRICS   6            // There are 6 metrics: left, top, right,
                                  // bottom, width and height.
#define NUMSIDES     4            // There are 4 sides: left, top, right and
                                  // bottom.


/////////////////////// Macros for Asserting Part Types ///////////////////////


#define ISPART(n)   INRANGE(lpLEFT, (n), lpGROUP)
#define ISMETRIC(n) INRANGE(lpLEFT, (n), lpHEIGHT)
#define ISSIDE(n)   INRANGE(lpLEFT, (n), lpBOTTOM)


///////////////////////////////// Metric Flags ////////////////////////////////


typedef enum {
   UNKNOWN,                       // Metric is unknown
   KNOWN                          // Metric is known
} MFLAG;


///////////////////////////////// Rule States /////////////////////////////////


enum {
   UNAPPLIED,                     // Rule has not been applied
   APPLIED                        // Rule has been applied
};


//////////////////////// Definition of a Child Control ////////////////////////


typedef struct tagCHILD {
   int idc;                       // Control id (lPARENT represents parent)
   union {                        // Child has either a list of metrics:
      int anMetric[NUMMETRICS];   //   left, top, right, bottom, width, height
      struct {                    // or a RECT, a width and a height:
         RECT rc;                 //   RECT (left, top, right, bottom)
         int nWidth;              //   width
         int nHeight;             //   height
      };                          // depending on which is more readable in
   };                             // a given context.
   BOOL fFixed;                   // TRUE if child doesn't need to be placed
   UINT fFlags;                   // 
   MFLAG afMetric[NUMMETRICS];    // Metric flags (KNOWN or UNKNOWN)
} CHILD;


////////////////////////// Local Function Prototypes //////////////////////////


BOOL adgMapDialogRect (HWND hwndParent, PRECT prc);

int Layout_GetOppositeSide (int nSide);
int Layout_GetOtherUnknownMetric (int nUnknownMetric);
BOOL Layout_MetricIsVertical (int nMetric);
void Layout_SolveChild (CHILD* pChild);
CHILD* Layout_FindChild (CHILD* pChildList, int idcChild);
CHILD* Layout_CreateChildList (HWND hwndParent, int* pnChildren);
void Layout_ConvertDlgUnits (HWND hwndParent, RULE* pRules, CHILD* pChildList);
void Layout_MarkUnknowns (HWND hwndParent, RULE* pRules, CHILD* pChildList);
BOOL Layout_CheckChild (CHILD* pChild);
BOOL Layout_ApplyRule (HWND hwndParent, RULE* pRules,
   CHILD* pChildList, RULE* pRule);
BOOL Layout_ApplyRules (HWND hwndParent, RULE* pRules, CHILD* pChildList);


///////////////////////////////////////////////////////////////////////////////


// A MapDialogRect function that works for non-dialog windows.
BOOL adgMapDialogRect (HWND hwndParent, PRECT prc) {
   
   HDC hdc;
   SIZE size;
   int cxChar, cyChar;
   HFONT hfont, hfontOriginal;

   // This is the set of characters that Windows uses to compute the average
   // character width.
   static TCHAR szChars[] =
      __TEXT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
   
   // If the window is a dialog, just use MapDialogRect.
   if (GetClassWord(hwndParent, GCW_ATOM) == 32770)
      return(MapDialogRect(hwndParent, prc));
   
   // Get a device context and select the window's font into it.
   hdc = GetDC(hwndParent);
   hfont = GetWindowFont(hwndParent);
   if (hfont != NULL)
      hfontOriginal = SelectFont(hdc, hfont);
   
   // Unfortunately, we cannot use GetTextMetrics to get the average character
   // width because the TEXTMETRIC structure's tmAveCharWidth member is 
   // incorrect for proportional fonts. So, instead we compute the average
   // character width ourselves using the same technique employed by Windows
   // itself:  We pass "a-zA-Z" to GetTextExtentPoint and average, rounding up.
   // (NOTE: We do not call GetTextExtentPoint32 because this function corrects
   // an error that Windows relies on)
   GetTextExtentPoint(hdc, szChars, ARRAY_SIZE(szChars), &size);
   cyChar = size.cy;
   cxChar = ( ( size.cx / ( ARRAY_SIZE ( szChars ) / 2 ) ) + 1 ) / 2;
   
   // Restore any original font and then release the device context.
   if (hfont != NULL)
      SelectFont(hdc, hfontOriginal);
   ReleaseDC(hwndParent, hdc);
   
   // Map rectangle prc based on the font dimensions (cxChar by cyChar).
   SetRect(prc, 
      prc->left  * cxChar / 4, prc->top    * cyChar / 8,
      prc->right * cxChar / 4, prc->bottom * cyChar / 8);
   return(TRUE);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int Layout_GetOppositeSide ( int nSide )
{
     int nOppositeSide;
     
     switch ( nSide )
     {
     case lpLEFT:
          nOppositeSide = lpRIGHT;
          break;
          
     case lpRIGHT:
          nOppositeSide = lpLEFT;  
          break;
          
     case lpTOP:    
          nOppositeSide = lpBOTTOM; 
          break;
          
     case lpBOTTOM: 
          nOppositeSide = lpTOP; 
          break;
     }
     return nOppositeSide;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int Layout_GetOtherUnknownMetric ( int nUnknownMetric )
{
     switch ( nUnknownMetric )
     {
     case lpLEFT:
     case lpRIGHT:
          return lpWIDTH;
          
     case lpTOP:    
     case lpBOTTOM: 
          return lpHEIGHT; 
          
     case lpWIDTH:  
          return lpRIGHT; 
          
     case lpHEIGHT: 
          return lpBOTTOM;
     }
     return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL Layout_MetricIsVertical ( int nMetric )
{
     switch ( nMetric )
     {
     case lpLEFT:
     case lpRIGHT:
     case lpWIDTH:  
          return FALSE;
          
     case lpTOP:    
     case lpBOTTOM: 
     case lpHEIGHT: 
          return TRUE;
     }
     return FALSE;
}


///////////////////////////////////////////////////////////////////////////////


void Layout_SolveChild (CHILD* pChild) {

   int i;

   // Loop through all six metrics of a child, computing values for unknown
   // metrics from values of known metrics (if any).
   for (i = 0; i < NUMMETRICS; i++) {

      // If this metric of the child is unknown, see if it can be computed in
      // terms of other metrics which we do know.
      if (pChild->afMetric[i] == UNKNOWN) {

         // Compute left/top as right/bottom - width/height
         if (i < 2) {
            if ((pChild->afMetric[i + 2] == KNOWN) && 
                (pChild->afMetric[i + 4] == KNOWN)) {
               pChild->anMetric[i] = pChild->anMetric[i + 2] -
                  pChild->anMetric[i + 4];
               pChild->afMetric[i] = KNOWN;
            }
         } 
         // Compute right/bottom as left/top + width/height
         else if (i < 4) {
            if ((pChild->afMetric[i - 2] == KNOWN) && 
                (pChild->afMetric[i + 2] == KNOWN)) {
               pChild->anMetric[i] = pChild->anMetric[i - 2] +
                  pChild->anMetric[i + 2];
               pChild->afMetric[i] = KNOWN;
            }
         }
         // Compute width/height as right/bottom - left/top
         else {
            if ((pChild->afMetric[i - 2] == KNOWN) && 
                (pChild->afMetric[i - 4] == KNOWN)) {
               pChild->anMetric[i] = pChild->anMetric[i - 2] -
                  pChild->anMetric[i - 4];
               pChild->afMetric[i] = KNOWN;
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CHILD * Layout_FindChild ( CHILD * pChildList, int idcChild )
{
     // Traverse the child list looking for an id
     for  ( CHILD * pChild = pChildList; pChild -> idc != IDC_LASTCHILD; ++pChild )
     {
          // If we find idcChild, we solve the child for unknowns and return it
          if   ( pChild -> idc == idcChild )
          {
               Layout_SolveChild ( pChild );
               return pChild;
          }
     }
     return NULL;
}


///////////////////////////////////////////////////////////////////////////////


CHILD* Layout_CreateChildList (HWND hwndParent, int* pnChildren) {
   
   int i;
   HWND hwnd, hwndFirst;
   CHILD* pChild, *pChildList;

   // Count the number of child windows in hwndParent.
   hwndFirst = hwnd = GetFirstChild(hwndParent);
   for (*pnChildren = 0; IsWindow(hwnd); hwnd = GetNextSibling(hwnd)) 
      (*pnChildren)++;
   if (*pnChildren == 0)
      return(NULL);

   // Allocate memory for the CHILD list. This list will have an entry for
   // each child of the dialog, plus a CHILD structure for the parent window
   // (lPARENT) and one which acts as a list terminator (IDC_LASTCHILD).
   pChildList = ( CHILD * ) malloc((*pnChildren + 2) * sizeof(CHILD));

   if (!pChildList)
      return(NULL);
   pChild = pChildList;

   // Add the special-case parent 'CHILD' structure
   pChild->idc = lPARENT;
   GetClientRect(hwndParent, &pChild->rc);
   for (i = 0; i < NUMSIDES; i++)
      pChild->afMetric[i] = KNOWN;
   pChild->afMetric[lpWIDTH] = pChild->afMetric[lpHEIGHT] = UNKNOWN;
   Layout_SolveChild(pChild);
   pChild++;

   // Add all the real children of the dialog to the list
   hwnd = hwndFirst;
   for (; IsWindow(hwnd); hwnd = GetNextSibling(hwnd)) {

      // Get child's id and bounding rectangle in client coordinates
      pChild->idc = GetWindowID(hwnd);
      GetWindowRect(hwnd, &pChild->rc);
      MapWindowRect(HWND_DESKTOP, hwndParent, &pChild->rc);
      for (i = 0; i < NUMSIDES; i++)
         pChild->afMetric[i] = KNOWN;

      // Solve for the width and height.
      pChild->afMetric[lpWIDTH] = pChild->afMetric[lpHEIGHT] = UNKNOWN;
      Layout_SolveChild(pChild);

      // All children are fixed, initially.
      pChild->fFixed = TRUE;
      pChild->fFlags = 0;
      pChild++;
   }

   // Terminate and return the list.
   pChild->idc = IDC_LASTCHILD;
   return(pChildList);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void Layout_ConvertDlgUnits ( HWND hwndParent, RULE * pRules, CHILD * pChildList )
{
     // Traverse the rules list
     for  ( RULE * pRule = pRules; pRule->Action != lEND; ++pRule )
     {
		 if ( pRule->nOffset != 0x7FFFFFFF )
		 {
			 RECT rc = { 0, 0, 0, 0 };
			 BOOL fVertical;
			 
			 // Simultaneously map the rule's offset value, vertically and
			 // horizontally, from dialog units to pixels.
			 rc.right = rc.bottom = pRule->nOffset;
			 adgMapDialogRect(hwndParent, &rc);
			 
			 // Determine if the current rule affects horizontal or vertical
			 // coordinates. We need to know this because dialog unit space is not
			 // isometric (horizontal and vertical dialog units are not equivalent).
			 switch (pRule->Action) {
				 
			 case lVCENTER:
				 fVertical = TRUE;
				 break;
				 
			 case lHCENTER:
				 fVertical = FALSE;
				 break;
				 
			 case lMOVE:
			 case lSTRETCH:
				 fVertical = Layout_MetricIsVertical(pRule->ActOn.nMetric);
				 break;
			 }
			 // Take the correct mapped value based on the rule being applied
			 pRule->nPixelOffset = fVertical ? rc.bottom : rc.right;
		 }
     }  
}


///////////////////////////////////////////////////////////////////////////////


void Layout_MarkUnknowns (HWND hwndParent, RULE* pRules, CHILD* pChildList) {

   RULE* pRule;
   CHILD* pChildActOn;
   HWND hwnd;
   int nOtherUnknown, nOppositeSide, idc, idcFirst, idcLast;

   // Traverse the rule list, marking unknowns in the child list.
   for (pRule = pRules; pRule->Action != lEND; pRule++) {

      // Set metric flags based on the rule's proposed action
      switch (pRule->Action) {

         case lSTRETCH:           // Metric should be stretched

            // Find the child to be acted upon
            pChildActOn = Layout_FindChild(pChildList, pRule->ActOn.idc);

            // Since the child is going to be acted upon, it is no longer fixed.
            pChildActOn->fFixed = FALSE;

            // The metric being stretched must be unknown.
            pChildActOn->afMetric[pRule->ActOn.nMetric] = UNKNOWN;

            // If the left/top or right/bottom is unknown, so is the
            // width/height. If the width/height is unknown, then the
            // right/bottom is also unknown.
            nOtherUnknown = Layout_GetOtherUnknownMetric(pRule->ActOn.nMetric);
            pChildActOn->afMetric[nOtherUnknown] = UNKNOWN;
            break;

         case lMOVE:              // Control should be moved

            // Find the child to be acted upon
            pChildActOn = Layout_FindChild(pChildList, pRule->ActOn.idc);

            // Since the child is going to be acted upon, it is no longer fixed.
            pChildActOn->fFixed = FALSE;

            // The side being moved is unknown.
            pChildActOn->afMetric[pRule->ActOn.nSide] = UNKNOWN;

            // So is the opposite side. But, the width/height remains known.
            // (Actually, this is the primary reason for having six metrics).
            nOppositeSide = Layout_GetOppositeSide(pRule->ActOn.nSide);
            pChildActOn->afMetric[nOppositeSide] = UNKNOWN;
            break;

         case lVCENTER:           // Vertically center control/group
         case lHCENTER:           // Horizontally center control/group
            // We must be centering a group of one or more controls.
            // Go through the group of one or more controls
            idcFirst = pRule->ActOn.idcFirst;
            idcLast = pRule->ActOn.idcLast;

            hwnd = GetFirstChild(hwndParent);
            for (; IsWindow(hwnd); hwnd = GetNextSibling(hwnd)) {
               idc = GetWindowID(hwnd);
               if (INRANGE(idcFirst, idc, idcLast)) {

                  // Find the child to be acted upon and set the appropriate
                  // sides to unknown. Width is still known.
                  pChildActOn = Layout_FindChild(pChildList, idc);
                  if (pRule->Action == lHCENTER) {
                     pChildActOn->afMetric[lpLEFT] = UNKNOWN;
                     pChildActOn->afMetric[lpRIGHT] = UNKNOWN;
                  } else {
                     pChildActOn->afMetric[lpTOP] = UNKNOWN;
                     pChildActOn->afMetric[lpBOTTOM] = UNKNOWN;
                  }

                  // Child acted upon is no longer fixed.
                  pChildActOn->fFixed = FALSE;
               }
            }
            break;

         case lFLAGS:             // Should set control flags
            // Find the child to be acted upon
            pChildActOn = Layout_FindChild(pChildList, pRule->ActOn.idc);
            
            pChildActOn->fFlags = pRule->ActOn.nFlags;
            break;
      }
   }
}


///////////////////////////////////////////////////////////////////////////////


BOOL Layout_CheckChild ( CHILD * pChild )
{
     static TCHAR * pszMetric [] = {
          __TEXT("left"),   __TEXT("top"),   __TEXT("right"),
          __TEXT("bottom"), __TEXT("width"), __TEXT("height") };
     
     // Any unknown metric indicates a problem with the rules, so we 'assert'.
     for  ( int i = 0; i < NUMMETRICS; ++i )
     {
          if   ( pChild -> afMetric [ i ] == UNKNOWN )
          {
               return FALSE;
          }
     }
     return TRUE;
}


///////////////////////////////////////////////////////////////////////////////


BOOL Layout_ApplyRule ( HWND hwndParent, RULE * pRules, CHILD * pChildList, RULE * pRule )
{
     CHILD* pChildRelTo, *pChildActOn, ChildRelTo;
     CHILD* pChild, *pChildListNew, *pSrc, *pDest;
     int nRules, nMetric, nChildren;
     int idcFirst, idcLast, nOffset, nCentered;
     RECT rcBounds;
     HWND hwnd, hwndFirst;
     RULE *pr, *prn, *prNew;
     
     // Find the child and part(s) that we are going to act relative to
     pChildRelTo = Layout_FindChild(pChildList, pRule->RelTo.idc);
     
     switch (pRule->RelTo.nPart) {
          
     case lpLEFT:
     case lpTOP:
     case lpRIGHT:
     case lpBOTTOM:
     case lpWIDTH:
     case lpHEIGHT:
          
          // We can't apply a rule relative to a metric that is unknown.
          if (pChildRelTo->afMetric[pRule->RelTo.nMetric] == UNKNOWN)
               return(FALSE);
          break;
          
     case lpGROUP:
          
          // We can't apply a rule relative to a control unless we know its
          // left/top and right/bottom sides (for centering).
          if (pRule->Action == lHCENTER) {
               if ((pChildRelTo->afMetric[lpLEFT] == UNKNOWN) ||
                    (pChildRelTo->afMetric[lpRIGHT] == UNKNOWN))
                    return(FALSE);
          } else {
               if ((pChildRelTo->afMetric[lpTOP] == UNKNOWN) ||
                    (pChildRelTo->afMetric[lpBOTTOM] == UNKNOWN))
                    return(FALSE);
          }
          break;
     }
     
     // Make a local copy of the child we are relative to. We need to do this
     // because we may need to apply a percentage to the width/height metrics
     // and we don't want to modify the actual child list.
     ChildRelTo = *pChildRelTo;
     
     // Use percentage to modify the width/height of the child we are relative to
     if ((pRule->RelTo.nMetric == lpWIDTH) || (pRule->RelTo.nMetric == lpHEIGHT)) {
          ChildRelTo.anMetric[pRule->RelTo.nMetric] *= pRule->RelTo.nPercent;
          ChildRelTo.anMetric[pRule->RelTo.nMetric] /= 100;
          Layout_SolveChild(&ChildRelTo);
     }
     
     // Apply our rule based on the action field
     switch (pRule->Action) {
          
     case lSTRETCH:              // Metric should be stretched
          // Find the child being acted on and stretch the specified metric.
          pChildActOn = Layout_FindChild(pChildList, pRule->ActOn.idc);
          
          pChildActOn->anMetric[pRule->ActOn.nMetric] =
               ChildRelTo.anMetric[pRule->RelTo.nMetric] + pRule->nPixelOffset;
          pChildActOn->afMetric[pRule->ActOn.nMetric] = KNOWN;
          Layout_SolveChild(pChildActOn);
          pRule->fState = APPLIED;
          return TRUE;
          
     case lMOVE:                 // Whole control should be moved
          // Find the child being acted on and move the specified side.
          pChildActOn = Layout_FindChild(pChildList, pRule->ActOn.idc);
          
          pChildActOn->anMetric[pRule->ActOn.nSide] =
               ChildRelTo.anMetric[pRule->RelTo.nMetric] + pRule->nPixelOffset;
          pChildActOn->afMetric[pRule->ActOn.nSide] = KNOWN;
          Layout_SolveChild(pChildActOn);
          pRule->fState = APPLIED;
          return TRUE;
          
     case lVCENTER:              // Vertically center a control/group
     case lHCENTER:              // Horizontally center a control/group
          // First id in group must be less than or equal to the last id
          idcFirst = pRule->ActOn.idcFirst;
          idcLast = pRule->ActOn.idcLast;
          
          // Ensure that the width/height is known for each control in the
          // group before proceeding with any centering.
          hwndFirst = GetFirstChild(hwndParent);
          for (hwnd = hwndFirst; IsWindow(hwnd); hwnd = GetNextSibling(hwnd)) {
               int idc = GetWindowID(hwnd);
               if (INRANGE(idcFirst, idc, idcLast)) {
                    pChildActOn = Layout_FindChild(pChildList, idc);
                    if (pRule->Action == lHCENTER) {
                         if (pChildActOn->afMetric[lpWIDTH] == UNKNOWN)
                              return(FALSE);
                    } else {
                         if (pChildActOn->afMetric[lpHEIGHT] == UNKNOWN)
                              return(FALSE);
                    }
               }
          }
          
          // Create a new list of rules which contains the subset of rules
          // which act on controls in the centered group.
          for (nRules = 0, pr = pRules; pr->Action != lEND; pr++)
               nRules++;
          nRules++;
          prNew = ( RULE * ) _alloca ( nRules * sizeof ( RULE ) );
          prn = prNew;
          for (pr = pRules; pr->Action != lEND; pr++) {
               if (INRANGE(idcFirst, pr->ActOn.idc, idcLast)) {
                    if (pRule->Action == lHCENTER) {
                         if ((pr->ActOn.nPart == lpLEFT) || 
                              (pr->ActOn.nPart == lpRIGHT)) {
                              *prn++ = *pr;
                         }
                    } else {
                         if ((pr->ActOn.nPart == lpTOP) ||
                              (pr->ActOn.nPart == lpBOTTOM)) {
                              *prn++ = *pr;
                         }
                    }
               }
          }
          prn->Action = lEND;
          
          // Make a local copy of the child list and set everything to KNOWN.
          nChildren = 0;
          for (pChild = pChildList; pChild->idc != IDC_LASTCHILD; pChild++)
               nChildren++;
          nChildren++;
          pChildListNew = ( CHILD * ) _alloca(nChildren * sizeof(CHILD));
          MoveMemory(pChildListNew, pChildList, nChildren * sizeof(CHILD));
          for  ( pChild = pChildListNew; pChild -> idc != IDC_LASTCHILD; ++pChild )
               for (nMetric = 0; nMetric < NUMMETRICS; nMetric++)
                    pChild->afMetric[nMetric] = KNOWN;
               
               // Solve for the children being centered as a sub-problem.
               if   ( !Layout_ApplyRules ( hwndParent, prNew, pChildListNew ) )
               {
                    return FALSE;
               }
               
               // Compute the bounding rectangle of the group
               SetRectEmpty(&rcBounds);
               hwndFirst = GetFirstChild(hwndParent);
               for (hwnd = hwndFirst; IsWindow(hwnd); hwnd = GetNextSibling(hwnd)) {
                    int idc = GetWindowID(hwnd);
                    if (INRANGE(idcFirst, idc, idcLast)) {
                         pChildActOn = Layout_FindChild(pChildListNew, idc);
                         UnionRect(&rcBounds, &rcBounds, &pChildActOn->rc);
                    }
               }
               
               // Find the offset required to center the group's bounding rectangle
               // against the control we are relative to.
               if (pRule->Action == lHCENTER) {
                    nCentered = ChildRelTo.anMetric[lpLEFT] +  
                         ((ChildRelTo.anMetric[lpWIDTH] -
                         (rcBounds.right - rcBounds.left)) / 2);
                    nOffset = nCentered - rcBounds.left;
               } else {
                    nCentered = ChildRelTo.anMetric[lpTOP] + 
                         ((ChildRelTo.anMetric[lpHEIGHT] -
                         (rcBounds.bottom - rcBounds.top)) / 2);
                    nOffset = nCentered - rcBounds.top;
               }
               
               // Add in any additional offset from the rule.
               nOffset += pRule->nPixelOffset;
               
               // Go through the new child list, moving each control. 
               for (hwnd = hwndFirst; IsWindow(hwnd); hwnd = GetNextSibling(hwnd)) {
                    int idc = GetWindowID(hwnd);
                    if (INRANGE(idcFirst, idc, idcLast)) { 
                         pChildActOn = Layout_FindChild(pChildListNew, idc);
                         if (pRule->Action == lHCENTER) {
                              pChildActOn->anMetric[lpLEFT]  += nOffset;
                              pChildActOn->anMetric[lpRIGHT] += nOffset;
                         } else {
                              pChildActOn->anMetric[lpTOP]    += nOffset;
                              pChildActOn->anMetric[lpBOTTOM] += nOffset;
                         }
                    }
               }
               
               // Now modify the real child list based on pChildListNew.
               for (pSrc = pChildListNew, pDest = pChildList;
               pSrc->idc != IDC_LASTCHILD; pSrc++, pDest++) {
                    
                    if (INRANGE(idcFirst, pSrc->idc, idcLast)) {
                         if (pRule->Action == lHCENTER) {
                              pDest->anMetric[lpLEFT]  = pSrc->anMetric[lpLEFT];
                              pDest->anMetric[lpRIGHT] = pSrc->anMetric[lpRIGHT];
                              pDest->afMetric[lpLEFT]  = KNOWN;
                              pDest->afMetric[lpRIGHT] = KNOWN;
                         } else {
                              pDest->anMetric[lpTOP]    = pSrc->anMetric[lpTOP];
                              pDest->anMetric[lpBOTTOM] = pSrc->anMetric[lpBOTTOM];
                              pDest->afMetric[lpTOP]    = KNOWN;
                              pDest->afMetric[lpBOTTOM] = KNOWN;
                         }
                    }
               }
               
               pRule->fState = APPLIED;
               return TRUE;
   }
   return FALSE;
}


///////////////////////////////////////////////////////////////////////////////


BOOL Layout_ApplyRules (HWND hwndParent, RULE* pRules, CHILD* pChildList) {   

   RULE* pRule;
   BOOL fAppliedAtLeastOneRule, fOK = TRUE;

   // Based on the list of rules, mark all unknown child metrics as UNKNOWN.
   Layout_MarkUnknowns(hwndParent, pRules, pChildList);

   // Traverse the rule list, converting offsets from dialog units to pixels.
   Layout_ConvertDlgUnits(hwndParent, pRules, pChildList);

   // Mark all the rules as unapplied before attempting to apply them.
   for (pRule = pRules; pRule->Action != lEND; pRule++)
      pRule->fState = UNAPPLIED;

   // Loop through the rule list for as long as we are able to apply at least
   // one rule (if we make a pass through the entire list, and we are unable
   // to apply any rule, we are finished).
   do {
      fAppliedAtLeastOneRule = FALSE;
      for (pRule = pRules; pRule->Action != lEND; pRule++) {
				if (pRule->fState != APPLIED ) {
					if ( pRule->Action != lFLAGS ) {
            if (Layout_ApplyRule(hwndParent, pRules, pChildList, pRule))
               fAppliedAtLeastOneRule = TRUE;
					}
					else {
            fAppliedAtLeastOneRule = TRUE;
						pRule->fState = APPLIED;
					}
        }
      }
   } while (fAppliedAtLeastOneRule);

   // Verify that all rules have been successfully applied.
   for (pRule = pRules; pRule->Action != lEND; pRule++) {
      if (pRule->fState != APPLIED)
         fOK = FALSE;
   }
	 if ( fOK ) {
		 for (pRule = pRules; pRule->Action != lEND; pRule++) {
			 if ( pRule->Action == lFLAGS ) {
				 CHILD* pChild;
				 for (pChild = pChildList + 1; pChild->idc != IDC_LASTCHILD; pChild++) {
					 if ( pChild->idc == pRule->ActOn.idc )	{
						 pChild->fFlags = pRule->ActOn.nFlags;
						 break;
					 }
				 }
			 }
		 }
	 }
   return(fOK);
}

///////////////////////////////////////////////////////////////////////////////


COMMONEXPORT BOOL Layout_ComputeLayout ( HWND hwndParent, RULE * pRules )
{
   HDWP hdwp;
   BOOL fOK = TRUE;
   CHILD* pChild, *pChildList;
   int nChildren;
   
   // Don't do anything to a minimized window.
   if (IsIconic(hwndParent))
      return(TRUE);

   // Enumerate all child windows of the dialog, allocating a CHILD structure
   // for each child, with all six metric flags set to KNOWN. To simplify
   // coding, we also add a special CHILD structure for the parent window with
   // the id lPARENT (defined in layout.h). If there are no children, or
   // memory cannot be allocated for the child list, we do nothing.
   pChildList = Layout_CreateChildList(hwndParent, &nChildren);
   if (pChildList == NULL)
      return(FALSE);
   if (nChildren == 0)
      return(TRUE);

   // Apply the rules from the rule list to solve for the locations of all the
   // child controls.
   if (!Layout_ApplyRules(hwndParent, pRules, pChildList)) {
      return FALSE;
   }

   // Simultaneously relocate all the children using DeferWindowPos.
   hdwp = BeginDeferWindowPos(0);

   // Move each child in the CHILD list.  We enumerate the child list starting
   // at pChildList + 1, because the first CHILD is lPARENT.
   for (pChild = pChildList + 1; pChild->idc != IDC_LASTCHILD; pChild++) {

      // Check child for any still-unsolved metrics. You may want to remove or
      // #ifdef out this check once your rules are known to be working.
      if (!Layout_CheckChild(pChild))
         fOK = FALSE;

      // Add child to DeferWindowPos list if it is not fixed.
      if (!pChild->fFixed) {
         HWND hwndChild = GetDlgItem(hwndParent, pChild->idc);
         hdwp = DeferWindowPos(hdwp, hwndChild, NULL,
            pChild->anMetric[lpLEFT],  pChild->anMetric[lpTOP], 
            pChild->anMetric[lpWIDTH], pChild->anMetric[lpHEIGHT],
            SWP_NOZORDER);

         if ( !(pChild->fFlags & cfNOTINVALIDATE) )
         {
              // Invalidation is necessary here because some controls (edit
              // controls in particular) don't repaint correctly under Windows NT
              // when they are moved with DeferWindowPos.
              InvalidateRect(hwndChild, NULL, TRUE);
         }
      }
   }

   // It is this function call which actually moves all the windows.
   EndDeferWindowPos(hdwp);

   // Free the allocated list of CHILD structures
   free(pChildList);
   return(fOK);
}


///////////////////////////////// End of File /////////////////////////////////
