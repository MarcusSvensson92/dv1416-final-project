#ifndef MENU_H
#define MENU_H

#include "EventReceiver.h"

namespace GUI
{
	class Menu
	{
	public:
		static Menu& getInstance(void)
		{
			static Menu instance;
			return instance;
		}
	private:
		Menu(void);
		Menu(Menu const&);
		void operator=(Menu const&);

	public:
		void addItem(const std::string& subMenuName, const std::string& itemName, EventReceiver* eventReceiver, const bool enabled = true, const bool checked = false);

		void enableItem(const std::string& itemName, const bool enable);
		void checkItem(const std::string& itemName, const bool check);

		bool isItemEnabled(const std::string& itemName);
		bool isItemChecked(const std::string& itemName);

		void assignToWindow(HWND hWnd);
		void processWindowsMessages(UINT message, WPARAM wParam);
		void update(HWND hWnd);
	private:
		HMENU m_hMenu;
		std::map<const std::string, HMENU> m_subMenus;
		std::vector<EventElement> m_items;

		bool isItemNameAvailable(const std::string& itemName);

		HMENU getHSubMenu(const std::string& subMenuName);
		int getItemID(const std::string& itemName);
	};
}

#endif