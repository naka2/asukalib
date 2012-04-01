#include "SoundBuffer.h"

extern IniData Ini;

// Lvup暫定処置ここから
#define us unsigned short
CHAR LvUpSoundSignature[SIG_SIZE] = {
	(us)0x04, (us)0x00, (us)0xff, (us)0xff, (us)0x1d, (us)0x00, (us)0x0b, (us)0x00,
	(us)0xf1, (us)0xff, (us)0xfa, (us)0xff, (us)0x13, (us)0x00, (us)0x02, (us)0x00,
	(us)0x14, (us)0x00, (us)0x05, (us)0x00, (us)0xec, (us)0xff, (us)0xf6, (us)0xff,
	(us)0x1a, (us)0x00, (us)0x05, (us)0x00, (us)0x07, (us)0x00, (us)0x00, (us)0x00,
	(us)0xef, (us)0xff, (us)0xf6, (us)0xff, (us)0x25, (us)0x00, (us)0x0e, (us)0x00,
	(us)0xff, (us)0xff, (us)0x02, (us)0x00, (us)0xf4, (us)0xff, (us)0xf4, (us)0xff,
	(us)0x1f, (us)0x00, (us)0x07, (us)0x00, (us)0xf1, (us)0xff, (us)0xf8, (us)0xff,
	(us)0xfc, (us)0xff, (us)0xf8, (us)0xff, (us)0x1a, (us)0x00, (us)0x0c, (us)0x00
};
LPDIRECTSOUNDBUFFER8 lpLvUpSound;
#undef us
// Lvup暫定処置ここまで


SoundBufferChain *SBCStart;

VOID ReleaseSoundBufferChain(VOID)
{
	SoundBufferChain *tmp = NULL, *tmp2 = NULL;
	if(!SBCStart) return;
	tmp = SBCStart;
	while(tmp->Next){
		tmp2 = tmp;
		tmp = tmp->Next;
		GlobalFree(tmp2);
	}
	GlobalFree(tmp);
}

VOID AddSoundBufferChain(LPDIRECTSOUNDBUFFER8 lpdssb8, WORD Type)
{
	SoundBufferChain *tmp = NULL, *tmp2 = NULL;

	LPVOID pBuff=NULL;
	char Sign[8]={0};

	if(!SBCStart){
		SBCStart = (SoundBufferChain *)GlobalAlloc(GPTR,sizeof(SoundBufferChain));
		SBCStart->Buffer = lpdssb8; SBCStart->Type = Type; SBCStart->Next = NULL;
		return;
	}

	tmp = SBCStart;
	while(1){
		if(tmp->Buffer == lpdssb8){
			tmp->Type = Type;
			return;
		}
		if(!tmp->Next)
			break;
		tmp = tmp->Next;
	}
	tmp2 = (SoundBufferChain *)GlobalAlloc(GPTR,sizeof(SoundBufferChain));
	tmp2->Buffer = lpdssb8; tmp2->Type = Type; tmp2->Next = NULL;
	tmp->Next = tmp2;
	return;
}


WORD TypeOf(LPDIRECTSOUNDBUFFER8 lpdssb8)
{
	SoundBufferChain *tmp = NULL, *tmp2 = NULL;

	if(!SBCStart) return TYPE_UNKNOWN;

	// Lvup暫定処置ここから
	if(lpLvUpSound && lpLvUpSound == lpdssb8)
		return TYPE_SE;
	// Lvup暫定処置ここまで

	tmp = SBCStart;
	while(tmp){
		if(tmp->Buffer == lpdssb8){
			return tmp->Type;
		}
		tmp = tmp->Next;
	}
	return TYPE_UNKNOWN;
}

VOID SetVolumeForeachSoundBufferChain(WORD Type, LONG Volume)
{
	//DEBUG_OUT("%s Type %d Volume %d\n",__FUNCTION__,Type,Volume);
	SoundBufferChain *tmp = NULL, *tmp2 = NULL;
	LONG Vol;
	if(!SBCStart) return;

	tmp = SBCStart;
	while(tmp){
		if(tmp->Buffer && (tmp->Type == Type || Type == TYPE_ALL)){
			if(Volume==VOLUME_ALL){
				if(tmp->Type == TYPE_BGM){
					Vol = Ini.BGMMute?DSBVOLUME_MIN:Ini.BGMVol;
				}else if(tmp->Type == TYPE_SE){
					Vol = Ini.SEMute?DSBVOLUME_MIN:Ini.SEVol;
				}else{
					Vol = 0;
				}
			}else if(Volume==VOLUME_BGM){
				Vol = Ini.BGMMute?DSBVOLUME_MIN:Ini.BGMVol;
			}else if(Volume==VOLUME_SE){
				Vol = Ini.SEMute?DSBVOLUME_MIN:Ini.SEVol;
			}else{
				Vol = Volume;
			}
			tmp->Buffer->SetVolume(Vol);
		}
		tmp = tmp->Next;
	}
	return;	
}

VOID DeleteSoundBufferChain(LPDIRECTSOUNDBUFFER8 lpdssb8)
{
	SoundBufferChain *tmp = NULL, *tmp2 = NULL;
//	DEBUG_OUT("%s\n",__FUNCTION__);
	if(!SBCStart) return;

//	DEBUG_OUT("%s SBCSTART-.Buffer = %X, lpdssb8 = %X\n",__FUNCTION__,SBCStart->Buffer,lpdssb8);
	if(SBCStart->Buffer == lpdssb8){
		tmp = SBCStart;
		SBCStart = SBCStart->Next;
		GlobalFree(tmp);
		return;
	}

	if(!SBCStart->Next) return;

	if(SBCStart->Next->Buffer == lpdssb8){
		tmp = SBCStart->Next;
		SBCStart->Next = tmp->Next;
		GlobalFree(tmp);
		return;
	}

//	DEBUG_OUT("%s 3\n",__FUNCTION__);
	tmp = SBCStart->Next;
	while(tmp->Next){
//		DEBUG_OUT("%s \n\ttmp = %X tmp->next = %X\n",__FUNCTION__,tmp,tmp->Next);
		if(tmp->Next->Buffer == lpdssb8){
			tmp2 = tmp->Next;
			tmp->Next = tmp2->Next;
			GlobalFree(tmp2);
			return;
		}
		tmp = tmp->Next;
	}
}
