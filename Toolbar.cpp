#include "StdAfx.h"
#include "Toolbar.h"

const UINT g_toolbarButtonIDStart = 2001;

GUI::Toolbar* g_toolbar;

LRESULT CALLBACK toolbarMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
{
	return g_toolbar->subWndProc(hWnd, message, wParam, lParam);
}

BOOL CALLBACK toolbarMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return g_toolbar->subWndProc(hWnd, message, wParam, lParam);
}

namespace GUI
{
	Toolbar::Toolbar(HINSTANCE	 hInstance,
					 HWND		 hParentWnd,
					 ToolbarDesc toolbarDesc)
	{
		m_hInstance	   = hInstance;
		m_toolbarDesc  = toolbarDesc;
		m_style		   = WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD;
		m_clientWidth  = toolbarDesc.width();
		m_clientHeight = toolbarDesc.height(0);

		const POINT wndSize = computeWndSize();

		CLIENTCREATESTRUCT ccs;
		ZeroMemory(&ccs, sizeof(ccs));

		m_hWnd = CreateWindow("MDICLIENT", m_toolbarDesc.caption.c_str(), m_style,
							  toolbarDesc.x, toolbarDesc.y, wndSize.x, wndSize.y,
							  hParentWnd, NULL, m_hInstance, (LPSTR)&ccs);

		g_toolbar = this;

		SetWindowSubclass(m_hWnd, toolbarMsgRouter, 0, 0);
	}

	Toolbar::~Toolbar(void) { }

	void Toolbar::addButton(const std::string& name, EventReceiver* eventReceiver, const std::string& bitmapFilename)
	{
		const UINT count = (UINT)m_buttons.size();
		const UINT x	 = m_toolbarDesc.buttonX(count);
		const UINT y	 = m_toolbarDesc.buttonY(count);
		const UINT size  = m_toolbarDesc.buttonSize;
		const UINT id	 = g_toolbarButtonIDStart + count;

		HWND hButton = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_PUSHLIKE,
									x, y, size, size, m_hWnd, (HMENU)id, m_hInstance, NULL);

		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, bitmapFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

		if (y >= m_clientHeight)
		{
			const UINT rowCount = (count + 2) / m_toolbarDesc.buttonsPerRow;
			m_clientHeight = m_toolbarDesc.height(rowCount);

			const POINT wndSize = computeWndSize();

			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			const UINT x = rect.left;
			const UINT y = rect.top;

			SetWindowPos(m_hWnd, HWND_TOP, x, y, wndSize.x, wndSize.y, NULL);
		}

		m_buttons.push_back(EventElement(name, eventReceiver));
	}

	void Toolbar::hide(const bool hide)
	{
		int cmdShow = (hide) ? SW_HIDE : SW_SHOW;
		ShowWindow(m_hWnd, cmdShow);
	}

	LRESULT Toolbar::subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_COMMAND)
		{
			const UINT id = LOWORD(wParam);
			for (UINT i = 0; i < (UINT)m_buttons.size(); i++)
			{
				const UINT buttonID = g_toolbarButtonIDStart + i;
				if (buttonID == id)
				{
					resetButtonHighlights();
					Button_SetState(GetDlgItem(m_hWnd, buttonID), true);

					m_buttons[i].second->onEvent("Toolbar", m_buttons[i].first);
				}
			}
		}

		return DefSubclassProc(hWnd, message, wParam, lParam);
	}

	POINT Toolbar::computeWndSize(void) const
	{
		RECT rect = {0, 0, m_clientWidth, m_clientHeight};
		AdjustWindowRect(&rect, m_style, false);
		POINT wndSize;
		wndSize.x = rect.right  - rect.left;
		wndSize.y = rect.bottom - rect.top;
		return wndSize;
	}

	void Toolbar::resetButtonHighlights(void)
	{
		for (UINT i = 0; i < (UINT)m_buttons.size(); i++)
			Button_SetState(GetDlgItem(m_hWnd, g_toolbarButtonIDStart + i), false);
	}
}