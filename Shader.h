#ifndef SHADER_H
#define SHADER_H

#include "StdAfx.h"

class Shader
{
	ID3D11Device*							m_Device;
	ID3D11DeviceContext*					m_DeviceContext;
	ID3D11InputLayout*						m_pInputLayout;

	ID3DX11Effect*							m_pEffect;
	ID3DX11EffectTechnique*					m_pTechnique;

public:
	Shader();
	~Shader();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCSTR filename,
				 const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements);

	void Apply();

	void setBool(const std::string& variable, const bool value);
	void setInt(const std::string& variable, const int value);
	void setFloat(const std::string& variable, const float value);
	void setFloat2(const std::string& variable, const XMFLOAT2 value);
	void setFloat3(const std::string& variable, const XMFLOAT3 value);
	void setFloat4(const std::string& variable, const XMFLOAT4 value);
	void setFrustum(const std::string& variable, const XMFLOAT4 value[6]);
	void setMatrix(const std::string& variable, const XMMATRIX& value);
	void setResource(const std::string& variable, ID3D11ShaderResourceView* value);
	void setRawData(const std::string& variable, const void* data, const UINT size);
};

#endif