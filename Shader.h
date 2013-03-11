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
	void setMatrix(const std::string& variable, const XMMATRIX& value);
	void setResource(const std::string& variable, ID3D11ShaderResourceView* value);
	void setRawData(const std::string& variable, const void* data, const UINT size);

	/*void SetFloat(char* variable, float value);
	void SetMatrix(char* variable, D3DXMATRIX& mat);
	void SetFloat3(char* variable, D3DXVECTOR3 value);
	void SetFloat4(char* variable, D3DXVECTOR4 value);
	void SetResource(char* variable, ID3D11ShaderResourceView* value);
	void SetUnorderedAccessView(char* variable, ID3D11UnorderedAccessView* value);
	void SetInt(char* variable, int value);
	void SetBool(char* variable, bool value);
	void SetRawData(char* variable, void* data, size_t size);*/
};

#endif