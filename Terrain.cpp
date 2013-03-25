#include "StdAfx.h"
#include "Terrain.h"
#include "TerrainOptions.h"
#include "SelectionOptions.h"

const UINT g_cellsPerPatch = 64;

Terrain::Terrain(void)
{
	m_minDistance     = 20.f;
	m_maxDistance	  = 500.f;
	m_minTessellation = 0.f;
	m_maxTessellation = 6.f;
	m_textureScale	  = 5.f;
	m_targetPosition  = XMFLOAT3(0.f, 0.f, 0.f);
	m_targetDiameter  = 10.f;

	m_selectionIntervalSample = 0.1f;
	m_selectionDistance		  = 500.f;

	m_vertexBuffer = NULL;
	m_indexBuffer  = NULL;

	m_heightmapTexture    = NULL;
	m_blendmapTexture     = NULL;
	m_heightmapSRV	      = NULL;
	m_blendmapSRV		  = NULL;
	m_layermapArraySRV[0] = NULL;
	m_layermapArraySRV[1] = NULL;
	m_layermapArraySRV[2] = NULL;
	m_layermapArraySRV[3] = NULL;

	m_created = false;
}

Terrain::~Terrain(void)
{
	if (m_vertexBuffer) delete m_vertexBuffer;
	if (m_indexBuffer)  delete m_indexBuffer;

	RELEASE(m_heightmapTexture);
	RELEASE(m_heightmapSRV);
	RELEASE(m_blendmapSRV);
	RELEASE(m_layermapArraySRV[0]);
	RELEASE(m_layermapArraySRV[1]);
	RELEASE(m_layermapArraySRV[2]);
	RELEASE(m_layermapArraySRV[3]);
}

void Terrain::create(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
					 const UINT width, const UINT depth)
{
	m_width = width;
	m_depth = depth;

	m_heightmap.clear();
	m_heightmap.resize((m_width + 1) * (m_depth + 1), 0);

	m_blendmap.texels.clear();
	m_blendmap.texels.resize(m_width * m_depth, XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_blendmap.width	 = m_width;
	m_blendmap.height	 = m_depth;
	m_blendmap.colorType = PNG_COLOR_TYPE_RGB_ALPHA;
	m_blendmap.bitDepth  = 8;

	createBuffers(device, deviceContext);
}

void Terrain::create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
					 const std::string& heightmapFilename, const std::string& blendmapFilename)
{
	m_blendmap.texels.clear();
	Utilities::loadPNG(blendmapFilename, m_blendmap);

	m_width = m_blendmap.width;
	m_depth = m_blendmap.height;

	UINT size = (m_width + 1) * (m_depth + 1);

	std::vector<unsigned char> in(size);

	std::ifstream file;
	file.open(heightmapFilename, std::ios_base::binary);
	if (file)
	{
		file.read((char*)&in[0], (std::streamsize)in.size());
		file.close();
	}

	m_heightmap.clear();
	m_heightmap.resize(size);
	for (UINT i = 0; i < size; i++)
		m_heightmap[i] = (float)in[i];
	
	createBuffers(device, deviceContext);
}

void Terrain::saveHeightmap(const std::string& filepath)
{
	UINT size = (m_width + 1) * (m_depth + 1);

	std::vector<unsigned char> out(size);
	for (UINT i = 0; i < size; i++)
		out[i] = (unsigned char)floor(m_heightmap[i] + 0.5f);

	std::ofstream file;
	file.open(filepath, std::ios_base::binary | std::ios_base::out);
	if (file)
	{
		file.write((char*)&out[0], (std::streamsize)out.size());
		file.close();
	}
}

void Terrain::saveBlendmap(const std::string& filepath)
{
	Utilities::savePNG(filepath, m_blendmap);
}

void Terrain::loadLayermap(ID3D11Device* device, const UINT i, const std::string& filename)
{
	RELEASE(m_layermapArraySRV[i]);
	D3DX11CreateShaderResourceViewFromFile(device, filename.c_str(), NULL, NULL, &m_layermapArraySRV[i], NULL);
}

void Terrain::render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera)
{
	if (m_created)
	{
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		XMMATRIX world			= XMMatrixIdentity();
		XMMATRIX viewProj		= camera.getViewProj();
		XMFLOAT3 cameraPosition = camera.getPosition();
		shader->setMatrix("gWorld", world);
		shader->setMatrix("gViewProj", viewProj);
		shader->setFloat3("gCameraPosition", cameraPosition);

		shader->setFloat("gTextureScale", m_textureScale);

		shader->setFloat("gMinDistance", m_minDistance);
		shader->setFloat("gMaxDistance", m_maxDistance);
		shader->setFloat("gMinTessellation", m_minTessellation);
		shader->setFloat("gMaxTessellation", m_maxTessellation);

		shader->setFloat2("gTexelSize", XMFLOAT2(1.f / (m_width + 1),
												 1.f / (m_depth + 1)));

		shader->setResource("gHeightmap", m_heightmapSRV);
		shader->setResource("gBlendmap", m_blendmapSRV);
		shader->setResource("gLayermap0", m_layermapArraySRV[0]);
		shader->setResource("gLayermap1", m_layermapArraySRV[1]);
		shader->setResource("gLayermap2", m_layermapArraySRV[2]);
		shader->setResource("gLayermap3", m_layermapArraySRV[3]);

		shader->setFloat3("gTargetPosition", m_targetPosition);
		shader->setFloat("gTargetDiameter", m_targetDiameter);

		m_vertexBuffer->apply(deviceContext);
		m_indexBuffer->apply(deviceContext);

		shader->Apply();

		deviceContext->DrawIndexed(m_indexCount, 0, 0);

		deviceContext->HSSetShader(NULL, NULL, 0);
		deviceContext->DSSetShader(NULL, NULL, 0);
	}
}

void Terrain::onEvent(const std::string& sender, const std::string& eventName)
{
	if (sender == "Terrain Options")
	{
		GUI::TerrainOptions& terrainOptions = GUI::TerrainOptions::getInstance();

		if (eventName == "Texture Scale")
			m_textureScale = (float)terrainOptions.getTrackbarValue(eventName);
		else if (eventName == "Minimum Tessellation")
			m_minTessellation = (float)terrainOptions.getTrackbarValue(eventName);
		else if (eventName == "Maximum Tessellation")
			m_maxTessellation = (float)terrainOptions.getTrackbarValue(eventName);
		else if (eventName == "Minimum Tessellation Distance")
			m_minDistance = (float)terrainOptions.getTrackbarValue(eventName);
		else if (eventName == "Maximum Tessellation Distance")
			m_maxDistance = (float)terrainOptions.getTrackbarValue(eventName);
	}

	if (sender == "Selection Options")
	{
		GUI::SelectionOptions& selectionOptions = GUI::SelectionOptions::getInstance();

		if (eventName == "Interval Sample")
			m_selectionIntervalSample = (float)selectionOptions.getTrackbarValue(eventName) / 100.f;
		else if (eventName == "Distance")
			m_selectionDistance = (float)selectionOptions.getTrackbarValue(eventName);
	}
}

float Terrain::getHeight(const XMFLOAT2 position)
{
	const float c =  position.x + 0.5f * (m_width + 1);
	const float d = -position.y + 0.5f * (m_depth + 1);

	const int row = (int)floorf(d);
	const int col = (int)floorf(c);

	const float A = m_heightmap[ row	  * m_width + col];
	const float B = m_heightmap[ row	  * m_width + col + 1];
	const float C = m_heightmap[(row + 1) * m_width + col];
	const float D = m_heightmap[(row + 1) * m_width + col + 1];

	const float s = c - (float)col;
	const float t = d - (float)row;

	if (s + t <= 1.f)
		return A + s * (B - A) + t * (C - A);
	else
		return D + (1.f - s) * (C - D) + (1.f - t) * (B - D);
}

bool Terrain::computeIntersection(const Ray& ray)
{
	const UINT width = m_width;
	const UINT depth = m_depth;

	const float halfWidth = 0.5f * (float)width;
	const float halfDepth = 0.5f * (float)depth;

	const UINT heightmapWidth = width + 1;
	const UINT heightmapDepth = depth + 1;

	const float halfHeightmapWidth = 0.5f * (float)heightmapWidth;
	const float halfHeightmapDepth = 0.5f * (float)heightmapDepth;

	const float heightmapDx = (float)heightmapWidth / ((float)heightmapWidth - 1.f);
	const float heightmapDz = (float)heightmapDepth / ((float)heightmapDepth - 1.f);

	XMVECTOR origin = XMVectorSet(XMVectorGetX(ray.origin), XMVectorGetZ(ray.origin), 0.f, 0.f);

	const float directionX = XMVectorGetX(ray.direction);
	const float directionZ = XMVectorGetZ(ray.direction);

	XMVECTOR direction = XMVectorSet(directionX, directionZ, 0.f, 0.f);
	if (directionX != 0.f && directionZ != 0.f)
	{
		const float absDirectionX = abs(directionX);
		const float absDirectionZ = abs(directionZ);
		direction /= (absDirectionX > absDirectionZ) ? absDirectionX : absDirectionZ;
	}
	direction *= m_selectionIntervalSample;

	float t = -1.f;

	XMVECTOR position = origin;

	int prevRow, prevCol;
	
	const UINT n = (UINT)(m_selectionDistance / m_selectionIntervalSample);
	for (UINT i = 0; i < n; i++)
	{
		if (i > 0)
			position += direction;
		XMFLOAT2 p;
		XMStoreFloat2(&p, position);

		const float c =  p.x + halfWidth;
		const float d = -p.y + halfDepth;

		const int row = (int)floorf(d);
		const int col = (int)floorf(c);

		if ((i <= 0 || (row != prevRow || col != prevCol)) &&
			inBounds(row, col) && inBounds(row + 1, col + 1))
		{
			const float x0 = -halfHeightmapWidth + (float)col * heightmapDx;
			const float x1 = -halfHeightmapWidth + (float)(col + 1) * heightmapDx;
			const UINT  y0 = row * heightmapWidth + col;
			const UINT  y1 = (row + 1) * heightmapWidth + col;
			const float z0 = halfHeightmapDepth - (float)row * heightmapDz;
			const float z1 = halfHeightmapDepth - (float)(row + 1) * heightmapDz;

			XMVECTOR A = XMVectorSet(x0, m_heightmap[y0    ], z0, 0.f);
			XMVECTOR B = XMVectorSet(x1, m_heightmap[y0 + 1], z0, 0.f);
			XMVECTOR C = XMVectorSet(x0, m_heightmap[y1	   ], z1, 0.f);
			XMVECTOR D = XMVectorSet(x1, m_heightmap[y1 + 1], z1, 0.f);

			XMVECTOR triangle1[3] = { A, B, C };
			XMVECTOR triangle2[3] = { D, C, B };

			const float t1 = computeTriangleIntersection(ray, triangle1);
			const float t2 = computeTriangleIntersection(ray, triangle2);

			if (t1 < 0.f && t2 < 0.f) { }
			else if (t1 >= 0.f && t2 < 0.f)
			{
				t = t1;
				break;
			}
			else if (t1 < 0.f && t2 >= 0.f)
			{
				t = t2;
				break;
			}
			else
			{
				t = (t1 > t2) ? t2 : t1;
				break;
			}
		}

		prevRow = row;
		prevCol = col;
	}

	if (t >= 0.f)
	{
		XMStoreFloat3(&m_targetPosition, ray.origin + t * ray.direction);
		return true;
	}
	else
		return false;
}

std::vector<std::pair<float, float*>> Terrain::getHeightmapDataWithinRadius(const XMFLOAT3 position, const UINT radius)
{
	const int col = (int)floorf( position.x + 0.5f * m_width);
	const int row = (int)floorf(-position.z + 0.5f * m_depth);

	const UINT width = m_width + 1;
	const UINT depth = m_depth + 1;

	const float halfWidth = 0.5f * (float)width;
	const float halfDepth = 0.5f * (float)depth;

	const float dx = (float)width / ((float)width - 1);
	const float dz = (float)depth / ((float)depth - 1);

	XMVECTOR p = XMVectorSet(position.x, position.z, 0.f, 0.f);

	std::vector<std::pair<float, float*>> heightmapData;
	for (int i = row - (int)radius; i < row + (int)radius + 1; i++)
	{
		if (i >= 0 && i < (int)depth)
		{
			for (int j = col - (int)radius; j < col + (int)radius + 1; j++)
			{
				if (j >= 0 && j < (int)width)
				{
					XMVECTOR v = XMVectorSet(-halfWidth + j * dx, halfDepth - i * dz, 0.f, 0.f);
					const float length = XMVectorGetX(XMVector2Length(v - p)) / radius;
					if (length <= 1.f)
						heightmapData.push_back(
							std::pair<float, float*>(length, &m_heightmap[i * width + j]));
				}
			}
		}
	}
	return heightmapData;
}

std::vector<std::pair<float, XMFLOAT4*>> Terrain::getBlendmapDataWithinRadius(const XMFLOAT3 position, const UINT radius)
{
	const int col = (int)floorf( position.x + 0.5f * m_width);
	const int row = (int)floorf(-position.z + 0.5f * m_depth);

	const UINT width = m_width;
	const UINT depth = m_depth;

	const float halfWidth = 0.5f * (float)width;
	const float halfDepth = 0.5f * (float)depth;

	const float dx = (float)width / ((float)width - 1);
	const float dz = (float)depth / ((float)depth - 1);

	XMVECTOR p = XMVectorSet(position.x, position.z, 0.f, 0.f);

	std::vector<std::pair<float, XMFLOAT4*>> blendmapData;
	for (int i = row - (int)radius; i < row + (int)radius + 1; i++)
	{
		if (i >= 0 && i < (int)depth)
		{
			for (int j = col - (int)radius; j < col + (int)radius + 1; j++)
			{
				if (j >= 0 && j < (int)width)
				{
					XMVECTOR v = XMVectorSet(-halfWidth + j * dx, halfDepth - i * dz, 0.f, 0.f);
					const float length = XMVectorGetX(XMVector2Length(v - p)) / radius;
					if (length <= 1.f)
						blendmapData.push_back(
							std::pair<float, XMFLOAT4*>(length, &m_blendmap.texels[i * width + j]));
				}
			}
		}
	}
	return blendmapData;
}

void Terrain::updateHeightmapTexture(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(m_heightmapTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	const uint32_t pitch = sizeof(float) * (m_width + 1);
	uint8_t* textureData = reinterpret_cast<uint8_t*>(resource.pData);
	const uint8_t* heightmapData = reinterpret_cast<uint8_t*>(&m_heightmap[0]);
	for (uint32_t i = 0; i < (m_depth + 1); i++)
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

void Terrain::createBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_patchVertexRowCount = m_depth / g_cellsPerPatch + 1;
	m_patchVertexColCount = m_width / g_cellsPerPatch + 1;
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
	if (m_vertexBuffer)
		delete m_vertexBuffer;
	m_vertexBuffer = new Buffer();
	m_vertexBuffer->init(device, vertexBufferInitDesc);

	BufferInitDesc indexBufferInitDesc;
	indexBufferInitDesc.usage		 = D3D11_USAGE_IMMUTABLE;
	indexBufferInitDesc.elementSize	 = sizeof(UINT);
	indexBufferInitDesc.elementCount = (UINT)indices.size();
	indexBufferInitDesc.type		 = IndexBuffer;
	indexBufferInitDesc.data		 = &indices[0];
	if (m_indexBuffer)
		delete m_indexBuffer;
	m_indexBuffer = new Buffer();
	m_indexBuffer->init(device, indexBufferInitDesc);

	buildHeightmapSRV(device);
	buildBlendmapSRV(device);

	updateHeightmapTexture(deviceContext);
	updateBlendmapTexture(deviceContext);

	m_created = true;
}

void Terrain::createQuadPatchGrid(std::vector<Vertex::Terrain>& vertices, std::vector<UINT>& indices)
{
	const float width = (float)m_width;
	const float depth = (float)m_depth;

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
	textureDesc.Width			   = m_width + 1;
	textureDesc.Height			   = m_depth + 1;
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
	data.SysMemPitch	  = (m_width + 1) * sizeof(float);
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

bool Terrain::inBounds(int i, int j)
{
	return i >= 0 && i < (int)m_depth + 1 &&
		   j >= 0 && j < (int)m_width + 1;
}

float Terrain::computeTriangleIntersection(const Ray& ray, const XMVECTOR triangle[3])
{
	XMVECTOR	e1 = triangle[1] - triangle[0];
	XMVECTOR	e2 = triangle[2] - triangle[0];
	XMVECTOR    q  = XMVector3Cross(ray.direction, e2);
	const float a  = XMVectorGetX(XMVector3Dot(e1, q));

	if (a > -std::numeric_limits<float>::epsilon() && a < std::numeric_limits<float>::epsilon())
		return -1.f;
	
	const float f = 1.f / a;
	XMVECTOR    s = ray.origin - triangle[0];
	const float u = f * XMVectorGetX(XMVector3Dot(s, q));

	if (u < 0.f)
		return -1.f;

	XMVECTOR	r = XMVector3Cross(s, e1);
	const float v = f * XMVectorGetX(XMVector3Dot(ray.direction, r));

	if (v < 0.f || u + v > 1.f)
		return -1.f;

	return f * XMVectorGetX(XMVector3Dot(e2, r));
}