//==============================================================================
// PRODUCT: PFWGUI
//==============================================================================
// AUTHOR:  Timur I. Amirkhanov
//==============================================================================
#ifndef __PFWGUI_MEMDC_H
#define __PFWGUI_MEMDC_H

//******************************************************************************
// class CCMemDC
//******************************************************************************
class CCMemDC : public CDC
{
public:
     
     // constructor sets up the memory DC
     CCMemDC ( CDC * pDC, LPRECT lpRect = NULL)
     {
          m_pDC = pDC;
          m_pOldBitmap = NULL;
          m_bMemDC = !pDC->IsPrinting ();
          
          if   ( m_bMemDC )
          {
			  if (lpRect == NULL)
					pDC -> GetClipBox ( &m_rect );
			  else
				  m_rect = *lpRect;
			  
               CreateCompatibleDC ( pDC );
               m_bitmap.CreateCompatibleBitmap ( pDC, m_rect.Width (), m_rect.Height () );
               m_pOldBitmap = SelectObject ( &m_bitmap );
               SetWindowOrg ( m_rect.left, m_rect.top );
          }
          else
          {
               m_bPrinting = pDC -> m_bPrinting;
               m_hDC       = pDC -> m_hDC;
               m_hAttribDC = pDC -> m_hAttribDC;
          }
     }
     
     // Destructor copies the contents of the mem DC to the original DC
     ~CCMemDC ()
     {
          if   ( m_bMemDC )
          {    
               // Copy the offscreen bitmap onto the screen.
               m_pDC -> BitBlt ( m_rect.left, m_rect.top, m_rect.Width (), m_rect.Height (),
                    this, m_rect.left, m_rect.top, SRCCOPY );
               
               // Swap back the original bitmap.
               SelectObject ( m_pOldBitmap );
          }
          else
          {
               // All we need to do is replace the DC with an illegal value,
               // this keeps us from accidently deleting the handles associated with
               // the CDC that was passed to the constructor.
               m_hDC = m_hAttribDC = NULL;
          }
     }
     
     // Allow usage as a pointer
     CCMemDC * operator -> ()
     {
          return this;
     }
     
     // Allow usage as a pointer
     operator CCMemDC * ()
     {
          return this;
     }
     
//private:
     CBitmap   m_bitmap;      // Offscreen bitmap
     CBitmap * m_pOldBitmap;  // bitmap originally found in CCMemDC
     CDC*      m_pDC;         // Saves CDC passed in constructor
     CRect     m_rect;        // Rectangle of drawing area.
     BOOL      m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

#endif // __PFWGUI_MEMDC_H

//==============================================================================
