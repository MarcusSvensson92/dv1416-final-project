#include "StdAfx.h"
#include "LevelToolWindow.h"

namespace GUI
{
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
				   m_trackbarSize.x + 2 * m_trackbarMargin, m_trackbarMargin,
				   3001);

		SetWindowSubclass(m_hWnd, levelToolWindowMsgRouter, 0, 0);
	}
}