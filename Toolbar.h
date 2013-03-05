#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "StdAfx.h"
#include "Subwindow.h"
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

	class Toolbar : public Subwindow
	{
	public:
		static Toolbar& getInstance(void)
		{
			static Toolbar instance;
			return instance;
		}
	private:
		Toolbar(void);
		Toolbar(Toolbar const&);
		void operator=(Toolbar const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd, const ToolbarDesc toolbarDesc);
		void addButton(const std::string& name, EventReceiver* eventReceiver,
					   const std::string& bitmapFilename);

		LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	protected:
		int getItemID(const UINT i) const;
	private:
		ToolbarDesc m_toolbarDesc;

		void resetButtonHighlights(void);
	};
}

#endif