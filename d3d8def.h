#pragma once

#include "AsukaLib.h"

typedef IUnknown    IDirect3D8;
typedef IDirect3D8* LPDIRECT3DDEVICE8;
typedef IUnknown    IDirect3DDevice8;
typedef IUnknown    IDirect3DTexture8;
typedef IUnknown    IDirect3DVolumeTexture8;
typedef IUnknown    IDirect3DCubeTexture8;
typedef IUnknown    IDirect3DVertexBuffer8;
typedef IUnknown    IDirect3DIndexBuffer8;

typedef DWORD D3DDEVTYPE;

typedef struct _pseudo_D3DPRESENT_PARAMETERS {
    UINT                BackBufferWidth;
    UINT                BackBufferHeight;
    DWORD               BackBufferFormat;
    UINT                BackBufferCount;
    DWORD               MultiSampleType;
    DWORD               SwapEffect;
    HWND                hDeviceWindow;
    BOOL                Windowed;
    BOOL                EnableAutoDepthStencil;
    DWORD               AutoDepthStencilFormat;
    DWORD               Flags;
    UINT                FullScreen_RefreshRateInHz;
    UINT                FullScreen_PresentationInterval;
} D3DPRESENT_PARAMETERS;
