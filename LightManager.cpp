#include "LightManager.h"


LightManager::LightManager(void)
{
	// Should be same as in Light.fx
	int POINTLIGHTS = 10;

	PointLight standardLight;
	standardLight.Ambient  = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	standardLight.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	standardLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 4.0f);
	standardLight.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	standardLight.Range    = -1.0f;
	standardLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
	standardLight.Pad	   = 0;

	for(int i = 0; i < POINTLIGHTS; i++)
	{
		m_Lights.push_back(standardLight);
	}

	m_state = Add;
	MouseDown = false;
}

LightManager::~LightManager(void)
{
}

void LightManager::init(HWND hWnd, ID3D11Device* device, Camera* camera)
{
	m_hWnd			= hWnd;
	m_camera		= camera;
	D3DX11CreateShaderResourceViewFromFile(device, "Content/img/light.png", 0, 0, &m_texture, 0 );
}

PointLight* LightManager::AddLight( XMFLOAT3 position, LightType type )
{
	for(int i = 0; i < m_Lights.size(); i++)
	{
		if (m_Lights[i].Range < 0)
		{
			m_Lights[i].Position	= position;
			m_Lights[i].Range		= 500;
			return &m_Lights[i];
		}
	}
	return NULL;
}
void LightManager::RemoveLight( PointLight* light )
{
	light->Position		= XMFLOAT3(0.0f, 1.0f, 0.0f);
	light->Range		= -1.f;
}
void LightManager::MoveLightY( PointLight* light, const Ray& ray )
{
	light->Position.y = computePlaneIntersection(light->Position.x, XMFLOAT3(1,0,0), ray).y;
}
void LightManager::MoveLightXZ( PointLight* light, const Ray& ray )
{
	light->Position = computePlaneIntersection(light->Position.y, XMFLOAT3(0,1,0), ray);
}
void LightManager::ClearLights()
{
	for(int i = 0; i < m_Lights.size(); i++)
	{
		RemoveLight(&m_Lights[i]);
	}
}

std::vector<PointLight> LightManager::getLights()
{
	return m_Lights;
}

void LightManager::update(float dt)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		POINT cursorPosition;
		if (GetCursorPos(&cursorPosition))
		{
			ScreenToClient(m_hWnd, &cursorPosition);
			Ray ray = m_camera->computeRay(cursorPosition);

			switch (m_state)
			{
			case Add:
				if (!MouseDown)
					m_light = AddLight(computePlaneIntersection(0.f, XMFLOAT3(0,1,0), ray), POINT_LIGHT);
				MoveLightY(m_light, ray);
				break;
			case Remove:
				m_light = computeIntersection(ray);
				if (m_light != NULL)
					RemoveLight(m_light);
				break;
			case MoveXZ:
				if (!MouseDown)
					m_light = computeIntersection(ray);
				if (m_light != NULL)
				{
					if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
						MoveLightY(m_light, ray);
					else
						MoveLightXZ(m_light, ray);
				}
				break;
			case MoveY:
				if (!MouseDown)
					m_light = computeIntersection(ray);
				if (m_light != NULL)
				{
					if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
						MoveLightXZ(m_light, ray);
					else
						MoveLightY(m_light, ray);
				}
				break;
			}
		}
		MouseDown = true;
	}
	else
		MouseDown = false;
}



void LightManager::render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera)
{
	struct data {
		PointLight* light;
		float order;
		data(PointLight* p, XMFLOAT3 cam)
		{
			light = p;
			XMVECTOR diff = XMLoadFloat3(&p->Position) - XMLoadFloat3(&cam);
			order = XMVectorGetX(XMVector3Dot(diff, diff));
		}
		bool operator<(const data& a) const
		{
			return order > a.order;
		}
	};

	std::vector<data> drawList;
	for(int i = 0; i < m_Lights.size(); i++)
	{
		if (m_Lights[i].Range > 0.f)
			drawList.push_back(data(&m_Lights[i], camera.getPosition()));
	}
	std::sort(drawList.begin(), drawList.end());

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	for (int i = 0; i < drawList.size(); i++) 
	{
		XMMATRIX world			= XMMatrixIdentity();
		XMMATRIX worldViewProj  = world * camera.getViewProj();
		XMFLOAT3 cameraPosition = camera.getPosition();
		shader->setMatrix("gWorld", world);
		shader->setMatrix("gWorldViewProj", worldViewProj);
		shader->setFloat3("gCameraPosition", cameraPosition);
		shader->setRawData("gLight", drawList[i].light, sizeof(PointLight));
		shader->setResource("gDiffuseMap", m_texture);

		shader->Apply();
		deviceContext->DrawIndexed(1, 0, 0);
	}
}

PointLight* LightManager::computeIntersection(const Ray& ray)
{
	PointLight* tempLight;
	float distance = -1;
	float light_r = 2.f;

	// Check for the closest light intersecting the ray.
	for(int i = 0; i < m_Lights.size(); i++)
	{
		if (m_Lights[i].Range > 0.f)
		{
		XMVECTOR light_pos = XMLoadFloat3(&m_Lights[i].Position);
		XMVECTOR l = light_pos - ray.origin;	
		float l2 = XMVectorGetX(XMVector3Dot(l, ray.direction));
		if (l2 > 0)
		{
			float l_sq = XMVectorGetX(XMVector3Dot(l, l));;
			float r_sq = light_r*light_r;
			if (l_sq > r_sq)
			{
				if(l_sq-l2*l2 <= r_sq) 
				{
					float b = XMVectorGetX(XMVector3Dot(ray.direction, (ray.origin - light_pos)));
					float c = XMVectorGetX(XMVector3Dot((ray.origin - light_pos), (ray.origin - light_pos)))-r_sq;
					float h = b*b-c;
					if (h > 0)
					{
						float t1 = -b + sqrtf(h);
						float t2 = -b - sqrtf(h);
						float t = t1 < t2 ? t1 : t2;
						if (t > 0)
						{
							if ((distance > 0 && distance > t) || distance < 0)
							{
								distance = t;
								tempLight = &m_Lights[i];
							}
						}
					}
				}
			}
		}
		}
	}
	if (distance > 0)
		return tempLight;
	else
		return NULL;
}
XMFLOAT3	LightManager::computePlaneIntersection(float Y, XMFLOAT3 normal, const Ray& ray)
{
	XMVECTOR pnormal = XMLoadFloat3(&normal);
	float t = (Y - XMVectorGetX(XMVector3Dot(ray.origin, pnormal))) / XMVectorGetX(XMVector3Dot(ray.direction, pnormal));
	XMFLOAT3 m_targetPosition;
	XMStoreFloat3(&m_targetPosition, ray.origin + t * ray.direction);
	return m_targetPosition;
}