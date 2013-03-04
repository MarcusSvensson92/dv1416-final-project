#ifndef UTILITIES_H
#define UTILITIES_H

#include "Stdafx.h"

namespace Utilities
{
	ID3D11ShaderResourceView* createTexture2DArraySRV(ID3D11Device* device,
													  ID3D11DeviceContext* deviceContext,
												      std::vector<std::string>& filenames);
}

#endif