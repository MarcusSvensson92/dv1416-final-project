#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "StdAfx.h"
#include "Vertex.h"
#include "Light.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include <algorithm>

using namespace Light;

enum LightType
{
	POINT_LIGHT,
};

class LightManager
{
public:
	LightManager(void);
	~LightManager(void);

	void		init(ID3D11Device* device);

	void		AddLight(XMFLOAT3, LightType);
	PointLight*	computeIntersection(const Ray& ray);

	std::vector<PointLight> getLights();

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);
private:
	std::vector<PointLight>				m_Lights;
	ID3D11ShaderResourceView*			m_texture;
};

#endif