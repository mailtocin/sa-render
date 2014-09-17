/********************************SA::Render source file*************************************/
/* File name   : CRenderTarget.h                                                           */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CRenderTarget is render target class.								       */
/* File created: 13.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#pragma once

#include "CRTMgr.h"
#include "CGame.h"

class CRenderTarget
{
	IDirect3DTexture9* m_pTexture;
	IDirect3DSurface9* m_pSurface;

	int m_Width;
	int m_Height;
	int m_Levels;
	D3DFORMAT m_Format;
	unsigned int m_Usage;

public:

	void Initialise(int Width, int Height, int Levels, D3DFORMAT Format, DWORD Usage)
	{
		m_Width = Width;
		m_Height = Height;
		m_Levels = Levels;
		m_Format = Format;
		m_Usage = Usage;
		g_Device->CreateTexture(m_Width, m_Height, m_Levels, m_Usage, m_Format, D3DPOOL_DEFAULT, &m_pTexture, NULL);
		m_pTexture->GetSurfaceLevel(0, &m_pSurface);
	}

	void UpdateSize(int Width, int Height)
	{
		m_Width = Width;
		m_Height = Height;
	}

	CRenderTarget(int Width, int Height, int Levels, D3DFORMAT Format, bool IsDS)
	{
		m_pTexture = NULL;
		m_pSurface = NULL;
		Initialise(Width, Height, Levels, Format, IsDS ? D3DUSAGE_DEPTHSTENCIL : D3DUSAGE_RENDERTARGET);
		gRTMgr.AddRT(this);
	}

	CRenderTarget(int Width, int Height, int Levels, D3DFORMAT Format, DWORD Usage)
	{
		m_pTexture = NULL;
		m_pSurface = NULL;
		Initialise(Width, Height, Levels, Format, Usage);
		gRTMgr.AddRT(this);
	}

	~CRenderTarget()
	{
		if (m_pTexture)
			m_pTexture->Release();
		gRTMgr.RemoveRT(this);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(m_pTexture);
		SAFE_RELEASE(m_pSurface);
	}

	void OnResetDevice()
	{
		SAFE_RELEASE(m_pTexture);
		SAFE_RELEASE(m_pSurface);
		Initialise(m_Width, m_Height, m_Levels, m_Format, m_Usage);
	}

	void SetRenderTarget(int Num)
	{
		if (m_Usage == D3DUSAGE_DEPTHSTENCIL)
			g_Device->SetDepthStencilSurface(m_pSurface);
		else
			g_Device->SetRenderTarget(Num, m_pSurface);
	}

	IDirect3DSurface9* GetSurf()
	{
		return m_pSurface;
	}

	IDirect3DTexture9* GetTex()
	{
		return m_pTexture;
	}
};