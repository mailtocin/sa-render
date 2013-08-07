/********************************SA::Render source file*************************************/
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac                                                                   */
/* File descrip: Normals generation functions. CNormalsGenerator class.                    */
/* File created: 17.04.2013                                                                */
/* File last ed: 06.05.2013                                                                */
/*******************************************************************************************/
#include "CNormalsGenerator.h"
#include "RenderWare.h"
#include <math.h>
#include "CPatch.h"

bool CNormalsGenerator::GenerateNormals;

#ifdef _RENDERWARE_NORMALS_GENERATION
extern "C" double _rwSqrt(float a)
{
	return ___rwSqrt(a);
}
extern "C" double _rwV3dNormalize(RwV3d *a, RwV3d *b)
{
	return ___rwV3dNormalize(a, b);
}
extern "C" RpGeometry *RpGeometryLock(RpGeometry *geometry, int lockMode)
{
	return __RpGeometryLock(geometry, lockMode);
}
extern "C" RpGeometry *RpGeometryUnlock(RpGeometry *geometry)
{
	return __RpGeometryUnlock(geometry);
}
void *RwEngineInstance;
#endif

void CNormalsGenerator::Patch()
{
	CPatch::RedirectCall(0x74D234, CreateGeometryCheckNormals);
	CPatch::RedirectJump(0x7581F6, FixRwTangentsE);
	CPatch::RedirectCall(0x5D71D9, SetGeometryUsageFlag);
#ifdef _RENDERWARE_NORMALS_GENERATION
	CPatch::RedirectCall(0x72E107, ReadGeometryProcessRwNormals);
	CPatch::RedirectCall(0x72E1E7, ReadGeometryProcessRwNormals);
	CPatch::RedirectCall(0x72E247, ReadGeometryProcessRwNormals);
	CPatch::RedirectCall(0x72E333, ReadGeometryProcessRwNormals);
	CPatch::RedirectCall(0x74B1CC, ReadGeometryProcessRwNormals);
	CPatch::RedirectCall(0x74B7A3, ReadGeometryProcessRwNormals);
#else
	CPatch::RedirectJump(0x74D333, LoadMorphTargetsExecutor);
#endif
}

void CNormalsGenerator::SetGeometryUsageFlag(RpGeometry *geometry, unsigned int flags)
{
	unsigned int usageFlags = RpD3D9GeometryGetUsageFlags(geometry);
	usageFlags |= flags;
	RpD3D9GeometrySetUsageFlags(geometry, usageFlags);
}

RpGeometry *CNormalsGenerator::CreateGeometryCheckNormals(int numVerts, int numTriangles, unsigned int format)
{
	RpGeometry *geometry;
	unsigned int usageFlags;
#ifdef _RECALCULATE_NORMALS
	format |= rpGEOMETRYNORMALS;
	GenerateNormals = true;
#else 	
	if(format & rpGEOMETRYNORMALS)
		GenerateNormals = false;
	else
		GenerateNormals = true;
#endif
	geometry = RpGeometryCreate(numVerts, numTriangles, format);
	CNormalsGenerator::SetGeometryUsageFlag(geometry, rpD3D9GEOMETRYUSAGE_CREATETANGENTS);
	return geometry;
}

#ifdef _RENDERWARE_NORMALS_GENERATION
RpGeometry *CNormalsGenerator::ReadGeometryProcessRwNormals(unsigned int version)
{
	RpGeometry *geometry = RpGeometryStreamRead(version);
	if(geometry && GenerateNormals)
	{
		RwEngineInstance = *(RwGlobals **)0xC97B24;
		RtGeometryCalculateVertexNormals(geometry);
	}
	return geometry;
}
#else
unsigned int CNormalsGenerator::LoadMorphTargetsProcessNormals(RpGeometry *geometry, RwStream *stream)
{
	int i, i_triangle, i_vertex, i_normal;
	RwV3d vec1, vec2, normal;
	WORD vert1, vert2, vert3;
	MorphTargetBuffer buffer;
#ifdef _NORMALS_GENERATOR_SMOOTH_NORMALS
	int b, c, d;
	// Указатель на выделенную карту обработанных вершин
	char *pVertsDoneMap;
	// Указатели на позиции и нормали вершин геометрии
	RwV3d *normals, *verts;
	// Список идентификаторов вершин с одинаковыми координатами
	unsigned int *pAssocVerts;
	// Количество вершин с одинаковыми координатами в списке
	int numAssocVerts;
#endif
	if(geometry->numMorphTargets > 0)
	{
		for(i = 0; i < geometry->numMorphTargets; i++)
		{
			if(RwStreamRead(stream, &buffer, 24) != 24)
				return MORPHTARGETSLOADINGRESULT_ERROR;
			geometry->morphTarget[i].boundingSphere = buffer.bounding;
			if(buffer.hasPositions)
			{
				if(buffer.hasNormals)
				{
					if(!RwStreamReadReal(stream, geometry->morphTarget[i].verts, geometry->numVertices * 12))
						return MORPHTARGETSLOADINGRESULT_ERROR;
					if(!RwStreamReadReal(stream, geometry->morphTarget[i].normals, geometry->numVertices * 12))
						return MORPHTARGETSLOADINGRESULT_ERROR;
				}
				else
				{
					if(!RwStreamReadReal(stream, geometry->morphTarget[i].verts, geometry->numVertices * 12))
						return MORPHTARGETSLOADINGRESULT_ERROR;
				}
			}
			else if(buffer.hasNormals)
			{
				if(!RwStreamReadReal(stream, geometry->morphTarget[i].normals, geometry->numVertices * 12))
					return MORPHTARGETSLOADINGRESULT_ERROR;
			}
		}
		if(GenerateNormals)
		{
			for(i = 0; i < geometry->numMorphTargets; i++)
			{
				if(geometry->morphTarget[i].normals)
				{
					//Устанавливаем все нормали в 0
					memset(geometry->morphTarget[i].normals, 0, geometry->numVertices * 12);
					for(i_triangle = 0; i_triangle < geometry->numTriangles; i_triangle++)
					{
						//Находим нормаль для треугольника
						vert1 = geometry->triangles[i_triangle].vertIndex[0];
						vert2 = geometry->triangles[i_triangle].vertIndex[1];
						vert3 = geometry->triangles[i_triangle].vertIndex[2];
						RwV3dSub(&vec1, &geometry->morphTarget[i].verts[vert1], &geometry->morphTarget[i].verts[vert2]);
						RwV3dSub(&vec2, &geometry->morphTarget[i].verts[vert1], &geometry->morphTarget[i].verts[vert3]);
						RwV3dCrossProduct(&normal, &vec1, &vec2);
						if(RwV3dNormalize(&normal) != 0.0)
						{
							for(i_vertex=0; i_vertex<3; i_vertex++)
								RwV3dAdd(&geometry->morphTarget[i].normals[geometry->triangles[i_triangle].vertIndex[i_vertex]], &normal);
						}
					}
					//Нормализируем нормали
					for(i_normal = 0; i_normal < geometry->numVertices; i_normal++)
					{
						if(RwV3dNormalize(&geometry->morphTarget[i].normals[i_normal]) == 0.0)
						{
							geometry->morphTarget[i].normals[i_normal].x = 0.0;
							geometry->morphTarget[i].normals[i_normal].y = 0.0;
							geometry->morphTarget[i].normals[i_normal].z = 1.0;
						}
					}
				}
				#ifdef _NORMALS_GENERATOR_SMOOTH_NORMALS
					// Выравниваем нормали для вершин в одинаковых координатах
					verts = geometry->morphTarget[i].verts;
					normals = geometry->morphTarget[i].normals;
					// Создаем карту обработанных вершин
					pVertsDoneMap = (char *)malloc(geometry->numVertices);
					// Создаем список вершин с одинаковыми координатами
					pAssocVerts = (unsigned int *)malloc(geometry->numVertices * 4);
					memset(pVertsDoneMap, 0, geometry->numVertices);
					// В цикле проходимся по всем вершинам
					for(b = 0; b < geometry->numVertices; b++)
					{
						// Если вершина ещё не обработана
						if(!pVertsDoneMap[b])
						{
							// То находим все идентичные вершины для этой вершины.
							pAssocVerts[0] = b;
							numAssocVerts = 1;
							for(c = b + 1; c < geometry->numVertices; c++)
							{
								if(!pVertsDoneMap[c])
								{
									// Сверяем координаты
									if(fabs(verts[b].x - verts[c].x) < FLT_EPSILON && fabs(verts[b].y - verts[c].y) < FLT_EPSILON &&
										fabs(verts[b].z - verts[c].z) < FLT_EPSILON)
									{
										pVertsDoneMap[c] = 1;
										pAssocVerts[numAssocVerts] = c;
										numAssocVerts++;
									}
								}
							}
							// Если мы нашли одинаковые вершины
							if(numAssocVerts > 1)
							{
								// То усредняем их нормали
								for(d = 1; d < numAssocVerts; d++)
									RwV3dAdd(&normals[b], &normals[b], &normals[pAssocVerts[d]]);
								RwV3dNormalize(&normals[b], &normals[b]);
								for(d = 1; d < numAssocVerts; d++)
									normals[pAssocVerts[d]] = normals[b];
							}
							pVertsDoneMap[b] = 1;
						}
					}
					free(pAssocVerts);
					free(pVertsDoneMap);
				#endif
			}
		}
	}
	return MORPHTARGETSLOADINGRESULT_OK;
}

void __declspec(naked)CNormalsGenerator::LoadMorphTargetsExecutor()
{
	__asm
	{
		pushad
		push      ebx
		push      esi
		call      CNormalsGenerator::LoadMorphTargetsProcessNormals
		add       esp, 8
		cmp       eax, MORPHTARGETSLOADINGRESULT_OK
		jne       LABEL_LOADMORPHTARGETSERROR
		popad
		mov       eax, 0x74D48B
		jmp       eax

LABEL_LOADMORPHTARGETSERROR:
		popad
		mov       eax, 0x74D522
		jmp       eax
	}
}
#endif

void CNormalsGenerator::FixRwTangents(RpGeometry *geometry, int type, void *mem, RwV3D *verts, RwTexCoords *texCoords, RxD3D9ResEntryHeader *resEntry, int stride)
{
 ((void (__cdecl *)(int, void *, RwV3D *, RwTexCoords *, RxD3D9ResEntryHeader *, int))0x754E20)
  (type, mem, verts, geometry->texCoords[0], resEntry, stride);
}

void __declspec(naked)CNormalsGenerator::FixRwTangentsE()
{
	__asm{
		push esi
		call FixRwTangents
		add esp, 0x1C
		mov eax, 0x7581FE
		jmp eax
	}
}