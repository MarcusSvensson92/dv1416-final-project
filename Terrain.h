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
	UINT vertexMultiplier;

	UINT m(void) { return depth * vertexMultiplier; }
	UINT n(void) { return width * vertexMultiplier; }
};

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);

	void init(ID3D11Device* device, const TerrainDesc terrainDesc);
	void loadHeightmap(const std::string& heightmapFilename);
	void loadBlendmap(ID3D11DeviceContext* deviceContext, const std::string& blendmapFilename,
					  std::vector<std::string> layermapFilenames);

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);
private:
	TerrainDesc m_terrainDesc;

	std::vector<Vertex::Basic> m_vertices;
	UINT m_indexCount;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;

	void createGrid(std::vector<Vertex::Basic>& vertices, std::vector<UINT>& indices);
};

#endif