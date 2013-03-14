#include "StdAfx.h"
#include "TerrainOptions.h"

namespace GUI
{
	TerrainOptions* g_terrainOptions;

	LRESULT CALLBACK terrainOptionsMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_terrainOptions->subWndProc(hWnd, message, wParam, lParam);
	}

	TerrainOptions::TerrainOptions(void)
	{
		g_terrainOptions = this;
	}

	void TerrainOptions::init(HINSTANCE hInstance, HWND hParentWnd,
							  const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_CHILD,
				   m_trackbarSize.x + 2 * m_trackbarMargin, m_trackbarMargin,
				   5001);

		SetWindowSubclass(m_hWnd, terrainOptionsMsgRouter, 0, 0);
	}
}