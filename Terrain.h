#ifndef TERRAIN_H
#define TERRAIN_H

#include "StdAfx.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Utilities.h"

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

	std::vector<std::pair<XMFLOAT2, float*>> getHeightmapDataWithinRadius(const XMFLOAT3 position, const UINT radius);
	void updateHeightmapTexture(ID3D11DeviceContext* deviceContext);
	void updateBlendmapTexture(ID3D11DeviceContext* deviceContext);
private:
	TerrainDesc m_terrainDesc;

	std::vector<Vertex::Terrain> m_vertices;
	UINT m_indexCount;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;

	UINT m_patchVertexCount;
	UINT m_patchQuadFaceCount;
	UINT m_patchVertexRowCount;
	UINT m_patchVertexColCount;

	std::vector<float> m_heightmap;
	Utilities::PNGData m_blendmap;

	ID3D11Texture2D* m_heightmapTexture;
	ID3D11Texture2D* m_blendmapTexture;

	ID3D11ShaderResourceView* m_heightmapSRV;
	ID3D11ShaderResourceView* m_blendmapSRV;
	ID3D11ShaderResourceView* m_layermapArraySRV;

	bool m_useBlendmap;

	void createQuadPatchGrid(std::vector<Vertex::Terrain>& vertices, std::vector<UINT>& indices);

	void buildHeightmapSRV(ID3D11Device* device);
	void buildBlendmapSRV(ID3D11Device* device);
};

#endif