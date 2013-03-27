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

	bool loadPNG(const std::string& filename, PNGData& data)
	{
		FILE* file = fopen(filename.c_str(), "rb");
		if (!file)
			return false;

		png_byte header[8];
		fread(header, 1, 8, file);
		if (png_sig_cmp(header, 0, 8))
			return false;

		png_structp readStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!readStruct)
			return false;

		png_infop infoStruct = png_create_info_struct(readStruct);
		if (!infoStruct)
		{
			png_destroy_read_struct(&readStruct, (png_infopp)NULL, (png_infopp)NULL);
			return false;
		}

		if (setjmp(png_jmpbuf(readStruct)))
		{
			png_destroy_read_struct(&readStruct, &infoStruct, (png_infopp)NULL);
			return false;
		}

		png_init_io(readStruct, file);
		png_set_sig_bytes(readStruct, 8);

		png_read_info(readStruct, infoStruct);

		data.width     = png_get_image_width(readStruct, infoStruct);
		data.height	   = png_get_image_height(readStruct, infoStruct);
		data.colorType = png_get_color_type(readStruct, infoStruct);
		data.bitDepth  = png_get_bit_depth(readStruct, infoStruct);

		png_read_update_info(readStruct, infoStruct);

		if (setjmp(png_jmpbuf(readStruct)))
		{
			png_destroy_read_struct(&readStruct, &infoStruct, (png_infopp)NULL);
			return false;
		}

		png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * data.height);
		for (UINT i = 0; i < data.height; i++)
			rows[i] = (png_byte*)malloc(png_get_rowbytes(readStruct, infoStruct));

		png_read_image(readStruct, rows);

		fclose(file);

		if (png_get_color_type(readStruct, infoStruct) != PNG_COLOR_TYPE_RGBA)
		{
			for (UINT i = 0; i < data.height; i++)
				free(rows[i]);
			free(rows);
			png_destroy_read_struct(&readStruct, &infoStruct, (png_infopp)NULL);
			return false;
		}

		data.texels.resize(data.width * data.height);

		for (UINT i = 0; i < data.height; i++)
		{
			png_byte* row = rows[i];
			for (UINT j = 0; j < data.width; j++)
			{
				png_byte* texel = &(row[j * 4]);
				data.texels[i * data.width + j]
					= XMFLOAT4(texel[0] / 255.f,
							   texel[1] / 255.f,
							   texel[2] / 255.f,
							   texel[3] / 255.f);
			}
		}

		for (UINT i = 0; i < data.height; i++)
			free(rows[i]);
		free(rows);
		png_destroy_read_struct(&readStruct, &infoStruct, (png_infopp)NULL);

		return true;
	}

	bool savePNG(const std::string& filename, PNGData data)
	{
		FILE* file = fopen(filename.c_str(), "wb");
		if (!file)
			return false;

		png_structp writeStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!writeStruct)
			return false;

		png_infop infoStruct = png_create_info_struct(writeStruct);
		if (!infoStruct)
		{
			png_destroy_write_struct(&writeStruct, &infoStruct);
			return false;
		}

		if (setjmp(png_jmpbuf(writeStruct)))
		{
			png_destroy_write_struct(&writeStruct, &infoStruct);
			return false;
		}

		png_init_io(writeStruct, file);

		if (setjmp(png_jmpbuf(writeStruct)))
		{
			png_destroy_write_struct(&writeStruct, &infoStruct);
			return false;
		}

		png_set_IHDR(writeStruct, infoStruct, data.width, data.height,
					 data.bitDepth, data.colorType, PNG_INTERLACE_NONE,
					 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(writeStruct, infoStruct);

		if (setjmp(png_jmpbuf(writeStruct)))
		{
			png_destroy_write_struct(&writeStruct, &infoStruct);
			return false;
		}

		png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * data.height);
		for (UINT i = 0; i < data.height; i++)
			rows[i] = (png_byte*)malloc(png_get_rowbytes(writeStruct, infoStruct));

		for (UINT i = 0; i < data.height; i++)
		{
			png_byte* row = rows[i];
			for (UINT j = 0; j < data.width; j++)
			{
				png_byte* texel = &(row[j * 4]);
				texel[0] = (png_byte)floor(data.texels[i * data.width + j].x * 255.f + 0.5f);
				texel[1] = (png_byte)floor(data.texels[i * data.width + j].y * 255.f + 0.5f);
				texel[2] = (png_byte)floor(data.texels[i * data.width + j].z * 255.f + 0.5f);
				texel[3] = (png_byte)floor(data.texels[i * data.width + j].w * 255.f + 0.5f);
			}
		}

		png_write_image(writeStruct, rows);

		if (setjmp(png_jmpbuf(writeStruct)))
		{
			for (UINT i = 0; i < data.height; i++)
				free(rows[i]);
			free(rows);
			png_destroy_write_struct(&writeStruct, &infoStruct);
			return false;
		}

		png_write_end(writeStruct, NULL);

		for (UINT i = 0; i < data.height; i++)
			free(rows[i]);
		free(rows);

		fclose(file);

		png_destroy_write_struct(&writeStruct, &infoStruct);

		return true;
	}
}