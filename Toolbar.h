#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "StdAfx.h"
#include "EventReceiver.h"

namespace GUI
{
	struct ToolbarDesc
	{
		std::string caption;
		UINT		x;
		UINT		y;
		UINT		buttonsPerRow;
		UINT		buttonSize;
		UINT		buttonMargin;

		UINT width(void) const { return buttonsPerRow * (buttonSize + buttonMargin) + buttonMargin; }
		UINT height(const UINT rowCount) const { return rowCount * (buttonSize + buttonMargin) + buttonMargin; }

		UINT buttonX(const UINT buttonCount) const { return (buttonCount % buttonsPerRow) * (buttonSize + buttonMargin) + buttonMargin; }
		UINT buttonY(const UINT buttonCount) const { return (buttonCount / buttonsPerRow) * (buttonSize + buttonMargin) + buttonMargin; }
	};

	class Toolbar
	{
	public:
		Toolbar(HINSTANCE   hInstance,
			    HWND		hParentWnd,
			    ToolbarDesc toolbarDesc);
		~Toolbar(void);

		void addButton(const std::string& name, EventReceiver* eventReceiver, const std::string& bitmapFilename);
		void hide(const bool hide);

		LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		HINSTANCE m_hInstance;
		HWND	  m_hWnd;

		ToolbarDesc m_toolbarDesc;
		DWORD		m_style;
		UINT		m_clientWidth;
		UINT		m_clientHeight;

		std::vector<EventElement> m_buttons;

		POINT computeWndSize(void) const;

		void resetButtonHighlights(void);
	};
}

#endif