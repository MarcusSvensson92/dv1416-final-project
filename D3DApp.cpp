#include "StdAfx.h"
#include "D3DApp.h"

D3DApp* g_d3dApp = NULL;

LRESULT CALLBACK msgRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_d3dApp->wndProc(hWnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE	   hInstance,
		 const std::string&    wndCaption,
		 const DWORD		   wndStyle,
		 const unsigned int	   clientWidth,
		 const unsigned int	   clientHeight,
		 const D3D_DRIVER_TYPE driverType)
:	 m_hInstance(hInstance),
	 m_hWnd(NULL),

	 m_device(NULL),
	 m_deviceContext(NULL),
	 m_swapChain(NULL),
	 m_renderTargetView(NULL),
	 m_depthStencilBuffer(NULL),
	 m_depthStencilView(NULL)
{
	m_wndCaption   = wndCaption;
	m_wndStyle     = wndStyle;
	m_clientWidth  = clientWidth;
	m_clientHeight = clientHeight;
	m_driverType   = driverType;

	m_paused	= false;
	m_minimized = false;
	m_maximized = false;
	m_resizing  = false;

	g_d3dApp = this;
}

D3DApp::~D3DApp(void)
{
	RELEASE(m_renderTargetView);
	RELEASE(m_depthStencilView);
	RELEASE(m_swapChain);
	RELEASE(m_depthStencilBuffer);
	if (m_deviceContext) m_deviceContext->ClearState();
	RELEASE(m_deviceContext);
	if (m_device) { while(m_device->Release()); m_device = NULL; }
}

int D3DApp::run(void)
{
	m_timer.reset();

	MSG msg = {0};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.tick();

			if (!m_timer.isStopped())
			{
				update();
				render();
			}
		}
	}
	return (int)msg.wParam;
}

bool D3DApp::init(void)
{
	if (!initMainWindow()) return false;
	if (!initDirect3D())   return false;
	initInputDevice();

	return true;
}

LRESULT D3DApp::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_paused = true;
			m_timer.stop();
		}
		else
		{
			m_paused = false;
			m_timer.start();
		}
		break;

	case WM_ENTERSIZEMOVE:
		m_paused   = true;
		m_resizing = true;
		m_timer.stop();
		break;
	case WM_SIZE:
		m_clientWidth  = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);
		if (m_device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_paused	= true;
				m_minimized = true;
				m_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_paused	= false;
				m_minimized = false;
				m_maximized = true;
				onResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_minimized)
				{
					m_paused	= false;
					m_minimized = false;
					onResize();
				}
				else if (m_maximized)
				{
					m_paused	= false;
					m_maximized = false;
					onResize();
				}
				else if (m_resizing) { }
				else
				{
					onResize();
				}
			}
		}
		break;
	case WM_EXITSIZEMOVE:
		m_paused   = false;
		m_resizing = false;
		m_timer.start();
		onResize();
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		onMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		onMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_INPUT:
	{
		UINT dwSize = 40;
		BYTE lpb[40];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
						lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEMOUSE) 
		{
			int dx = raw->data.mouse.lLastX;
			int dy = raw->data.mouse.lLastY;
			onMouseMove(dx, dy);
		}
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

bool D3DApp::initMainWindow(void)
{
	std::string className = "MainWindowClass";

	WNDCLASS wc;
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = msgRouter;
	wc.cbClsExtra	 = NULL;
	wc.cbWndExtra	 = NULL;
	wc.hInstance	 = m_hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = className.c_str();
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "RegisterClass failed.", m_wndCaption.c_str(), MB_ICONERROR);
		return false;
	}

	RECT rect = {0, 0, m_clientWidth, m_clientHeight};
	AdjustWindowRect(&rect, m_wndStyle, false);
	int wndWidth  = rect.right  - rect.left;
	int wndHeight = rect.bottom - rect.top;

	m_hWnd = CreateWindow(className.c_str(), m_wndCaption.c_str(), m_wndStyle,
						  CW_USEDEFAULT, CW_USEDEFAULT, wndWidth, wndHeight,
						  NULL, NULL, m_hInstance, NULL);
	if (!m_hWnd)
	{
		MessageBox(NULL, "CreateWindow failed.", m_wndCaption.c_str(), MB_ICONERROR);
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

bool D3DApp::initDirect3D(void)
{
	HRESULT hr;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width					  = m_clientWidth;
	sd.BufferDesc.Height				  = m_clientHeight;
	sd.BufferDesc.RefreshRate.Numerator   = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count					  = 1;
	sd.SampleDesc.Quality				  = 0;
	sd.BufferUsage						  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount						  = 1;
	sd.OutputWindow						  = m_hWnd;
	sd.Windowed							  = true;

	hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags,
									   featureLevels, ARRAYSIZE(featureLevels),
									   D3D11_SDK_VERSION, &sd, &m_swapChain,
									   &m_device, &featureLevel, &m_deviceContext);
	if (FAILED(hr))
	{
		MessageBox(NULL, "D3D11CreateDeviceAndSwapChain failed.", m_wndCaption.c_str(), MB_ICONERROR);
		return false;
	}

	onResize();

	return true;
}

void D3DApp::initInputDevice(void)
{
	RAWINPUTDEVICE rawInputDevice[1];
    rawInputDevice[0].usUsagePage = ((USHORT) 0x01);
    rawInputDevice[0].usUsage	  = ((USHORT) 0x02);
    rawInputDevice[0].dwFlags	  = RIDEV_INPUTSINK;
    rawInputDevice[0].hwndTarget  = m_hWnd;
    RegisterRawInputDevices(rawInputDevice, 1, sizeof(rawInputDevice[0]));
}

void D3DApp::onResize(void)
{
	assert(m_deviceContext);
	assert(m_device);
	assert(m_swapChain);

	RELEASE(m_renderTargetView);
	RELEASE(m_depthStencilView);
	RELEASE(m_depthStencilBuffer);

	m_swapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D11Texture2D* backBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	m_device->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView);
	RELEASE(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width				= m_clientWidth;
	depthStencilDesc.Height				= m_clientHeight;
	depthStencilDesc.MipLevels			= 1;
	depthStencilDesc.ArraySize			= 1;
	depthStencilDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.SampleDesc.Count	= 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags		= 0;
	depthStencilDesc.MiscFlags			= 0;
	m_device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer);
	m_device->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView);

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	m_viewport.Width	= (float)m_clientWidth;
	m_viewport.Height	= (float)m_clientHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1, &m_viewport);
}