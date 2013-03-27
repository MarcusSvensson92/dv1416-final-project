#ifndef TERRAIN_H
#define TERRAIN_H

#include "StdAfx.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Utilities.h"
#include "EventReceiver.h"

class Terrain : public GUI::EventReceiver
{
public:
	Terrain(void);
	~Terrain(void);

	bool isCreated(void) const { return m_created; }

	XMFLOAT3 getTargetPosition(void) const { return m_targetPosition; }
	void setTargetDiameter(const float targetDiameter) { m_targetDiameter = targetDiameter; }

	void create(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
				const UINT width, const UINT depth);
	void create(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
				const std::string& heightmapFilename, const std::string& blendmapFilename);

	void saveHeightmap(void);
	void saveHeightmap(const std::string& filepath);
	void saveBlendmap(void);
	void saveBlendmap(const std::string& filepath);

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
	UINT m_width;
	UINT m_depth;

	UINT m_indexCount;

	Buffer* m_vertexBuffer;
	Buffer* m_indexBuffer;

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

	bool m_created;

	float m_minDistance;
	float m_maxDistance;
	float m_minTessellation;
	float m_maxTessellation;
	float m_textureScale;

	float m_selectionIntervalSample;
	float m_selectionDistance;

	XMFLOAT3 m_targetPosition;
	float m_targetDiameter;

	std::string m_currentHeightmapFilepath;
	std::string m_currentBlendmapFilepath;

	void createBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void createQuadPatchGrid(std::vector<Vertex::Terrain>& vertices, std::vector<UINT>& indices);

	void buildHeightmapSRV(ID3D11Device* device);
	void buildBlendmapSRV(ID3D11Device* device);

	void smooth(void);
	float average(int i, int j);
	bool inBounds(int i, int j);

	float computeTriangleIntersection(const Ray& ray, const XMVECTOR triangle[3]);
};

#endif