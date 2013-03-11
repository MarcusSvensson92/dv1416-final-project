#include "StdAfx.h"
#include "Terrain.h"

const UINT g_cellsPerPatch = 64;

Terrain::Terrain(void)
{
	m_heightmapTexture    = NULL;
	m_blendmapTexture     = NULL;
	m_heightmapSRV	      = NULL;
	m_blendmapSRV		  = NULL;
	m_layermapArraySRV[0] = NULL;
	m_layermapArraySRV[1] = NULL;
	m_layermapArraySRV[2] = NULL;
	m_layermapArraySRV[3] = NULL;

	m_useBlendmap = false;
}

Terrain::~Terrain(void)
{
	RELEASE(m_heightmapTexture);
	RELEASE(m_heightmapSRV);
	RELEASE(m_blendmapSRV);
	RELEASE(m_layermapArraySRV[0]);
	RELEASE(m_layermapArraySRV[1]);
	RELEASE(m_layermapArraySRV[2]);
	RELEASE(m_layermapArraySRV[3]);
}

void Terrain::init(ID3D11Device* device, const TerrainDesc terrainDesc)
{
	m_terrainDesc = terrainDesc;

	m_patchVertexRowCount = m_terrainDesc.depth / g_cellsPerPatch + 1;
	m_patchVertexColCount = m_terrainDesc.width / g_cellsPerPatch + 1;
	m_patchVertexCount	  = m_patchVertexRowCount * m_patchVertexColCount;
	m_patchQuadFaceCount  = (m_patchVertexRowCount - 1) * (m_patchVertexColCount - 1);

	std::vector<Vertex::Terrain> vertices;
	std::vector<UINT> indices;
	createQuadPatchGrid(vertices, indices);
	m_indexCount = (UINT)indices.size();

	BufferInitDesc vertexBufferInitDesc;
	vertexBufferInitDesc.usage		  = D3D11_USAGE_IMMUTABLE;
	vertexBufferInitDesc.elementSize  = sizeof(Vertex::Terrain);
	vertexBufferInitDesc.elementCount = (UINT)vertices.size();
	vertexBufferInitDesc.type		  = VertexBuffer;
	vertexBufferInitDesc.data		  = &vertices[0];
	m_vertexBuffer.init(device, vertexBufferInitDesc);

	BufferInitDesc indexBufferInitDesc;
	indexBufferInitDesc.usage		 = D3D11_USAGE_IMMUTABLE;
	indexBufferInitDesc.elementSize	 = sizeof(UINT);
	indexBufferInitDesc.elementCount = (UINT)indices.size();
	indexBufferInitDesc.type		 = IndexBuffer;
	indexBufferInitDesc.data		 = &indices[0];
	m_indexBuffer.init(device, indexBufferInitDesc);

	m_heightmap.resize((m_terrainDesc.width + 1) * (m_terrainDesc.depth + 1), 0);
	buildHeightmapSRV(device);
}

void Terrain::loadHeightmap(ID3D11DeviceContext* deviceContext,
							const std::string& heightmapFilename, const float heightmapScale)
{
	UINT size = (m_terrainDesc.width + 1) * (m_terrainDesc.depth + 1);

	std::vector<unsigned char> in(size);

	std::ifstream file;
	file.open(heightmapFilename, std::ios_base::binary);
	if (file)
	{
		file.read((char*)&in[0], (std::streamsize)in.size());
		file.close();
	}

	m_heightmap.resize(size);
	for (UINT i = 0; i < size; i++)
		m_heightmap[i] = (in[i] / 255.f) * heightmapScale;

	updateHeightmapTexture(deviceContext);
}

void Terrain::loadBlendmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
						   const std::string& blendmapFilename,
						   std::vector<std::string> layermapFilenames)
{
	Utilities::loadPNG(blendmapFilename, m_blendmap);
	buildBlendmapSRV(device);
	//m_layermapArraySRV = Utilities::createTexture2DArraySRV(device, deviceContext, layermapFilenames);

	m_useBlendmap = true;
}

void Terrain::loadLayermap(ID3D11Device* device, const UINT i, const std::string& filename)
{
	RELEASE(m_layermapArraySRV[i]);
	D3DX11CreateShaderResourceViewFromFile(device, filename.c_str(), NULL, NULL, &m_layermapArraySRV[i], NULL);
}

void Terrain::render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	XMMATRIX world			= XMMatrixIdentity();
	XMMATRIX viewProj		= camera.getViewProj();
	XMFLOAT3 cameraPosition = camera.getPosition();
	shader->setMatrix("gWorld", world);
	shader->setMatrix("gViewProj", viewProj);
	shader->setFloat3("gCameraPosition", cameraPosition);

	shader->setFloat("gMinDistance", 20.f);
	shader->setFloat("gMaxDistance", 500.f);
	shader->setFloat("gMinTessellation", 0.f);
	shader->setFloat("gMaxTessellation", 6.f);

	shader->setFloat2("gTexelSize", XMFLOAT2(1.f / (m_terrainDesc.width + 1),
											 1.f / (m_terrainDesc.depth + 1)));

	shader->setResource("gHeightmap", m_heightmapSRV);
	shader->setResource("gBlendmap", m_blendmapSRV);
	//shader->setResource("gLayermapArray", m_layermapArraySRV);
	shader->setResource("gLayermap0", m_layermapArraySRV[0]);
	shader->setResource("gLayermap1", m_layermapArraySRV[1]);
	shader->setResource("gLayermap2", m_layermapArraySRV[2]);
	shader->setResource("gLayermap3", m_layermapArraySRV[3]);

	shader->setBool("gUseBlendmap", m_useBlendmap);

	m_vertexBuffer.apply(deviceContext);
	m_indexBuffer.apply(deviceContext);

	shader->Apply();

	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	deviceContext->HSSetShader(NULL, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
}

std::vector<std::pair<XMFLOAT2, float*>> Terrain::getHeightmapDataWithinRadius(const XMFLOAT3 position, const UINT radius)
{
	const int col = (int)floorf( position.x + 0.5f * m_terrainDesc.width);
	const int row = (int)floorf(-position.z + 0.5f * m_terrainDesc.depth);

	const UINT width = m_terrainDesc.width + 1;
	const UINT depth = m_terrainDesc.depth + 1;

	const float halfWidth = 0.5f * (float)width;
	const float halfDepth = 0.5f * (float)depth;

	const float dx = (float)width / ((float)width - 1);
	const float dz = (float)depth / ((float)depth - 1);

	std::vector<std::pair<XMFLOAT2, float*>> heightmapData;
	for (int i = row - (int)radius; i < row + (int)radius + 1; i++)
		if (i >= 0 && i < depth)
			for (int j = col - (int)radius; j < col + (int)radius + 1; j++)
				if (j >= 0 && j < width)
					heightmapData.push_back(
						std::pair<XMFLOAT2, float*>(
							XMFLOAT2(-halfWidth + j * dx, halfDepth - i * dz),
							&m_heightmap[i * width + j]));
	return heightmapData;
}

std::vector<std::pair<XMFLOAT2, XMFLOAT4*>> Terrain::getBlendmapDataWithinRadius(const XMFLOAT3 position, const UINT radius)
{
	const int col = (int)floorf( position.x + 0.5f * m_terrainDesc.width);
	const int row = (int)floorf(-position.z + 0.5f * m_terrainDesc.depth);

	const UINT width = m_terrainDesc.width;
	const UINT depth = m_terrainDesc.depth;

	const float halfWidth = 0.5f * (float)width;
	const float halfDepth = 0.5f * (float)depth;

	const float dx = (float)width / ((float)width - 1);
	const float dz = (float)depth / ((float)depth - 1);

	std::vector<std::pair<XMFLOAT2, XMFLOAT4*>> blendmapData;
	for (int i = row - (int)radius; i < row + (int)radius + 1; i++)
		if (i >= 0 && i < depth)
			for (int j = col - (int)radius; j < col + (int)radius + 1; j++)
				if (j >= 0 && j < width)
					blendmapData.push_back(
						std::pair<XMFLOAT2, XMFLOAT4*>(
							XMFLOAT2(-halfWidth + j * dx, halfDepth - i * dz),
							&m_blendmap.texels[i * width + j]));
	return blendmapData;
}

void Terrain::updateHeightmapTexture(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(m_heightmapTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	const uint32_t pitch = sizeof(float) * (m_terrainDesc.width + 1);
	uint8_t* textureData = reinterpret_cast<uint8_t*>(resource.pData);
	const uint8_t* heightmapData = reinterpret_cast<uint8_t*>(&m_heightmap[0]);
	for (uint32_t i = 0; i < (m_terrainDesc.depth + 1); i++)
	{
		memcpy(textureData, heightmapData, pitch);

		textureData += resource.RowPitch;
		heightmapData += pitch;
	}

	deviceContext->Unmap(m_heightmapTexture, 0);
}

void Terrain::updateBlendmapTexture(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(m_blendmapTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	const uint32_t pitch = sizeof(XMFLOAT4) * m_blendmap.width;
	uint8_t* textureData = reinterpret_cast<uint8_t*>(resource.pData);
	const uint8_t* blendmapData = reinterpret_cast<uint8_t*>(&m_blendmap.texels[0]);
	for (uint32_t i = 0; i < m_blendmap.height; i++)
	{
		memcpy(textureData, blendmapData, pitch);

		textureData += resource.RowPitch;
		blendmapData += pitch;
	}

	deviceContext->Unmap(m_blendmapTexture, 0);
}

void Terrain::createQuadPatchGrid(std::vector<Vertex::Terrain>& vertices, std::vector<UINT>& indices)
{
	const float width = (float)m_terrainDesc.width;
	const float depth = (float)m_terrainDesc.depth;

	const float halfWidth = 0.5f * width;
	const float halfDepth = 0.5f * depth;

	const float patchWidth = width / (m_patchVertexColCount - 1);
	const float patchDepth = depth / (m_patchVertexRowCount - 1);

	const float du = 1.f / (m_patchVertexColCount - 1);
	const float dv = 1.f / (m_patchVertexRowCount - 1);

	vertices.resize(m_patchVertexCount);

	for (UINT i = 0; i < m_patchVertexRowCount; i++)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < m_patchVertexColCount; j++)
		{
			float x = -halfWidth + j * patchWidth;

			vertices[i * m_patchVertexColCount + j].position = XMFLOAT3(x, 0.f, z);
			vertices[i * m_patchVertexColCount + j].tex0.x   = j * du;
			vertices[i * m_patchVertexColCount + j].tex0.y   = i * dv;
		}
	}

	indices.resize(m_patchQuadFaceCount * 4);

	UINT k = 0;
	for (UINT i = 0; i < m_patchVertexRowCount - 1; i++)
	{
		for (UINT j = 0; j < m_patchVertexColCount - 1; j++)
		{
			indices[k]	   = i * m_patchVertexColCount + j;
			indices[k + 1] = i * m_patchVertexColCount + j + 1;
			
			indices[k + 2] = (i + 1) * m_patchVertexColCount + j;
			indices[k + 3] = (i + 1) * m_patchVertexColCount + j + 1;

			k += 4;
		}
	}
}

void Terrain::buildHeightmapSRV(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width			   = m_terrainDesc.width + 1;
	textureDesc.Height			   = m_terrainDesc.depth + 1;
	textureDesc.MipLevels		   = 1;
	textureDesc.ArraySize		   = 1;
	textureDesc.Format			   = DXGI_FORMAT_R32_FLOAT;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage			   = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags		   = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags		   = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem		  = &m_heightmap[0];
	data.SysMemPitch	  = (m_terrainDesc.width + 1) * sizeof(float);
	data.SysMemSlicePitch = 0;

	RELEASE(m_heightmapTexture);
	device->CreateTexture2D(&textureDesc, &data, &m_heightmapTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format					  = textureDesc.Format;
	srvDesc.ViewDimension			  = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels		  = -1;
	device->CreateShaderResourceView(m_heightmapTexture, &srvDesc, &m_heightmapSRV);
}

void Terrain::buildBlendmapSRV(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width			   = m_blendmap.width;
	textureDesc.Height			   = m_blendmap.height;
	textureDesc.MipLevels		   = 1;
	textureDesc.ArraySize		   = 1;
	textureDesc.Format			   = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage			   = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags		   = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags		   = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem		  = &m_blendmap.texels[0];
	data.SysMemPitch	  = sizeof(XMFLOAT4) * m_blendmap.width;
	data.SysMemSlicePitch = 0;

	RELEASE(m_blendmapTexture);
	device->CreateTexture2D(&textureDesc, &data, &m_blendmapTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format					  = textureDesc.Format;
	srvDesc.ViewDimension			  = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels		  = -1;
	device->CreateShaderResourceView(m_blendmapTexture, &srvDesc, &m_blendmapSRV);
}