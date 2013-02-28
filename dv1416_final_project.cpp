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

void dv1416_final_project::update(void) { }

void dv1416_final_project::render(void) { }

void dv1416_final_project::onMouseDown(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseUp(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseMove(int dx, int dy) { }