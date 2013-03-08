#ifndef LEVELTOOL_H
#define LEVELTOOL_H

#include "StdAfx.h"
#include "Camera.h"
#include "Tool.h"
#include "Terrain.h"

class LevelTool : public Tool
{
public:
	LevelTool(void);
	~LevelTool(void);

	void setTerrain(Terrain* terrain) { m_terrain = terrain; }

	void init(HWND hWnd, ID3D11DeviceContext* deviceContext, Camera* camera);
	void update(const float dt);
protected:
	void onEvent(const std::string& sender, const std::string& eventName);
private:
	HWND m_hWnd;
	Camera* m_camera;
	ID3D11DeviceContext* m_deviceContext;

	Terrain* m_terrain;

	XMFLOAT3 m_targetPosition;

	float m_brushDiameter;
	float m_brushHardness;
	float m_brushStrength;

	void computeTargetPosition(POINT cursorPosition);

	void updateTerrainAltitude(const float dt);
};

#endif