#include "ShadowManager.h"


ShadowManager::ShadowManager(void)
{
	// Should be same as in Light.fx
	int POINTLIGHTS = 10;

	std::string Names [6] = { "ShadowMap0", "ShadowMap1", "ShadowMap0", "ShadowMap1", "ShadowMap0", "ShadowMap1" };

	PLightShadowMap standardMap;
	standardMap.ShaderNames;
	standardMap.MatrixNames;
	standardMap.Light;
}

ShadowManager::~ShadowManager(void)
{
}

bool ShadowManager::AddPointShadow( ID3D11Device* device, int res, float screenNear, float screenDepth, Light::PointLight* pLight )
{
	for(int i = 0; i < m_PLightShadowMaps.size(); i++)
	{
		if (m_PLightShadowMaps[i].Light == NULL || m_PLightShadowMaps[i].Light->Pad == 0)
		{
			RENDER_TARGET_DESC sMapDesc;
			sMapDesc.textureWidth	= res;
			sMapDesc.textureHeight	= res;
			sMapDesc.screenNear		= screenNear;
			sMapDesc.screenDepth	= screenDepth;
			sMapDesc.Type			= SHADOW_MAP;
			m_PLightShadowMaps[i].ShadowMaps[0].Init(device, sMapDesc);
			m_PLightShadowMaps[i].ShadowMaps[1].Init(device, sMapDesc);
			m_PLightShadowMaps[i].ShadowMaps[2].Init(device, sMapDesc);
			m_PLightShadowMaps[i].ShadowMaps[3].Init(device, sMapDesc);
			m_PLightShadowMaps[i].ShadowMaps[4].Init(device, sMapDesc);
			m_PLightShadowMaps[i].ShadowMaps[5].Init(device, sMapDesc);
			m_PLightShadowMaps[i].Light->Pad = m_PLightShadowMaps[i].id;
		}
	}
	return false;
}
void ShadowManager::RemovePointShadow( Light::PointLight* light )
{
	light->Pad = 0;
}
void ShadowManager::ClearPointShadows()
{
	// not finished
}

void ShadowManager::SetupShadowMaps(Shader* shader)
{
	for(int i = 0; i < m_PLightShadowMaps.size(); i++)
	{
		if (m_PLightShadowMaps[i].Light != NULL || m_PLightShadowMaps[i].Light->Pad != 0)
		{
			for (int j = 0; j < 6; j++)
			{
				//shader->setMatrix(m_PLightShadowMaps[i].MatrixName[j], /* calc for view proj here */);
				shader->setResource(m_PLightShadowMaps[i].ShaderNames[j], m_PLightShadowMaps[i].ShadowMaps[j].GetShaderResourceView());
			}
		}
	}
}
void ShadowManager::CreatePLightShadowMap(int id,
										std::string t1, std::string t2, std::string t3, std::string t4, std::string t5, std::string t6,
										std::string m1, std::string m2, std::string m3, std::string m4, std::string m5, std::string m6)
{

}