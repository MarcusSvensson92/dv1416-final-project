#include "StdAfx.h"
#include "SelectionOptions.h"

namespace GUI
{
	SelectionOptions* g_selectionOptions;

	LRESULT CALLBACK selectionOptionsMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_selectionOptions->subWndProc(hWnd, message, wParam, lParam);
	}

	SelectionOptions::SelectionOptions(void)
	{
		g_selectionOptions = this;
	}

	void SelectionOptions::init(HINSTANCE hInstance, HWND hParentWnd,
							    const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   m_trackbarSize.x + 2 * m_trackbarMargin, m_trackbarMargin,
				   6001);

		SetWindowSubclass(m_hWnd, selectionOptionsMsgRouter, 0, 0);
	}
}