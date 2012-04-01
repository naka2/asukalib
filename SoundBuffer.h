#pragma once

#include "AsukaLib.h"

#define SIG_SIZE 64

typedef struct _SoundBufferChain
{
	LPDIRECTSOUNDBUFFER8	Buffer;
	WORD					Type;
	_SoundBufferChain		*Next;
} SoundBufferChain;

enum SoundBufferType
{
	TYPE_UNKNOWN,
	TYPE_BGM,
	TYPE_SE,
	TYPE_ALL
};

enum VolumeType
{
	VOLUME_BGM = 1,
	VOLUME_SE,
	VOLUME_ALL
};

VOID AddSoundBufferChain(LPDIRECTSOUNDBUFFER8 lpdssb8, WORD Type);
VOID DeleteSoundBufferChain(LPDIRECTSOUNDBUFFER8 lpdssb8);
VOID ReleaseSoundBufferChain(VOID);
WORD TypeOf(LPDIRECTSOUNDBUFFER8 lpdssb8);
VOID SetVolumeForeachSoundBufferChain(WORD Type, LONG Volume);
