#include "RenderWare.h"



void RwV3dCrossProduct(RwV3d *out, RwV3d *ina, RwV3d *inb)
{
	out->x = inb->z * ina->y - ina->z * inb->y;
	out->y = ina->z * inb->x - inb->z * ina->x;
	out->z = ina->x * inb->y - inb->x * ina->y;
}
void RwV3dCrossProduct(RwV3d *inaout, RwV3d *inb)
{
	RwV3d ina;
	ina.x = inaout->x;
	ina.y = inaout->y;
	ina.z = inaout->z;
	inaout->x = inb->z * ina.y - ina.z * inb->y;
	inaout->y = ina.z * inb->x - inb->z * ina.x;
	inaout->z = ina.x * inb->y - inb->x * ina.y;
}
float RwV3dNormalize(RwV3d *out, RwV3d *in)
{
	float w = sqrt(in->x * in->x + in->y * in->y + in->z * in->z);
	if(w > FLT_EPSILON)
	{
		out->x = in->x / w;
		out->y = in->y / w;
		out->z = in->z / w;
	}
	else
		w = 0.0;
	return w;
}
float RwV3dNormalize(RwV3d *inout)
{
	float w = sqrt(inout->x * inout->x + inout->y * inout->y + inout->z * inout->z);
	if(w > FLT_EPSILON)
	{
		inout->x /= w;
		inout->y /= w;
		inout->z /= w;
	}
	else
		w = 0.0;
	return w;
}
void RwV3dAdd(RwV3d *out, RwV3d *ina, RwV3d *inb)
{
	out->x = ina->x + inb->x;
	out->y = ina->y + inb->y;
	out->z = ina->z + inb->z;
}
void RwV3dAdd(RwV3d *inaout, RwV3d *inb)
{
	inaout->x += inb->x;
	inaout->y += inb->y;
	inaout->z += inb->z;
}
void RwV3dSub(RwV3d *out, RwV3d *ina, RwV3d *inb)
{
	out->x = ina->x - inb->x;
	out->y = ina->y - inb->y;
	out->z = ina->z - inb->z;
}
void RwV3dSub(RwV3d *inaout, RwV3d *inb)
{
	inaout->x -= inb->x;
	inaout->y -= inb->y;
	inaout->z -= inb->z;
}
void RwV3dNegate(RwV3d *out, RwV3d *in)
{
	out->x = -in->x;
	out->y = -in->y;
	out->z = -in->z;
}
void RwV3dNegate(RwV3d *inout)
{
	inout->x = -inout->x;
	inout->y = -inout->y;
	inout->z = -inout->z;
}