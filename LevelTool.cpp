#include "StdAfx.h"
#include "LevelTool.h"
#include "LevelToolWindow.h"

LevelTool::LevelTool(void)
{
	m_targetPosition = XMFLOAT3(0.f, 0.f, 0.f);

	m_brushDiameter = 10.f;
	m_brushHardness = 0.5f;
	m_brushStrength = 5.f;

	m_state = Raising;
}

LevelTool::~LevelTool(void)
{
}

void LevelTool::init(HWND hWnd, ID3D11DeviceContext* deviceContext, Camera* camera)
{
	m_hWnd			= hWnd;
	m_deviceContext = deviceContext;
	m_camera		= camera;
}

void LevelTool::update(const float dt)
{
	m_terrain->setTargetDiameter(m_brushDiameter);

	POINT cursorPosition;
	if (GetCursorPos(&cursorPosition))
	{
		ScreenToClient(m_hWnd, &cursorPosition);
		Ray ray = m_camera->computeRay(cursorPosition);
		if (m_terrain->computeIntersection(ray))
			m_targetPosition = m_terrain->getTargetPosition();
	}

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetFocus() == m_hWnd)
		updateTerrainHeightmap(dt);

}

void LevelTool::onEvent(const std::string& sender, const std::string& eventName)
{
	if (sender == "Level Tool")
	{
		GUI::LevelToolWindow& levelToolWindow = GUI::LevelToolWindow::getInstance();

		if (eventName == "Brush Diameter")
			m_brushDiameter = (float)levelToolWindow.getTrackbarValue(eventName);
		else if (eventName == "Brush Hardness")
			m_brushHardness = (float)levelToolWindow.getTrackbarValue(eventName) / 100.f;
		else if (eventName == "Brush Strength")
			m_brushStrength = (float)levelToolWindow.getTrackbarValue(eventName);
	}
}

void LevelTool::updateTerrainHeightmap(const float dt)
{
	const UINT brushRadius = (UINT)ceil(m_brushDiameter / 2.f);
	std::vector<std::pair<float, float*>> vertices = m_terrain->getHeightmapDataWithinRadius(m_targetPosition, brushRadius);
	for (std::vector<std::pair<float, float*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		if ((*it).first <= 1.f)
		{
			float dy = (m_brushHardness < 0.99f) ? 1 - pow((*it).first, 1.f / (1.f - m_brushHardness)) : 1.f;
			if (dy > 0.f && m_state == Raising)
				*it->second += dy * dt * m_brushStrength;
			if (dy > 0.f && m_state == Lowering)
				*it->second -= dy * dt * m_brushStrength;
		}
	}
	m_terrain->updateHeightmapTexture(m_deviceContext);
}