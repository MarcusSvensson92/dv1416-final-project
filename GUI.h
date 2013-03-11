#ifndef GUI_H
#define GUI_H

#include "Menu.h"

#include <string>

namespace GUI
{
	bool openFileBox(HWND hWnd, const std::string& filter, std::string& filepath, const UINT flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER);
	bool saveFileBox(HWND hWnd, const std::string& filter, std::string& filepath, const UINT flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT);
}

#endif