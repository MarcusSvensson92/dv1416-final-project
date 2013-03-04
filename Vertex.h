#ifndef VERTEX_H
#define VERTEX_H

#include "StdAfx.h"

namespace Vertex
{
	struct Basic
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 tex0;
	};
	struct Light
	{
		XMFLOAT3 position;
		XMFLOAT2 size;
		XMFLOAT4 color;
	};
}

#endif