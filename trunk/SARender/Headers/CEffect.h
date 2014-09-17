/********************************SA::Render source file*************************************/
/* File name   : CEffect.h                                                                 */
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac, PetkaGtA                                                         */
/* File descrip: CEffect class which controls pixel and vertex shaders.                    */
/* File created: 01.12.2013                                                                */
/* File last ed: 03.12.2013                                                                */
/*******************************************************************************************/
#pragma once
#include <d3dx9.h>
#include "game_sa\RenderWare.h"
#include "game_sa\CVector.h"
#include "game_sa\CVector2D.h"
#include "game_sa\CRGBA.h"

enum eEffect
{
	D3D_EFFECT_VEHICLE,
	D3D_EFFECT_BUILDING,
	D3D_EFFECT_OBJECT,
	D3D_EFFECT_SHADOWMAPPING,
	D3D_EFFECT_DEBUG,
	D3D_EFFECT_DEFERRED
};

typedef unsigned int TEXTURESTAGE;

class CEffect
{
	static IDirect3DDevice9 *GetDevice();
	ID3DXEffect *m_pEffect;
	eEffect m_type;

public:
	CEffect();
	CEffect(eEffect type, D3DXMACRO *defines = NULL);
	~CEffect();

	void OnLost();
	void OnReset();
	void Begin();
	void End();
	HRESULT Reload(D3DXMACRO *defines = NULL);

	ID3DXEffect* GetD3DEffect();
	void SetFloat(D3DXHANDLE param, float value);
	void SetFloatArray(D3DXHANDLE param, float *values, unsigned int count);
	void SetInt(D3DXHANDLE param, int value);
	void SetIntArray(D3DXHANDLE param, int *values, unsigned int count);
	void SetBool(D3DXHANDLE param, bool value);
	void SetBoolArray(D3DXHANDLE param, bool *values, unsigned int count);
	void SetVector(D3DXHANDLE param, RwV2d *vector);
	void SetVector(D3DXHANDLE param, RwV3d *vector);
	void SetVector(D3DXHANDLE param, D3DXVECTOR2 *vector);
	void SetVector(D3DXHANDLE param, D3DXVECTOR3 *vector);
	void SetVector(D3DXHANDLE param, D3DXVECTOR4 *vector);
	void SetVector(D3DXHANDLE param, CVector2D &vector);
	void SetVector(D3DXHANDLE param, CVector &vector);
	void SetVector(D3DXHANDLE param, D3DXVECTOR2 &vector);
	void SetVector(D3DXHANDLE param, D3DXVECTOR3 &vector);
	void SetVector(D3DXHANDLE param, D3DXVECTOR4 &vector);
	void SetVectorArray(D3DXHANDLE param, D3DXVECTOR4 *vectors, unsigned int count);
	void SetColor(D3DXHANDLE param, RwRGBA *color);
	void SetColor(D3DXHANDLE param, RwRGBAReal *color);
	void SetColor(D3DXHANDLE param, D3DXCOLOR *color);
	void SetColor(D3DXHANDLE param, CRGBA &color);
	void SetColor(D3DXHANDLE param, D3DXCOLOR &color);
	void SetMatrix(D3DXHANDLE param, RwMatrix *matrix);
	void SetMatrix(D3DXHANDLE param, D3DXMATRIX *matrix);
	void SetMatrixArray(D3DXHANDLE param, RwMatrix *matrices, unsigned int count);
	void SetMatrixArray(D3DXHANDLE param, D3DXMATRIX *matrices, unsigned int count);
	void SetTexture(TEXTURESTAGE param, RwTexture *texture);
	void SetTexture(D3DXHANDLE param, LPDIRECT3DBASETEXTURE9 texture);
	D3DXHANDLE GetParameterByName(char *name);
	D3DXHANDLE GetParameterBySemantic(char *semantic);
	D3DXHANDLE GetTechniqueByName(char *name);
	void SetTechnique(D3DXHANDLE technique);

	union{
		struct{
			D3DXHANDLE PaintColor;
			D3DXHANDLE Specularity;
			D3DXHANDLE Reflectivity;
			TEXTURESTAGE ColorTexture;
			TEXTURESTAGE BurnoutTexture;
			TEXTURESTAGE RaindropTexture;
			D3DXHANDLE LightPosition;
			D3DXHANDLE ViewPosition;
			D3DXHANDLE Ambient0;
			D3DXHANDLE Ambient1;
			D3DXHANDLE AmbientMultiplier;
		} vehicle;
		struct{
			D3DXHANDLE Color;
			D3DXHANDLE Reflectivity;
			D3DXHANDLE Emissivity;
			TEXTURESTAGE ColorTexture;
			D3DXHANDLE LightPosition;
			D3DXHANDLE ViewPosition;
			D3DXHANDLE Ambient0;
			D3DXHANDLE Ambient1;
			D3DXHANDLE AmbientMultiplier;
			D3DXHANDLE ReflectDir;
		} building;
		struct{
			D3DXHANDLE Color;
			D3DXHANDLE Reflectivity;
			D3DXHANDLE Emissivity;
			TEXTURESTAGE ColorTexture;
			D3DXHANDLE LightPosition;
			D3DXHANDLE ViewPosition;
			D3DXHANDLE Ambient0;
			D3DXHANDLE Ambient1;
			D3DXHANDLE AmbientMultiplier;
			D3DXHANDLE ReflectDir;
		} object;
		struct{
			D3DXHANDLE Alpha;
			TEXTURESTAGE ColorTexture;
		} shadows;
	} m_params;

	union
	{
		struct{
			D3DXHANDLE LightViewProjMatrix;
			D3DXHANDLE WorldViewMatrix;
			D3DXHANDLE WorldMatrix;
			D3DXHANDLE ViewMatrix;
			D3DXHANDLE LightPosition;
			D3DXHANDLE ViewPosition;
			D3DXHANDLE Ambient0;
			D3DXHANDLE Ambient1;
			D3DXHANDLE AmbientMultiplier;
		} globals;
	} m_globals;

	union{
		struct{
			D3DXHANDLE ForwardPaint;
			D3DXHANDLE ForwardTextured;
			D3DXHANDLE ForwardBurnout;
			D3DXHANDLE ForwardBurnoutTex;
			D3DXHANDLE DeferredPaint;
			D3DXHANDLE DeferredTextured;
			D3DXHANDLE DeferredBurnout;
			D3DXHANDLE DeferredBurnoutTex;
		} vehicle;
		struct{
			D3DXHANDLE ForwardColor;
			D3DXHANDLE ForwardTextured;
			D3DXHANDLE DeferredColor;
			D3DXHANDLE DeferredTextured;
			D3DXHANDLE ReflectionTextured;
		} building;
		struct{
			D3DXHANDLE ForwardColor;
			D3DXHANDLE ForwardTextured;
			D3DXHANDLE DeferredColor;
			D3DXHANDLE DeferredTextured;
			D3DXHANDLE ReflectionTextured;
		} object;
		struct{
			D3DXHANDLE DirectLightShadows;
		} shadows;
	} m_techniques;
};

