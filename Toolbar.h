#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
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
		void init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc);
		void addButton(const std::string& name, EventReceiver* eventReceiver,
					   const std::string& bitmapFilename);

		LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		UINT m_buttonsPerRow;
		UINT m_buttonSize;
		UINT m_buttonMargin;

		void resetButtonHighlights(void);
	};
}

#endif