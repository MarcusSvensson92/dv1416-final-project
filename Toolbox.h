#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "StdAfx.h"
#include "EventReceiver.h"
#include "Camera.h"
#include "Terrain.h"
#include "LevelTool.h"
#include "TextureTool.h"
#include "LightManager.h"

class Toolbox : public GUI::EventReceiver
{
public:
	static Toolbox& getInstance(void)
	{
		static Toolbox instance;
		return instance;
	}
private:
	Toolbox(void);
	Toolbox(Toolbox const&);
	void operator=(Toolbox const&);

public:
	void setTerrain(Terrain* terrain);

	void init(HWND hWnd, Camera* camera, ID3D11Device* device,
			  ID3D11DeviceContext* deviceContext, LightManager* lightManager);
	void initGUI(HINSTANCE hInstance, HWND hWnd);
	void update(const float dt);

	void clearUndoStack(void);
	void undo(void);

	void onEvent(const std::string& sender, const std::string& eventName);
private:
	enum Activity
	{
		None,
		RaisingLevel,
		LoweringLevel,
		Texturing,
		Light,
	} m_activity;

	HWND m_hWnd;
	Camera* m_camera;

	Terrain* m_terrain;

	LevelTool m_levelTool;
	TextureTool m_textureTool;
	LightManager* m_lightManager;

	XMFLOAT3 m_targetPosition;

	bool m_prevMouseDown;
	bool m_prevZDown;

	std::vector<Activity> m_undoStack;
};

#endif