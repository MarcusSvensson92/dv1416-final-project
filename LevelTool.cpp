#include "StdAfx.h"
#include "LevelTool.h"
#include "LevelToolWindow.h"

LevelTool::LevelTool(void)
{
	m_brushDiameter = 10.f;
	m_brushHardness = 0.5f;
	m_brushStrength = 5.f;
}

LevelTool::~LevelTool(void) { }

void LevelTool::update(const float dt)
{
	const UINT brushRadius = (UINT)ceil(m_brushDiameter / 2.f);
	std::vector<std::pair<float, float*>> vertices = m_terrain->getHeightmapDataWithinRadius(*m_targetPosition, brushRadius);
	for (std::vector<std::pair<float, float*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		if ((*it).first <= 1.f)
		{
			float dy = (m_brushHardness < 0.99f) ? 1 - pow((*it).first, 1.f / (1.f - m_brushHardness)) : 1.f;
			if (dy > 0.f)
				*it->second += dy * dt * m_brushStrength;
		}
	}
	m_terrain->updateHeightmapTexture(m_deviceContext);
}

void LevelTool::onEvent(const std::string& sender, const std::string& eventName)
{
	if (sender == "Level Tool")
	{
		GUI::LevelToolWindow& levelToolWindow = GUI::LevelToolWindow::getInstance();

		if (eventName == "Brush Diameter")
		{
			m_brushDiameter = (float)levelToolWindow.getTrackbarValue(eventName);
			m_terrain->setTargetDiameter(m_brushDiameter);
		}
		else if (eventName == "Brush Hardness")
			m_brushHardness = (float)levelToolWindow.getTrackbarValue(eventName) / 100.f;
		else if (eventName == "Brush Strength")
			m_brushStrength = (float)levelToolWindow.getTrackbarValue(eventName);
	}
}