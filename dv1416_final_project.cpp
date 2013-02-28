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
			 800, 600, D3D_DRIVER_TYPE_HARDWARE)
{
	m_toolbar = NULL;
}

dv1416_final_project::~dv1416_final_project(void)
{
	if (m_toolbar) delete m_toolbar;
}

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
	if (sender == "Menu" && eventName == "Exit")
	{
		DestroyWindow(m_hWnd);
	}
	if (sender == "Menu" && eventName == "Tools")
	{
		GUI::ToolbarDesc tbd;
		tbd.caption		  = "Toolbar";
		tbd.x			  = CW_USEDEFAULT;
		tbd.y			  = CW_USEDEFAULT;
		tbd.buttonsPerRow = 2;
		tbd.buttonSize	  = 32;
		tbd.buttonMargin  = 4;

		m_toolbar = new GUI::Toolbar(m_hInstance, m_hWnd, tbd);
		m_toolbar->addButton("1", this, "life.bmp");
		m_toolbar->addButton("2", this, "life.bmp");
		m_toolbar->addButton("3", this, "life.bmp");
		m_toolbar->addButton("4", this, "life.bmp");
		m_toolbar->addButton("5", this, "life.bmp");
		m_toolbar->addButton("6", this, "life.bmp");
		m_toolbar->addButton("7", this, "life.bmp");
		m_toolbar->addButton("8", this, "life.bmp");
		m_toolbar->addButton("9", this, "life.bmp");
	}
}

void dv1416_final_project::update(void) { }

void dv1416_final_project::render(void) { }

void dv1416_final_project::onMouseDown(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseUp(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseMove(int dx, int dy) { }