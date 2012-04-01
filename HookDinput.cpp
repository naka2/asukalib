#include "HookDinput.h"

FuncPtrDirectInput8Create fpDI8Create;
FuncPtrDICreateDevice fpDICD;
FuncPtrDIDAcquire fpDIDA;
FuncPtrDIDGetDeviceState fpDIDGDS;

HRESULT WINAPI DICreateDevice(LPDIRECTINPUT lpdi, REFGUID rguid,LPDIRECTINPUTDEVICE *lplpdid, LPUNKNOWN pu)
{
	HRESULT ret; 
	DEBUG_OUT("DICreateDevice\n");
	ret = (*fpDICD)(lpdi, rguid, lplpdid, pu);
	if(lplpdid)
	{
		if(!fpDIDA) fpDIDA = (FuncPtrDIDAcquire)SetHook((void *)(*(DWORD *)(*lplpdid) + 7*4),DIDAcquire);
		if(!fpDIDGDS) fpDIDGDS = (FuncPtrDIDGetDeviceState)SetHook((void *)(*(DWORD *)(*lplpdid) + 9*4),DIDGetDeviceState);
	}
	return ret;
}

HRESULT WINAPI DIDAcquire(LPDIRECTINPUTDEVICE lpdid)
{
	if(!bActive) return DIERR_OTHERAPPHASPRIO;
	return (*fpDIDA)(lpdid);
}

HRESULT WINAPI DIDGetDeviceState(LPDIRECTINPUTDEVICE lpdid, DWORD cbdata, LPDIMOUSESTATE lpvdata)
{
	if(!bActive) return DIERR_INPUTLOST;
	return (*fpDIDGDS)(lpdid, cbdata, lpvdata);
}
