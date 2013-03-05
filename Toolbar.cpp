#include "StdAfx.h"
#include "Toolbar.h"

namespace GUI
{
	const UINT g_toolbarItemIDStart = 2001;

	Toolbar* g_toolbar;

	LRESULT CALLBACK toolbarMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_toolbar->subWndProc(hWnd, message, wParam, lParam);
	}

	Toolbar::Toolbar(void)
	{
		g_toolbar = this;
	}

	void Toolbar::init(HINSTANCE hInstance, HWND hParentWnd, const ToolbarDesc toolbarDesc)
	{
		m_toolbarDesc = toolbarDesc;

		initWindow(hInstance, hParentWnd, toolbarDesc.caption, WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   toolbarDesc.x, toolbarDesc.y, toolbarDesc.width(), toolbarDesc.height(0));

		SetWindowSubclass(m_hWnd, toolbarMsgRouter, 0, 0);
	}

	void Toolbar::addButton(const std::string& name, EventReceiver* eventReceiver, const std::string& bitmapFilename)
	{
		const UINT count = (UINT)m_items.size();
		const UINT x	 = m_toolbarDesc.buttonX(count);
		const UINT y	 = m_toolbarDesc.buttonY(count);
		const UINT size  = m_toolbarDesc.buttonSize;
		const UINT id	 = g_toolbarItemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_PUSHLIKE,
									x, y, size, size, m_hWnd, (HMENU)id, m_hInstance, NULL);

		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, bitmapFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

		if (y >= m_clientHeight)
		{
			const UINT rowCount = count / m_toolbarDesc.buttonsPerRow + 1;
			m_clientHeight = m_toolbarDesc.height(rowCount);

			const POINT wndSize = computeWindowSize();

			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			const UINT x = rect.left;
			const UINT y = rect.top;

			SetWindowPos(m_hWnd, HWND_TOP, x, y, wndSize.x, wndSize.y, NULL);
		}

		m_items.push_back(EventElement(name, eventReceiver));
	}

	LRESULT Toolbar::subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_COMMAND)
		{
			const UINT id = LOWORD(wParam);
			for (UINT i = 0; i < (UINT)m_items.size(); i++)
			{
				const UINT itemID = getItemID(i);
				if (itemID == id)
				{
					resetButtonHighlights();
					Button_SetState(GetDlgItem(m_hWnd, itemID), true);
				}
			}
		}

		return Subwindow::subWndProc(hWnd, message, wParam, lParam);
	}

	int Toolbar::getItemID(const UINT i) const
	{
		return (i < (UINT)m_items.size()) ? g_toolbarItemIDStart + i : -1;
	}

	void Toolbar::resetButtonHighlights(void)
	{
		for (UINT i = 0; i < (UINT)m_items.size(); i++)
			Button_SetState(GetDlgItem(m_hWnd, getItemID(i)), false);
	}
}