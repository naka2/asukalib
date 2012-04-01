#pragma once
#include "AsukaLib.h"
#if 0 // DirectX8 is deprecated for latest DirectX SDK
#include <d3d8.h>
#include <d3dx8.h>
#else
#include "d3d8def.h"
#endif // 0

typedef IDirect3D8* (WINAPI *FuncPtrDirect3DCreate8)(UINT);
typedef HRESULT	    (WINAPI *FuncPtrD3D8CreateDevice)(IDirect3D8 *, UINT ,D3DDEVTYPE ,HWND ,DWORD ,D3DPRESENT_PARAMETERS* ,IDirect3DDevice8**);
typedef HRESULT     (WINAPI *FuncPtrD3D8DReset)(IDirect3DDevice8 *, D3DPRESENT_PARAMETERS*);
typedef HRESULT     (WINAPI *FuncPtrD3D8DPresent)(IDirect3DDevice8 *, CONST RECT* , CONST RECT*, HWND , CONST RGNDATA*);

IDirect3D8* WINAPI Direct3DCreate8(UINT SDKVersion);
HRESULT     WINAPI D3D8DReset(IDirect3DDevice8 *lpD3DD8, D3DPRESENT_PARAMETERS* pPresentationParameters);
HRESULT     WINAPI D3D8DPresent(IDirect3DDevice8 *lpD3DD8, CONST RECT* pSourceRect,CONST RECT* pDestRect
					             ,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
HRESULT     WINAPI D3D8CreateDevice(IDirect3D8 *lpD3D8, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags
                                     ,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface);
