#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "StdAfx.h"
#include "Shader.h"

class ShaderManager
{
public:
	ShaderManager(void);
	~ShaderManager(void);

	void	init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool	add(const std::string& keyword, const std::string& filename,
			    const D3D11_INPUT_ELEMENT_DESC* inputDesc, const UINT elementCount);
	Shader* get(const std::string& keyword);
private:
	std::map<const std::string, Shader*> m_shaders;

	ID3D11Device*		 m_device;
	ID3D11DeviceContext* m_deviceContext;
};

#endif