#include "AsukaLib.h"

HINSTANCE D3D8inst,DI8inst,DS8inst;
HANDLE hDLL;

IniData Ini;
WNDPROC pWindowProc;
HWND MainWnd;

LPDIRECT3DDEVICE8 MainDD8;
LPDIRECTINPUT8 MainDI8;
LPDIRECTSOUND8 MainDS8;
HMENU MainPopup,SubPopup;
BOOL bMinimize, bFullscreen, bActive = 1,bIniLoaded;

extern FuncPtrDirect3DCreate8 fpD3DCreate8;
extern FuncPtrDirectInput8Create fpDI8Create;
extern FuncPtrDirectSoundCreate8 fpDSCreate8;

LPCSTR rgwzPriorityName[] = {"null", "Idle", "Normal", "High", "RealTime"};
LPCSTR wzTitleFormat4Args = "風来のシレン外伝 女剣士アスカ見参！ with AsukaLib P:%s W:%d BGM:%s SE:%s";

void Indicate(DWORD P, DWORD S,LONG BGM,LONG SE)
{
	CHAR Title[128],BGMStr[32],SEStr[32];

	if(!MainWnd)
	{
		return;
	}

	if(Ini.BGMMute || BGM == DSBVOLUME_MIN)
	{
		lstrcpy(BGMStr,"Mute");
	}
	else
	{
		wsprintf(BGMStr,"%d",(10000+BGM)/100);
	}

	if(Ini.SEMute || SE == DSBVOLUME_MIN)
	{
		lstrcpy(SEStr,"Mute");
	}
	else
	{
		wsprintf(SEStr,"%d",(10000+SE)/100);
	}

	sprintf_s(Title,ARRAYSIZE(Title),wzTitleFormat4Args,rgwzPriorityName[P],S,BGMStr,SEStr);

	SetWindowText(MainWnd,Title);
}


/*** VTBL書き換えによるAPIフック(クラスメソッド用) ***/
void *SetHook(void *target,  void *hookproc)
{
	DWORD tmp, oldprot;
	tmp = *(DWORD *)target;
	
	if(tmp == (DWORD)hookproc)
	{
		return 0;
	}
	
	if(!VirtualProtect(target, 4, PAGE_READWRITE, &oldprot)){
		return 0;
	}
	
	*(DWORD *)target = (DWORD)hookproc;
	VirtualProtect(target, 4, oldprot, &oldprot);
	return (void *)tmp;
}


/*** ローカルフックされたゲームウィンドウのウィンドウプロシージャ ***/
LRESULT CALLBACK Hook_WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	DWORD PriorityClass[] = {0, IDLE_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS
		, HIGH_PRIORITY_CLASS, REALTIME_PRIORITY_CLASS};
	RECT WindowRect,DesktopRect,ClientRect;
	int LocX,LocY;
	switch(message)
	{
		case WM_DESTROY:
			if(Ini.SaveWindowSettings)
			{
				GetWindowRect(MainWnd,&WindowRect);
				Ini.WindowLoc_h = WindowRect.top;
				Ini.WindowLoc_l = WindowRect.left;
				Ini.WindowSize_h = WindowRect.bottom - WindowRect.top;
				Ini.WindowSize_l = WindowRect.right - WindowRect.left;
			}

			break;
		case WM_NCACTIVATE:
			if(bFullscreen || lparam)
			{
				break;
			}

			if(!wparam)
			{
				bActive = FALSE;
				if(!Ini.EnableSound)
					SetVolumeForeachSoundBufferChain(TYPE_ALL,DSBVOLUME_MIN);
			}
			else
			{
				bActive = TRUE;
				if(!Ini.EnableSound)
				{
					SetVolumeForeachSoundBufferChain(TYPE_ALL,VOLUME_ALL);
				}
			}

			Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
			DefWindowProc(hwnd,message,wparam,lparam);

			if(Ini.EnableDraw)
			{
				return FALSE;
			}

			break;

		case WM_SIZE:
			if(wparam == SIZE_RESTORED)
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			else if(wparam == SIZE_MINIMIZED)
			{
				SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
				bMinimize = 1;
				Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
			}
			else if(wparam == SIZE_MAXIMIZED)
			{
				SetPriorityClass(GetCurrentProcess(), PriorityClass[Ini.Priority]);
				bMinimize = 0;
				Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
			}
			break;

		case WM_SETCURSOR:
			if((HWND)wparam == MainWnd && Ini.WindowResizable)
			{
				switch(lparam&0xFFFF )
				{
					case HTTOP:
					case HTBOTTOM:
						SetCursor(LoadCursor(NULL,IDC_SIZENS));
						break;
					case HTLEFT:
					case HTRIGHT:
						SetCursor(LoadCursor(NULL,IDC_SIZEWE));
						break;
					case HTTOPLEFT:
					case HTBOTTOMRIGHT:
						SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
						break;
					case HTTOPRIGHT:
					case HTBOTTOMLEFT:
						SetCursor(LoadCursor(NULL,IDC_SIZENESW));
						break;
					default:
						SetCursor(LoadCursor(NULL,IDC_ARROW));
						break;
				}
				return 1;
			}
			return 0;

		case WM_SIZING:
			if(Ini.WindowResizable)
			{
				RECT *rect = (RECT *)lparam;
				GetClientRect(MainWnd,&ClientRect);
				GetWindowRect(MainWnd,&WindowRect);
				double Ratio = ((double)(ClientRect.right-ClientRect.left)/(double)(ClientRect.bottom-ClientRect.top));
				int width = rect->right-rect->left-((WindowRect.right-WindowRect.left)-(ClientRect.right-ClientRect.left));
				int height = rect->bottom-rect->top-((WindowRect.bottom-WindowRect.top)-(ClientRect.bottom-ClientRect.top));
				int dw = (int)(height * Ratio + 0.5) - width;
				int dh = (int)(width / Ratio + 0.5) - height;
	
				switch(wparam)
				{
					case WMSZ_TOP:
						SetCursor(LoadCursor(NULL,IDC_SIZENS));
						if(Ini.HoldAspectRatio)
						{
							rect->right += dw;
						}
						break;
	
					case WMSZ_BOTTOM:
						SetCursor(LoadCursor(NULL,IDC_SIZENS));
						if(Ini.HoldAspectRatio)
						{
							rect->right += dw;
						}
						break;	
	
					case WMSZ_LEFT:
						SetCursor(LoadCursor(NULL,IDC_SIZEWE));
						if(Ini.HoldAspectRatio)
						{
							rect->bottom += dh;
						}
						break;
	
					case WMSZ_RIGHT:
						SetCursor(LoadCursor(NULL,IDC_SIZEWE));
						if(Ini.HoldAspectRatio)
						{
							rect->bottom += dh;
						}
						break;
	
					case WMSZ_TOPLEFT:
						SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
						if(Ini.HoldAspectRatio)
						{
							if(dw > 0) rect->left -= dw;
							else rect->top -= dh;
						}
						break;
				
					case WMSZ_TOPRIGHT:
						SetCursor(LoadCursor(NULL,IDC_SIZENESW));
						if(Ini.HoldAspectRatio)
						{
							if(dw > 0) rect->right += dw;
							else rect->top -= dh;
						}
						break;
	
					case WMSZ_BOTTOMLEFT:
						SetCursor(LoadCursor(NULL,IDC_SIZENESW));
						if(Ini.HoldAspectRatio)
						{
		                    if(dw > 0) rect->left -= dw;
		                    else rect->bottom += dh;
						}
						break;
	
					case WMSZ_BOTTOMRIGHT:
						SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
						if(Ini.HoldAspectRatio)
						{
                            if(dw > 0) rect->right += dw;
				            else rect->bottom += dh;
						}
						break;
				}
			}
			break;
	
			case WM_KEYDOWN:
				if(!bActive)
				{
					return true;
				}
				if(!Ini.LockConfig)
				{
					if(GetKeyState(VK_CONTROL) & 0x8000)
					{
						if(GetKeyState(VK_SHIFT) & 0x8000)
						{
							if(wparam == VK_UP) Ini.WaitVal += 1;
							if(wparam == VK_DOWN && Ini.WaitVal) Ini.WaitVal -= 1;
							if(wparam == VK_PRIOR) Ini.SEVol += 100;
							if(wparam == VK_NEXT) Ini.SEVol -= 100;
							// WaitValを0~1000の範囲に収める
							Ini.WaitVal = Ini.WaitVal <= 1000 ? Ini.WaitVal >=0 ? Ini.WaitVal : 0 : 1000;
							// SEVolを-10000~0の範囲に収める
							Ini.SEVol = Ini.SEVol <= 0 ? Ini.SEVol >= -10000 ? Ini.SEVol : -10000 : 0;
							Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
							SetVolumeForeachSoundBufferChain(TYPE_SE,VOLUME_SE);
							break;
						}
						if(wparam == VK_UP) Ini.Priority ++;
						if(wparam == VK_DOWN) Ini.Priority --;
						if(wparam == VK_PRIOR) Ini.BGMVol += 100;
						if(wparam == VK_NEXT) Ini.BGMVol -= 100;
						// Priorityを1~4の範囲に収める
						Ini.Priority = Ini.Priority <= 4 ? Ini.Priority >= 1 ? Ini.Priority : 1 : 4;
						SetPriorityClass(GetCurrentProcess(), PriorityClass[Ini.Priority]);
						// BGMVolを-10000~0の範囲に収める
						Ini.BGMVol = Ini.BGMVol <= 0 ? Ini.BGMVol >=-10000 ? Ini.BGMVol : -10000 : 0;
						SetVolumeForeachSoundBufferChain(TYPE_BGM,VOLUME_BGM);
						Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
					}
				}
				break;

		case WM_RBUTTONUP:
			POINT pt;
			pt.x = LOWORD(lparam);
			pt.y = HIWORD(lparam);
			ClientToScreen(hwnd, &pt);
			TrackPopupMenu(SubPopup,TPM_LEFTALIGN,(int)pt.x,pt.y,0,hwnd,NULL);
			break;

		case WM_COMMAND:
			switch(LOWORD(wparam)){
				case IDM_CONFIG:
					if(DialogBox((HINSTANCE)hDLL,MAKEINTRESOURCE(IDD_CONFIG),hwnd,ConfigDialogProc)){
						SetVolumeForeachSoundBufferChain(TYPE_ALL,VOLUME_ALL);
						Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
					}	
					break;
				case IDM_MUTE_BGM:
					Ini.BGMMute = !Ini.BGMMute;
					if(Ini.BGMMute)
						CheckMenuItem(SubPopup,IDM_MUTE_BGM,MF_CHECKED);
					else
						CheckMenuItem(SubPopup,IDM_MUTE_BGM,MF_UNCHECKED);
					SetVolumeForeachSoundBufferChain(TYPE_BGM,VOLUME_BGM);
					Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
					break;
				case IDM_MUTE_SE:
					Ini.SEMute = !Ini.SEMute;
					if(Ini.SEMute)
						CheckMenuItem(SubPopup,IDM_MUTE_SE,MF_CHECKED);
					else
						CheckMenuItem(SubPopup,IDM_MUTE_SE,MF_UNCHECKED);
					SetVolumeForeachSoundBufferChain(TYPE_SE,VOLUME_SE);
					Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
					break;
				case IDM_LOCK_CONFIG:
					Ini.LockConfig = !Ini.LockConfig;
					if(Ini.LockConfig)
						CheckMenuItem(SubPopup,IDM_LOCK_CONFIG,MF_CHECKED);
					else
						CheckMenuItem(SubPopup,IDM_LOCK_CONFIG,MF_UNCHECKED);
					Indicate(Ini.Priority,Ini.WaitVal,Ini.BGMVol,Ini.SEVol);
					break;
				case IDM_RESIZABLE:
					Ini.WindowResizable = !Ini.WindowResizable;
					if(Ini.WindowResizable){
						CheckMenuItem(SubPopup,IDM_RESIZABLE,MF_CHECKED);
						EnableMenuItem(SubPopup,IDM_HOLDASPECTRATIO,MF_ENABLED);
						EnableMenuItem(SubPopup,IDM_SIZE_640,MF_ENABLED);
						EnableMenuItem(SubPopup,IDM_SIZE_800,MF_ENABLED);
						EnableMenuItem(SubPopup,IDM_SIZE_960,MF_ENABLED);
						SetWindowLong(MainWnd,GWL_STYLE,GetWindowLong(MainWnd,GWL_STYLE)|WS_MAXIMIZEBOX|WS_THICKFRAME);
					}else{
						CheckMenuItem(SubPopup,IDM_RESIZABLE,MF_UNCHECKED);
						EnableMenuItem(SubPopup,IDM_HOLDASPECTRATIO,MF_GRAYED);
						EnableMenuItem(SubPopup,IDM_SIZE_640,MF_GRAYED);
						EnableMenuItem(SubPopup,IDM_SIZE_800,MF_GRAYED);
						EnableMenuItem(SubPopup,IDM_SIZE_960,MF_GRAYED);
						SetWindowLong(MainWnd,GWL_STYLE,GetWindowLong(MainWnd,GWL_STYLE)^WS_MAXIMIZEBOX^WS_THICKFRAME);
					}
					InvalidateRect(NULL , NULL , TRUE);
					break;

				case IDM_HOLDASPECTRATIO:
					Ini.HoldAspectRatio = !Ini.HoldAspectRatio;
					if(Ini.HoldAspectRatio)
						CheckMenuItem(SubPopup,IDM_HOLDASPECTRATIO,MF_CHECKED);
					else
						CheckMenuItem(SubPopup,IDM_HOLDASPECTRATIO,MF_UNCHECKED);
					break;

				case IDM_SIZE_640:
					GetWindowRect(MainWnd,&WindowRect);
					GetClientRect(MainWnd,&ClientRect);
					SetWindowPos(MainWnd,NULL,0,0
						,(WindowRect.right-WindowRect.left)-(ClientRect.right-ClientRect.left)+640
						,(WindowRect.bottom-WindowRect.top)-(ClientRect.bottom-ClientRect.top)+480
						,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
					break;
				case IDM_SIZE_800:
					GetWindowRect(MainWnd,&WindowRect);
					GetClientRect(MainWnd,&ClientRect);
					SetWindowPos(MainWnd,NULL,0,0
						,(WindowRect.right-WindowRect.left)-(ClientRect.right-ClientRect.left)+800
						,(WindowRect.bottom-WindowRect.top)-(ClientRect.bottom-ClientRect.top)+600
						,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
					break;
				case IDM_SIZE_960:
					GetWindowRect(MainWnd,&WindowRect);
					GetClientRect(MainWnd,&ClientRect);
					SetWindowPos(MainWnd,NULL,0,0
						,(WindowRect.right-WindowRect.left)-(ClientRect.right-ClientRect.left)+960
						,(WindowRect.bottom-WindowRect.top)-(ClientRect.bottom-ClientRect.top)+720
						,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
					break;

				case IDM_LOC_LEFTTOP:
					SetWindowPos(MainWnd,NULL,0,0,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
					break;
				case IDM_LOC_LEFTBOTTOM:
					GetWindowRect(MainWnd,&WindowRect);
					GetWindowRect(GetDesktopWindow(),&DesktopRect);
					LocX = 0;
					LocY = DesktopRect.bottom - (WindowRect.bottom-WindowRect.top);
					SetWindowPos(MainWnd,NULL,LocX,LocY,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
					break;
				case IDM_LOC_RIGHTTOP:
					GetWindowRect(MainWnd,&WindowRect);
					GetWindowRect(GetDesktopWindow(),&DesktopRect);
					LocX = DesktopRect.right - (WindowRect.right-WindowRect.left);
					LocY = 0;
					SetWindowPos(MainWnd,NULL,LocX,LocY,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
					break;
				case IDM_LOC_RIGHTBOTTOM:
					GetWindowRect(MainWnd,&WindowRect);
					GetWindowRect(GetDesktopWindow(),&DesktopRect);
					LocX = DesktopRect.right - (WindowRect.right-WindowRect.left);
					LocY = DesktopRect.bottom - (WindowRect.bottom-WindowRect.top);
					SetWindowPos(MainWnd,NULL,LocX,LocY,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
					break;
				case IDM_LOC_CENTER:
					GetWindowRect(MainWnd,&WindowRect);
					GetWindowRect(GetDesktopWindow(),&DesktopRect);
					LocX = (DesktopRect.right - (WindowRect.right-WindowRect.left))/2;
					LocY = (DesktopRect.bottom - (WindowRect.bottom-WindowRect.top))/2;
					SetWindowPos(MainWnd,NULL,LocX,LocY,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
					break;

			}
			break;
	}
	return (*pWindowProc)(hwnd, message, wparam, lparam);
}

INT_PTR CALLBACK ConfigDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	CHAR tmp[16];
	LPCSTR PriorityName[] = {"null", "Idle", "Normal", "High", "RealTime"};
	switch(uMsg){
		case WM_INITDIALOG:
			SendDlgItemMessage(hwndDlg,IDSL_PRIORITY,TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(1,4));
			SendDlgItemMessage(hwndDlg,IDSL_WAIT,TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(0,1000));
			SendDlgItemMessage(hwndDlg,IDSL_BGM_VOLUME,TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(0,100));
			SendDlgItemMessage(hwndDlg,IDSL_SE_VOLUME,TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(0,100));
			SendDlgItemMessage(hwndDlg,IDSL_PRIORITY,TBM_SETPOS,(WPARAM)TRUE,(LPARAM)Ini.Priority);
			SendDlgItemMessage(hwndDlg,IDSL_WAIT,TBM_SETPOS,(WPARAM)TRUE,(LPARAM)Ini.WaitVal);
			SendDlgItemMessage(hwndDlg,IDSL_BGM_VOLUME,TBM_SETPOS,(WPARAM)TRUE,(LPARAM)(10000+Ini.BGMVol)/100);
			SendDlgItemMessage(hwndDlg,IDSL_SE_VOLUME,TBM_SETPOS,(WPARAM)TRUE,(LPARAM)(10000+Ini.SEVol)/100);
			SetDlgItemText(hwndDlg,IDL_PRIORITY,PriorityName[Ini.Priority]);
			wsprintf(tmp,"%d",Ini.WaitVal); SetDlgItemText(hwndDlg,IDL_WAIT,tmp);
			wsprintf(tmp,"%d",(10000+Ini.BGMVol)/100); SetDlgItemText(hwndDlg,IDL_BGM_VOLUME,tmp);
			wsprintf(tmp,"%d",(10000+Ini.SEVol)/100); SetDlgItemText(hwndDlg,IDL_SE_VOLUME,tmp);
			SendDlgItemMessage(hwndDlg,IDC_ENABLEDRAW,BM_SETCHECK,(WPARAM)Ini.EnableDraw?BST_CHECKED:BST_UNCHECKED,(LPARAM)0);
			SendDlgItemMessage(hwndDlg,IDC_ENABLESOUND,BM_SETCHECK,(WPARAM)Ini.EnableSound?BST_CHECKED:BST_UNCHECKED,(LPARAM)0);
			SendDlgItemMessage(hwndDlg,IDC_SAVEWINDOWSET,BM_SETCHECK,(WPARAM)Ini.SaveWindowSettings?BST_CHECKED:BST_UNCHECKED,(LPARAM)0);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDB_APPLY:
					{
						Ini.Priority = (DWORD)SendDlgItemMessage(hwndDlg,IDSL_PRIORITY,TBM_GETPOS,0,0);
						Ini.WaitVal = (DWORD)SendDlgItemMessage(hwndDlg,IDSL_WAIT,TBM_GETPOS,0,0);
						Ini.BGMVol = (LONG)(SendDlgItemMessage(hwndDlg,IDSL_BGM_VOLUME,TBM_GETPOS,0,0)*100)-10000;
						Ini.SEVol = (LONG)(SendDlgItemMessage(hwndDlg,IDSL_SE_VOLUME,TBM_GETPOS,0,0)*100)-10000;
						Ini.EnableDraw = (SendDlgItemMessage(hwndDlg,IDC_ENABLEDRAW,BM_GETCHECK,0,0)==BST_CHECKED)?TRUE:FALSE;
						Ini.EnableSound = (SendDlgItemMessage(hwndDlg,IDC_ENABLESOUND,BM_GETCHECK,0,0)==BST_CHECKED)?TRUE:FALSE;
						Ini.SaveWindowSettings = (SendDlgItemMessage(hwndDlg,IDC_SAVEWINDOWSET,BM_GETCHECK,0,0)==BST_CHECKED)?TRUE:FALSE;
						EndDialog(hwndDlg,1);
					}
					break;
				case IDB_CANCEL:
					EndDialog(hwndDlg,0);
					break;
			}
			return TRUE;
			break;
		case WM_HSCROLL:
			{
				LONG ID = GetDlgCtrlID((HWND)lParam);
				LONG num = (LONG)SendDlgItemMessage(hwndDlg,ID,TBM_GETPOS,0,0);
				switch(ID){
					case IDSL_PRIORITY:
						SetDlgItemText(hwndDlg,IDL_PRIORITY,PriorityName[num]);
						break;
					case IDSL_WAIT:
						wsprintf(tmp,"%d",num); SetDlgItemText(hwndDlg,IDL_WAIT,tmp);
						break;
					case IDSL_BGM_VOLUME:
						wsprintf(tmp,"%d",num); SetDlgItemText(hwndDlg,IDL_BGM_VOLUME,tmp);
						break;
					case IDSL_SE_VOLUME:
						wsprintf(tmp,"%d",num); SetDlgItemText(hwndDlg,IDL_SE_VOLUME,tmp);
						break;
				}
			}
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			return TRUE;
	}
	return FALSE;
}

/*** DLLメイン関数 ***/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	CHAR path[512],file[128],ext[64];
	switch(ul_reason_for_call){
		case DLL_PROCESS_DETACH:
			GetModuleFileName(NULL, path,MAX_PATH-1);
			DEBUG_OUT("AsukaLibは Module %s から切り離されました\n",path);
			DestroyMenu(MainPopup);
			if(D3D8inst) FreeLibrary(D3D8inst);
			if(DI8inst) FreeLibrary(DI8inst);
			if(DS8inst) FreeLibrary(DS8inst);
			if(MainDI8) MainDI8->Release();// DirectInputは明示的に生成したからRelease
			if(MainDS8) MainDS8->Release();// 
			if(bIniLoaded) SaveIniData(&Ini);
			ReleaseSoundBufferChain();
			return TRUE;
		case DLL_PROCESS_ATTACH:
			bIniLoaded = FALSE;
			GetModuleFileName(NULL, path,MAX_PATH-1);
			DEBUG_OUT("AsukaLibは Module %s から呼び出されました\n",path);
			hDLL = hModule;
			break;
		default:
			return TRUE;
	}



	GetModuleFileName(NULL,path,MAX_PATH-1);
	_splitpath_s(path,NULL,0,NULL,0,file,ARRAYSIZE(file),ext,ARRAYSIZE(ext));
	strcat_s(file,ARRAYSIZE(file),ext);
	if(lstrcmpi(file,TARGET_EXE) != 0){
		DEBUG_OUT("%sによるDLL呼び出しなのでフック中止(Target: %s)\n",file,TARGET_EXE);
		return TRUE;
	}

	LoadIniData(&Ini);
	bIniLoaded = TRUE;

	// ポップアップメニューのロード
	MainPopup = LoadMenu((HINSTANCE)hModule,MAKEINTRESOURCE(ID_POPUPMENU));
	SubPopup = GetSubMenu(MainPopup,0);

	// Iniからデフォルト状態の復帰
	CheckMenuItem(SubPopup,IDM_MUTE_BGM,Ini.BGMMute?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(SubPopup,IDM_MUTE_SE,Ini.SEMute?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(SubPopup,IDM_LOCK_CONFIG,Ini.LockConfig?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(SubPopup,IDC_SAVEWINDOWSET,Ini.SaveWindowSettings?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(SubPopup,IDM_RESIZABLE,Ini.WindowResizable?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(SubPopup,IDM_HOLDASPECTRATIO,Ini.HoldAspectRatio?MF_CHECKED:MF_UNCHECKED);
	EnableMenuItem(SubPopup,IDM_HOLDASPECTRATIO,Ini.WindowResizable?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(SubPopup,IDM_SIZE_640,Ini.WindowResizable?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(SubPopup,IDM_SIZE_800,Ini.WindowResizable?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(SubPopup,IDM_SIZE_960,Ini.WindowResizable?MF_ENABLED:MF_GRAYED);

	// Direct3Dをロード
	GetSystemDirectory(path,ARRAYSIZE(path));
	strcat_s(path,ARRAYSIZE(path),"\\d3d8.dll");
	D3D8inst = LoadLibrary(path);
	fpD3DCreate8 = (FuncPtrDirect3DCreate8)GetProcAddress(D3D8inst, "Direct3DCreate8");

	// DirectInput8をロード
	GetSystemDirectory(path,ARRAYSIZE(path));
	strcat_s(path,ARRAYSIZE(path),"\\dinput8.dll");
	DI8inst = LoadLibrary(path);
	fpDI8Create = (FuncPtrDirectInput8Create)GetProcAddress(DI8inst, "DirectInput8Create");

	// DirectSound8をロード
	GetSystemDirectory(path,ARRAYSIZE(path));
	strcat_s(path,ARRAYSIZE(path),"\\dsound.dll");
	DS8inst = LoadLibrary(path);
	fpDSCreate8 = (FuncPtrDirectSoundCreate8)GetProcAddress(DS8inst, "DirectSoundCreate8");

	return TRUE;
}
