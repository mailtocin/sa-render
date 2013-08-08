#pragma once
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>

#define DEBUG_ENABLED
//#define DEBUG_VERTEX_DECLARATION

class CDebug
{
public:
	static FILE *m_pVertexDeclarationFile;

	static void Initialize();
	static void Shutdown();
	static void StoreVertexDeclaration(IDirect3DVertexDeclaration9 *declaration);
	static bool CheckForD3D9Errors(ID3DXBuffer *declaration, LPCSTR message, HRESULT result);
};

// Vertex declaration stuff
char *gDeclTypes[] ;
char *gDeclMethods[];
char *gDeclUsages[];