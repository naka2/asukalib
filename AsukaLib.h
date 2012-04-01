#pragma once

#pragma warning(disable:4312)
//#pragma comment(lib, "d3d8.lib")
//#pragma comment(lib, "d3dx8.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//#define WIN32_LEAN_AND_MEAN
//#define _WIN32_WINNT 0x0400

#define DIRECTINPUT_VERSION 0x0800
#define DIRECTSOUND_VERSION 0x0800

#include <windows.h>
#include <comctrl.h>
#include <stdio.h>
#include <string.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <dinput.h>
#include <dsound.h>

#include "d3d8def.h"
#include "SoundBuffer.h"
#include "DbgUtil.h"
#include "IniUtil.h"
#include "HookDsound.h"
#include "HookD3d.h"
#include "HookDinput.h"
#include "resource.h"

#define DLLVERSION #PRODUCTVERSION
#define TARGET_EXE "AsfPC.exe"
#define TARGET_WNDCLASS "AsfPC"

void *SetHook(void *target,  void *hookproc);
LRESULT CALLBACK Hook_WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
INT_PTR CALLBACK ConfigDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam);
void Indicate(DWORD P, DWORD S,LONG BGM,LONG SE);

extern HINSTANCE D3D8inst,DI8inst,DS8inst;
extern HANDLE hDLL;

extern WNDPROC pWindowProc;
extern HWND MainWnd;

extern LPDIRECT3DDEVICE8 MainDD8;
extern LPDIRECTINPUT8 MainDI8;
extern LPDIRECTSOUND8 MainDS8;
extern HMENU MainPopup,SubPopup;
extern BOOL bMinimize, bFullscreen, bActive,bIniLoaded;

extern CHAR LvUpSoundSignature[];
extern LPDIRECTSOUNDBUFFER8 lpLvUpSound;
