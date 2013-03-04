#include "Stdafx.h"
#include "Utilities.h"

namespace Utilities
{
	ID3D11ShaderResourceView* createTexture2DArraySRV(ID3D11Device* device,
													  ID3D11DeviceContext* deviceContext,
													  std::vector<std::string>& filenames)
	{
		UINT size = filenames.size();
		std::vector<ID3D11Texture2D*> textures(size);
		for (UINT i = 0; i < size; i++)
		{
			D3DX11_IMAGE_LOAD_INFO loadInfo;
			loadInfo.Width		    = D3DX11_FROM_FILE;
			loadInfo.Height		    = D3DX11_FROM_FILE;
			loadInfo.Depth		    = D3DX11_FROM_FILE;
			loadInfo.FirstMipLevel  = 0;
			loadInfo.MipLevels	    = D3DX11_FROM_FILE;
			loadInfo.Usage		    = D3D11_USAGE_STAGING;
			loadInfo.BindFlags	    = 0;
			loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			loadInfo.MiscFlags		= 0;
			loadInfo.Format			= DXGI_FORMAT_FROM_FILE;
			loadInfo.Filter			= D3DX11_FILTER_NONE;
			loadInfo.MipFilter		= D3DX11_FILTER_LINEAR;
			loadInfo.pSrcInfo		= 0;

			D3DX11CreateTextureFromFile(device, filenames[i].c_str(), &loadInfo,
										NULL, (ID3D11Resource**)&textures[i], NULL);
		}

		D3D11_TEXTURE2D_DESC textureDesc;
		textures[0]->GetDesc(&textureDesc);

		D3D11_TEXTURE2D_DESC textureArrayDesc;
		textureArrayDesc.Width				= textureDesc.Width;
		textureArrayDesc.Height				= textureDesc.Height;
		textureArrayDesc.MipLevels			= textureDesc.MipLevels;
		textureArrayDesc.ArraySize			= size;
		textureArrayDesc.Format				= textureDesc.Format;
		textureArrayDesc.SampleDesc.Count	= 1;
		textureArrayDesc.SampleDesc.Quality = 0;
		textureArrayDesc.Usage				= D3D11_USAGE_DEFAULT;
		textureArrayDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureArrayDesc.CPUAccessFlags		= 0;
		textureArrayDesc.MiscFlags			= 0;

		ID3D11Texture2D* textureArray = NULL;
		device->CreateTexture2D(&textureArrayDesc, NULL, &textureArray);

		for (UINT texture = 0; texture < size; texture++)
		{
			for (UINT mipLevel = 0; mipLevel < textureDesc.MipLevels; mipLevel++)
			{
				D3D11_MAPPED_SUBRESOURCE mappedTexture;
				deviceContext->Map(textures[texture], mipLevel, D3D11_MAP_READ, 0, &mappedTexture);
				deviceContext->UpdateSubresource(textureArray, D3D11CalcSubresource(mipLevel, texture, textureDesc.MipLevels),
												 NULL, mappedTexture.pData, mappedTexture.RowPitch, mappedTexture.DepthPitch);

				deviceContext->Unmap(textures[texture], mipLevel);
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format							= textureArrayDesc.Format;
		viewDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels		= textureArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize		= size;

		ID3D11ShaderResourceView* textureArraySRV = NULL;
		device->CreateShaderResourceView(textureArray, &viewDesc, &textureArraySRV);

		RELEASE(textureArray);
		for (UINT texture = 0; texture < size; texture++)
			RELEASE(textures[texture]);

		return textureArraySRV;
	}
}