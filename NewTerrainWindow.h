#ifndef NEWTERRAINWINDOW_H
#define NEWTERRAINWINDOW_H

#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
	class NewTerrainWindow : public Subwindow
	{
	public:
		static NewTerrainWindow& getInstance(void)
		{
			static NewTerrainWindow instance;
			return instance;
		}
	private:
		NewTerrainWindow(void);
		NewTerrainWindow(NewTerrainWindow const&);
		void operator=(NewTerrainWindow const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc);
		//LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void addDropdownListPair(const std::string& name, std::vector<UINT> listData);
		void addButton(const std::string& name, EventReceiver* eventReceiver);

		POINT getDropdownListPairValues(const std::string& name) const;
	private:
		UINT m_itemMargin;
		UINT m_dropdownListWidth;
		UINT m_dropdownListHeight;
		UINT m_dropdownListTextSize;
		UINT m_buttonHeight;
		UINT m_buttonTextSize;
	};
}

#endif