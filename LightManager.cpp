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
}

LightManager::~LightManager(void)
{
}

void LightManager::init(ID3D11Device* device)
{
	D3DX11CreateShaderResourceViewFromFile(device, "Content/img/light.png", 0, 0, &m_texture, 0 );
}

bool LightManager::AddLight( XMFLOAT3 position, LightType type )
{
	for(int i = 0; i < m_Lights.size(); i++)
	{
		if (m_Lights[i].Range < 0)
		{
			m_Lights[i].Position	= position;
			m_Lights[i].Range		= 500;
			return true;
		}
	}
	return false;
}
void LightManager::RemoveLight( PointLight* light )
{
	light->Position		= XMFLOAT3(0.0f, 1.0f, 0.0f);
	light->Range		= -1.f;
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