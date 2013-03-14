#ifndef TERRAIN_H
#define TERRAIN_H

#include "StdAfx.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Utilities.h"
#include "EventReceiver.h"

struct TerrainDesc
{
	UINT width;
	UINT depth;
};

class Terrain : public GUI::EventReceiver
{
public:
	Terrain(void);
	~Terrain(void);

	XMFLOAT3 getTargetPosition(void) const { return m_targetPosition; }
	void setTargetDiameter(const float targetDiameter) { m_targetDiameter = targetDiameter; }

	void init(ID3D11Device* device, const TerrainDesc terrainDesc);
	void loadHeightmap(ID3D11DeviceContext* deviceContext, const std::string& heightmapFilename,
					   const float heightmapScale);
	void loadBlendmap(ID3D11DeviceContext* deviceContext,
					  const std::string& blendmapFilename,
					  std::vector<std::string> layermapFilenames);
	void loadLayermap(ID3D11Device* device, const UINT i, const std::string& filename);

	void render(ID3D11DeviceContext* deviceContext, Shader* shader, const Camera& camera);

	void onEvent(const std::string& sender, const std::string& eventName);

	float getHeight(const XMFLOAT2 position);

	bool computeIntersection(const Ray& ray);

	std::vector<std::pair<float, float*>> getHeightmapDataWithinRadius(const XMFLOAT3 position, const UINT radius);
	std::vector<std::pair<float, XMFLOAT4*>> getBlendmapDataWithinRadius(const XMFLOAT3 position, const UINT radius);

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
	ID3D11ShaderResourceView* m_layermapArraySRV[4];

	bool m_useBlendmap;

	float m_minDistance;
	float m_maxDistance;
	float m_minTessellation;
	float m_maxTessellation;
	float m_textureScale;

	XMFLOAT3 m_targetPosition;
	float m_targetDiameter;

	float m_selectionIntervalSample;
	float m_selectionDistance;

	void createQuadPatchGrid(std::vector<Vertex::Terrain>& vertices, std::vector<UINT>& indices);

	void buildHeightmapSRV(ID3D11Device* device);
	void buildBlendmapSRV(ID3D11Device* device);

	bool inBounds(int i, int j);

	float computeTriangleIntersection(const Ray& ray, const XMVECTOR triangle[3]);
};

#endif