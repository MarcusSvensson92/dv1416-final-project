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

	void SetFloat(char* variable, float value);
	void SetMatrix(char* variable, D3DXMATRIX& mat);
	void SetFloat3(char* variable, D3DXVECTOR3 value);
	void SetFloat4(char* variable, D3DXVECTOR4 value);
	void SetResource(char* variable, ID3D11ShaderResourceView* value);
	void SetUnorderedAccessView(char* variable, ID3D11UnorderedAccessView* value);
	void SetInt(char* variable, int value);
	void SetBool(char* variable, bool value);
	void SetRawData(char* variable, void* data, size_t size);
};

#endif