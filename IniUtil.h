#pragma once

#include "AsukaLib.h"

#define INI_FILE "AsukaLib.ini"

typedef struct _IniData {
	DWORD Priority;//Priority Default: 2(NORMAL)
	DWORD WaitVal;// Sleep value Default: 10ms
	LONG BGMVol;// BGM Volume: 0(Max)
	LONG SEVol;// SE Volume: 0(Max)
	BOOL EnableDraw;
	BOOL EnableSound;
	BOOL BGMMute;
	BOOL SEMute;
	BOOL LockConfig;

	INT WindowLoc_h;
	INT WindowLoc_l;
	INT WindowSize_h;
	INT WindowSize_l;
	BOOL WindowResizable;
	BOOL HoldAspectRatio;
	BOOL SaveWindowSettings;
} IniData;

VOID LoadIniData(IniData *lpIni);
VOID SaveIniData(IniData *lpIni);
