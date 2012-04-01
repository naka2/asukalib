#include "HookD3d.h"

FuncPtrDirect3DCreate8  fpD3DCreate8;
FuncPtrD3D8CreateDevice fpD3D8CD;
FuncPtrD3D8DReset       fpD3DD8Reset;
FuncPtrD3D8DPresent     fpD3DD8P;

extern IniData                    Ini;
extern FuncPtrDirectSoundCreate8  fpDSCreate8;
extern FuncPtrDSCreateSoundBuffer fpDSCSB;
extern FuncPtrDirectInput8Create  fpDI8Create;
extern FuncPtrDICreateDevice      fpDICD;

/*** Direct3DDevice8 オブジェクトメソッドフック関数 ***/
HRESULT WINAPI D3D8DReset(IDirect3DDevice8 *lpD3DD8, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	bFullscreen = pPresentationParameters->Windowed;
	return (*fpD3DD8Reset)(lpD3DD8,pPresentationParameters);
}

HRESULT WINAPI D3D8DPresent(IDirect3DDevice8 *lpD3DD8, CONST RECT* pSourceRect,CONST RECT* pDestRect
							,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	Sleep(Ini.WaitVal);
	return (*fpD3DD8P)(lpD3DD8,pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
}

/*** Direct3D8 ラッパ関数 ***/
IDirect3D8* WINAPI Direct3DCreate8(UINT SDKVersion)
{
	DEBUG_OUT("Direct3DCreate8 %X \n",SDKVersion);
	IDirect3D8 *lpD3D8;

	lpD3D8 = (*fpD3DCreate8)(SDKVersion);

	if(lpD3D8)
	{
		if(!fpD3D8CD) fpD3D8CD = (FuncPtrD3D8CreateDevice)SetHook((void *)(*(DWORD *)lpD3D8 + 15*4),D3D8CreateDevice);
	}	
	return lpD3D8;
}

/*** Direct3D8 オブジェクトメソッドフック関数 ***/
HRESULT WINAPI D3D8CreateDevice(IDirect3D8 *lpD3D8, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags
								,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface)
{
	DEBUG_OUT("D3D8CreateDevice\n");
	HRESULT ret;
	LPDIRECTINPUT8 lpDI8 = NULL;
	IDirect3DDevice8 *lpDD = NULL;
	LPDIRECTSOUND8 lpDS8 = NULL;
	CHAR ClassName[128];

	DEBUG_OUT("Direct3DCreate. Address = %X\n",lpD3D8);
	if(!MainWnd && hFocusWindow 
		&& GetClassName(hFocusWindow,ClassName,128) && !lstrcmpi(ClassName, TARGET_WNDCLASS))
	{
			// ゲームウィンドウに対してCreateDeviceが呼ばれたら
			// ローカルフックをかけてDirectInputもフック
			DEBUG_OUT("TargetWindow(Class:%s) Found. Hooking WindowProc...\n",ClassName);
			MainWnd = hFocusWindow;
			ret = (*fpDI8Create)((HINSTANCE)GetWindowLong(hFocusWindow,GWL_HINSTANCE),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&lpDI8,NULL);
			if(lpDI8)
			{
				DEBUG_OUT("DirectInput8 Hook Success. Address = %X\n",lpDI8);
				fpDICD = (FuncPtrDICreateDevice)SetHook((void *)(*(DWORD *)lpDI8 + 3*4),DICreateDevice);
			}
			else
			{
				DEBUG_OUT("Cannot Initialize DirectInput8 ret = %X\n",ret);
			}
			
			ret = (*fpDSCreate8)(&DSDEVID_DefaultPlayback,&lpDS8,NULL);
			if(lpDS8)
			{
				DEBUG_OUT("DirectSound8 Hook Success. Address = %X\n",lpDS8);
				if(!fpDSCSB) fpDSCSB = (FuncPtrDSCreateSoundBuffer)SetHook((void *)(*(DWORD *)lpDS8 + 3*4),DSCreateSoundBuffer);
			}
			else
			{
				DEBUG_OUT("Cannot Initialize DirectSound8 ret = %X\n",ret);
			}
			
			pWindowProc = (WNDPROC)SetWindowLong(hFocusWindow, GWL_WNDPROC, (DWORD)Hook_WindowProc);

			if(Ini.WindowResizable)
			{
				SetWindowLong(hFocusWindow,GWL_STYLE,GetWindowLong(hFocusWindow,GWL_STYLE)|WS_MAXIMIZEBOX|WS_THICKFRAME);
			}
	
			SetWindowPos(hFocusWindow,NULL,Ini.WindowLoc_l,Ini.WindowLoc_h,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

			if(Ini.WindowSize_h || Ini.WindowSize_l)
			{
				SetWindowPos(hFocusWindow,NULL,0,0,Ini.WindowSize_l,Ini.WindowSize_h,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
			}

			Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
	}

	ret = (*fpD3D8CD)(lpD3D8,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters,ppReturnedDeviceInterface);
	if(lpDD = *ppReturnedDeviceInterface)
	{
		DEBUG_OUT("Direct3DDevice Hook Success. Address = %X\n",lpDD);
		MainDD8 = lpDD;
		fpD3DD8Reset = (FuncPtrD3D8DReset)SetHook((void *)(*(DWORD *)lpDD + 14*4),D3D8DReset);
		fpD3DD8P = (FuncPtrD3D8DPresent)SetHook((void *)(*(DWORD *)lpDD + 15*4),D3D8DPresent);
	}

	return ret;
}
