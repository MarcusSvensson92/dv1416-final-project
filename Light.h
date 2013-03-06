#ifndef LIGHT_H
#define LIGHT_H

#include "StdAfx.h"

namespace Light
{
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT3 Direction;
		float Pad;
	};
	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		XMFLOAT3 Position;
		float Range;

		XMFLOAT3 Att;
		float Pad;
	};

	struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }
		//std::string name;

		D3DXVECTOR4 Ambient;
		D3DXVECTOR4 Diffuse;
		D3DXVECTOR4 Specular; // w = SpecPower
		D3DXVECTOR4 Reflect;
	};
};

#endif