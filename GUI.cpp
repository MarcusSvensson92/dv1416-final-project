#include "StdAfx.h"
#include "GUI.h"

namespace GUI
{
	const UINT g_maxPath = 256;

	OPENFILENAME getFileBoxData(HWND hWnd, const char* filter, const char* defaultExtension,
								char* buffer, const UINT flags);

	bool openFileBox(HWND hWnd, const char* filter, const char* defaultExtension,
					 std::string& filepath, const UINT flags)
	{
		char buffer[g_maxPath];
		OPENFILENAME ofn = getFileBoxData(hWnd, filter, defaultExtension, buffer, flags);
		bool result = GetOpenFileName(&ofn);
		filepath = ofn.lpstrFile;
		return result;
	}

	bool saveFileBox(HWND hWnd, const char* filter, const char* defaultExtension,
					 std::string& filepath, const UINT flags)
	{
		char buffer[g_maxPath];
		OPENFILENAME ofn = getFileBoxData(hWnd, filter, defaultExtension, buffer, flags);
		bool result = GetSaveFileName(&ofn);
		filepath = ofn.lpstrFile;
		return result;
	}

	OPENFILENAME getFileBoxData(HWND hWnd, const char* filter, const char* defaultExtension,
								char* buffer, const UINT flags)
	{
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize  = sizeof(ofn);
		ofn.hwndOwner    = hWnd;
		ofn.lpstrFile	 = buffer;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile	 = g_maxPath;
		ofn.lpstrFilter	 = filter;
		ofn.nFilterIndex = 0;
		ofn.lpstrDefExt  = defaultExtension;
		ofn.Flags		 = flags;
		return ofn;
	}
}