#include "StdAfx.h"
#include "Toolbar.h"

namespace GUI
{
	Toolbar* g_toolbar;

	LRESULT CALLBACK toolbarMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_toolbar->subWndProc(hWnd, message, wParam, lParam);
	}

	Toolbar::Toolbar(void)
	{
		m_buttonsPerRow = 2;
		m_buttonSize	= 32;
		m_buttonMargin	= 4;

		g_toolbar = this;
	}

	void Toolbar::init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CHILD,
				   m_buttonsPerRow * (m_buttonSize + m_buttonMargin) + m_buttonMargin, m_buttonMargin,
				   2001);

		SetWindowSubclass(m_hWnd, toolbarMsgRouter, 0, 0);
	}

	void Toolbar::addButton(const std::string& name, EventReceiver* eventReceiver, const std::string& bitmapFilename)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT x	  = (count % m_buttonsPerRow) * (m_buttonSize + m_buttonMargin) + m_buttonMargin;
		const UINT y	  = (count / m_buttonsPerRow) * (m_buttonSize + m_buttonMargin) + m_buttonMargin;
		const UINT id	  = m_itemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_PUSHLIKE,
									x, y, m_buttonSize, m_buttonSize, m_hWnd, (HMENU)id, m_hInstance, NULL);

		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, bitmapFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

		m_items.push_back(EventElement(name, eventReceiver));

		if (y >= m_clientHeight)
		{
			const UINT rowCount = count / m_buttonsPerRow + 1;
			m_clientHeight = rowCount * (m_buttonSize + m_buttonMargin) + m_buttonMargin;

			const POINT windowSize	   = getWindowSize();
			const POINT windowPosition = getWindowPosition();
			SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
		}

		if (count <= 0)
			Button_SetState(GetDlgItem(m_hWnd, id), true);
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

	void Toolbar::resetButtonHighlights(void)
	{
		for (UINT i = 0; i < (UINT)m_items.size(); i++)
			Button_SetState(GetDlgItem(m_hWnd, getItemID(i)), false);
	}
}