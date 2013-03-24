#ifndef GUI_H
#define GUI_H

#include "Menu.h"

#include <string>

namespace GUI
{
	bool openFileBox(HWND hWnd, const std::string& filter,
					 const std::string& defaultExtension, std::string& filepath,
					 const UINT flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
	bool saveFileBox(HWND hWnd, const std::string& filter,
					 const std::string& defaultExtension, std::string& filepath,
					 const UINT flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT);
}

#endif