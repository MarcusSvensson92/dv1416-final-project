#include "StdAfx.h"
#include "dv1416_final_project.h"
#include "Toolbox.h"
#include "TerrainOptions.h"
#include "SelectionOptions.h"
#include "PointLightOptions.h"
#include "DirectionalLightOptions.h"
#include "NewTerrainWindow.h"
#include "GUI.h"

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

	Toolbox& toolbox = Toolbox::getInstance();
	toolbox.init(m_hWnd, &m_camera, m_device, m_deviceContext, &m_LightManager);
	toolbox.setTerrain(&m_terrain);

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
	if (sender == "Menu")
	{
		GUI::Menu& menu = GUI::Menu::getInstance();

		if (eventName == "New...")
		{
			GUI::NewTerrainWindow::getInstance().show(true);
			EnableWindow(m_hWnd, false);
		}
		else if (eventName == "Open...")
		{
			std::string filepath;
			if (GUI::openFileBox(m_hWnd, "Map Files (*.map)\0*.map\0All Files (*.*)\0*.*\0\0", ".map", filepath))
			{
				std::string extension = PathFindExtension(&filepath[0]);
				if (extension == ".map")
				{
					std::ifstream file;
					file.open(filepath);
					if (file)
					{
						std::string heightmapFilepath, blendmapFilepath;

						while (!file.eof())
						{
							std::string command;
							file >> command;

							if (command == "heightmap")
								file >> heightmapFilepath;
							else if (command == "blendmap")
								file >> blendmapFilepath;

							file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
						}

						if (!heightmapFilepath.empty() &&
							!blendmapFilepath.empty())
						{
							m_terrain.create(m_device, m_deviceContext,
											 heightmapFilepath, blendmapFilepath);

							m_camera.setPosition(0.f, 50.f, 0.f);
						}
					}
				}
				else
				{
					std::string errorMessage = "Invalid extension: '" + extension + "'";
					MessageBox(m_hWnd, errorMessage.c_str(), m_wndCaption.c_str(), MB_ICONERROR);
				}
			}
		}
		else if (eventName == "Save")
		{
			if (m_terrain.isCreated())
			{
				m_terrain.saveHeightmap();
				m_terrain.saveBlendmap();
			}
		}
		else if (eventName == "Save as...")
		{
			std::string filepath;
			if (m_terrain.isCreated() && GUI::saveFileBox(m_hWnd, "Map Files (*.map)\0*.map\0All Files (*.*)\0*.*\0\0", "map", filepath))
			{
				std::string extension = PathFindExtension(&filepath[0]);
				if (extension == ".map")
				{
					PathRemoveExtension(&filepath[0]);
					filepath.erase(std::find(filepath.begin(), filepath.end(), '\0'), filepath.end());
					std::string heightmapFilepath = filepath + "_heightmap.raw";
					std::string blendmapFilepath  = filepath + "_blendmap.png";

					std::ofstream file;
					file.open(filepath + ".map");
					if (file)
					{
						file << "heightmap " << heightmapFilepath <<
								"\nblendmap " << blendmapFilepath;
						file.close();

						m_terrain.saveHeightmap(heightmapFilepath);
						m_terrain.saveBlendmap(blendmapFilepath);
					}
				}
				else
				{
					std::string errorMessage = "Invalid extension: '" + extension + "'";
					MessageBox(m_hWnd, errorMessage.c_str(), m_wndCaption.c_str(), MB_ICONERROR);
				}
			}
		}
		else if (eventName == "Exit")
			DestroyWindow(m_hWnd);
		else if (eventName == "Undo")
			Toolbox::getInstance().undo();
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
		else if (eventName == "Selection Options")
		{
			bool check = menu.isItemChecked(eventName);
			menu.checkItem(eventName, !check);
			GUI::SelectionOptions::getInstance().show(!check);
		}
		else if (eventName == "DirectionalLight Options")
		{
			bool check = menu.isItemChecked(eventName);
			menu.checkItem(eventName, !check);
			GUI::DirectionalLightOptions::getInstance().show(!check);
		}
	}

	if (sender == "New...")
	{
		GUI::NewTerrainWindow& newTerrainWindow = GUI::NewTerrainWindow::getInstance();

		if (eventName == "Create")
		{
			POINT size = newTerrainWindow.getDropdownListPairValues("Terrain Size");
			m_terrain.create(m_device, m_deviceContext, size.x, size.y);

			GUI::NewTerrainWindow::getInstance().show(false);
			EnableWindow(m_hWnd, true);
			SetFocus(m_hWnd);
		}
		else if (eventName == "Cancel")
		{
			GUI::NewTerrainWindow::getInstance().show(false);
			EnableWindow(m_hWnd, true);
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

	if (m_terrain.isCreated())
		Toolbox::getInstance().update(dt);
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
	m_shaderManager.get("Terrain")->setRawData("gPointLights", &m_LightManager.getPLights()[0], sizeof(PointLight)*m_LightManager.getPLights().size());
	m_shaderManager.get("Terrain")->setRawData("gDirectionalLights", &m_LightManager.getDLights()[0], sizeof(DirectionalLight)*m_LightManager.getDLights().size());

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
}

void dv1416_final_project::initTerrain(void)
{
	std::vector<std::string> layermapFilenames;
	layermapFilenames.push_back("temp-textures/longGrass.png");
	layermapFilenames.push_back("temp-textures/cliff.png");
	layermapFilenames.push_back("temp-textures/grayRock.png");
	layermapFilenames.push_back("temp-textures/sandripple.png");
	for (UINT i = 0; i < 4; i++)
		m_terrain.loadLayermap(m_device, i, layermapFilenames[i]);
}

void dv1416_final_project::initGUI(HWND hWnd)
{
	GUI::Menu& menu = GUI::Menu::getInstance();
	menu.addItem("File", "New...", this);
	menu.addItem("File", "Open...", this);
	menu.addItem("File", "Save", this);
	menu.addItem("File", "Save as...", this);
	menu.addItem("File", "Exit", this);
	menu.addItem("Edit", "Undo", this);
	menu.addItem("View", "Toolbar", this, true, true);
	menu.addItem("Options", "Terrain Options", this);
	menu.addItem("Options", "Selection Options", this);
	menu.addItem("Options", "DirectionalLight Options", this);
	menu.assignToWindow(hWnd);

	GUI::SubwindowDesc sd;
	sd.caption = "New...";
	sd.x	   = 400;
	sd.y	   = 300;
	GUI::NewTerrainWindow& newTerrainWindow = GUI::NewTerrainWindow::getInstance();
	newTerrainWindow.init(m_hInstance, hWnd, sd);
	std::vector<UINT> listData;
	for (UINT value = 64; value <= 2048; value *= 2)
		listData.push_back(value);
	newTerrainWindow.addDropdownListPair("Terrain Size", listData);
	newTerrainWindow.addButton("Create", this);
	newTerrainWindow.addButton("Cancel", this);

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

	sd.caption = "Selection Options";
	sd.x	   = 500;
	sd.y	   = 400;
	GUI::SelectionOptions& selectionOptions = GUI::SelectionOptions::getInstance();
	selectionOptions.init(m_hInstance, hWnd, sd);
	selectionOptions.addTrackbar("Interval Sample", &m_terrain, 1, 100, 10);
	selectionOptions.addTrackbar("Distance", &m_terrain, 1, 1000, 500);

	sd.caption = "PointLight Options";
	sd.x	   = 500;
	sd.y	   = 400;
	GUI::PointLightOptions& pointlightOptions = GUI::PointLightOptions::getInstance();
	pointlightOptions.init(m_hInstance, hWnd, sd);
	pointlightOptions.addTrackbar("Ambient Range", &m_LightManager, 1, 1000, 100);
	pointlightOptions.addTrackbar("Ambient R", &m_LightManager, 0, 1000, 100);
	pointlightOptions.addTrackbar("Ambient G", &m_LightManager, 0, 1000, 100);
	pointlightOptions.addTrackbar("Ambient B", &m_LightManager, 0, 1000, 100);

	pointlightOptions.addTrackbar("Linear Modifier", &m_LightManager, 1, 100, 10);
	pointlightOptions.addTrackbar("Diffuse R", &m_LightManager, 0, 1000, 100);
	pointlightOptions.addTrackbar("Diffuse G", &m_LightManager, 0, 1000, 100);
	pointlightOptions.addTrackbar("Diffuse B", &m_LightManager, 0, 1000, 100);

	pointlightOptions.addTrackbar("Specular Power", &m_LightManager, 1, 1000, 100);
	pointlightOptions.addTrackbar("Specular R", &m_LightManager, 0, 1000, 100);
	pointlightOptions.addTrackbar("Specular G", &m_LightManager, 0, 1000, 100);
	pointlightOptions.addTrackbar("Specular B", &m_LightManager, 0, 1000, 100);

	sd.caption = "DirectionalLight Options";
	sd.x	   = 500;
	sd.y	   = 400;
	GUI::DirectionalLightOptions& directionallightOptions = GUI::DirectionalLightOptions::getInstance();
	directionallightOptions.init(m_hInstance, hWnd, sd);
	directionallightOptions.addTrackbar("Directional Light", &m_LightManager, 1, m_LightManager.getDLights().size(), 1);
	directionallightOptions.addTrackbar("OFF / ON", &m_LightManager, 0, 1, 1);
	directionallightOptions.addTrackbar("X", &m_LightManager, 0, 200, 100);
	directionallightOptions.addTrackbar("Z", &m_LightManager, 0, 200, 100);

	Toolbox::getInstance().initGUI(m_hInstance, hWnd);
}