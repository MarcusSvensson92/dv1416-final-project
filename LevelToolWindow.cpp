#include "StdAfx.h"
#include "LevelToolWindow.h"

namespace GUI
{
	const UINT g_levelToolWindowItemIDStart = 3001;

	LevelToolWindow* g_levelToolWindow;

	LRESULT CALLBACK levelToolWindowMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_levelToolWindow->subWndProc(hWnd, message, wParam, lParam);
	}

	LevelToolWindow::LevelToolWindow(void)
	{
		g_levelToolWindow = this;
	}

	void LevelToolWindow::init(HINSTANCE hInstance,HWND hParentWnd,
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
		const UINT height = m_levelToolWindowDesc.trackbarHeight;
		const UINT x	  = margin;
		const UINT y	  = count * (height + margin) + margin;
		const UINT id	  = g_levelToolWindowItemIDStart + count;

		HWND hTrackbar = CreateWindow(TRACKBAR_CLASS, m_levelToolWindowDesc.caption.c_str(),
									  WS_CHILD | WS_VISIBLE | TBS_TOOLTIPS,
									  x, y, width, height, m_hWnd, (HMENU)id, m_hInstance, NULL);

		SendMessage(hTrackbar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(minValue, maxValue));
		SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)startValue);

		m_items.push_back(EventElement(name, eventReceiver));
	}

	int LevelToolWindow::getItemID(const UINT i) const
	{
		return (i < (UINT)m_items.size()) ? g_levelToolWindowItemIDStart + i : -1;
	}
}