#include "CResourceLoader.h"
#include "CGame.h"
#include "CPatch.h"

RwTexture *CResources::LoadTextureFromFile(char *filename)
{
	unsigned int width, height, depth, flags;
	RwImage *image = RtPNGImageRead(filename);
	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster *raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);
	RwImageDestroy(image);
	return RwTextureCreate(raster);
}

RwTexture *CResources::LoadTextureFromMemory(char *data, unsigned int size)
{
	CPatch::SetChar(0x7CF9CA, rwSTREAMMEMORY);
	RwMemory memoryImage;
	unsigned int width, height, depth, flags;
	memoryImage.start = data;
	memoryImage.length = size;
	RwImage *image = RtPNGImageRead((char *)&memoryImage);
	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster *raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);
	RwImageDestroy(image);
	CPatch::SetChar(0x7CF9CA, rwSTREAMFILENAME);
	return RwTextureCreate(raster);
}

RpAtomic *CResources::LoadModelFromFile(char *filename)
{
	RpClump *clump = LoadClumpFromFile(filename);
	if(clump)
	{
		RpAtomic *atomic = NULL, *copy = NULL;
        RpClumpForAllAtomics(clump, (void *)0x734810, &atomic);
        if(atomic)
        {
            copy = RpAtomicClone(atomic);
            RpAtomicSetFrame(copy, RwFrameCreate());
			copy->object.object.flags = rpATOMICRENDER;
            RpClumpDestroy(clump);
			return copy;
		}
	}
	return NULL;
}

RpAtomic *CResources::LoadModelFromMemory(char *data, unsigned int size)
{
	CPatch::SetChar(0x53706A, rwSTREAMMEMORY);
	RwMemory memoryModel;
	memoryModel.start = data;
	memoryModel.length = size;
	RpClump *clump = LoadClumpFromFile((char *)&memoryModel);
	CPatch::SetChar(0x53706A, rwSTREAMFILENAME);
	if(clump)
	{
		RpAtomic *atomic = NULL, *copy = NULL;
		RpClumpForAllAtomics(clump, (void *)0x734810, &atomic);
		if(atomic)
		{
			copy = RpAtomicClone(atomic);
			RpAtomicSetFrame(copy, RwFrameCreate());
			copy->object.object.flags = rpATOMICRENDER;
			RpClumpDestroy(clump);
			return copy;
		}
	}
	return NULL;
}

void CResources::UnloadTexture(RwTexture *texture)
{
	if(texture)
		RwTextureDestroy(texture);
}

void CResources::UnloadModel(RpAtomic *model)
{
	if(model)
	{
		RwFrame *frame = model->object.object.parent;
		if(frame)
		{
			RpAtomicSetFrame(model, NULL);
			RwFrameDestroy(frame);
		}
		RpAtomicDestroy(model);
	}
}