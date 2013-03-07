#include "StdAfx.h"
#include "LevelTool.h"
#include "LevelToolWindow.h"

LevelTool::LevelTool(void)
{
	m_targetPosition = XMFLOAT3(0.f, 0.f, 0.f);

	m_brushDiameter = 10.f;
	m_brushHardness = 0.5f;
	m_brushOpacity  = 1.f;
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
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		POINT cursorPosition;
		if (GetCursorPos(&cursorPosition))
		{
			ScreenToClient(m_hWnd, &cursorPosition);

			computeTargetPosition(cursorPosition);

			updateTerrainAltitude(dt);
		}
	}
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
		else if (eventName == "Brush Opacity")
			m_brushOpacity = (float)levelToolWindow.getTrackbarValue(eventName) / 100.f;
	}
}

void LevelTool::computeTargetPosition(POINT cursorPosition)
{
	Ray ray = m_camera->computeRay(cursorPosition);
	XMVECTOR n = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	float t = -XMVectorGetX(XMVector3Dot(ray.origin, n)) / XMVectorGetX(XMVector3Dot(ray.direction, n));
	if (t > 0.f)
		XMStoreFloat3(&m_targetPosition, ray.origin + (t * ray.direction));
}

void LevelTool::updateTerrainAltitude(const float dt)
{
	const UINT brushRadius = (UINT)ceil(m_brushDiameter / 2.f);
	XMVECTOR targetPosition = XMVectorSet(m_targetPosition.x, m_targetPosition.z, 0.f, 0.f);
	std::vector<std::pair<XMFLOAT2, float*>> vertices = m_terrain->getHeightmapDataWithinRadius(m_targetPosition, brushRadius);
	for (std::vector<std::pair<XMFLOAT2, float*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		XMVECTOR vertexPosition = XMLoadFloat2(&it->first);
		const float length = XMVectorGetX(XMVector2Length(vertexPosition - targetPosition)) / brushRadius;
		if (length <= 1.f)
		{
			const float dy = (m_brushHardness < 0.99f) ? 1 - pow(length, 1.f / (1.f - m_brushHardness)) : 1.f;
			if (dy > 0.f)
				*it->second += dy * dt * m_brushOpacity * 5.f;
		}
	}
	m_terrain->updateHeightmapTexture(m_deviceContext);
}