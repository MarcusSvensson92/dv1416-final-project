#include "Stdafx.h"
#include "Camera.h"

Camera::Camera(void)
	: m_position(0.0f, 0.0f, 0.0f),
	  m_right(1.0f, 0.0f, 0.0f),
	  m_up(0.0f, 1.0f, 0.0f),
	  m_look(0.0f, 0.0f, 1.0f)
{
}

Camera::~Camera(void)
{
}

void Camera::setPosition(XMFLOAT3 position)
{
	m_position = position;
}

void Camera::setPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void Camera::setProj(UINT clientWidth, UINT clientHeight, float fovAngleY, float nearZ, float farZ)
{
	m_clientWidth  = clientWidth;
	m_clientHeight = clientHeight;

	XMMATRIX P = XMMatrixPerspectiveFovLH(fovAngleY, (float)clientWidth / (float)clientHeight, nearZ, farZ);
	XMStoreFloat4x4(&m_proj, P);
}

XMFLOAT3 Camera::getPosition(void) const
{
	return m_position;
}

XMMATRIX Camera::getView(void) const
{
	return XMLoadFloat4x4(&m_view);
}

XMMATRIX Camera::getProj(void) const
{
	return XMLoadFloat4x4(&m_proj);
}

XMMATRIX Camera::getViewProj(void) const
{
	return XMMatrixMultiply(getView(), getProj());
}

void Camera::strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void Camera::rotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_up,    XMVector3TransformNormal(XMLoadFloat3(&m_up),    R));
	XMStoreFloat3(&m_look,  XMVector3TransformNormal(XMLoadFloat3(&m_look),  R));
}

void Camera::updateViewMatrix(void)
{
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR u = XMLoadFloat3(&m_up);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);

	l = XMVector3Normalize(l);
	u = XMVector3Normalize(XMVector3Cross(l, r));

	r = XMVector3Cross(u, l);

	float x = -XMVectorGetX(XMVector3Dot(p, r));
	float y = -XMVectorGetX(XMVector3Dot(p, u));
	float z = -XMVectorGetX(XMVector3Dot(p, l));

	XMStoreFloat3(&m_right, r);
	XMStoreFloat3(&m_up, u);
	XMStoreFloat3(&m_look, l);

	m_view(0, 0) = m_right.x;
	m_view(1, 0) = m_right.y;
	m_view(2, 0) = m_right.z;
	m_view(3, 0) = x;

	m_view(0, 1) = m_up.x;
	m_view(1, 1) = m_up.y;
	m_view(2, 1) = m_up.z;
	m_view(3, 1) = y;

	m_view(0, 2) = m_look.x;
	m_view(1, 2) = m_look.y;
	m_view(2, 2) = m_look.z;
	m_view(3, 2) = z;

	m_view(0, 3) = 0.0f;
	m_view(1, 3) = 0.0f;
	m_view(2, 3) = 0.0f;
	m_view(3, 3) = 1.0f;
}

Ray Camera::computeRay(POINT cursorPosition)
{
	float x = ((2.f * (float)cursorPosition.x) / (float)m_clientWidth) - 1.f;
	float y = (((2.f * (float)cursorPosition.y) / (float)m_clientHeight) - 1.f) * -1.f;

	x /= m_proj._11;
	y /= m_proj._22;

	XMVECTOR determinant; // Dummy
	XMMATRIX inverseView = XMMatrixInverse(&determinant, getView());

	Ray ray;
	ray.origin	  = XMLoadFloat3(&m_position);
	ray.direction = XMVector3Normalize(XMVectorSet(
						x * inverseView._11 + y * inverseView._21 + inverseView._31,
						x * inverseView._12 + y * inverseView._22 + inverseView._32,
						x * inverseView._13 + y * inverseView._23 + inverseView._33,
						0.f));
	return ray;
}