#include "StdAfx.h"
#include "DirectionalLightOptions.h"

namespace GUI
{
	DirectionalLightOptions* g_DirectionalLightOptions;

	LRESULT CALLBACK DirectionalLightOptionsMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_DirectionalLightOptions->subWndProc(hWnd, message, wParam, lParam);
	}

	DirectionalLightOptions::DirectionalLightOptions(void)
	{
		g_DirectionalLightOptions = this;
	}

	void DirectionalLightOptions::init(HINSTANCE hInstance, HWND hParentWnd,
							    const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CHILD,
				   m_trackbarSize.x + 2 * m_trackbarMargin, m_trackbarMargin,
				   6001);

		SetWindowSubclass(m_hWnd, DirectionalLightOptionsMsgRouter, 0, 0);
	}
}