#ifndef _MEMDC_H_   
#define _MEMDC_H_   
    
//////////////////////////////////////////////////   
// CMemDC - memory DC   
//   
// Author: Keith Rule   
// Email:     
// Copyright 1996-2002, Keith Rule   
//   
// You may freely use or modify this code provided this   
// Copyright is included in all derived versions.   
//   
// History - 10/3/97 Fixed scrolling bug.   
//               Added print support. - KR   
//   
//       11/3/99 Fixed most common complaint. Added   
//            background color fill. - KR   
//   
//       11/3/99 Added support for mapping modes other than   
//            MM_TEXT as suggested by Lee Sang Hun. - KR   
//   
//       02/11/02 Added support for CScrollView as supplied   
//             by Gary Kirkham. - KR   
//   
// This class implements a memory Device Context which allows   
// flicker free drawing.   
    
class CMemDC_GDI : public CDC {   
private:          
    CBitmap    m_bitmap;        // Offscreen bitmap   
    CBitmap*       m_oldBitmap; // bitmap originally found in CMemDC   
    CDC*       m_pDC;           // Saves CDC passed in constructor   
    CRect      m_rect;          // Rectangle of drawing area.   
    BOOL       m_bMemDC;        // TRUE if CDC really is a Memory DC.   
public:   
       
	int GetWidth()
	{
		return m_rect.Width();
	}
	int GetHeight()
	{
		return m_rect.Height();
	}


    CMemDC_GDI(CDC* pDCGDI, const CRect* pRect = NULL) : CDC()   
    {   
        ASSERT(pDCGDI != NULL);    
    
        // Some initialization   
        m_pDC = pDCGDI;   
        m_oldBitmap = NULL;   
        m_bMemDC = !pDCGDI->IsPrinting();   
    
        // Get the rectangle to draw   
        if (pRect == NULL) {   
             pDCGDI->GetClipBox(&m_rect);   
        } else {   
             m_rect = *pRect;   
        }   
    
        if (m_bMemDC) {   
             // Create a Memory DC   
             CreateCompatibleDC(pDCGDI);   
             pDCGDI->LPtoDP(&m_rect);   
    
             m_bitmap.CreateCompatibleBitmap(pDCGDI, m_rect.Width(),    
                                                  m_rect.Height());   
             m_oldBitmap = SelectObject(&m_bitmap);   
    
             SetMapMode(pDCGDI->GetMapMode());   
    
             SetWindowExt(pDCGDI->GetWindowExt());   
             SetViewportExt(pDCGDI->GetViewportExt());   
    
             pDCGDI->DPtoLP(&m_rect);   
             SetWindowOrg(m_rect.left, m_rect.top);   
        } else {   
             // Make a copy of the relevent parts of the current    
             // DC for printing   
             m_bPrinting = pDCGDI->m_bPrinting;   
             m_hDC       = pDCGDI->m_hDC;   
             m_hAttribDC = pDCGDI->m_hAttribDC;   
        }   
    
        // Fill background    
        FillSolidRect(m_rect, pDCGDI->GetBkColor());   
    }   
       
    ~CMemDC_GDI()         
    {             
        if (m_bMemDC) {   
             // Copy the offscreen bitmap onto the screen.   
             m_pDC->BitBlt(m_rect.left, m_rect.top,    
                           m_rect.Width(),  m_rect.Height(),   
                  this, m_rect.left, m_rect.top, SRCCOPY);               
                
             //Swap back the original bitmap.   
             SelectObject(m_oldBitmap);           
        } else {   
             // All we need to do is replace the DC with an illegal   
             // value, this keeps us from accidentally deleting the    
             // handles associated with the CDC that was passed to    
             // the constructor.                 
             m_hDC = m_hAttribDC = NULL;   
        }          
    }   
       
    // Allow usage as a pointer       
    CMemDC_GDI* operator->()    
    {   
        return this;   
    }          
    
    // Allow usage as a pointer       
    operator CMemDC_GDI*()    
    {   
        return this;   
    }   
};   
    
#endif 