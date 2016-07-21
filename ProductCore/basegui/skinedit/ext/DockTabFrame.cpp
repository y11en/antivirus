#include "stdafx.h"
#include "DockTabFrame.h"

namespace DockSplitTab
{
	int Frame::DockPane::instanceCount = 0;
	CBrush Frame::DockPane::systemFrameBrush;
	CBrush Frame::DockPane::activeCaptionBrush;
	CBrush Frame::DockPane::inactiveCaptionBrush;
	CBrush Frame::DockPane::activeBorderBrush;
	CBrush Frame::DockPane::inactiveBorderBrush;
			
	COLORREF Frame::DockPane::activeCaptionTextColor;
	COLORREF Frame::DockPane::inactiveCaptionTextColor;
			
	int   Frame::DockPane::systemCxFrame;
	int   Frame::DockPane::systemCyFrame;
	int   Frame::DockPane::systemCaptionHeight;
	CFont Frame::DockPane::systemCaptionFont;
			
	HCURSOR Frame::DockPane::verCursor;
	HCURSOR Frame::DockPane::horCursor;
	
}; // namespace DockSplitTab
