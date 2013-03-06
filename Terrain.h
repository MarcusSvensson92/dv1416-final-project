#ifndef TERRAIN_H
#define TERRAIN_H

#include "StdAfx.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"

struct TerrainDesc
{
	UINT width;
	UINT depth;
};

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);

	void init(ID3D11Device* device, const TerrainDesc terrainDesc);
	void loadHeightmap(ID3D11DeviceContext* deviceContext, const std::string& heightmapFilename,
					   const float heightmapScale);
	void loadBlendmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
					  const std::string& blendmapFilename,
					  std::vector<std::string> layermapFilenames);

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);

	std::vector<Vertex::Basic*> getVerticesWithinRadius(const XMFLOAT3 position, const UINT radius);
	void updateVertexBuffer(ID3D11DeviceContext* deviceContext);
private:
	TerrainDesc m_terrainDesc;

	std::vector<Vertex::Basic> m_vertices;
	UINT m_indexCount;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;

	ID3D11ShaderResourceView* m_blendmapSRV;
	ID3D11ShaderResourceView* m_layermapArraySRV;

	bool m_useBlendmap;

	XMFLOAT3 m_targetPosition;

	void createGrid(std::vector<Vertex::Basic>& vertices, std::vector<UINT>& indices);
};

#endif