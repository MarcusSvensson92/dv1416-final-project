#ifndef SHADOWMANAGER_H
#define SHADOWMANAGER_H

#include "StdAfx.h"
#include "renderTarget.h"
#include "Light.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include <algorithm>

struct PLightShadowMap
{
	int							id;
	std::string					ShaderName [6];
	std::string					MatrixName [6];
	renderTarget				ShadowMaps [6];
	Light::PointLight*			Light;
};
struct DLightShadowMap
{
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

	bool		AddPointShadow(ID3D11Device*, int, float, float, Light::PointLight*);
	void		RemovePointShadow(Light::PointLight*);
	void		ClearPointShadows();

	void		SetupShadowMaps(Shader*);

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);
private:
	std::vector<PLightShadowMap>		m_PLightShadowMaps;
	std::vector<DLightShadowMap>		m_DLightShadowMaps;
};

#endif

