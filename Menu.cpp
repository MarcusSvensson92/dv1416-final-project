#include "StdAfx.h"
#include "Menu.h"

namespace GUI
{
	const UINT g_itemIDStart = 1001;

	Menu::Menu(void)
	{
		m_hMenu = CreateMenu();
	}

	void Menu::addItem(const std::string& subMenuName, const std::string& itemName, EventReceiver* eventReceiver, const bool enabled, const bool checked)
	{
		if (!isItemNameAvailable(itemName)) return;

		HMENU hSubMenu;
		if (!(hSubMenu = getHSubMenu(subMenuName)))
		{
			hSubMenu = CreatePopupMenu();
			AppendMenu(m_hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, subMenuName.c_str());
			m_subMenus.insert(std::pair<const std::string, HMENU>(subMenuName, hSubMenu));
		}

		UINT flags = MF_STRING;
		flags |= (enabled) ? MF_ENABLED : MF_DISABLED;
		flags |= (checked) ? MF_CHECKED : MF_UNCHECKED;

		const UINT id = g_itemIDStart + (UINT)m_items.size();

		AppendMenu(hSubMenu, flags, id, itemName.c_str());
		m_items.push_back(EventElement(itemName, eventReceiver));
	}

	void Menu::enableItem(const std::string& itemName, const bool enable)
	{
		EnableMenuItem(m_hMenu, getItemID(itemName), (enable) ? MF_ENABLED : MF_DISABLED);
	}
	
	void Menu::checkItem(const std::string& itemName, const bool check)
	{
		CheckMenuItem(m_hMenu, getItemID(itemName), (check) ? MF_CHECKED : MF_UNCHECKED);
	}

	bool Menu::isItemEnabled(const std::string& itemName)
	{
		return (GetMenuState(m_hMenu, getItemID(itemName), 0) & MF_ENABLED);
	}

	bool Menu::isItemChecked(const std::string& itemName)
	{
		return (GetMenuState(m_hMenu, getItemID(itemName), 0) & MF_CHECKED);
	}

	void Menu::assignToWindow(HWND hWnd)
	{
		SetMenu(hWnd, m_hMenu);
	}

	void Menu::processWindowsMessages(UINT message, WPARAM wParam)
	{
		if (message == WM_COMMAND)
		{
			const UINT id = LOWORD(wParam);
			for (UINT i = 0; i < (UINT)m_items.size(); i++)
				if (g_itemIDStart + i == id)
					m_items[i].second->onEvent("Menu", m_items[i].first);
		}
	}

	void Menu::update(HWND hWnd)
	{
		DrawMenuBar(hWnd);
	}

	bool Menu::isItemNameAvailable(const std::string& itemName)
	{
		for (std::vector<EventElement>::iterator it = m_items.begin(); it != m_items.end(); it++)
			if (it->first == itemName)
				return false;
		return true;
	}

	HMENU Menu::getHSubMenu(const std::string& subMenuName)
	{
		for (std::map<const std::string, HMENU>::iterator it = m_subMenus.begin(); it != m_subMenus.end(); it++)
			if (it->first == subMenuName)
				return it->second;
		return NULL;
	}

	int Menu::getItemID(const std::string& itemName)
	{
		for (UINT i = 0; i < (UINT)m_items.size(); i++)
			if (m_items[i].first == itemName)
				return g_itemIDStart + i;
		return -1;
	}
}