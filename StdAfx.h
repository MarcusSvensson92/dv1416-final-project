#ifndef STDAFX_H
#define STDAFX_H

#include <D3D11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <DxErr.h>
#include <d3dx11effect.h>
#include <d3dx10math.h>
#include <d3dCompiler.h>

#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>

#include <string>
#include <vector>
#include <sstream>

#define RELEASE(x) { if (x) { x->Release(); x = NULL; } }

#define PI 3.14159265358979323846f

#endif