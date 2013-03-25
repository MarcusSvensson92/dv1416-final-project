#ifndef GUI_H
#define GUI_H

#include "Menu.h"

#include <string>

namespace GUI
{
	bool openFileBox(HWND hWnd, const char* filter, const char* defaultExtension, std::string& filepath,
					 const UINT flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER);
	bool saveFileBox(HWND hWnd, const char* filter, const char* defaultExtension, std::string& filepath,
					 const UINT flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_EXPLORER);
}

#endif