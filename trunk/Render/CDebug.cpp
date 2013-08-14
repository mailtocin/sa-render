#include "CDebug.h"
#include <stdio.h>

FILE *CDebug::m_pVertexDeclarationFile;

void CDebug::Initialize()
{
// Opening/creating all needed files here!
#if defined(DEBUG_ENABLED)

    #if defined(DEBUG_VERTEX_DECLARATION)
       m_pVertexDeclarationFile = fopen("sa_render_DebugVD.txt", "w");
    #endif

#endif
}

bool CDebug::CheckForShaderErrors(ID3DXBuffer* errors, char *className, char *shaderName, HRESULT result){
	char message[128];
	if(errors)
	{
		MessageBox(0, (char *)errors->GetBufferPointer(), 0, 0);
		errors->Release();
	}
	if(FAILED(result))
	{
		strcpy_s(message, className);
		strcat_s(message, "::Setup : D3DXCreateEffectFromFile() - failed while compiling ");
		strcat_s(message, shaderName);
		strcat_s(message, ".fx");
		MessageBox(0, message, 0, 0);
		return false;
	}
	return true;
}

void CDebug::Shutdown()
{
// Closing all files - here!
#if defined(DEBUG_ENABLED)

    #if defined(DEBUG_VERTEX_DECLARATION)
       fclose(m_pVertexDeclarationFile);
   #endif

#endif
}

// The func gets desription of vertex declaration and stores it to sa_render_DebugVD.txt file.
void CDebug::StoreVertexDeclaration(IDirect3DVertexDeclaration9 *declaration)
{
#if defined(DEBUG_ENABLED) && defined(DEBUG_VERTEX_DECLARATION)
   char temp[128];
   D3DVERTEXELEMENT9 decl[20];
   UINT numElements;
   declaration->GetDeclaration(decl, &numElements);
   for(int i = 0; i < numElements; i++)
   {
      sprintf(temp, "Stream: %3d Offset: %3d Type: %s Method: %s Usage: %s UsageIndex: %3d\n", decl[i].Stream, decl[i].Offset,
         gDeclTypes[decl[i].Type], gDeclMethods[decl[i].Method], gDeclUsages[decl[i].Usage], decl[i].UsageIndex);
      fputs(temp, m_pVertexDeclarationFile);
   }
   fputs("\n", m_pVertexDeclarationFile);
#endif
}

#if defined(DEBUG_ENABLED) && defined(DEBUG_VERTEX_DECLARATION)
// Vertex declaration stuff
char *gDeclTypes[] = {
   "FLOAT1   ",
   "FLOAT2   ",
    "FLOAT3   ",
    "FLOAT4   ",
    "D3DCOLOR ",
    "UBYTE4   ",
    "SHORT2   ",
    "SHORT4   ",
    "UBYTE4N  ",
    "SHORT2N  ",
    "SHORT4N  ",
    "USHORT2N ",
    "USHORT4N ",
    "UDEC3    ",
    "DEC3N    ",
    "FLOAT16_2",
    "FLOAT16_4",
    "UNUSED   "
};

char *gDeclMethods[] = {
   "DEFAULT         ",
   "PARTIALU        ",
    "PARTIALV        ",
    "CROSSUV         ",
    "UV              ",
    "LOOKUP          ",
    "LOOKUPPRESAMPLED"
};

char *gDeclUsages[] = {
   "D3DDECLUSAGE_POSITION    ",
   "D3DDECLUSAGE_BLENDWEIGHT ",
    "D3DDECLUSAGE_BLENDINDICES",
    "D3DDECLUSAGE_NORMAL      ",
    "D3DDECLUSAGE_PSIZE       ",
    "D3DDECLUSAGE_TEXCOORD    ",
    "D3DDECLUSAGE_TANGENT     ",
    "D3DDECLUSAGE_BINORMAL    ",
    "D3DDECLUSAGE_TESSFACTOR  ",
    "D3DDECLUSAGE_POSITIONT   ",
    "D3DDECLUSAGE_COLOR       ",
    "D3DDECLUSAGE_FOG         ",
    "D3DDECLUSAGE_DEPTH       ",
    "D3DDECLUSAGE_SAMPLE      "
};
#endif