#ifndef DV1416_FINAL_PROJECT_H
#define DV1416_FINAL_PROJECT_H

#include "D3DApp.h"
#include "Menu.h"
#include "Toolbar.h"
#include "Camera.h"
#include "Terrain.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "LevelToolWindow.h"
#include "LevelTool.h"
#include "TextureTool.h"

class dv1416_final_project : public D3DApp, GUI::EventReceiver
{
public:
	dv1416_final_project(HINSTANCE hInstance);
	~dv1416_final_project(void);

	bool init(void);
	LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void onEvent(const std::string& sender, const std::string& eventName);
protected:
	void update(void);
	void render(void);

	void onMouseDown(WPARAM buttonState, int x, int y);
	void onMouseUp(WPARAM buttonState, int x, int y);
	void onMouseMove(int dx, int dy);

	void onResize(void);
private:
	enum Activity
	{
		RaisingLevel,
		LoweringLevel,
		Texturing,
		AddLight,
		RemoveLight
	} m_currentActivity;

	Camera m_camera;
	ShaderManager m_shaderManager;
	LightManager m_LightManager;
	Terrain m_terrain;
	LevelTool m_levelTool;
	TextureTool m_textureTool;

	void initShaders(void);
	void initLights(void);
	void initTerrain(void);
	void initGUI(HWND hWnd);
};

#endif