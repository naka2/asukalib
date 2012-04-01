#include "HookDsound.h"

FuncPtrDirectSoundCreate8 fpDSCreate8;
FuncPtrDSCreateSoundBuffer fpDSCSB;

FuncPtrIURelease fpIUR;
FuncPtrDSSBPlay fpDSSBP;
FuncPtrDSSBSetVolume fpDSSBSV;
FuncPtrDSSBUnlock fpDSSBU;

extern IniData Ini;

HRESULT WINAPI DSCreateSoundBuffer(LPDIRECTSOUND8 lpds8, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER8 *ppDSBuffer, LPUNKNOWN pUnkOuter)
{
	HRESULT ret = NULL;

	DSBUFFERDESC newDesc;
	CopyMemory(&newDesc,pcDSBufferDesc,sizeof(DSBUFFERDESC));
	if(!(newDesc.dwFlags&DSBCAPS_PRIMARYBUFFER)) newDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	DWORD f=newDesc.dwFlags;

	DEBUG_OUT("dwFlags = %X(%s%s%s%s%s%s%s%s%s%s)\n",f,f&DSBCAPS_CTRL3D?"DSBCAPS_CTRL3D ":"",f&DSBCAPS_CTRLFREQUENCY?"DSBCAPS_CTRLFREQUENCY ":""
		,f&DSBCAPS_CTRLFX?"DSBCAPS_CTRLFX ":"",f&DSBCAPS_CTRLPAN?"DSBCAPS_CTRLPAN ":"",f&DSBCAPS_CTRLPOSITIONNOTIFY?"DSBCAPS_CTRLPOSITIONNOTIFY ":""
		,f&DSBCAPS_CTRLVOLUME?"DSBCAPS_CTRLVOLUME ":"",f&DSBCAPS_GETCURRENTPOSITION2?"DSBCAPS_GETCURRENTPOSITION2 ":"",f&DSBCAPS_GLOBALFOCUS?"DSBCAPS_GLOBALFOCUS ":""
		,f&DSBCAPS_PRIMARYBUFFER?"DSBCAPS_PRIMARYBUFFER ":"",f&DSBCAPS_STICKYFOCUS?"DSBCAPS_STICKYFOCUS ":"");
	ret = (*fpDSCSB)(lpds8, &newDesc, ppDSBuffer, pUnkOuter);

	if(ppDSBuffer)
	{
				if(!fpIUR) fpIUR = (FuncPtrIURelease)SetHook((void*)(*(DWORD *)(*ppDSBuffer) + 2*4),IURelease);
				if(!fpDSSBP) fpDSSBP = (FuncPtrDSSBPlay)SetHook((void *)(*(DWORD *)*ppDSBuffer + 12*4),DSSBPlay);
				if(!fpDSSBSV) fpDSSBSV = (FuncPtrDSSBSetVolume)SetHook((void *)(*(DWORD *)*ppDSBuffer + 15*4),DSSBSetVolume);
				if(!fpDSSBU) fpDSSBU = (FuncPtrDSSBUnlock)SetHook((void *)(*(DWORD *)*ppDSBuffer + 19*4),DSSBUnlock);
				DEBUG_OUT("return DirectSoundBuffer8 Interface %X\n",*ppDSBuffer);
	}
	return ret;
}

ULONG WINAPI IURelease(IUnknown *IU)
{
	DEBUG_OUT("Interface Released from %X\n",IU);
	DeleteSoundBufferChain((LPDIRECTSOUNDBUFFER8)IU);
	return (*fpIUR)(IU);
}

HRESULT WINAPI DSSBPlay(LPDIRECTSOUNDBUFFER8 lpdssb8, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)
{
	DEBUG_OUT("%s %X\n",__FUNCTION__,lpdssb8);
	WORD Type = dwFlags&DSBPLAY_LOOPING?TYPE_BGM:TYPE_SE;
	AddSoundBufferChain(lpdssb8,Type);
	if(Type == TYPE_BGM){
		if(Ini.BGMMute)
			lpdssb8->SetVolume(DSBVOLUME_MIN);
		else
			lpdssb8->SetVolume(Ini.BGMVol);
	}else if(Type == TYPE_SE){
		if(Ini.SEMute)
			lpdssb8->SetVolume(DSBVOLUME_MIN);
		else
			lpdssb8->SetVolume(Ini.SEVol);
	}else{
		lpdssb8->SetVolume(0);
	}
	return (*fpDSSBP)(lpdssb8, dwReserved1, dwPriority, dwFlags);
}


HRESULT WINAPI DSSBSetVolume(LPDIRECTSOUNDBUFFER8 lpdssb8, LONG lVolume)
{
	WORD Type = TypeOf(lpdssb8);
	DEBUG_OUT("%s %X\n",__FUNCTION__,lpdssb8);
	DEBUG_OUT("\tVolume : %d\n",lVolume);

	if(Type == TYPE_BGM)
		lVolume = Ini.BGMMute?DSBVOLUME_MIN:Ini.BGMVol;
	else if(Type == TYPE_SE)
		lVolume = Ini.SEMute?DSBVOLUME_MIN:Ini.SEVol;

	if(!bActive)
		lVolume = DSBVOLUME_MIN;

	return (*fpDSSBSV)(lpdssb8, lVolume);
}

HRESULT WINAPI DSSBUnlock(LPDIRECTSOUNDBUFFER8 lpdssb8, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2)
{

	FILE *fp=NULL;
	char filename[256]={0};
	unsigned int i,flag=0;
#if 0
	LPVOID lpVoid = NULL;
	char RIFF[] = {'R','I','F','F'},WAVE[] = {'W','A','V','E'},fmt[] = {'f','m','t',' '},data[] = {'d','a','t','a'};
	DWORD DATA_size,FMT_size,RIFF_size;

	sprintf(filename,"dump/dumpbin_%X_%010d.wav",lpdssb8,timeGetTime());
	if(!(fp = fopen(filename,"wb"))){
		DEBUG_OUT("Fail(file_open_err)\n");
	}else{
		lpdssb8->GetFormat(NULL,0,&FMT_size);
		lpVoid = (LPVOID)GlobalAlloc(GPTR,FMT_size);
		lpdssb8->GetFormat((LPWAVEFORMATEX)lpVoid,FMT_size,NULL);
		RIFF_size = dwAudioBytes1 + dwAudioBytes2 + FMT_size + 20;
		DATA_size = dwAudioBytes1 + dwAudioBytes2;
		fwrite(RIFF,4,1,fp);
		fwrite(&RIFF_size,4,1,fp);
		fwrite(WAVE,4,1,fp);
		fwrite(fmt,4,1,fp);
		fwrite(&FMT_size,4,1,fp);
		fwrite(lpVoid,FMT_size,1,fp);
		fwrite(data,4,1,fp);
		fwrite(&DATA_size,4,1,fp);
		fwrite(pvAudioPtr2,dwAudioBytes2,1,fp);
		fwrite(pvAudioPtr1,dwAudioBytes1,1,fp);
		fclose(fp);fp=NULL;GlobalFree(lpVoid);
	}
#endif

	if(lpLvUpSound && lpLvUpSound == lpdssb8)
	{
		for(i=0;i<dwAudioBytes1;i++)
		{
			if(((unsigned short *)pvAudioPtr1)[i])
			{
				flag = 1;
			}
			else
			{
				break;
			}
		}
		for(i=0;i<dwAudioBytes2;i++)
		{
			if(((unsigned short *)pvAudioPtr2)[i])
			{
				flag = 1;
			}
			else
			{
				break;
			}
		}

		if(flag)
		{
			AddSoundBufferChain(lpdssb8,TYPE_BGM);
			lpLvUpSound = NULL;
		}
	}

	if(pvAudioPtr1 && !memcmp(pvAudioPtr1,LvUpSoundSignature,SIG_SIZE))
	{
		DEBUG_OUT("LvupSound Buffer Detected %X\n",lpdssb8);
		lpLvUpSound = lpdssb8;
	}
	else if(pvAudioPtr2 && !memcmp(pvAudioPtr2,LvUpSoundSignature,SIG_SIZE))
	{
		DEBUG_OUT("LvupSound Buffer Detected %X\n",lpdssb8);
		lpLvUpSound = lpdssb8;
	}
	
	return (*fpDSSBU)(lpdssb8, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
}
