#include "..\Headers\CDebug.h"

CEffect *CDebug::m_pEffect;

void CDebug::Initialize()
{
	m_pEffect = new CEffect(D3D_EFFECT_DEBUG);
}

void CDebug::RenderDebug()
{
	m_pEffect->Begin();
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
	Im2DRenderQuad(0, 0, 512, 512, 0, 0, 0, 0);
	m_pEffect->End();
}
