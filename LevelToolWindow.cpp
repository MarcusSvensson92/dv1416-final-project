#include "StdAfx.h"
#include "LevelToolWindow.h"

namespace GUI
{
	const UINT g_levelToolWindowItemIDStart = 3001;

	const UINT g_trackbarHeight		= 30;
	const UINT g_trackbarTextHeight = 12;

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
							   const LevelToolWindowDesc levelToolWindowDesc)
	{
		m_levelToolWindowDesc = levelToolWindowDesc;

		initWindow(hInstance, hParentWnd, levelToolWindowDesc.caption,
				   WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   levelToolWindowDesc.x, levelToolWindowDesc.y,
				   levelToolWindowDesc.width, levelToolWindowDesc.height);

		SetWindowSubclass(m_hWnd, levelToolWindowMsgRouter, 0, 0);
	}

	void LevelToolWindow::addTrackbar(const std::string& name, EventReceiver* eventReceiver,
									  const UINT minValue, const UINT maxValue, const UINT startValue)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT margin = m_levelToolWindowDesc.margin;
		const UINT width  = m_levelToolWindowDesc.width - 2 * margin;
		const UINT height = g_trackbarHeight;
		const UINT x	  = margin;
			  UINT y	  = count * (height + margin + g_trackbarTextHeight) + margin;
		const UINT id	  = g_levelToolWindowItemIDStart + count;

		HWND hText = CreateWindow("STATIC", name.c_str(), WS_VISIBLE | WS_CHILD,
								  x, y, width, g_trackbarTextHeight,
								  m_hWnd, NULL, m_hInstance, NULL);

		HFONT hFont = CreateFont(g_trackbarTextHeight, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);

		y += g_trackbarTextHeight;

		HWND hTrackbar = CreateWindow(TRACKBAR_CLASS, m_levelToolWindowDesc.caption.c_str(),
									  WS_CHILD | WS_VISIBLE | TBS_TOOLTIPS,
									  x, y, width, height, m_hWnd, (HMENU)id, m_hInstance, NULL);

		SendMessage(hTrackbar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(minValue, maxValue));
		SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)startValue);

		m_items.push_back(EventElement(name, eventReceiver));
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
		return (i < (UINT)m_items.size()) ? g_levelToolWindowItemIDStart + i : -1;
	}

	int LevelToolWindow::getItemID(const std::string& itemName) const
	{
		for (UINT i = 0; i < (UINT)m_items.size(); i++)
			if (m_items[i].first == itemName)
				return g_levelToolWindowItemIDStart + i;
		return -1;
	}
}