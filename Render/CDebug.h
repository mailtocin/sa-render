#pragma once
#include <stdio.h>
#include "D3D9Headers\d3d9.h"
#include "D3D9Headers\d3dx9.h"

#define DEBUG_ENABLED
//#define DEBUG_VERTEX_DECLARATION
//#define DEBUG_BLEND_STATES

class CDebug
{
public:
	static FILE *m_pVertexDeclarationFile;
	static FILE *m_pBlendStateFile;

	static void Initialize();
	static void Shutdown();
	static void StoreVertexDeclaration(IDirect3DVertexDeclaration9 *declaration);
	static void StoreBlendStates(DWORD dest,DWORD src);
	static bool CheckForShaderErrors(ID3DXBuffer* errors, char *className, char *shaderName, HRESULT result);
};

// Vertex declaration stuff
char *gDeclTypes[] ;
char *gDeclMethods[];
char *gDeclUsages[];

char *gDestStates[];