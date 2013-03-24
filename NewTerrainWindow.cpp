#include "StdAfx.h"
#include "NewTerrainWindow.h"

namespace GUI
{
	NewTerrainWindow* g_newTerrainWindow;

	LRESULT CALLBACK newTerrainWindowMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_newTerrainWindow->subWndProc(hWnd, message, wParam, lParam);
	}

	NewTerrainWindow::NewTerrainWindow(void)
	{
		m_itemMargin		   = 5;
		m_dropdownListWidth    = 70;
		m_dropdownListHeight   = 20;
		m_dropdownListTextSize = 12;
		m_buttonHeight		   = 24;
		m_buttonTextSize	   = 14;

		g_newTerrainWindow = this;
	}

	void NewTerrainWindow::init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CHILD,
				   2 * (m_dropdownListWidth + m_itemMargin) + m_itemMargin, m_itemMargin,
				   7001);

		SetWindowSubclass(m_hWnd, newTerrainWindowMsgRouter, 0, 0);
	}

	void NewTerrainWindow::addDropdownListPair(const std::string& name, std::vector<UINT> listData)
	{
		const UINT count			  = (UINT)m_items.size();
		const UINT width			  = 2 * m_dropdownListWidth + m_itemMargin;
			  UINT x				  = m_itemMargin;
			  UINT y				  = (count / 2) * (m_dropdownListTextSize + m_dropdownListHeight + m_itemMargin) + m_itemMargin;
		const UINT id				  = m_itemIDStart + count;
		const UINT dropdownListHeight = 200;

		HWND hText = CreateWindow("STATIC", name.c_str(), WS_VISIBLE | WS_CHILD,
								  x, y, width, m_dropdownListTextSize,
								  m_hWnd, NULL, m_hInstance, NULL);

		HFONT hFont = CreateFont(m_dropdownListTextSize, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);

		y += m_dropdownListTextSize;

		HWND hDropdownList1 = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST,
									       x, y, m_dropdownListWidth, dropdownListHeight, m_hWnd, (HMENU)id, m_hInstance, NULL);

		x += m_dropdownListWidth + m_itemMargin;

		HWND hDropdownList2 = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST,
									       x, y, m_dropdownListWidth, dropdownListHeight, m_hWnd, (HMENU)(id + 1), m_hInstance, NULL);

		hFont = CreateFont(14, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
						   ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						   DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hDropdownList1, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(hDropdownList2, WM_SETFONT, (WPARAM)hFont, TRUE);

		m_items.push_back(EventElement(name, nullptr));
		m_items.push_back(EventElement(name, nullptr));

		for (std::vector<UINT>::iterator it = listData.begin(); it != listData.end(); it++)
		{
			std::string element = std::to_string((long double)(*it));
			ComboBox_AddString(hDropdownList1, element.c_str());
			ComboBox_AddString(hDropdownList2, element.c_str());

			if (it == listData.begin())
			{
				ComboBox_SelectString(hDropdownList1, -1, element.c_str());
				ComboBox_SelectString(hDropdownList2, -1, element.c_str());
			}
		}

		m_clientHeight += m_dropdownListTextSize + m_dropdownListHeight + m_itemMargin;

		const POINT windowSize	   = getWindowSize();
		const POINT windowPosition = getWindowPosition();
		SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
	}

	void NewTerrainWindow::addButton(const std::string& name, EventReceiver* eventReceiver)
	{
		const UINT count = (UINT)m_items.size();
		const UINT x	 = (count % 2) * (m_dropdownListWidth + m_itemMargin) + m_itemMargin;
		const UINT y	 = (count / 2) * (m_dropdownListTextSize + m_dropdownListHeight + m_itemMargin) + m_itemMargin;
		const UINT id	 = m_itemIDStart + count;

		HWND hButton = CreateWindow("BUTTON", name.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
									x, y, m_dropdownListWidth, m_buttonHeight, m_hWnd, (HMENU)id, m_hInstance, NULL);

		HFONT hFont = CreateFont(m_buttonTextSize, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
								 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								 DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
		SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

		m_items.push_back(EventElement(name, eventReceiver));

		if (!(count % 2))
			m_clientHeight += m_buttonHeight + m_itemMargin;

		const POINT windowSize	   = getWindowSize();
		const POINT windowPosition = getWindowPosition();
		SetWindowPos(m_hWnd, HWND_TOP, windowPosition.x, windowPosition.y, windowSize.x, windowSize.y, NULL);
	}

	POINT NewTerrainWindow::getDropdownListPairValues(const std::string& name) const
	{
		POINT value;
		UINT id = getItemID(name);
		if (id >= 0)
		{
			char buffer[32];
			ComboBox_GetText(GetDlgItem(m_hWnd, id), buffer, 31);
			value.x = atoi(buffer);
			ComboBox_GetText(GetDlgItem(m_hWnd, id + 1), buffer, 31);
			value.y = atoi(buffer);
		}
		else
		{
			value.x = -1;
			value.y = -1;
		}
		return value;
	}
}