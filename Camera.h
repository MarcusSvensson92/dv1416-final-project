#ifndef CAMERA_H
#define CAMERA_H

#include "Stdafx.h"

struct Ray
{
	XMVECTOR origin;
	XMVECTOR direction;
};

class Camera
{
public:
	Camera(void);
	~Camera(void);

	void setPosition(XMFLOAT3 position);
	void setPosition(float x, float y, float z);
	void setProj(UINT clientWidth, UINT clientHeight, float fovAngleY, float nearZ, float farZ); 

	XMFLOAT3 getPosition(void) const;
	XMMATRIX getView(void) const;
	XMMATRIX getProj(void) const;
	XMMATRIX getViewProj(void) const;

	void strafe(float d);
	void walk(float d);
	void pitch(float angle);
	void rotateY(float angle);

	void updateViewMatrix(void);

	Ray computeRay(POINT cursorPosition);
private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;

	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;

	UINT  m_clientWidth;
	UINT  m_clientHeight;
};

#endif