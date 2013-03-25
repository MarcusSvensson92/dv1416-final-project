#include "StdAfx.h"
#include "PointLightOptions.h"

namespace GUI
{
	PointLightOptions* g_PointLightOptions;

	LRESULT CALLBACK PointLightOptionsMsgRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR idSubClass, DWORD_PTR refDat)
	{
		return g_PointLightOptions->subWndProc(hWnd, message, wParam, lParam);
	}

	PointLightOptions::PointLightOptions(void)
	{
		g_PointLightOptions = this;
	}

	void PointLightOptions::init(HINSTANCE hInstance, HWND hParentWnd,
							    const SubwindowDesc subwindowDesc)
	{
		initWindow(hInstance, hParentWnd, subwindowDesc,
				   WS_POPUP | WS_CAPTION | WS_CHILD,
				   m_trackbarSize.x + 2 * m_trackbarMargin, m_trackbarMargin,
				   6001);

		SetWindowSubclass(m_hWnd, PointLightOptionsMsgRouter, 0, 0);
	}
}