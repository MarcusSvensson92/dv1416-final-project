#include "StdAfx.h"
#include "Terrain.h"

Terrain::Terrain(void)
{
}

Terrain::~Terrain(void)
{
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
}

void Terrain::loadHeightmap(const std::string& heightmapFilename)
{
}

void Terrain::loadBlendmap(ID3D11DeviceContext* deviceContext, const std::string& blendmapFilename,
						   std::vector<std::string> layermapFilenames)
{
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

	m_vertexBuffer.apply(deviceContext);
	m_indexBuffer.apply(deviceContext);

	shader->Apply();

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void Terrain::createGrid(std::vector<Vertex::Basic>& vertices, std::vector<UINT>& indices)
{
	const UINT width = m_terrainDesc.width;
	const UINT depth = m_terrainDesc.depth;
	const UINT m	 = m_terrainDesc.m();
	const UINT n	 = m_terrainDesc.n();

	UINT vertexCount   = m * n;
	UINT triangleCount = (m - 1) * (n - 1) * 2;

	const float halfWidth = 0.5f * width;
	const float halfDepth = 0.5f * depth;

	const float dx = (UINT)(width / (n - 1));
	const float dz = (UINT)(depth / (m - 1));

	vertices.resize(vertexCount);

	for (UINT i = 0; i < m; i++)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; j++)
		{
			float x = -halfWidth + j * dx;

			vertices[i * n + j].position = XMFLOAT3(x, 0.f, z);
			vertices[i * n + j].normal	 = XMFLOAT3(0.f, 1.f, 0.f);
			vertices[i * n + j].tex0.x   = (x + halfWidth) / width;
			vertices[i * n + j].tex0.y   = (z - halfDepth) / -depth;
		}
	}

	indices.resize(triangleCount * 3);

	UINT k = 0;
	for (UINT i = 0; i < m - 1; i++)
	{
		for (UINT j = 0; j < n - 1; j++)
		{
			indices[k]	   = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}
}