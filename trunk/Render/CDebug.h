#pragma once
#include <stdio.h>
#include <d3d9.h>

#define DEBUG_ENABLED
//#define DEBUG_VERTEX_DECLARATION

class CDebug
{
public:
	static FILE *m_pVertexDeclarationFile;

	static void Initialize();
	static void Shutdown();
	static void StoreVertexDeclaration(IDirect3DVertexDeclaration9 *declaration);
};

// Vertex declaration stuff
char *gDeclTypes[] ;
char *gDeclMethods[];
char *gDeclUsages[];