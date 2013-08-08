#pragma once
#include "CRender.h"
#include "D3DDeviceReset.h"
#include "CNormalsGenerator.h"
#include "CTextureMaps.h"
#include "CVehicleRender.h"
#include "CDeferredRendering.h"
#include "CObjectRender.h"
#include "CPedsRender.h"
#include "CPatch.h"
#include "CGame.h"
#pragma comment(lib, "D3DDeviceReset.lib")

D3DXMATRIX CRender::m_mTransform;

void CRender::Patch()
{
	DefineReset();
	CPatch::RedirectCall(0x5BD765, Setup);
	CNormalsGenerator::Patch();
	CTextureMaps::Patch();
	CVehicleRender::Patch();
	CObjectRender::Patch();
	CDeferredRendering::Patch();
	CPedsRender::Patch();
}

void CRender::Setup()
{
	CDeferredRendering::Setup();
	CVehicleRender::Setup();
	CObjectRender::Setup();
	CPedsRender::Setup();
}

void CRender::DefineReset()
{
	D3DDeviceResetAdd(BeforeReset, true);
	D3DDeviceResetAdd(AfterReset, false);
}

void CRender::BeforeReset()
{
	CVehicleRender::Lost();
	CDeferredRendering::Lost();
	CObjectRender::Lost();
	CPedsRender::Lost();
}

void CRender::AfterReset()
{
	CVehicleRender::Reset();
	CDeferredRendering::Reset();
	CObjectRender::Reset();
	CPedsRender::Reset();
}

D3DXMATRIX *__cdecl CRender::_getComposedMatrix(D3DXMATRIX *m_out)
{
	D3DXMATRIX viewproj; // eax@1
	D3DXMATRIX *result; // eax@4
	D3DXMATRIX view,proj;
	g_Device->GetTransform(D3DTS_VIEW,&view);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	D3DXMatrixMultiply(&viewproj,&view,&proj);
	if ( D3D9ActiveTransform )
		result = D3DXMatrixMultiplyTranspose(m_out, D3D9Transform, &viewproj);
	else
		result = D3DXMatrixTranspose(m_out, &viewproj);

	return result;
}
