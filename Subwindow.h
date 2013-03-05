#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include "StdAfx.h"
#include "EventReceiver.h"

namespace GUI
{
	class Subwindow
	{
	public:
		Subwindow(void);
		virtual ~Subwindow(void);

		virtual LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void show(const bool show);
	protected:
		HINSTANCE m_hInstance;
		HWND	  m_hWnd;

		std::string m_caption;
		DWORD		m_style;
		UINT		m_clientWidth;
		UINT		m_clientHeight;

		std::vector<EventElement> m_items;

		void initWindow(HINSTANCE hInstance, HWND hParentWnd, const std::string& caption, const DWORD style,
						const UINT x, const UINT y, const UINT clientWidth, const UINT clientHeight);
		POINT computeWindowSize(void) const;
	
		virtual int getItemID(const UINT i) const = 0;
	};
}

#endif