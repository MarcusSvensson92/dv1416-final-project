#ifndef D3DAPP_H
#define D3DAPP_H

#include "StdAfx.h"
#include "Timer.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE			 hInstance,
		   const std::string&	 wndCaption,
		   const DWORD			 wndStyle,
		   const unsigned int	 clientWidth,
		   const unsigned int	 clientHeight,
		   const D3D_DRIVER_TYPE driverType);
	virtual ~D3DApp(void);

	int run(void);

	virtual bool init(void);
	virtual LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	HINSTANCE m_hInstance;
	HWND	  m_hWnd;

	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;
	IDXGISwapChain*			m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D*		m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT			m_viewport;

	Timer m_timer;

	std::string		m_wndCaption;
	DWORD			m_wndStyle;
	unsigned int	m_clientWidth;
	unsigned int	m_clientHeight;
	D3D_DRIVER_TYPE m_driverType;

	virtual void update(void) = 0;
	virtual void render(void) = 0;

	virtual void onMouseDown(WPARAM buttonState, int x, int y) = 0;
	virtual void onMouseUp(WPARAM buttonState, int x, int y)   = 0;
	virtual void onMouseMove(int dx, int dy)				   = 0;

	virtual void onResize(void);
private:
	bool m_paused;
	bool m_minimized;
	bool m_maximized;
	bool m_resizing;

	bool initMainWindow(void);
	bool initDirect3D(void);
	void initInputDevice(void);
};

#endif