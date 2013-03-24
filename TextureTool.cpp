#include "StdAfx.h"
#include "TextureTool.h"
#include "TextureToolWindow.h"
#include "GUI.h"

TextureTool::TextureTool(void)
{
	m_brushDiameter = 10.f;
	m_brushStrength = 5.f;

	m_brush = XMVectorSet(1.f, -1.f, -1.f, -1.f);
	m_brushIndex = 0;
}

TextureTool::~TextureTool(void) { }

void TextureTool::update(const float dt)
{
	const UINT brushRadius = (UINT)ceil(m_brushDiameter / 2.f);
	std::vector<std::pair<float, XMFLOAT4*>> vertices = m_terrain->getBlendmapDataWithinRadius(*m_targetPosition, brushRadius);
	const XMVECTOR brush = m_brush * dt * m_brushStrength;
	if (m_undoStacks.empty())
		addUndoStack();
	for (std::vector<std::pair<float, XMFLOAT4*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		if (m_undoStacks.back().find(it->second) == m_undoStacks.back().end())
			m_undoStacks.back()[it->second] = *it->second;

		XMVECTOR texel = XMLoadFloat4(it->second);
		texel += brush;
		texel = XMVectorSaturate(texel);
		XMStoreFloat4(it->second, texel);
	}
	m_terrain->updateBlendmapTexture(m_deviceContext);
}

void TextureTool::addUndoStack(void)
{
	m_undoStacks.push_back(std::map<XMFLOAT4*, XMFLOAT4>());
}

void TextureTool::undo(void)
{
	while (!m_undoStacks.empty())
	{
		if (!m_undoStacks.back().empty())
		{
			for (std::map<XMFLOAT4*, XMFLOAT4>::iterator it = m_undoStacks.back().begin(); it != m_undoStacks.back().end(); it++)
				*it->first = it->second;
			m_undoStacks.back().clear();
			break;
		}
		else
			m_undoStacks.pop_back();
	}

	if (m_undoStacks.empty())
		addUndoStack();

	m_terrain->updateBlendmapTexture(m_deviceContext);
}

void TextureTool::onEvent(const std::string& sender, const std::string& eventName)
{
	if (sender == "Texture Tool")
	{
		GUI::TextureToolWindow& textureToolWindow = GUI::TextureToolWindow::getInstance();

		std::string filepath;

		if (eventName == "Texture R")
		{
			m_brush = XMVectorSet(1.f, -1.f, -1.f, -1.f);
			m_brushIndex = 0;
		}
		else if (eventName == "Texture G")
		{
			m_brush = XMVectorSet(-1.f, 1.f, -1.f, -1.f);
			m_brushIndex = 1;
		}
		else if (eventName == "Texture B")
		{
			m_brush = XMVectorSet(-1.f, -1.f, 1.f, -1.f);
			m_brushIndex = 2;
		}
		else if (eventName == "Texture A")
		{
			m_brush = XMVectorSet(-1.f, -1.f, -1.f, 1.f);
			m_brushIndex = 3;
		}

		else if (eventName == "Load Texture" && GUI::openFileBox(m_hWnd, "PNG Files (*.png)\0*.png\0All Files (*.*)\0*.*\0", "png", filepath))
			m_terrain->loadLayermap(m_device, m_brushIndex, filepath);

		else if (eventName == "Brush Diameter")
		{
			m_brushDiameter = (float)textureToolWindow.getTrackbarValue(eventName);
			m_terrain->setTargetDiameter(m_brushDiameter);
		}
		else if (eventName == "Brush Strength")
			m_brushStrength = (float)textureToolWindow.getTrackbarValue(eventName) / 10.f;
	}
}