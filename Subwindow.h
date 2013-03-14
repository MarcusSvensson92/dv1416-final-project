#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include "StdAfx.h"
#include "EventReceiver.h"

namespace GUI
{
	struct SubwindowDesc
	{
		std::string caption;
		UINT		x;
		UINT		y;
	};

	class Subwindow
	{
	public:
		Subwindow(void);
		virtual ~Subwindow(void);

		virtual void init(HINSTANCE hInstance, HWND hParentWnd,
						  const SubwindowDesc subwindowDesc) = 0;
		virtual LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void show(const bool show);
	protected:
		HINSTANCE m_hInstance;
		HWND	  m_hWnd;
		HWND	  m_hParentWnd;

		SubwindowDesc m_subwindowDesc;
		DWORD		  m_style;
		UINT		  m_clientWidth;
		UINT		  m_clientHeight;

		std::vector<EventElement> m_items;
		UINT					  m_itemIDStart;

		void initWindow(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc,
						const DWORD style, const UINT clientWidth, const UINT clientHeight,
						const UINT itemIDStart);
	
		POINT getWindowSize(void) const;
		POINT getWindowPosition(void) const;

		int getItemID(const UINT i) const;
		int getItemID(const std::string& itemName) const;
	};
}

#endif