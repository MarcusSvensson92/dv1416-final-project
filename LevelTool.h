#ifndef LEVELTOOL_H
#define LEVELTOOL_H

#include "StdAfx.h"
#include "Tool.h"
#include "Terrain.h"

class LevelTool : public Tool
{
public:
	LevelTool(void);
	~LevelTool(void);

	void setDeviceContext(ID3D11DeviceContext* deviceContext) { m_deviceContext = deviceContext; }
	void setTerrain(Terrain* terrain) { m_terrain = terrain; }
	void setTargetPosition(XMFLOAT3* targetPosition) { m_targetPosition = targetPosition; }

	void update(const float dt);

	void addUndoStack(void);
	void clearUndoStack(void);
	void undo(void);

	void onEvent(const std::string& sender, const std::string& eventName);
private:
	ID3D11DeviceContext* m_deviceContext;

	Terrain* m_terrain;

	XMFLOAT3* m_targetPosition;

	float m_brushDiameter;
	float m_brushHardness;
	float m_brushStrength;

	std::vector<std::map<float*, float>> m_undoStack;
};

#endif