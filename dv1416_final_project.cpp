#include "StdAfx.h"
#include "dv1416_final_project.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   PSTR cmdLine, int showCmd)
{
	dv1416_final_project app(hInstance);
	if (!app.init())
		return EXIT_FAILURE;
	return app.run();
}

dv1416_final_project::dv1416_final_project(HINSTANCE hInstance)
	: D3DApp(hInstance, "dv1416-final-project", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			 800, 600, D3D_DRIVER_TYPE_HARDWARE) { }

dv1416_final_project::~dv1416_final_project(void) { }

LRESULT dv1416_final_project::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GUI::Menu& menu = GUI::Menu::getInstance();
	menu.processWindowsMessages(msg, wParam);

	switch (msg)
	{
	case WM_CREATE:
		menu.addItem("File", "Exit", this);
		menu.addItem("View", "Tools", this);
		menu.assignToWindow(hWnd);
	}
	
	return D3DApp::wndProc(hWnd, msg, wParam, lParam);
}

void dv1416_final_project::onEvent(const std::string& sender, const std::string& eventName)
{
}

void dv1416_final_project::update(void) { }

void dv1416_final_project::render(void) { }

void dv1416_final_project::onMouseDown(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseUp(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseMove(int dx, int dy) { }