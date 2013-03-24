#include "StdAfx.h"
#include "GUI.h"

namespace GUI
{
	const UINT g_maxPath = 256;

	OPENFILENAME getFileBoxData(HWND hWnd, char* buffer, const std::string& filter,
								const std::string& defaultExtension, const UINT flags);

	bool openFileBox(HWND hWnd, const std::string& filter,
					 const std::string& defaultExtension, std::string& filepath,
					 const UINT flags)
	{
		char buffer[g_maxPath];
		OPENFILENAME ofn = getFileBoxData(hWnd, buffer, filter, defaultExtension, flags);
		bool result = GetOpenFileName(&ofn);
		filepath = ofn.lpstrFile;
		return result;
	}

	bool saveFileBox(HWND hWnd, const std::string& filter,
					 const std::string& defaultExtension, std::string& filepath,
					 const UINT flags)
	{
		char buffer[g_maxPath];
		OPENFILENAME ofn = getFileBoxData(hWnd, buffer, filter, defaultExtension, flags);
		bool result = GetSaveFileName(&ofn);
		filepath = ofn.lpstrFile;
		return result;
	}

	OPENFILENAME getFileBoxData(HWND hWnd, char* buffer, const std::string& filter,
								const std::string& defaultExtension, const UINT flags)
	{
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize  = sizeof(ofn);
		ofn.hwndOwner    = hWnd;
		ofn.lpstrFile	 = buffer;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile	 = g_maxPath;
		ofn.lpstrFilter	 = filter.c_str();
		ofn.lpstrDefExt  = defaultExtension.c_str();
		ofn.Flags		 = flags;
		return ofn;
	}
}