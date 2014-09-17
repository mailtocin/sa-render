/********************************SA::Render source file*************************************/
/* File name   : CEffect.cpp                                                               */
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac, PetkaGtA                                                         */
/* File descrip: CEffect class which controls pixel and vertex shaders.                    */
/* File created: 01.12.2013                                                                */
/* File last ed: 03.12.2013                                                                */
/*******************************************************************************************/
#include "..\Headers\CEffect.h"
#include "..\Headers\CEffectMgr.h"
#include "game_sa\RenderWare.h"
#include <iostream>
static char m_dirName[512];
static char m_tempName[512];
char *MakePath(char *subPath)
{
	strcpy_s(m_tempName, m_dirName);
	strcat_s(m_tempName, subPath);
	return m_tempName;
}

#define MAKE_PATH(subPath) MakePath(subPath)

CEffect::CEffect()
{
	m_pEffect = NULL;
}

ID3DXEffect* CEffect::GetD3DEffect(){
	return m_pEffect;
}

CEffect::CEffect(eEffect type, D3DXMACRO *defines)
{
	ID3DXBuffer *errorBuffer = 0;
	char *effectFilePath;
	switch (type)
	{
	case D3D_EFFECT_VEHICLE:
		effectFilePath = "Render\\Shaders\\ObjectRendering\\VehicleDrawable.fx";
		break;
	case D3D_EFFECT_BUILDING:
		effectFilePath = "Render\\Shaders\\ObjectRendering\\BuildingDrawable.fx";
		break;
	case D3D_EFFECT_OBJECT:
		effectFilePath = "Render\\Shaders\\ObjectRendering\\ObjectDrawable.fx";
		break;
	case D3D_EFFECT_SHADOWMAPPING:
		effectFilePath = "Render\\Shaders\\Other\\ShadowMapping.fx";
		break;
	case D3D_EFFECT_DEBUG:
		effectFilePath = "Render\\Shaders\\Other\\Debug.fx";
		break;
	case D3D_EFFECT_DEFERRED:
		effectFilePath = "Render\\Shaders\\Other\\Deferred.fx";
		break;
	default:
		MessageBox(0, "Unknown effect type", "CEffect::CEffect", 0);
		m_pEffect = NULL;
		return;
	}
	HRESULT hr = D3DXCreateEffectFromFile(GetDevice(), MAKE_PATH(effectFilePath), defines, 0, D3DXSHADER_OPTIMIZATION_LEVEL3, gEffectMgr.m_pEffectPool, &m_pEffect, &errorBuffer);
	if (errorBuffer)
	{
		MessageBox(0, (char*)errorBuffer->GetBufferPointer(), "CEffect::CreateFromFile", 0);
		errorBuffer->Release();
	}
	if (FAILED(hr))
	{
		MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", "CEffect::CreateFromFile", 0);
		m_pEffect = NULL;
		return;
	}
	gEffectMgr.AddEffect(this);
	m_type = type;
	memset(&m_techniques, 0, sizeof(m_techniques));
	memset(&m_params, 0, sizeof(m_params));
	memset(&m_globals, 0, sizeof(m_globals));

	m_globals.globals.WorldViewMatrix = GetParameterByName("mWorldViewProj");
	m_globals.globals.WorldMatrix = GetParameterByName("mWorld");
	m_globals.globals.ViewMatrix = GetParameterByName("gmView");
	m_globals.globals.LightViewProjMatrix = GetParameterByName("mLightViewProj");
	m_globals.globals.Ambient0 = GetParameterByName("g_cAmbient0");
	m_globals.globals.Ambient1 = GetParameterByName("g_cAmbient1");
	m_globals.globals.AmbientMultiplier = GetParameterByName("g_fAmbientMultiplier");
	switch (type)
	{
	case D3D_EFFECT_VEHICLE:
		m_techniques.vehicle.ForwardPaint = GetTechniqueByName("ForwardPaint");
		m_techniques.vehicle.ForwardTextured = GetTechniqueByName("ForwardTextured");
		m_techniques.vehicle.ForwardBurnout = GetTechniqueByName("ForwardBurnout");
		m_techniques.vehicle.ForwardBurnoutTex = GetTechniqueByName("ForwardBurnoutTex");
		m_techniques.vehicle.DeferredPaint = GetTechniqueByName("DeferredPaint");
		m_techniques.vehicle.DeferredTextured = GetTechniqueByName("DeferredTextured");
		m_techniques.vehicle.DeferredBurnout = GetTechniqueByName("DeferredBurnout");
		m_techniques.vehicle.DeferredBurnoutTex = GetTechniqueByName("DeferredBurnoutTex");
		m_params.vehicle.PaintColor = GetParameterByName("vPaintColor");
		m_params.vehicle.Specularity = GetParameterByName("fSpecularFactor");
		m_params.vehicle.Reflectivity = GetParameterByName("fReflectionFactor");
		m_params.vehicle.ColorTexture = 0;
		m_params.vehicle.BurnoutTexture = 3;
		m_params.vehicle.RaindropTexture = 4;
		m_params.vehicle.LightPosition = GetParameterByName("g_vLightPos");
		m_params.vehicle.ViewPosition = GetParameterByName("g_vViewPos");
		m_params.vehicle.Ambient0 = GetParameterByName("g_cAmbient0");
		m_params.vehicle.Ambient1 = GetParameterByName("g_cAmbient1");
		m_params.vehicle.AmbientMultiplier = GetParameterByName("g_fAmbientMultiplier");
		break;
	case D3D_EFFECT_BUILDING:
		m_techniques.building.ForwardColor = GetTechniqueByName("ForwardColor");
		m_techniques.building.ForwardTextured = GetTechniqueByName("ForwardTextured");
		m_techniques.building.DeferredColor = GetTechniqueByName("DeferredColor");
		m_techniques.building.DeferredTextured = GetTechniqueByName("DeferredTextured");
		m_techniques.building.ReflectionTextured = GetTechniqueByName("ReflectionTextured");
		m_params.building.Color = GetParameterByName("vColor");
		m_params.building.Reflectivity = GetParameterByName("fReflectionFactor");
		m_params.building.Emissivity = GetParameterByName("fEmmisionFactor");
		m_params.building.ColorTexture = 0;
		m_params.building.LightPosition = GetParameterByName("g_vLightPos");
		m_params.building.ViewPosition = GetParameterByName("g_vViewPos");
		m_params.building.Ambient0 = GetParameterByName("g_cAmbient0");
		m_params.building.Ambient1 = GetParameterByName("g_cAmbient1");
		m_params.building.AmbientMultiplier = GetParameterByName("g_fAmbientMultiplier");
		m_params.building.ReflectDir = GetParameterByName("fReflectDir");
		break;
	case D3D_EFFECT_OBJECT:
		m_techniques.object.ForwardColor = GetTechniqueByName("ForwardColor");
		m_techniques.object.ForwardTextured = GetTechniqueByName("ForwardTextured");
		m_techniques.object.DeferredColor = GetTechniqueByName("DeferredColor");
		m_techniques.object.DeferredTextured = GetTechniqueByName("DeferredTextured");
		m_techniques.object.ReflectionTextured = GetTechniqueByName("ReflectionTextured");
		m_params.object.Color = GetParameterByName("vColor");
		m_params.object.Reflectivity = GetParameterByName("fReflectionFactor");
		m_params.object.Emissivity = GetParameterByName("fEmmisionFactor");
		m_params.object.ColorTexture = 0;
		m_params.object.LightPosition = GetParameterByName("g_vLightPos");
		m_params.object.ViewPosition = GetParameterByName("g_vViewPos");
		m_params.object.Ambient0 = GetParameterByName("g_cAmbient0");
		m_params.object.Ambient1 = GetParameterByName("g_cAmbient1");
		m_params.object.AmbientMultiplier = GetParameterByName("g_fAmbientMultiplier");
		m_params.object.ReflectDir = GetParameterByName("fReflectDir");
		break;
	case D3D_EFFECT_SHADOWMAPPING:
		m_techniques.shadows.DirectLightShadows = GetTechniqueByName("DirectLightShadows");
		//m_techniques.object.ReflectionTextured = GetTechniqueByName("ReflectionTextured");
		m_params.shadows.Alpha = GetParameterByName("fAlpha");
		m_params.shadows.ColorTexture = 0;
		break;
	case D3D_EFFECT_DEBUG:
		break;
	case D3D_EFFECT_DEFERRED:
		break;
	default:
		MessageBox(0, "Unknown effect type", "CEffect::CEffect", 0);
		m_pEffect = NULL;
		return;
	}
}

CEffect::~CEffect()
{
	if (m_pEffect)
		m_pEffect->Release();
	gEffectMgr.RemoveEffect(this);
}

void CEffect::OnLost()
{
	if (m_pEffect)
		m_pEffect->OnLostDevice();
}

void CEffect::OnReset()
{
	if (m_pEffect)
		m_pEffect->OnResetDevice();
}
HRESULT CEffect::Reload(D3DXMACRO *defines)
{
	ID3DXEffect* effect;
	ID3DXBuffer *errorBuffer = 0;
	char *effectFilePath;
	switch (m_type)
	{
	case D3D_EFFECT_VEHICLE:
		effectFilePath = "Render\\Shaders\\ObjectRendering\\VehicleDrawable.fx";
		break;
	case D3D_EFFECT_BUILDING:
		effectFilePath = "Render\\Shaders\\ObjectRendering\\BuildingDrawable.fx";
		break;
	default:
		MessageBox(0, "Unknown effect type", "CEffect::CEffect", 0);
		m_pEffect = NULL;
		return false;
	}
	HRESULT hr = D3DXCreateEffectFromFile(GetDevice(), MAKE_PATH(effectFilePath), defines, 0, D3DXSHADER_OPTIMIZATION_LEVEL3, gEffectMgr.m_pEffectPool, &effect, &errorBuffer);
	if (errorBuffer)
	{
		MessageBox(0, (char*)errorBuffer->GetBufferPointer(), "CEffect::CreateFromFile", 0);
		errorBuffer->Release();
	}
	if (FAILED(hr))
	{
		MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", "CEffect::CreateFromFile", 0);
		effect = NULL;
		return false;
	}
	if (effect){
		if (m_pEffect)
			m_pEffect->Release();
		effect->CloneEffect(GetDevice(), &m_pEffect);
		return true;
	}
	return true;
}

IDirect3DDevice9 *CEffect::GetDevice()
{
	return *(IDirect3DDevice9 **)0xC97C28;
}

void CEffect::SetTechnique(D3DXHANDLE technique)
{
	m_pEffect->SetTechnique(technique);
}

void CEffect::Begin()
{
	UINT numPasses;
	if (m_pEffect)
	{
		m_pEffect->Begin(&numPasses, 0);
		m_pEffect->BeginPass(0);
	}
}

void CEffect::End()
{
	if (m_pEffect)
	{
		m_pEffect->EndPass();
		m_pEffect->End();
	}
}

void CEffect::SetFloat(D3DXHANDLE param, float value)
{
	m_pEffect->SetFloat(param, value);
}

void CEffect::SetFloatArray(D3DXHANDLE param, float *values, unsigned int count)
{
	m_pEffect->SetFloatArray(param, values, count);
}

void CEffect::SetInt(D3DXHANDLE param, int value)
{
	m_pEffect->SetInt(param, value);
}

void CEffect::SetIntArray(D3DXHANDLE param, int *values, unsigned int count)
{
	m_pEffect->SetIntArray(param, values, count);
}

void CEffect::SetBool(D3DXHANDLE param, bool value)
{
	m_pEffect->SetBool(param, value);
}

void CEffect::SetBoolArray(D3DXHANDLE param, bool *values, unsigned int count)
{
	BOOL *boolarray = new BOOL[count];
	for (unsigned int i = 0; i < count; i++)
		boolarray[i] = values[i];
	m_pEffect->SetBoolArray(param, boolarray, count);
	delete boolarray;
}

void CEffect::SetVector(D3DXHANDLE param, RwV2d *vector)
{
	m_pEffect->SetFloatArray(param, (float *)vector, 2);
}

void CEffect::SetVector(D3DXHANDLE param, RwV3d *vector)
{
	m_pEffect->SetFloatArray(param, (float *)vector, 3);
}

void CEffect::SetVector(D3DXHANDLE param, D3DXVECTOR2 *vector)
{
	m_pEffect->SetFloatArray(param, (float *)vector, 2);
}

void CEffect::SetVector(D3DXHANDLE param, D3DXVECTOR3 *vector)
{
	m_pEffect->SetFloatArray(param, (float *)vector, 3);
}

void CEffect::SetVector(D3DXHANDLE param, D3DXVECTOR4 *vector)
{
	m_pEffect->SetVector(param, vector);
}

void CEffect::SetVector(D3DXHANDLE param, CVector2D &vector)
{
	m_pEffect->SetFloatArray(param, (float *)&vector, 2);
}

void CEffect::SetVector(D3DXHANDLE param, CVector &vector)
{
	m_pEffect->SetFloatArray(param, (float *)&vector, 3);
}

void CEffect::SetVector(D3DXHANDLE param, D3DXVECTOR2 &vector)
{
	m_pEffect->SetFloatArray(param, (float *)&vector, 2);
}

void CEffect::SetVector(D3DXHANDLE param, D3DXVECTOR3 &vector)
{
	m_pEffect->SetFloatArray(param, (float *)&vector, 3);
}

void CEffect::SetVector(D3DXHANDLE param, D3DXVECTOR4 &vector)
{
	m_pEffect->SetVector(param, &vector);
}

void CEffect::SetVectorArray(D3DXHANDLE param, D3DXVECTOR4 *vectors, unsigned int count)
{
	m_pEffect->SetVectorArray(param, vectors, count);
}

void CEffect::SetColor(D3DXHANDLE param, RwRGBA *color)
{
	D3DXVECTOR4 vec(color->red / 255.0f, color->green / 255.0f, color->blue / 255.0f, color->alpha / 255.0f);
	m_pEffect->SetVector(param, &vec);
}

void CEffect::SetColor(D3DXHANDLE param, RwRGBAReal *color)
{
	m_pEffect->SetVector(param, (D3DXVECTOR4 *)color);
}

void CEffect::SetColor(D3DXHANDLE param, D3DXCOLOR *color)
{
	m_pEffect->SetVector(param, (D3DXVECTOR4 *)color);
}

void CEffect::SetColor(D3DXHANDLE param, CRGBA &color)
{
	D3DXVECTOR4 vec(color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f);
	m_pEffect->SetVector(param, &vec);
}

void CEffect::SetColor(D3DXHANDLE param, D3DXCOLOR &color)
{
	m_pEffect->SetVector(param, (D3DXVECTOR4 *)&color);
}

void CEffect::SetMatrix(D3DXHANDLE param, RwMatrix *matrix)
{
	RwMatrix dummy;
	memcpy(&dummy, matrix, sizeof(RwMatrix));
	dummy.flags = 0;
	dummy.pad1 = 0;
	dummy.pad2 = 0;
	*(float *)&dummy.pad3 = 1.0;
	m_pEffect->SetMatrix(param, (D3DXMATRIX *)matrix);
}

void CEffect::SetMatrix(D3DXHANDLE param, D3DXMATRIX *matrix)
{
	m_pEffect->SetMatrix(param, matrix);
}

void CEffect::SetMatrixArray(D3DXHANDLE param, RwMatrix *matrices, unsigned int count)
{
	RwMatrix *array = new RwMatrix[count];
	for (unsigned int i = 0; i < count; i++)
	{
		memcpy(&array[i], &matrices[i], sizeof(RwMatrix));
		array[i].flags = 0;
		array[i].pad1 = 0;
		array[i].pad2 = 0;
		*(float *)&array[i].pad3 = 1.0;
	}
	m_pEffect->SetMatrixArray(param, (D3DXMATRIX *)array, count);
	delete array;
}

void CEffect::SetMatrixArray(D3DXHANDLE param, D3DXMATRIX *matrices, unsigned int count)
{
	m_pEffect->SetMatrixArray(param, matrices, count);
}

void CEffect::SetTexture(TEXTURESTAGE param, RwTexture *texture)
{
	rwD3D9SetTexture(texture, param);
}

void CEffect::SetTexture(D3DXHANDLE param, LPDIRECT3DBASETEXTURE9 texture)
{
	m_pEffect->SetTexture(param, texture);
}

D3DXHANDLE CEffect::GetParameterByName(char *name)
{
	return m_pEffect->GetParameterByName(0, name);
}

D3DXHANDLE CEffect::GetParameterBySemantic(char *semantic)
{
	return m_pEffect->GetParameterBySemantic(0, semantic);
}

D3DXHANDLE CEffect::GetTechniqueByName(char *name)
{
	return m_pEffect->GetTechniqueByName(name);
}