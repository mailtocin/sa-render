#pragma once
#include "CTextureMaps.h"
#include "RenderWare.h"
#include "CPatch.h"

const char *NormalMapIdentify[] = {"_norm", "_nrm", "_n"};
const char *SpecMapIdentify[] = {"_spec", "_s"};
const char *NormSpecMapIdentify[] = {"_nors", "_ns"};
const char *DirtMapIdentify[] = {"_dirt", "_d"};

void CTextureMaps::Patch()
{
	CPatch::RedirectCall(0x53D8DF, AttachTMPlugin);
	CPatch::RedirectCall(0x5B388D, RwTexDictionaryGtaStreamRead);
	CPatch::RedirectCall(0x731E09, RwTexDictionaryGtaStreamRead);
}

int CTextureMaps::AttachTMPlugin()
{
	signed int result = RpAnisotPluginAttach();
	RwTextureRegisterPlugin(20, TMPLUGINID, InitTM, DestroyTM, CopyTM);
	return result;
}

RwTexDictionary *CTextureMaps::RwTexDictionaryGtaStreamRead(RwStream *stream)
{
	// variables
	int length;
	RwTexDictionary *txd;
	STexture *texture;
	__int16 buffer[2];
	int lengthOut;
	int versionOut;
	// code
	if (!RwStreamFindChunk(stream, 1, &lengthOut, &versionOut)
		|| (length = RwStreamRead(stream, buffer, lengthOut), length != lengthOut))
		return 0;
	txd = RwTexDictionaryCreate();
	if(txd)
	{
		if(!buffer[0])
			return txd;
		while(1)
		{
			--buffer[0];
			texture = (STexture *)readTextureNative(stream);
			if (!texture)
				break;
			RwTexDictionaryAddTexture(txd, texture);
			if(!buffer[0])
			{
				RwTexDictionaryForAllTextures(txd, GetTextureMapsCB, txd);
				return txd;
			}
		}
		RwTexDictionaryForAllTextures(txd, (void *)0x730E50, 0);
		RwTexDictionaryDestroy(txd);
	}
	return 0;
}

STexture *CTextureMaps::GetTextureMapsCB(STexture *texture, RwTexDictionary *txd)
{
	// variables
	STexture *mainTex;
	char name[32];
	int i;
	// code
	for(i = 0; i < sizeof(NormalMapIdentify)/4; i++)
	{
		if(!_strnicmp(texture->name + strlen(texture->name) - strlen(NormalMapIdentify[i]), NormalMapIdentify[i], 
			strlen(NormalMapIdentify[i])))
		{
			strncpy_s(name, texture->name, strlen(texture->name) - strlen(NormalMapIdentify[i]));
			mainTex = (STexture *)RwTexDictionaryFindNamedTexture(txd, name);
			if(mainTex)
			{
				mainTex->m_pNormalMap = texture;
				texture->m_pAttachToTexture = mainTex;
				texture->m_dwTextureType = TEXTURE_NORMAL_MAP;
			}
			return texture;
		}
	}
	for(i = 0; i < sizeof(SpecMapIdentify)/4; i++)
	{
		if(!_strnicmp(texture->name + strlen(texture->name) - strlen(SpecMapIdentify[i]), SpecMapIdentify[i], 
			strlen(SpecMapIdentify[i])))
		{
			strncpy_s(name, texture->name, strlen(texture->name) - strlen(SpecMapIdentify[i]));
			mainTex = (STexture *)RwTexDictionaryFindNamedTexture(txd, name);
			if(mainTex)
			{
				mainTex->m_pSpecMap = texture;
				texture->m_pAttachToTexture = mainTex;
				texture->m_dwTextureType = TEXTURE_SPECULAR_MAP;
			}
			return texture;
		}
	}
	for(i = 0; i < sizeof(NormSpecMapIdentify)/4; i++)
	{
		if(!_strnicmp(texture->name + strlen(texture->name) - strlen(NormSpecMapIdentify[i]), NormSpecMapIdentify[i], 
			strlen(NormSpecMapIdentify[i])))
		{
			strncpy_s(name, texture->name, strlen(texture->name) - strlen(NormSpecMapIdentify[i]));
			mainTex = (STexture *)RwTexDictionaryFindNamedTexture(txd, name);
			if(mainTex)
			{
				mainTex->m_pNormSpecMap = texture;
				texture->m_pAttachToTexture = mainTex;
				texture->m_dwTextureType = TEXTURE_NORMAL_SPECULAR_MAP;
			}
			return texture;
		}
	}
	for(i = 0; i < sizeof(DirtMapIdentify)/4; i++)
	{
		if(!_strnicmp(texture->name + strlen(texture->name) - strlen(DirtMapIdentify[i]), DirtMapIdentify[i], 
			strlen(DirtMapIdentify[i])))
		{
			strncpy_s(name, texture->name, strlen(texture->name) - strlen(DirtMapIdentify[i]));
			mainTex = (STexture *)RwTexDictionaryFindNamedTexture(txd, name);
			if(mainTex)
			{
				mainTex->m_pNormSpecMap = texture;
				texture->m_pAttachToTexture = mainTex;
				texture->m_dwTextureType = TEXTURE_DIRT;
			}
			return texture;
		}
	}
	return texture;
}

STexture *CTextureMaps::InitTM(STexture *texture)
{
	texture->m_dwTextureType = TEXTURE_DEFAULT;
	texture->m_pAttachToTexture = NULL;
	texture->m_pNormalMap = NULL;
	texture->m_pSpecMap = NULL;
	texture->m_pDirtTexture = NULL;
	return texture;
}

STexture *CTextureMaps::DestroyTM(STexture *texture)
{
	return texture;
}

STexture *CTextureMaps::CopyTM(STexture *texture1, STexture *texture2)
{
	texture1->m_dwTextureType = texture2->m_dwTextureType;
	texture1->m_pAttachToTexture = texture2->m_pAttachToTexture;
	texture1->m_pNormalMap = texture2->m_pNormalMap;
	texture1->m_pSpecMap = texture2->m_pSpecMap;
	texture1->m_pDirtTexture = texture2->m_pDirtTexture;
	return texture1;
}