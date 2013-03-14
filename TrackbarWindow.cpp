#include "StdAfx.h"
#include "TrackbarWindow.h"

namespace GUI
{
	TrackbarWindow::TrackbarWindow(void)
	{
		m_trackbarSize.x   = 143;
		m_trackbarSize.y   = 30;
		m_trackbarTextSize = 12;
		m_trackbarMargin   = 5;
	}

	TrackbarWindow::~TrackbarWindow(void) { }

	void TrackbarWindow::addTrackbar(const std::string& name, EventReceiver* eventReceiver,
									 const UINT minValue, const UINT maxValue, const UINT startValue)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT width  = m_trackbarSize.x;
		const UINT height = m_trackbarSize.y;
		const UINT x	  = m_trackbarMargin;
			  UINT y	  = count * (height + m_trackbarMargin + m_trackbarTextSize) + m_trackbarMargin;
		const UINT id	  = m_itemIDStart + count;

		HWND hText = CreateWindow("STATIC", name.c_str(), WS_VISIBLE | WS_CHILD,
									x, y, width, m_trackbarTextSize,
									m_hWnd, NULL, m_hInstance, NULL);

		HFONT hFont = CreateFont(m_trackbarTextSize, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);

		y += m_trackbarTextSize;

		HWND hTrackbar = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_TOOLTIPS,
									  x, y, width, height, m_hWnd, (HMENU)id, m_hInstance, NULL);

		SendMessage(hTrackbar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(minValue, maxValue));
		SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)startValue);

		m_items.push_back(EventElement(name, eventReceiver));

		m_clientHeight += height + m_trackbarTextSize + m_trackbarMargin;

		const POINT windowSize	   = getWindowSize();
		const POINT windowPosition = getWindowPosition();
		SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
	}

	UINT TrackbarWindow::getTrackbarValue(const std::string& itemName) const
	{
		const int id = getItemID(itemName);
		if (id != -1)
			return (UINT)SendMessage(GetDlgItem(m_hWnd, id), TBM_GETPOS, NULL, NULL);
		return 0;
	}
}