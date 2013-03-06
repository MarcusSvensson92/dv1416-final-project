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

	std::ifstream file(filename, std::ios::binary);
	if (!file) return;

	file.seekg(0, std::ios_base::end);
	int size = (int)file.tellg();
	file.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	file.read(&compiledShader[0], size);
	file.close();

	hr = D3DX11CreateEffectFromMemory(
		&compiledShader[0],
		size,
		0,
		m_Device,
		&m_pEffect
		);
	//other erros?
	if ( FAILED(hr) )
		MessageBoxA(0, "FX error 2", 0, 0);

		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	if (inputElementDesc)
	{
		D3DX11_PASS_DESC PassDesc;
		m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
		hr = m_Device->CreateInputLayout(
				inputElementDesc,
				numElements,
				PassDesc.pIAInputSignature,
				PassDesc.IAInputSignatureSize,
				&m_pInputLayout
		);
		if ( FAILED(hr) )
			MessageBoxA(0, "Cannot create input layout.", "CreateInputLayout error", MB_OK | MB_ICONERROR);
	}
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

void Shader::setBool(const std::string& variable, const bool value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsScalar()->SetBool(value);
}

void Shader::setInt(const std::string& variable, const int value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsScalar()->SetInt(value);
}

void Shader::setFloat(const std::string& variable, const float value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsScalar()->SetFloat(value);
}

void Shader::setFloat3(const std::string& variable, const XMFLOAT3 value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsVector()->SetFloatVector((float*)&value);
}

void Shader::setFloat4(const std::string& variable, const XMFLOAT4 value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsVector()->SetFloatVector((float*)&value);
}

void Shader::setMatrix(const std::string& variable, const XMMATRIX& value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsMatrix()->SetMatrix((float*)&value);
}

void Shader::setResource(const std::string& variable, ID3D11ShaderResourceView* value)
{
	m_pEffect->GetVariableByName(variable.c_str())->AsShaderResource()->SetResource(value);
}

void Shader::setRawData(const std::string& variable, const void* data, const UINT size)
{
	m_pEffect->GetVariableByName(variable.c_str())->SetRawValue(data, 0, size);
}

/*void Shader::SetFloat(char* variable, float value)
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
}*/