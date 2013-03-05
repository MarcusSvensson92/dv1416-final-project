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

bool dv1416_final_project::init(void)
{
	if (!D3DApp::init()) return false;

	initShaders();
	initLights();
	initTerrain();

	m_camera.setPosition(0.f, 50.f, 0.f);
	m_camera.setProj(m_clientWidth, m_clientHeight, PI * 0.25f, 1.f, 1000.f);

	return true;
}

LRESULT dv1416_final_project::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GUI::Menu& menu = GUI::Menu::getInstance();
	menu.processWindowsMessages(msg, wParam);

	switch (msg)
	{
	case WM_CREATE:
	{
		menu.addItem("File", "Exit", this);
		menu.addItem("View", "Tools", this);
		menu.assignToWindow(hWnd);

		GUI::ToolbarDesc tbd;
		tbd.caption		  = "Toolbar";
		tbd.x			  = CW_USEDEFAULT;
		tbd.y			  = CW_USEDEFAULT;
		tbd.buttonsPerRow = 2;
		tbd.buttonSize	  = 32;
		tbd.buttonMargin  = 4;
		GUI::Toolbar& toolbar = GUI::Toolbar::getInstance();
		toolbar.init(m_hInstance, hWnd, tbd);
		toolbar.addButton("1", this, "life.bmp");
		toolbar.addButton("2", this, "life.bmp");
		toolbar.addButton("3", this, "life.bmp");
		toolbar.addButton("4", this, "life.bmp");
		toolbar.addButton("5", this, "life.bmp");
		toolbar.addButton("6", this, "life.bmp");
		toolbar.addButton("7", this, "life.bmp");
		toolbar.addButton("8", this, "life.bmp");
		toolbar.addButton("9", this, "life.bmp");

		GUI::LevelToolWindowDesc ltwd;
		ltwd.caption	= "Level Tool";
		ltwd.x				= 0;
		ltwd.y				= 0;
		ltwd.width			= 150;
		ltwd.height			= 300;
		ltwd.margin			= 5;
		ltwd.trackbarHeight = 30;
		GUI::LevelToolWindow& levelToolWindow = GUI::LevelToolWindow::getInstance();
		levelToolWindow.init(m_hInstance, hWnd, ltwd);
		levelToolWindow.addTrackbar("1", this, 1, 10, 5);
		levelToolWindow.addTrackbar("2", this, 1, 10, 2);
		levelToolWindow.addTrackbar("3", this, 1, 10, 8);
		levelToolWindow.show(true);

		break;
	}
	}
	
	return D3DApp::wndProc(hWnd, msg, wParam, lParam);
}

void dv1416_final_project::onEvent(const std::string& sender, const std::string& eventName)
{
	DOUT("Sender: " << sender << " | Event: " << eventName << std::endl);
	if (sender == "Menu" && eventName == "Exit")
	{
		DestroyWindow(m_hWnd);
	}
	if (sender == "Menu" && eventName == "Tools")
	{
		GUI::Menu& menu = GUI::Menu::getInstance();
		bool check = menu.isItemChecked(eventName);
		menu.checkItem(eventName, !check);
		GUI::Toolbar::getInstance().show(!check);
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

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		POINT cursorPosition;
		if (GetCursorPos(&cursorPosition))
		{
			ScreenToClient(m_hWnd, &cursorPosition);

			Ray ray = m_camera.computeRay(cursorPosition);
			m_terrain.computeIntersection(ray);

			// Click to turn the lights red
			PointLight* selected_light = m_lightManager.computeIntersection(ray);
			if (selected_light != NULL)
				selected_light->Diffuse = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
		}
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
	m_shaderManager.get("Terrain")->setRawData("gPointLights", &m_lightManager.getLights()[0], sizeof(PointLight)*m_lightManager.getLights().size());

	m_terrain.render(m_deviceContext, m_shaderManager.get("Terrain"), m_camera);
	m_lightManager.render(m_deviceContext, m_shaderManager.get("Light"), m_camera);

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
	m_shaderManager.add("Terrain", "Shaders/Terrain.fxo", basicInputDesc, 3);
	m_shaderManager.add("Light", "Shaders/LightShader.fxo", NULL, 0);
}

void dv1416_final_project::initLights(void)
{
	m_lightManager.init(m_device);

	m_lightManager.AddLight(XMFLOAT3(-80,30,-80),POINT_LIGHT);
	m_lightManager.AddLight(XMFLOAT3(-80,30, 80),POINT_LIGHT);
	m_lightManager.AddLight(XMFLOAT3( 80,30,-80),POINT_LIGHT);
	m_lightManager.AddLight(XMFLOAT3( 80,30, 80),POINT_LIGHT);
}

void dv1416_final_project::initTerrain(void)
{
	TerrainDesc td;
	td.width			= 257;
	td.depth			= 257;
	m_terrain.init(m_device, td);
	m_terrain.loadHeightmap(m_deviceContext, "temp-textures/DV1222_heightmap.raw", 80.f);
	std::vector<std::string> layermapFilenames;
	layermapFilenames.push_back("temp-textures/sandripple.png");
	layermapFilenames.push_back("temp-textures/longGrass.png");
	layermapFilenames.push_back("temp-textures/cliff.png");
	layermapFilenames.push_back("temp-textures/grayRock.png");
	m_terrain.loadBlendmap(m_device, m_deviceContext, "temp-textures/DV1222_blendmap.png", layermapFilenames);
}
