#include "StdAfx.h"
#include "TextureTool.h"
#include "TextureToolWindow.h"
#include "GUI.h"

TextureTool::TextureTool(void)
{
	m_targetPosition = XMFLOAT3(0.f, 0.f, 0.f);

	m_brushDiameter = 10.f;
	m_brushStrength = 5.f;

	m_brush = XMVectorSet(m_brushStrength, -m_brushStrength, -m_brushStrength, -m_brushStrength);
	m_brushIndex = 0;
}

TextureTool::~TextureTool(void)
{
}

void TextureTool::init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* camera)
{
	m_hWnd			= hWnd;
	m_device		= device;
	m_deviceContext = deviceContext;
	m_camera		= camera;
}

void TextureTool::update(const float dt)
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
		updateTerrainBlendmap(dt);
}

void TextureTool::onEvent(const std::string& sender, const std::string& eventName)
{
	if (sender == "Texture Tool")
	{
		GUI::TextureToolWindow& textureToolWindow = GUI::TextureToolWindow::getInstance();

		std::string filepath;

		if (eventName == "Texture R")
		{
			m_brush = XMVectorSet(m_brushStrength, -m_brushStrength, -m_brushStrength, -m_brushStrength);
			m_brushIndex = 0;
		}
		else if (eventName == "Texture G")
		{
			m_brush = XMVectorSet(-m_brushStrength, m_brushStrength, -m_brushStrength, -m_brushStrength);
			m_brushIndex = 1;
		}
		else if (eventName == "Texture B")
		{
			m_brush = XMVectorSet(-m_brushStrength, -m_brushStrength, m_brushStrength, -m_brushStrength);
			m_brushIndex = 2;
		}
		else if (eventName == "Texture A")
		{
			m_brush = XMVectorSet(-m_brushStrength, -m_brushStrength, -m_brushStrength, m_brushStrength);
			m_brushIndex = 3;
		}

		else if (eventName == "Load Texture" && GUI::openFileBox(m_hWnd, "", filepath))
			m_terrain->loadLayermap(m_device, m_brushIndex, filepath);

		else if (eventName == "Brush Diameter")
			m_brushDiameter = (float)textureToolWindow.getTrackbarValue(eventName);
		else if (eventName == "Brush Strength")
		{
			m_brush /= m_brushStrength;
			m_brushStrength = (float)textureToolWindow.getTrackbarValue(eventName) / 10.f;
			m_brush *= m_brushStrength;
		}
	}
}

void TextureTool::updateTerrainBlendmap(const float dt)
{
	const UINT brushRadius = (UINT)ceil(m_brushDiameter / 2.f);
	XMVECTOR targetPosition = XMVectorSet(m_targetPosition.x, m_targetPosition.z, 0.f, 0.f);
	std::vector<std::pair<float, XMFLOAT4*>> vertices = m_terrain->getBlendmapDataWithinRadius(m_targetPosition, brushRadius);
	for (std::vector<std::pair<float, XMFLOAT4*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		if (it->first <= 1.f)
		{
			XMVECTOR texel = XMLoadFloat4(&(*it->second));
			texel += m_brush * dt;
			texel = XMVectorSaturate(texel);
			XMStoreFloat4(it->second, texel);
		}
	}
	m_terrain->updateBlendmapTexture(m_deviceContext);
}