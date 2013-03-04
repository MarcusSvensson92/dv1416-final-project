#ifndef LIGHT_H
#define LIGHT_H

#include "StdAfx.h"

namespace Light
{
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
};

#endif