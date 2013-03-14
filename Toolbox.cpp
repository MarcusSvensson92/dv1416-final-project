#include "StdAfx.h"
#include "Toolbox.h"
#include "Toolbar.h"
#include "LevelToolWindow.h"
#include "TextureToolWindow.h"

Toolbox::Toolbox(void)
{
	m_activity = RaisingLevel;
	m_targetPosition = XMFLOAT3(0.f, 0.f, 0.f);

	m_levelTool.setTargetPosition(&m_targetPosition);
	m_textureTool.setTargetPosition(&m_targetPosition);
}

void Toolbox::setTerrain(Terrain* terrain)
{
	m_terrain = terrain;

	m_levelTool.setTerrain(terrain);
	m_textureTool.setTerrain(terrain);
}

void Toolbox::init(HWND hWnd, Camera* camera, ID3D11Device* device,
				   ID3D11DeviceContext* deviceContext, LightManager* lightManager)
{
	m_hWnd   = hWnd;
	m_camera = camera;

	m_lightManager = lightManager;

	m_levelTool.setDeviceContext(deviceContext);
	
	m_textureTool.setHWnd(hWnd);
	m_textureTool.setDevice(device);
	m_textureTool.setDeviceContext(deviceContext);
}

void Toolbox::initGUI(HINSTANCE hInstance, HWND hWnd)
{
	GUI::SubwindowDesc sd;
	sd.caption = "Toolbar";
	sd.x	   = 50;
	sd.y	   = 100;
	GUI::Toolbar& toolbar = GUI::Toolbar::getInstance();
	toolbar.init(hInstance, hWnd, sd);
	toolbar.addButton("Raising Level", this, "Content/img/raise_level.bmp");
	toolbar.addButton("Lowering Level", this, "Content/img/lower_level.bmp");
	toolbar.addButton("Texturing", this, "Content/img/texturing.bmp");
	toolbar.addButton("Add Light", this, "Content/img/add_light.bmp");
	toolbar.addButton("Remove Light", this, "Content/img/remove_light.bmp");
	toolbar.addButton("Move LightXZ", this, "Content/img/movexz_light.bmp");
	toolbar.addButton("Move LightY", this, "Content/img/movey_light.bmp");
	toolbar.show(true);

	sd.caption = "Level Tool";
	sd.x	   = 700;
	sd.y	   = 100;
	GUI::LevelToolWindow& levelToolWindow = GUI::LevelToolWindow::getInstance();
	levelToolWindow.init(hInstance, hWnd, sd);
	levelToolWindow.addTrackbar("Brush Diameter", &m_levelTool, 1, 100, 10);
	levelToolWindow.addTrackbar("Brush Hardness", &m_levelTool, 0, 100, 50);
	levelToolWindow.addTrackbar("Brush Strength", &m_levelTool, 1, 100, 5);
	levelToolWindow.show(true);

	sd.caption = "Texture Tool";
	sd.x	   = 700;
	sd.y	   = 300;
	GUI::TextureToolWindow& textureToolWindow = GUI::TextureToolWindow::getInstance();
	textureToolWindow.init(hInstance, hWnd, sd);
	textureToolWindow.addTextureButton("Texture R", &m_textureTool, "Content/img/texture_r.bmp");
	textureToolWindow.addTextureButton("Texture G", &m_textureTool, "Content/img/texture_g.bmp");
	textureToolWindow.addTextureButton("Texture B", &m_textureTool, "Content/img/texture_b.bmp");
	textureToolWindow.addTextureButton("Texture A", &m_textureTool, "Content/img/texture_a.bmp");
	textureToolWindow.addLoadFileButton("Load Texture", &m_textureTool);
	textureToolWindow.addTrackbar("Brush Diameter", &m_textureTool, 1, 100, 10);
	textureToolWindow.addTrackbar("Brush Strength", &m_textureTool, 1, 100, 50);
	textureToolWindow.show(true);
}

void Toolbox::update(const float dt)
{
	POINT cursorPosition;
	if (GetCursorPos(&cursorPosition))
	{
		ScreenToClient(m_hWnd, &cursorPosition);
		Ray ray = m_camera->computeRay(cursorPosition);
		if (m_terrain->computeIntersection(ray))
			m_targetPosition = m_terrain->getTargetPosition();
	}

	
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetFocus() == m_hWnd)
	{
		switch (m_activity)
		{
		case RaisingLevel:
			m_levelTool.update(dt);
			break;
		case LoweringLevel:
			m_levelTool.update(-dt);
			break;
		case Texturing:
			m_textureTool.update(dt);
			break;
		case Light:
			m_lightManager->update(dt);
			break;
		}
	}
}

void Toolbox::onEvent(const std::string& sender, const std::string& eventName)
{
	if (sender == "Toolbar")
	{
		if (eventName == "Raising Level")
		{
			m_activity = RaisingLevel;
			m_levelTool.onEvent("Level Tool", "Brush Diameter");
		}
		else if (eventName == "Lowering Level")
		{
			m_activity = LoweringLevel;
			m_levelTool.onEvent("Level Tool", "Brush Diameter");
		}
		else if (eventName == "Texturing")
		{
			m_activity = Texturing;
			m_textureTool.onEvent("Texture Tool", "Brush Diameter");
		}
		else if (eventName == "Add Light")
		{
			m_activity = Light;
			m_lightManager->setState(LightManager::State::Add);
		}
		else if (eventName == "Remove Light")
		{
			m_activity = Light;
			m_lightManager->setState(LightManager::State::Remove);
		}
		else if (eventName == "Move LightXZ")
		{
			m_activity = Light;
			m_lightManager->setState(LightManager::State::MoveXZ);
		}
		else if (eventName == "Move LightY")
		{
			m_activity = Light;
			m_lightManager->setState(LightManager::State::MoveY);
		}
	}
}