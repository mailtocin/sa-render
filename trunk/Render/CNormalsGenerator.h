/********************************SA::Render source file*************************************/
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac                                                                   */
/* File descrip: Normals generation header. CNormalsGenerator class.                       */
/* File created: 17.04.2013                                                                */
/* File last ed: 06.05.2013                                                                */
/*******************************************************************************************/
#pragma once
#include "RenderWare.h"

// ћы используем код RenderWare дл€ генерации нормалей
//#define _RENDERWARE_NORMALS_GENERATION
// ѕересчитать нормали дл€ моделей с нормал€ми
#define _RECALCULATE_NORMALS
// —гладить нормали, не работает с _RENDERWARE_NORMALS_GENERATION
//#define _NORMALS_GENERATOR_SMOOTH_NORMALS

#ifdef _RENDERWARE_NORMALS_GENERATION
#define ___rwV3dNormalize(a, b) ((double (__cdecl *)(RwV3d *, RwV3d *))0x7ED910)(a, b)
#define ___rwSqrt(a) ((double (__cdecl *)(float))0x7EDB30)(a)
#define __RpGeometryLock(geometry, lockMode) ((RpGeometry *(__cdecl *)(RpGeometry *, int))0x74C7D0)(geometry, lockMode)
#define __RpGeometryUnlock(geometry) ((RpGeometry *(__cdecl *)(RpGeometry *))0x74C800)(geometry)
extern "C" RpGeometry *RtGeometryCalculateVertexNormals(RpGeometry *geometry);
extern "C" double _rwSqrt(float a);
extern "C" double _rwV3dNormalize(RwV3d *a, RwV3d *b);
extern "C" RpGeometry *RpGeometryLock(RpGeometry *geometry, int lockMode);
extern "C" RpGeometry *RpGeometryUnlock(RpGeometry *geometry);
extern "C" void *RwEngineInstance;
#else
enum eMorphTargetLoadingResult
{
	MORPHTARGETSLOADINGRESULT_OK = 1,
	MORPHTARGETSLOADINGRESULT_ERROR
};

struct MorphTargetBuffer
{
	RwSphere bounding;
	int hasPositions;
	int hasNormals;
};
#endif

class CNormalsGenerator
{
public:
	static RpGeometry *CreateGeometryCheckNormals(int numVerts, int numTriangles, unsigned int format);
	static bool GenerateNormals;
#ifdef _RENDERWARE_NORMALS_GENERATION
	static RpGeometry *ReadGeometryProcessRwNormals(unsigned int version);
#else
	static unsigned int LoadMorphTargetsProcessNormals(RpGeometry *geometry, RwStream *stream);
	static void LoadMorphTargetsExecutor();
#endif
	static void Patch();

	static void CNormalsGenerator::FixRwTangents(RpGeometry *geometry, int type, void *mem, RwV3D *verts, RwTexCoords *texCoords, 
		RxD3D9ResEntryHeader *resEntry, int stride);
	static void CNormalsGenerator::FixRwTangentsE();
	static void CNormalsGenerator::SetGeometryUsageFlag(RpGeometry *geometry, unsigned int flags);
};