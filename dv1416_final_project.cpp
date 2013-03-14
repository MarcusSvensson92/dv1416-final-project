#include "StdAfx.h"
#include "dv1416_final_project.h"
#include "TextureToolWindow.h"
#include "TerrainOptions.h"

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

bool dv1416_final_project::init(void)
{
	if (!D3DApp::init()) return false;

	initShaders();
	initLights();
	initTerrain();

	m_camera.setPosition(0.f, 50.f, 0.f);
	m_camera.setProj(m_clientWidth, m_clientHeight, PI * 0.25f, 1.f, 1000.f);

	m_levelTool.init(m_hWnd, m_deviceContext, &m_camera);
	m_levelTool.setTerrain(&m_terrain);

	m_textureTool.init(m_hWnd, m_device, m_deviceContext, &m_camera);
	m_textureTool.setTerrain(&m_terrain);

	m_currentActivity = RaisingLevel;

	return true;
}

LRESULT dv1416_final_project::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GUI::Menu& menu = GUI::Menu::getInstance();
	menu.processWindowsMessages(msg, wParam);

	switch (msg)
	{
	case WM_CREATE:
		initGUI(hWnd);
		break;
	}
	
	return D3DApp::wndProc(hWnd, msg, wParam, lParam);
}

void dv1416_final_project::onEvent(const std::string& sender, const std::string& eventName)
{
	DOUT("Sender: " << sender << " | Event: " << eventName << std::endl);

	if (sender == "Menu")
	{
		GUI::Menu& menu = GUI::Menu::getInstance();

		if (eventName == "Exit")
			DestroyWindow(m_hWnd);
		else if (eventName == "Toolbar")
		{
			bool check = menu.isItemChecked(eventName);
			menu.checkItem(eventName, !check);
			GUI::Toolbar::getInstance().show(!check);
		}
		else if (eventName == "Terrain Options")
		{
			bool check = menu.isItemChecked(eventName);
			menu.checkItem(eventName, !check);
			GUI::TerrainOptions::getInstance().show(!check);
		}
	}

	if (sender == "Toolbar")
	{
		if (eventName == "Raise Level")
		{
			m_currentActivity = RaisingLevel;
			m_levelTool.setState(LevelTool::State::Raising);
		}
		else if (eventName == "Lower Level")
		{
			m_currentActivity = LoweringLevel;
			m_levelTool.setState(LevelTool::State::Lowering);
		}
		else if (eventName == "Texturing")
			m_currentActivity = Texturing;
		else if (eventName == "Remove Light")
		{
			m_currentActivity = RemoveLight;
			m_LightManager.setState(LightManager::State::Remove);
		}
	}
}

void dv1416_final_project::update(void)
{
	float dt = (float)m_timer.getDeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		m_camera.walk(20.0f * dt);
	if (GetAsyncKeyState('S') & 0x8000)
		m_camera.walk(-20.0f * dt);
	if (GetAsyncKeyState('A') & 0x8000)
		m_camera.strafe(-20.0f * dt);
	if (GetAsyncKeyState('D') & 0x8000)
		m_camera.strafe(20.0f * dt);

	m_camera.updateViewMatrix();

	switch (m_currentActivity)
	{
	case RaisingLevel:
		m_levelTool.update(dt);
		break;
	case LoweringLevel:
		m_levelTool.update(dt);
		break;
	case Texturing:
		m_textureTool.update(dt);
		break;
	case RemoveLight:
		m_LightManager.update(dt);
		break;
	}
}

void dv1416_final_project::render(void)
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// TEMP FIX
	Material	mMaterial;
	mMaterial.Ambient  = D3DXVECTOR4(0.8f, 0.8f, 0.8f, 1.0f);
	mMaterial.Diffuse  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial.Specular = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

	m_shaderManager.get("Terrain")->setRawData("gMaterial", &mMaterial, sizeof(Material)); 
	m_shaderManager.get("Terrain")->setRawData("gPointLights", &m_LightManager.getLights()[0], sizeof(PointLight)*m_LightManager.getLights().size());

	m_terrain.render(m_deviceContext, m_shaderManager.get("Terrain"), m_camera);
	m_LightManager.render(m_deviceContext, m_shaderManager.get("Light"), m_camera);

	m_swapChain->Present(0, 0);
}

void dv1416_final_project::onMouseDown(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseUp(WPARAM buttonState, int x, int y) { }

void dv1416_final_project::onMouseMove(int dx, int dy)
{
	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) && GetFocus() == m_hWnd)
	{
		m_camera.pitch(XMConvertToRadians(0.25f * (float)dy));
		m_camera.rotateY(XMConvertToRadians(0.25f * (float)dx));
	}
}

void dv1416_final_project::onResize(void)
{
	D3DApp::onResize();

	m_camera.setProj(m_clientWidth, m_clientHeight, PI * 0.25f, 1.f, 1000.f);
}

void dv1416_final_project::initShaders(void)
{
	m_shaderManager.init(m_device, m_deviceContext);
	
	D3D11_INPUT_ELEMENT_DESC basicInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX",	  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	D3D11_INPUT_ELEMENT_DESC terrainInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX",	  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_shaderManager.add("Terrain", "Shaders/Terrain.fxo", terrainInputDesc, 2);
	m_shaderManager.add("Light", "Shaders/LightShader.fxo", NULL, 0);
}

void dv1416_final_project::initLights(void)
{
	m_LightManager.init(m_hWnd, m_device, &m_camera);

	m_LightManager.AddLight(XMFLOAT3(-80,30,-80),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3(-80,30, 80),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3( 80,30,-80),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3( 80,30, 80),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3( 80,30, 00),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3(-80,30, 00),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3( 00,30,-80),POINT_LIGHT);
	m_LightManager.AddLight(XMFLOAT3( 00,30, 80),POINT_LIGHT);
}

void dv1416_final_project::initTerrain(void)
{
	TerrainDesc td;
	td.width = 2048;
	td.depth = 2048;
	m_terrain.init(m_device, td);
	//m_terrain.loadHeightmap(m_deviceContext, "temp-textures/DV1222_heightmap.raw", 80.f);
	std::vector<std::string> layermapFilenames;
	layermapFilenames.push_back("temp-textures/longGrass.png");
	layermapFilenames.push_back("temp-textures/cliff.png");
	layermapFilenames.push_back("temp-textures/grayRock.png");
	layermapFilenames.push_back("temp-textures/sandripple.png");
	for (UINT i = 0; i < 4; i++)
		m_terrain.loadLayermap(m_device, i, layermapFilenames[i]);
	//m_terrain.loadBlendmap(m_device, m_deviceContext, "temp-textures/DV1222_blendmap.png", layermapFilenames);
}

void dv1416_final_project::initGUI(HWND hWnd)
{
	GUI::Menu& menu = GUI::Menu::getInstance();
	menu.addItem("File", "Exit", this);
	menu.addItem("View", "Toolbar", this, true, true);
	menu.addItem("Options", "Terrain Options", this);
	menu.assignToWindow(hWnd);

	GUI::SubwindowDesc sd;
	sd.caption = "Toolbar";
	sd.x	   = 50;
	sd.y	   = 100;
	GUI::Toolbar& toolbar = GUI::Toolbar::getInstance();
	toolbar.init(m_hInstance, hWnd, sd);
	toolbar.addButton("Raise Level", this, "Content/img/raise_level.bmp");
	toolbar.addButton("Lower Level", this, "Content/img/lower_level.bmp");
	toolbar.addButton("Texturing", this, "Content/img/texturing.bmp");
	toolbar.addButton("Remove Light", this, "Content/img/remove_light.bmp");
	toolbar.show(true);

	sd.caption = "Level Tool";
	sd.x	   = 700;
	sd.y	   = 100;
	GUI::LevelToolWindow& levelToolWindow = GUI::LevelToolWindow::getInstance();
	levelToolWindow.init(m_hInstance, hWnd, sd);
	levelToolWindow.addTrackbar("Brush Diameter", &m_levelTool, 1, 100, 10);
	levelToolWindow.addTrackbar("Brush Hardness", &m_levelTool, 0, 100, 50);
	levelToolWindow.addTrackbar("Brush Strength", &m_levelTool, 1, 100, 5);
	levelToolWindow.show(true);

	sd.caption = "Texture Tool";
	sd.x	   = 700;
	sd.y	   = 300;
	GUI::TextureToolWindow& textureToolWindow = GUI::TextureToolWindow::getInstance();
	textureToolWindow.init(m_hInstance, hWnd, sd);
	textureToolWindow.addTextureButton("Texture R", &m_textureTool, "Content/img/texture_r.bmp");
	textureToolWindow.addTextureButton("Texture G", &m_textureTool, "Content/img/texture_g.bmp");
	textureToolWindow.addTextureButton("Texture B", &m_textureTool, "Content/img/texture_b.bmp");
	textureToolWindow.addTextureButton("Texture A", &m_textureTool, "Content/img/texture_a.bmp");
	textureToolWindow.addLoadFileButton("Load Texture", &m_textureTool);
	textureToolWindow.addTrackbar("Brush Diameter", &m_textureTool, 1, 100, 10);
	textureToolWindow.addTrackbar("Brush Strength", &m_textureTool, 1, 100, 50);
	textureToolWindow.show(true);

	sd.caption = "Terrain Options";
	sd.x	   = 500;
	sd.y	   = 400;
	GUI::TerrainOptions& terrainOptions = GUI::TerrainOptions::getInstance();
	terrainOptions.init(m_hInstance, hWnd, sd);
	terrainOptions.addTrackbar("Texture Scale", &m_terrain, 1, 100, 5);
	terrainOptions.addTrackbar("Minimum Tessellation", &m_terrain, 0, 6, 0);
	terrainOptions.addTrackbar("Maximum Tessellation", &m_terrain, 0, 6, 6);
	terrainOptions.addTrackbar("Minimum Tessellation Distance", &m_terrain, 1, 1000, 20);
	terrainOptions.addTrackbar("Maximum Tessellation Distance", &m_terrain, 1, 1000, 500);
}