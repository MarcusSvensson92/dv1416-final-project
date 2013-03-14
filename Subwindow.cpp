#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
	Subwindow::Subwindow(void) { }

	Subwindow::~Subwindow(void) { }

	LRESULT Subwindow::subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
		{
			const UINT id = LOWORD(wParam);
			for (UINT i = 0; i < (UINT)m_items.size(); i++)
			{
				if (getItemID(i) == id)
				{
					m_items[i].second->onEvent(m_subwindowDesc.caption, m_items[i].first);
					SetFocus(m_hParentWnd);
				}
			}
			break;
		}
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->code == NM_RELEASEDCAPTURE)
			{
				const UINT id = ((LPNMHDR)lParam)->idFrom;
				for (UINT i = 0; i < (UINT)m_items.size(); i++)
				{
					if (getItemID(i) == id)
					{
						m_items[i].second->onEvent(m_subwindowDesc.caption, m_items[i].first);
						SetFocus(m_hParentWnd);
					}
				}
			}
			break;
		}

		case WM_VSCROLL:
		case WM_HSCROLL:
			break;

		case WM_ERASEBKGND:
		{
			HDC hdc;
			HBRUSH hbr;
			PAINTSTRUCT ps;
			RECT rect;

			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rect);
			hbr = GetSysColorBrush(COLOR_3DFACE);
			FillRect(hdc, &rect, hbr);
			EndPaint(hWnd, &ps);
			break;
		}

		default:
			return DefSubclassProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void Subwindow::show(const bool show)
	{
		const int cmdShow = (show) ? SW_SHOW : SW_HIDE;
		ShowWindow(m_hWnd, cmdShow);
	}

	void Subwindow::initWindow(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc,
							   const DWORD style, const UINT clientWidth, const UINT clientHeight)
	{
		m_hInstance     = hInstance;
		m_hParentWnd	= hParentWnd;
		m_subwindowDesc = subwindowDesc;
		m_style		    = style;
		m_clientWidth   = clientWidth;
		m_clientHeight  = clientHeight;

		const POINT windowSize = getWindowSize();

		CLIENTCREATESTRUCT ccs;
		ZeroMemory(&ccs, sizeof(ccs));

		m_hWnd = CreateWindow("MDICLIENT", subwindowDesc.caption.c_str(), style,
							  subwindowDesc.x, subwindowDesc.y, windowSize.x, windowSize.y,
							  hParentWnd, NULL, hInstance, (LPSTR)&ccs);
	}

	POINT Subwindow::getWindowSize(void) const
	{
		RECT rect = {0, 0, m_clientWidth, m_clientHeight};
		AdjustWindowRect(&rect, m_style, false);
		POINT windowSize;
		windowSize.x = rect.right  - rect.left;
		windowSize.y = rect.bottom - rect.top;
		return windowSize;
	}

	POINT Subwindow::getWindowPosition(void) const
	{
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		POINT windowPosition;
		windowPosition.x = rect.left;
		windowPosition.y = rect.top;
		return windowPosition;
	}
}