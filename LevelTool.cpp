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
	const float y = m_brushStrength * dt;
	if (m_undoStacks.empty())
		addUndoStack();
	for (std::vector<std::pair<float, float*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		const float s = (m_brushHardness < 0.99f) ? 1 - pow(it->first, 1.f / (1.f - m_brushHardness)) : 1.f;
		if (s > 0.f)
		{
			if (m_undoStacks.back().find(it->second) == m_undoStacks.back().end())
				m_undoStacks.back()[it->second] = *it->second;

			*it->second += s * y;

			*it->second = (*it->second < 0.f)   ? 0.f   : *it->second;
			*it->second = (*it->second > 255.f) ? 255.f : *it->second;
		}
	}
	m_terrain->updateHeightmapTexture(m_deviceContext);
}

void LevelTool::addUndoStack(void)
{
	m_undoStacks.push_back(std::map<float*, float>());
}

void LevelTool::undo(void)
{
	while (!m_undoStacks.empty())
	{
		if (!m_undoStacks.back().empty())
		{
			for (std::map<float*, float>::iterator it = m_undoStacks.back().begin(); it != m_undoStacks.back().end(); it++)
				*it->first = it->second;
			m_undoStacks.back().clear();
			break;
		}
		else
			m_undoStacks.pop_back();
	}

	if (m_undoStacks.empty())
		addUndoStack();

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