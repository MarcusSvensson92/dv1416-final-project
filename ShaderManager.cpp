#include "StdAfx.h"
#include "ShaderManager.h"

ShaderManager::ShaderManager(void) { }

ShaderManager::~ShaderManager(void)
{
	for (std::map<const std::string, Shader*>::iterator it = m_shaders.begin(); it != m_shaders.end(); it++)
		delete it->second;
}

void ShaderManager::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device		= device;
	m_deviceContext = deviceContext;
}

bool ShaderManager::add(const std::string& keyword, const std::string& filename,
						const D3D11_INPUT_ELEMENT_DESC* inputDesc, const UINT elementCount)
{
	for (std::map<const std::string, Shader*>::iterator it = m_shaders.begin(); it != m_shaders.end(); it++)
		if (it->first == filename)
			return false;
	Shader* shader = new Shader();
	shader->Init(m_device, m_deviceContext, filename.c_str(), inputDesc, elementCount);
	m_shaders.insert(std::pair<const std::string, Shader*>(keyword, shader));
	return true;
}

Shader* ShaderManager::get(const std::string& keyword)
{
	for (std::map<const std::string, Shader*>::iterator it = m_shaders.begin(); it != m_shaders.end(); it++)
		if (it->first == keyword)
			return it->second;
	return NULL;
}