#include "StdAfx.h"
#include "Shader.h"

Shader::Shader(void)
{
	m_pEffect			= 0;
	m_pTechnique		= 0;
	m_pInputLayout		= 0;
}

Shader::~Shader(void)
{
	if( m_pInputLayout ) 
	{ 
		m_pInputLayout->Release();
		m_pInputLayout = NULL; 
	}
	if(m_pEffect)
	{
		while(m_pEffect->Release());
		m_pEffect = NULL;
	}
}

void Shader::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCSTR filename,
		const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements)
{	
	m_Device			= device;
	m_DeviceContext		= deviceContext;

	HRESULT hr = S_OK;

	ID3DBlob*		compShader = 0;
	ID3DBlob*		compMsgs = 0;

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	hr = D3DX11CompileFromFile(
		filename,
		0,
		0,
		0,
		"fx_5_0",
		dwShaderFlags,
		0,
		0,
		&compShader,
		&compMsgs,
		0
	);
	// any error with shader?
	if ( compMsgs != 0 )
	{
		MessageBoxA(0, (char*)compMsgs->GetBufferPointer(), 0, 0);
	}
	//other erros?
	if ( FAILED(hr) )
		MessageBoxA(0, "FX error 1", 0, 0);

	hr = D3DX11CreateEffectFromMemory(
		compShader->GetBufferPointer(),
		compShader->GetBufferSize(),
		dwShaderFlags,
		m_Device,
		&m_pEffect
		);
	//other erros?
	if ( FAILED(hr) )
		MessageBoxA(0, "FX error 2", 0, 0);

	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_PASS_DESC PassDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	hr = m_Device->CreateInputLayout(
			inputElementDesc,
			numElements,
			PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,
			&m_pInputLayout
	);
	//other erros?
	if ( FAILED(hr) )
		MessageBoxA(0, "Cannot create input layout.", "CreateInputLayout error", MB_OK | MB_ICONERROR);
}

void Shader::Apply()
{
	D3DX11_TECHNIQUE_DESC techDesc;
    m_pTechnique->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {	
		m_pTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		if(m_pInputLayout)
		{
			m_DeviceContext->IASetInputLayout(m_pInputLayout);
		}
    }
}

void Shader::SetFloat(char* variable, float value)
{
	m_pEffect->GetVariableByName(variable)->AsScalar()->SetFloat(value);
}
void Shader::SetMatrix(char* variable, D3DXMATRIX& mat)
{
	m_pEffect->GetVariableByName(variable)->AsMatrix()->SetMatrix((float*)&mat);
}
void Shader::SetFloat3(char* variable, D3DXVECTOR3 value)
{
	m_pEffect->GetVariableByName(variable)->AsVector()->SetFloatVector((float*)&value);
}
void Shader::SetFloat4(char* variable, D3DXVECTOR4 value)
{
	m_pEffect->GetVariableByName(variable)->AsVector()->SetFloatVector((float*)&value);
}
void Shader::SetResource(char* variable, ID3D11ShaderResourceView* value)
{
	m_pEffect->GetVariableByName(variable)->AsShaderResource()->SetResource(value);
}
void Shader::SetUnorderedAccessView(char* variable, ID3D11UnorderedAccessView* value)
{
	m_pEffect->GetVariableByName(variable)->AsUnorderedAccessView()->SetUnorderedAccessView(value);
}
void Shader::SetInt(char* variable, int value)
{
	m_pEffect->GetVariableByName(variable)->AsScalar()->SetInt(value);
}
void Shader::SetBool(char* variable, bool value)
{
	m_pEffect->GetVariableByName(variable)->AsScalar()->SetBool(value);
}
void Shader::SetRawData(char* variable, void* data, size_t size)
{
	m_pEffect->GetVariableByName(variable)->SetRawValue(data, 0, (UINT)size);
}