#ifndef DV1416_FINAL_PROJECT_H
#define DV1416_FINAL_PROJECT_H

#include "D3DApp.h"

class dv1416_final_project : public D3DApp
{
public:
	dv1416_final_project(HINSTANCE hInstance);
	~dv1416_final_project(void);
protected:
	void update(void);
	void render(void);

	void onMouseDown(WPARAM buttonState, int x, int y);
	void onMouseUp(WPARAM buttonState, int x, int y);
	void onMouseMove(int dx, int dy);
};

#endif