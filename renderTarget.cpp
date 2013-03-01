#include "renderTarget.h"


renderTarget::renderTarget(void)
{
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
	m_depthStencilView = 0;
}

renderTarget::~renderTarget(void)
{
	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	if(m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}
	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
}

void renderTarget::Init(ID3D11Device* device, RENDER_TARGET_DESC& initDesc)
{
	D3D11_TEXTURE2D_DESC				textureDesc;	
	D3D11_RENDER_TARGET_VIEW_DESC		renderTargetViewDesc;
	D3D11_TEXTURE2D_DESC				depthBufferDesc;	
	D3D11_DEPTH_STENCIL_VIEW_DESC		depthStencilViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC		shaderResourceViewDesc;

	// Set the type of render target
	m_Type = initDesc.Type;

	if ( m_Type == TEXTURE_MAP )
	{
		// Initialize the render target texture description.
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Setup the render target texture description.
		textureDesc.Width				= initDesc.textureWidth;
		textureDesc.Height				= initDesc.textureHeight;
		textureDesc.MipLevels			= 1;
		textureDesc.ArraySize			= 1;
		textureDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.Usage				= D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | 
										  D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags		= 0;
		textureDesc.MiscFlags			= 0;

		// Create the render target texture.
		ID3D11Texture2D* m_renderTargetTexture;
		device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

		// Initailze the render target view description.
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

		// Setup the description of the render target view.
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width				= initDesc.textureWidth;
	depthBufferDesc.Height				= initDesc.textureHeight;
	depthBufferDesc.MipLevels			= 1;
	depthBufferDesc.ArraySize			= 1;
	depthBufferDesc.Format				= DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count	= 1;
	depthBufferDesc.SampleDesc.Quality	= 0;
	depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL |
										  D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ID3D11Texture2D* m_depthStencilBuffer;
	device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);


	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	device->CreateShaderResourceView(m_depthStencilBuffer, &shaderResourceViewDesc, &m_shaderResourceView);

	// Setup the viewport for rendering.
    m_viewport.Width = (float)initDesc.textureWidth;
    m_viewport.Height = (float)initDesc.textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, (float)D3DX_PI * 0.5f, (float)initDesc.textureWidth / (float)initDesc.textureHeight, initDesc.screenNear, initDesc.screenDepth);
}

void renderTarget::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	if ( m_Type == TEXTURE_MAP )
	{
		// Bind the render target view and depth stencil buffer to the output render pipeline.
		deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	}
	if ( m_Type == SHADOW_MAP )
	{
		// Bind the depth stencil buffer to the output render pipeline.
		deviceContext->OMSetRenderTargets(0, 0, m_depthStencilView);
	}

	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);
}

void renderTarget::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	if ( m_Type == TEXTURE_MAP )
	{
		float color[4];

		// Setup the color to clear the buffer to.
		color[0] = red;
		color[1] = green;
		color[2] = blue;
		color[3] = alpha;

		// Clear the back buffer.
		deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	}
    
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* renderTarget::GetShaderResourceView()
{
	return m_shaderResourceView;
}

D3DXMATRIX renderTarget::GetProjectionMatrix()
{
	return m_projectionMatrix;
}