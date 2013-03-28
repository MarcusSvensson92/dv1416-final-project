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
	if (m_undoStack.empty())
		addUndoStack();
	for (std::vector<std::pair<float, float*>>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		const float s = (m_brushHardness < 0.99f) ? 1 - pow(it->first, 1.f / (1.f - m_brushHardness)) : 1.f;
		if (s > 0.f)
		{
			if (m_undoStack.back().find(it->second) == m_undoStack.back().end())
				m_undoStack.back()[it->second] = *it->second;

			*it->second += s * y;

			*it->second = (*it->second < 0.f)   ? 0.f   : *it->second;
			*it->second = (*it->second > 255.f) ? 255.f : *it->second;
		}
	}
	m_terrain->updateHeightmapTexture(m_deviceContext);
}

void LevelTool::addUndoStack(void)
{
	m_undoStack.push_back(std::map<float*, float>());
}

void LevelTool::clearUndoStack(void)
{
	m_undoStack.clear();
	addUndoStack();
}

void LevelTool::undo(void)
{
	while (!m_undoStack.empty())
	{
		if (!m_undoStack.back().empty())
		{
			for (std::map<float*, float>::iterator it = m_undoStack.back().begin(); it != m_undoStack.back().end(); it++)
				*it->first = it->second;
			m_undoStack.back().clear();
			break;
		}
		else
			m_undoStack.pop_back();
	}

	if (m_undoStack.empty())
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