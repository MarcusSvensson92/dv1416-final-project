#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "StdAfx.h"
#include "Vertex.h"
#include "Light.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include "EventReceiver.h"
#include "PointLightOptions.h"
#include "DirectionalLightOptions.h"

#include <algorithm>

using namespace Light;

enum LightType
{
	POINT_LIGHT,
};

class LightManager : public GUI::EventReceiver
{
public:
	enum State
	{
		Add,
		Remove,
		MoveXZ,
		MoveY
	};

	LightManager(void);
	~LightManager(void);

	void		init(HWND hWnd, ID3D11Device* device, Camera* camera);

	void		setState(State state) { m_state = state; }

	PointLight*	AddLight(XMFLOAT3, LightType);
	void		RemoveLight(PointLight*);
	void		MoveLightY(PointLight*,const Ray&);
	void		MoveLightXZ(PointLight*,const Ray&);
	void		ClearLights();
	void		update(float);

	PointLight*	computeIntersection(const Ray& ray);
	XMFLOAT3	computePlaneIntersection(float Y, XMFLOAT3 normal, const Ray& ray);

	std::vector<PointLight>			getPLights();
	std::vector<DirectionalLight>	getDLights();
	std::vector<DirectionalLight>*	getDLightsPointer() { return &m_DLights; };

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);

	void onEvent(const std::string& sender, const std::string& eventName);
private:
	std::vector<PointLight>				m_PLights;
	std::vector<DirectionalLight>		m_DLights;
	ID3D11ShaderResourceView*			m_texture;

	State m_state;

	HWND m_hWnd;
	Camera* m_camera;

	bool MouseDown;
	PointLight* m_light;
};

#endif