#ifndef SHADOWMANAGER_H
#define SHADOWMANAGER_H

#include "StdAfx.h"
#include "renderTarget.h"
#include "Light.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include <algorithm>

struct DLightShadowMap
{
	DLightShadowMap() { ZeroMemory(this, sizeof(this)); }

	std::string					ShaderName;
	std::string					MatrixName;
	renderTarget				ShadowMap;
	Light::DirectionalLight*	Light;
};

class ShadowManager
{
public:
	ShadowManager(void);
	~ShadowManager(void);

	void		init(ID3D11Device* device);

	void		initDirectionalShadows(ID3D11Device*, int, float, float, std::vector<Light::DirectionalLight>*);

	void		SetupShadowMaps(Shader*);

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);

	std::vector<DLightShadowMap>		m_DLightShadowMaps;
};

#endif

