#include "StdAfx.h"
#include "LevelToolWindow.h"

namespace GUI
{
	const UINT g_itemIDStart = 3001;

	const UINT g_trackbarWidth		= 111;
	const UINT g_trackbarHeight		= 30;
	const UINT g_trackbarTextHeight = 12;
	const UINT g_margin				= 5;

	LevelToolWindow* g_levelToolWindow;

	LRESULT CALLBACK levelToolWindowMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_levelToolWindow->subWndProc(hWnd, message, wParam, lParam);
	}

	LevelToolWindow::LevelToolWindow(void)
	{
		g_levelToolWindow = this;
	}

	void LevelToolWindow::init(HINSTANCE hInstance, HWND hParentWnd,
							   const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   g_trackbarWidth + 2 * g_margin, g_margin);

		SetWindowSubclass(m_hWnd, levelToolWindowMsgRouter, 0, 0);
	}

	void LevelToolWindow::addTrackbar(const std::string& name, EventReceiver* eventReceiver,
									  const UINT minValue, const UINT maxValue, const UINT startValue)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT width  = g_trackbarWidth;
		const UINT height = g_trackbarHeight;
		const UINT x	  = g_margin;
			  UINT y	  = count * (height + g_margin + g_trackbarTextHeight) + g_margin;
		const UINT id	  = g_itemIDStart + count;

		HWND hText = CreateWindow("STATIC", name.c_str(), WS_VISIBLE | WS_CHILD,
								  x, y, width, g_trackbarTextHeight,
								  m_hWnd, NULL, m_hInstance, NULL);

		HFONT hFont = CreateFont(g_trackbarTextHeight, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);

		y += g_trackbarTextHeight;

		HWND hTrackbar = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_TOOLTIPS,
									  x, y, width, height, m_hWnd, (HMENU)id, m_hInstance, NULL);

		SendMessage(hTrackbar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(minValue, maxValue));
		SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)startValue);

		m_items.push_back(EventElement(name, eventReceiver));

		m_clientHeight += height + g_trackbarTextHeight + g_margin;

		const POINT windowSize	   = getWindowSize();
		const POINT windowPosition = getWindowPosition();
		SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
	}

	UINT LevelToolWindow::getTrackbarValue(const std::string& itemName) const
	{
		const int id = getItemID(itemName);
		if (id != -1)
			return (UINT)SendMessage(GetDlgItem(m_hWnd, id), TBM_GETPOS, NULL, NULL);
		return 0;
	}

	int LevelToolWindow::getItemID(const UINT i) const
	{
		return (i < (UINT)m_items.size()) ? g_itemIDStart + i : -1;
	}

	int LevelToolWindow::getItemID(const std::string& itemName) const
	{
		for (UINT i = 0; i < (UINT)m_items.size(); i++)
			if (m_items[i].first == itemName)
				return g_itemIDStart + i;
		return -1;
	}
}