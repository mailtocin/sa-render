#pragma once
#include <Windows.h>
#include "RenderWare.h"

#define TMPLUGINID 0x30000000

enum eTextureType
{
	TEXTURE_DEFAULT = 1,
	TEXTURE_NORMAL_MAP,
	TEXTURE_SPECULAR_MAP,
	TEXTURE_NORMAL_SPECULAR_MAP,
	TEXTURE_DIRT
};

struct STexture : public RwTexture
{
	eTextureType m_dwTextureType;
	struct STexture *m_pAttachToTexture;
	union
	{
		struct STexture *m_pNormalMap;
		struct STexture *m_pNormSpecMap;
	};
	struct STexture *m_pSpecMap;
	struct STexture *m_pDirtTexture;
};

class CTextureMaps
{
	static int AttachTMPlugin();
	static RwTexDictionary *RwTexDictionaryGtaStreamRead(RwStream *stream);
	static STexture *GetTextureMapsCB(STexture *texture, RwTexDictionary *txd);
	static STexture *InitTM(STexture *texture);
	static STexture *DestroyTM(STexture *texture);
	static STexture *CopyTM(STexture *texture1, STexture *texture2);
public:
	static void Patch();
};