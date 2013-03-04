#include "StdAfx.h"
#include "Terrain.h"
#include "Utilities.h"

Terrain::Terrain(void)
{
	m_blendmapSRV	   = NULL;
	m_layermapArraySRV = NULL;

	m_useBlendmap = false;
}

Terrain::~Terrain(void)
{
	RELEASE(m_blendmapSRV);
	RELEASE(m_layermapArraySRV);
}

void Terrain::init(ID3D11Device* device, const TerrainDesc terrainDesc)
{
	m_terrainDesc = terrainDesc;

	std::vector<UINT> indices;
	createGrid(m_vertices, indices);
	m_indexCount = (UINT)indices.size();

	BufferInitDesc vertexBufferInitDesc;
	vertexBufferInitDesc.usage			= D3D11_USAGE_DYNAMIC;
	vertexBufferInitDesc.elementSize	= sizeof(Vertex::Basic);
	vertexBufferInitDesc.elementCount	= (UINT)m_vertices.size();
	vertexBufferInitDesc.type			= VertexBuffer;
	vertexBufferInitDesc.data			= &m_vertices[0];
	m_vertexBuffer.init(device, vertexBufferInitDesc);

	BufferInitDesc indexBufferInitDesc;
	indexBufferInitDesc.usage		 = D3D11_USAGE_IMMUTABLE;
	indexBufferInitDesc.elementSize	 = sizeof(UINT);
	indexBufferInitDesc.elementCount = (UINT)indices.size();
	indexBufferInitDesc.type		 = IndexBuffer;
	indexBufferInitDesc.data		 = &indices[0];
	m_indexBuffer.init(device, indexBufferInitDesc);

	m_targetPosition = XMFLOAT3(0.f, 0.f, 0.f);
}

void Terrain::loadHeightmap(ID3D11DeviceContext* deviceContext, const std::string& heightmapFilename,
							const float heightmapScale)
{
	UINT size = m_terrainDesc.width * m_terrainDesc.depth;

	std::vector<unsigned char> in(size);

	std::ifstream file;
	file.open(heightmapFilename, std::ios_base::binary);
	if (file)
	{
		file.read((char*)&in[0], (std::streamsize)in.size());
		file.close();
	}

	for (UINT i = 0; i < size; i++)
		m_vertices[i].position.y = (in[i] / 255.f) * heightmapScale;

	void* data = m_vertexBuffer.map(deviceContext);
	memcpy(data, &m_vertices[0], sizeof(Vertex::Basic) * (UINT)m_vertices.size());
	m_vertexBuffer.unmap(deviceContext);
}

void Terrain::loadBlendmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
						   const std::string& blendmapFilename,
						   std::vector<std::string> layermapFilenames)
{
	D3DX11CreateShaderResourceViewFromFile(device, blendmapFilename.c_str(), NULL, NULL, &m_blendmapSRV, NULL);
	m_layermapArraySRV = Utilities::createTexture2DArraySRV(device, deviceContext, layermapFilenames);

	m_useBlendmap = true;
}

void Terrain::render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world			= XMMatrixIdentity();
	XMMATRIX worldViewProj  = world * camera.getViewProj();
	XMFLOAT3 cameraPosition = camera.getPosition();
	shader->setMatrix("gWorld", world);
	shader->setMatrix("gWorldViewProj", worldViewProj);
	shader->setFloat3("gCameraPosition", cameraPosition);
	shader->setResource("gBlendmap", m_blendmapSRV);
	shader->setResource("gLayermapArray", m_layermapArraySRV);
	shader->setBool("gUseBlendmap", m_useBlendmap);
	shader->setFloat3("gTargetPosition", m_targetPosition);

	m_vertexBuffer.apply(deviceContext);
	m_indexBuffer.apply(deviceContext);

	shader->Apply();

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void Terrain::computeIntersection(const Ray& ray)
{
	XMVECTOR n = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	float t = -XMVectorGetX(XMVector3Dot(ray.origin, n)) / XMVectorGetX(XMVector3Dot(ray.direction, n));
	if (t > 0.f)
		XMStoreFloat3(&m_targetPosition, ray.origin + (t * ray.direction));
}

void Terrain::createGrid(std::vector<Vertex::Basic>& vertices, std::vector<UINT>& indices)
{
	const float width = (float)m_terrainDesc.width;
	const float depth = (float)m_terrainDesc.depth;

	UINT vertexCount   = depth * width;
	UINT triangleCount = (depth - 1) * (width - 1) * 2;

	const float halfWidth = 0.5f * width;
	const float halfDepth = 0.5f * depth;

	const float dx = (float)(width / (width - 1));
	const float dz = (float)(depth / (depth - 1));

	vertices.resize(vertexCount);

	for (UINT i = 0; i < depth; i++)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < width; j++)
		{
			float x = -halfWidth + j * dx;

			vertices[i * width + j].position = XMFLOAT3(x, 0.f, z);
			vertices[i * width + j].normal	 = XMFLOAT3(0.f, 1.f, 0.f);
			vertices[i * width + j].tex0.x   = (x + halfWidth) / width;
			vertices[i * width + j].tex0.y   = (z - halfDepth) / -depth;
		}
	}

	indices.resize(triangleCount * 3);

	UINT k = 0;
	for (UINT i = 0; i < depth - 1; i++)
	{
		for (UINT j = 0; j < width - 1; j++)
		{
			indices[k]	   = i * width + j;
			indices[k + 1] = i * width + j + 1;
			indices[k + 2] = (i + 1) * width + j;

			indices[k + 3] = (i + 1) * width + j;
			indices[k + 4] = i * width + j + 1;
			indices[k + 5] = (i + 1) * width + j + 1;

			k += 6;
		}
	}
}