#include "CPostProcess.h"
#include "CDebug.h"

IDirect3DTexture9 *CPostProcess::lensTex;
IDirect3DTexture9 *CPostProcess::screenTex;
IDirect3DSurface9 *CPostProcess::screenSurf;
IDirect3DTexture9 *CPostProcess::bluredTex;
IDirect3DSurface9 *CPostProcess::bluredSurf;
ID3DXEffect *CPostProcess::m_pEffect;
bool CPostProcess::Setup()
{
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/lens.tga",&lensTex);
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"resources/Shaders/PostProcess.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CPostProcess", "PostProcess", result))
	{
		return false;
	}
	return true;
}

void CPostProcess::CreateRTs() {
	if (!screenSurf)
	{
		g_Device->CreateTexture(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &screenTex, NULL);
		screenTex->GetSurfaceLevel(0, &screenSurf);
	}
	if (!bluredSurf)
	{
		g_Device->CreateTexture(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &bluredTex, NULL);
		bluredTex->GetSurfaceLevel(0, &bluredSurf);
	}
}

void CPostProcess::DoBlur() {
	g_Device->SetRenderTarget(0,bluredSurf);
	m_pEffect->SetVector("fInverseViewportDimensions", &D3DXVECTOR4(1.0f/(float)RsGlobal->MaximumWidth,1.0f/(float)RsGlobal->MaximumHeight,1,1));
	m_pEffect->SetTexture("tScreen",screenTex);
	m_pEffect->SetTechnique("Blur");
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
	DrawPostProcessPass(m_pEffect);
}

void CPostProcess::Render() {
	IDirect3DSurface9 *outSurf;
	g_Device->GetRenderTarget(0,&outSurf);
	outSurf->Release();
	g_Device->StretchRect(outSurf,NULL,screenSurf,NULL,D3DTEXF_NONE);
	DoBlur();
	g_Device->SetRenderTarget(0,outSurf);
	m_pEffect->SetVector("fInverseViewportDimensions", &D3DXVECTOR4(1.0f/(float)RsGlobal->MaximumWidth,1.0f/(float)RsGlobal->MaximumHeight,1,1));
	m_pEffect->SetTexture("tScreen",screenTex);
	m_pEffect->SetTexture("tBlur",bluredTex);
	m_pEffect->SetTexture("tLens",lensTex);
	WORD mode4 = TheCamera->Cams[TheCamera->ActiveCam].Mode;
	if(mode4 == 53 || mode4 == 55 || mode4 == 65) {
		m_pEffect->SetVector("vFocusPos",&D3DXVECTOR4( *(float *)0xB6EC14,*(float *)0xB6EC10,1.0f,1.0f));
		m_pEffect->SetBool("bAutofocus",true);
	} else {
		m_pEffect->SetVector("vFocusPos",&D3DXVECTOR4( 0.5f,0.5f,1.0f,1.0f));
		m_pEffect->SetBool("bAutofocus",false);
	}
	m_pEffect->SetTechnique("DepthOfField");
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
	DrawPostProcessPass(m_pEffect);
}

void CPostProcess::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CPostProcess::Lost()
{
	SAFE_RELEASE(screenSurf);
	SAFE_RELEASE(screenTex);
	SAFE_RELEASE(bluredSurf);
	SAFE_RELEASE(bluredTex);
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}