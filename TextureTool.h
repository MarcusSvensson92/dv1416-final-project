#ifndef TEXTURETOOL_H
#define TEXTURETOOL_H

#include "StdAfx.h"
#include "Tool.h"
#include "Terrain.h"
#include "Camera.h"

class TextureTool : public Tool
{
public:
	TextureTool(void);
	~TextureTool(void);

	void setTerrain(Terrain* terrain) { m_terrain = terrain; }

	void init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* camera);
	void update(const float dt);
protected:
	void onEvent(const std::string& sender, const std::string& eventName);
private:
	HWND m_hWnd;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	Camera* m_camera;

	Terrain* m_terrain;

	XMFLOAT3 m_targetPosition;
	XMFLOAT4 m_brush;
	UINT m_brushIndex;

	float m_brushDiameter;
	float m_brushStrength;

	void computeTargetPosition(POINT cursorPosition);

	void updateTerrainBlendmap(const float dt);
};

#endif