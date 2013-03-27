#include "ShadowManager.h"


ShadowManager::ShadowManager(void)
{
	// Should be same as in Light.fx
	DLightShadowMap standardMap;
	standardMap.ShaderName = "gDLightShadow0";
	standardMap.MatrixName = "gDLVP0";
	standardMap.Light	   = NULL;
	m_DLightShadowMaps.push_back(standardMap);

	standardMap.ShaderName = "gDLightShadow1";
	standardMap.MatrixName = "gDLVP1";
	m_DLightShadowMaps.push_back(standardMap);
}

ShadowManager::~ShadowManager(void)
{
}

void ShadowManager::initDirectionalShadows( ID3D11Device* device, int res, float screenNear, float screenDepth, std::vector<Light::DirectionalLight>* Lights )
{
	for( int i = 0; i < m_DLightShadowMaps.size(); i++ )
	{
		RENDER_TARGET_DESC sMapDesc;
		sMapDesc.textureWidth	= res;
		sMapDesc.textureHeight	= res;
		sMapDesc.screenNear		= screenNear;
		sMapDesc.screenDepth	= screenDepth;
		sMapDesc.Type			= SHADOW_MAP;
		m_DLightShadowMaps[i].ShadowMap.Init(device, sMapDesc);
		m_DLightShadowMaps[i].Light = &(*Lights)[i];
	}
}

void ShadowManager::SetupShadowMaps(Shader* shader)
{
	for(int i = 0; i < m_DLightShadowMaps.size(); i++)
	{
		if (m_DLightShadowMaps[i].Light->Pad > 1)
		{
			// calc for view
			float distance = 1500;
			XMVECTOR shadowmapDirection = XMLoadFloat3( &m_DLightShadowMaps[i].Light->Direction );
			shadowmapDirection = XMVector3Normalize(shadowmapDirection);
			XMVECTOR shadowmapPos = shadowmapDirection*-1*distance;
			XMMATRIX view = XMMatrixLookAtLH(shadowmapPos, XMVectorZero(), XMLoadFloat3(&XMFLOAT3(1.0f,0.0f,0.0f)));
			XMMATRIX proj = XMMatrixPerspectiveFovLH(0.1f, 1.f, 1.f, 2000.f);

			shader->setMatrix(m_DLightShadowMaps[i].MatrixName, XMMatrixMultiply(view, proj));
			shader->setResource(m_DLightShadowMaps[i].ShaderName, m_DLightShadowMaps[i].ShadowMap.GetShaderResourceView());
		}
	}
}