#include "StdAfx.h"
#include "TextureToolWindow.h"

namespace GUI
{
	TextureToolWindow* g_textureToolWindow;

	LRESULT CALLBACK textureToolWindowMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_textureToolWindow->subWndProc(hWnd, message, wParam, lParam);
	}

	TextureToolWindow::TextureToolWindow(void)
	{
		m_textureButtonsPerRow   = 4;
		m_textureButtonSize	     = 32;
		m_loadFileButtonHeight	 = 30;
		m_loadFileButtonTextSize = 14;
		m_trackbarHeight		 = 30;
		m_trackbarTextSize		 = 12;
		m_itemMargin			 = 5;

		g_textureToolWindow = this;
	}

	void TextureToolWindow::init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   m_textureButtonsPerRow * (m_textureButtonSize + m_itemMargin) + m_itemMargin, m_itemMargin,
				   4001);

		SetWindowSubclass(m_hWnd, textureToolWindowMsgRouter, 0, 0);

		m_trackbarCount = 0;
	}

	LRESULT TextureToolWindow::subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_COMMAND)
		{
			const UINT id = LOWORD(wParam);
			for (std::vector<UINT>::iterator it = m_checkableButtonIDs.begin(); it != m_checkableButtonIDs.end(); it++)
			{
				if ((*it) == id)
				{
					resetButtonHighlights();
					Button_SetState(GetDlgItem(m_hWnd, (*it)), true);
				}
			}
		}
		
		return Subwindow::subWndProc(hWnd, message, wParam, lParam);
	}

	void TextureToolWindow::addTextureButton(const std::string& name, EventReceiver* eventReceiver,
											 const std::string& bitmapFilename)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT x	  = (count % m_textureButtonsPerRow) * (m_textureButtonSize + m_itemMargin) + m_itemMargin;
		const UINT y	  = m_itemMargin;
		const UINT id	  = m_itemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_PUSHLIKE,
									x, y, m_textureButtonSize, m_textureButtonSize,
									m_hWnd, (HMENU)id, m_hInstance, NULL);

		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, bitmapFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

		m_items.push_back(EventElement(name, eventReceiver));
		m_checkableButtonIDs.push_back(id);

		if (y >= m_clientHeight)
		{
			m_clientHeight = m_textureButtonSize + 2 * m_itemMargin;

			const POINT windowSize	   = getWindowSize();
			const POINT windowPosition = getWindowPosition();
			SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
		}

		if (count <= 0)
			Button_SetState(GetDlgItem(m_hWnd, id), true);
	}

	void TextureToolWindow::addLoadFileButton(const std::string& name, EventReceiver* eventReceiver)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT width  = m_clientWidth - 2 * m_itemMargin;
		const UINT x	  = m_itemMargin;
		const UINT y	  = m_textureButtonSize + 2 * m_itemMargin;
		const UINT id	  = m_itemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", name.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
									x, y, width, m_loadFileButtonHeight, m_hWnd, (HMENU)id, m_hInstance, NULL);

		HFONT hFont = CreateFont(m_loadFileButtonTextSize, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

		m_items.push_back(EventElement(name, eventReceiver));

		if (y >= m_clientHeight)
		{
			m_clientHeight = m_textureButtonSize + m_loadFileButtonHeight + 3 * m_itemMargin;

			const POINT windowSize	   = getWindowSize();
			const POINT windowPosition = getWindowPosition();
			SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
		}
	}

	void TextureToolWindow::addTrackbar(const std::string& name, EventReceiver* eventReceiver,
										const UINT minValue, const UINT maxValue, const UINT startValue)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT width  = m_clientWidth - 2 * m_itemMargin;
		const UINT x	  = m_itemMargin;
			  UINT y	  = m_textureButtonSize + m_loadFileButtonHeight + m_trackbarCount * (m_trackbarHeight + m_itemMargin + m_trackbarTextSize) + 4 * m_itemMargin;
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
									  x, y, width, m_trackbarHeight, m_hWnd, (HMENU)id, m_hInstance, NULL);

		SendMessage(hTrackbar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(minValue, maxValue));
		SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)startValue);

		m_items.push_back(EventElement(name, eventReceiver));

		m_trackbarCount++;

		m_clientHeight = m_textureButtonSize + m_loadFileButtonHeight + m_trackbarCount * (m_trackbarHeight + m_itemMargin + m_trackbarTextSize) + 4 * m_itemMargin;

		const POINT windowSize	   = getWindowSize();
		const POINT windowPosition = getWindowPosition();
		SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
	}

	UINT TextureToolWindow::getTrackbarValue(const std::string& itemName) const
	{
		const int id = getItemID(itemName);
		if (id != -1)
			return (UINT)SendMessage(GetDlgItem(m_hWnd, id), TBM_GETPOS, NULL, NULL);
		return 0;
	}

	void TextureToolWindow::resetButtonHighlights(void)
	{
		for (std::vector<UINT>::iterator it = m_checkableButtonIDs.begin(); it != m_checkableButtonIDs.end(); it++)
			Button_SetState(GetDlgItem(m_hWnd, (*it)), false);
	}
}