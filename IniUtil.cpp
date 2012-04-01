#include "IniUtil.h"

VOID LoadIniData(IniData *lpIni)
{
	char path[MAX_PATH];

	GetCurrentDirectory(MAX_PATH-1,path);
	wsprintf(path,"%s\\%s",path,INI_FILE);

	lpIni->LockConfig = GetPrivateProfileInt("System","LockConfig",0,path);
	lpIni->EnableDraw = GetPrivateProfileInt("System","EnableDrawContinue",1,path);
	lpIni->Priority = GetPrivateProfileInt("System","Priority",2,path);
	lpIni->WaitVal = GetPrivateProfileInt("System","Wait",10,path);

	lpIni->EnableSound = GetPrivateProfileInt("Sound","EnableSoundContinue",1,path);
	lpIni->BGMMute = GetPrivateProfileInt("Sound","MuteBGM",0,path);
	lpIni->SEMute = GetPrivateProfileInt("Sound","MuteSE",0,path);
	lpIni->BGMVol = GetPrivateProfileInt("Sound","BGMVolume",0,path);
	lpIni->SEVol = GetPrivateProfileInt("Sound","SEVolume",0,path);

	lpIni->SaveWindowSettings = GetPrivateProfileInt("Window","SaveWindowSettings",1,path);
	lpIni->WindowResizable = GetPrivateProfileInt("Window","Resizable",0,path);
	lpIni->HoldAspectRatio = GetPrivateProfileInt("Window","HoldAspectRatio",0,path);
	lpIni->WindowLoc_h = GetPrivateProfileInt("Window","Location_Hight",0,path);
	lpIni->WindowLoc_l = GetPrivateProfileInt("Window","Location_Left",0,path);
	lpIni->WindowSize_h = GetPrivateProfileInt("Window","Size_Hight",0,path);
	lpIni->WindowSize_l = GetPrivateProfileInt("Window","Size_Left",0,path);

	lpIni->LockConfig = (lpIni->LockConfig > 1 || lpIni->LockConfig < 0)?0:lpIni->LockConfig;
	lpIni->EnableDraw = (lpIni->EnableDraw > 1 || lpIni->EnableDraw < 0)?1:lpIni->EnableDraw;
	lpIni->Priority = (lpIni->Priority > 4 || lpIni->Priority < 1)?2:lpIni->Priority;
	lpIni->WaitVal = (lpIni->WaitVal > 1000 || lpIni->WaitVal < 0)?10:lpIni->WaitVal;

	lpIni->EnableSound = (lpIni->EnableSound > 1 || lpIni->EnableSound < 0)?1:lpIni->EnableSound;
	lpIni->BGMMute = (lpIni->BGMMute > 1 || lpIni->BGMMute < 0)?0:lpIni->BGMMute;
	lpIni->SEMute = (lpIni->SEMute > 1 || lpIni->SEMute < 0)?0:lpIni->SEMute;
	lpIni->BGMVol = (lpIni->BGMVol > 0 || lpIni->BGMVol < -10000)?0:lpIni->BGMVol;
	lpIni->SEVol = (lpIni->SEVol > 0 || lpIni->SEVol < -10000)?0:lpIni->SEVol;

	lpIni->SaveWindowSettings = (lpIni->SaveWindowSettings > 1 || lpIni->SaveWindowSettings < 0)?1:lpIni->SaveWindowSettings;
	lpIni->WindowResizable = (lpIni->WindowResizable > 1 || lpIni->WindowResizable < 0)?0:lpIni->WindowResizable;
	lpIni->HoldAspectRatio = (lpIni->HoldAspectRatio > 1 || lpIni->HoldAspectRatio < 0)?0:lpIni->HoldAspectRatio;
	lpIni->WindowLoc_h = (lpIni->WindowLoc_h > 65535 || lpIni->WindowLoc_h < -65535)?0:lpIni->WindowLoc_h;
	lpIni->WindowLoc_l = (lpIni->WindowLoc_l > 65535 || lpIni->WindowLoc_l < -65535)?0:lpIni->WindowLoc_l;
	lpIni->WindowSize_h = (lpIni->WindowSize_h > 65535 || lpIni->WindowSize_h < -65535)?0:lpIni->WindowSize_h;
	lpIni->WindowSize_l = (lpIni->WindowSize_l > 65535 || lpIni->WindowSize_l < -65535)?0:lpIni->WindowSize_l;
}

VOID SaveIniData(IniData *lpIni)
{
	CHAR path[MAX_PATH],tmp[64];

	GetCurrentDirectory(MAX_PATH-1,path);
	wsprintf(path,"%s\\%s",path,INI_FILE);

	wsprintf(tmp,"%d",lpIni->LockConfig);
	WritePrivateProfileString("System","LockConfig",tmp,path);
	wsprintf(tmp,"%d",lpIni->EnableDraw);
	WritePrivateProfileString("System","EnableDrawContinue",tmp,path);
	wsprintf(tmp,"%d",lpIni->Priority);
	WritePrivateProfileString("System","Priority",tmp,path);
	wsprintf(tmp,"%d",lpIni->WaitVal);
	WritePrivateProfileString("System","Wait",tmp,path);

	wsprintf(tmp,"%d",lpIni->EnableSound);
	WritePrivateProfileString("Sound","EnableSoundContinue",tmp,path);
	wsprintf(tmp,"%d",lpIni->BGMMute);
	WritePrivateProfileString("Sound","MuteBGM",tmp,path);
	wsprintf(tmp,"%d",lpIni->SEMute);
	WritePrivateProfileString("Sound","MuteSE",tmp,path);
	wsprintf(tmp,"%d",lpIni->BGMVol);
	WritePrivateProfileString("Sound","BGMVolume",tmp,path);
	wsprintf(tmp,"%d",lpIni->SEVol);
	WritePrivateProfileString("Sound","SEVolume",tmp,path);

	wsprintf(tmp,"%d",lpIni->SaveWindowSettings);
	WritePrivateProfileString("Window","SaveWindowSettings",tmp,path);
	wsprintf(tmp,"%d",lpIni->WindowResizable);
	WritePrivateProfileString("Window","Resizable",tmp,path);
	wsprintf(tmp,"%d",lpIni->HoldAspectRatio);
	WritePrivateProfileString("Window","HoldAspectRatio",tmp,path);
	wsprintf(tmp,"%d",lpIni->WindowLoc_h);
	WritePrivateProfileString("Window","Location_Hight",tmp,path);
	wsprintf(tmp,"%d",lpIni->WindowLoc_l);
	WritePrivateProfileString("Window","Location_Left",tmp,path);
	wsprintf(tmp,"%d",lpIni->WindowSize_h);
	WritePrivateProfileString("Window","Size_Hight",tmp,path);
	wsprintf(tmp,"%d",lpIni->WindowSize_l);
	WritePrivateProfileString("Window","Size_Left",tmp,path);
}
