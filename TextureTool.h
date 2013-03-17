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

	void setHWnd(HWND hWnd) { m_hWnd = hWnd; }
	void setDevice(ID3D11Device* device) { m_device = device; }
	void setDeviceContext(ID3D11DeviceContext* deviceContext) { m_deviceContext = deviceContext; }
	void setTerrain(Terrain* terrain) { m_terrain = terrain; }
	void setTargetPosition(XMFLOAT3* targetPosition) { m_targetPosition = targetPosition; }

	void update(const float dt);

	void addUndoStack(void);
	void undo(void);

	void onEvent(const std::string& sender, const std::string& eventName);
private:
	HWND m_hWnd;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	Terrain* m_terrain;

	XMFLOAT3* m_targetPosition;
	XMVECTOR m_brush;
	UINT m_brushIndex;

	float m_brushDiameter;
	float m_brushStrength;

	std::vector<std::map<XMFLOAT4*, XMFLOAT4>> m_undoStacks;
};

#endif