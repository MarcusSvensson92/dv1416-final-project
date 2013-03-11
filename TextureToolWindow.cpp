#include "StdAfx.h"
#include "TextureToolWindow.h"

namespace GUI
{
	const UINT g_itemIDStart = 4001;

	const UINT g_textureButtonsPerRow	  = 4;
	const UINT g_textureButtonSize		  = 32;
	const UINT g_loadFileButtonHeight	  = 30;
	const UINT g_loadFileButtonTextHeight = 14;
	const UINT g_trackbarHeight			  = 30;
	const UINT g_trackbarTextHeight		  = 12;
	const UINT g_margin					  = 5;

	TextureToolWindow* g_textureToolWindow;

	LRESULT CALLBACK textureToolWindowMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_textureToolWindow->subWndProc(hWnd, message, wParam, lParam);
	}

	TextureToolWindow::TextureToolWindow(void)
	{
		g_textureToolWindow = this;
	}

	void TextureToolWindow::init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   g_textureButtonsPerRow * (g_textureButtonSize + g_margin) + g_margin, g_margin);

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
		const UINT x	  = (count % g_textureButtonsPerRow) * (g_textureButtonSize + g_margin) + g_margin;
		const UINT y	  = g_margin;
		const UINT id	  = g_itemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_PUSHLIKE,
									x, y, g_textureButtonSize, g_textureButtonSize,
									m_hWnd, (HMENU)id, m_hInstance, NULL);

		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, bitmapFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

		m_items.push_back(EventElement(name, eventReceiver));
		m_checkableButtonIDs.push_back(id);

		if (y >= m_clientHeight)
		{
			m_clientHeight = g_textureButtonSize + 2 * g_margin;

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
		const UINT width  = m_clientWidth - 2 * g_margin;
		const UINT x	  = g_margin;
		const UINT y	  = g_textureButtonSize + 2 * g_margin;
		const UINT id	  = g_itemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", name.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
									x, y, width, g_loadFileButtonHeight, m_hWnd, (HMENU)id, m_hInstance, NULL);

		HFONT hFont = CreateFont(g_loadFileButtonTextHeight, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

		m_items.push_back(EventElement(name, eventReceiver));

		if (y >= m_clientHeight)
		{
			m_clientHeight = g_textureButtonSize + g_loadFileButtonHeight + 3 * g_margin;

			const POINT windowSize	   = getWindowSize();
			const POINT windowPosition = getWindowPosition();
			SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
		}
	}

	void TextureToolWindow::addTrackbar(const std::string& name, EventReceiver* eventReceiver,
										const UINT minValue, const UINT maxValue, const UINT startValue)
	{
		const UINT count  = (UINT)m_items.size();
		const UINT width  = m_clientWidth - 2 * g_margin;
		const UINT x	  = g_margin;
			  UINT y	  = g_textureButtonSize + g_loadFileButtonHeight + m_trackbarCount * (g_trackbarHeight + g_margin + g_trackbarTextHeight) + 4 * g_margin;
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
									  x, y, width, g_trackbarHeight, m_hWnd, (HMENU)id, m_hInstance, NULL);

		SendMessage(hTrackbar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(minValue, maxValue));
		SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)startValue);

		m_items.push_back(EventElement(name, eventReceiver));

		m_trackbarCount++;

		m_clientHeight = g_textureButtonSize + g_loadFileButtonHeight + m_trackbarCount * (g_trackbarHeight + g_margin + g_trackbarTextHeight) + 4 * g_margin;

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

	int TextureToolWindow::getItemID(const UINT i) const
	{
		return (i < (UINT)m_items.size()) ? g_itemIDStart + i : -1;
	}

	int TextureToolWindow::getItemID(const std::string& itemName) const
	{
		for (UINT i = 0; i < (UINT)m_items.size(); i++)
			if (m_items[i].first == itemName)
				return g_itemIDStart + i;
		return -1;
	}

	void TextureToolWindow::resetButtonHighlights(void)
	{
		for (std::vector<UINT>::iterator it = m_checkableButtonIDs.begin(); it != m_checkableButtonIDs.end(); it++)
			Button_SetState(GetDlgItem(m_hWnd, (*it)), false);
	}
}