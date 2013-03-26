#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "StdAfx.h"

enum RENDER_TARGET_TYPE
{
	SHADOW_MAP,
	TEXTURE_MAP,
};

struct RENDER_TARGET_DESC
{
	RENDER_TARGET_TYPE		Type;
	UINT32					textureWidth;
	UINT32					textureHeight;
	float					screenDepth;
	float					screenNear;
};

class renderTarget
{
public:
	renderTarget(void);
	~renderTarget(void);

	void Init(ID3D11Device*, RENDER_TARGET_DESC&);
	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float red, float green, float blue, float alpha);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11RenderTargetView*		m_renderTargetView;
	ID3D11ShaderResourceView*	m_shaderResourceView;
	ID3D11DepthStencilView*		m_depthStencilView;

	D3D11_VIEWPORT m_viewport;

	RENDER_TARGET_TYPE			m_Type;
};

#endif

