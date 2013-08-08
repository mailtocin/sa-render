#pragma once
#include "RenderWare.h"
#include <d3d9.h>
#include <d3dx9.h>

struct CLight
{
public:
	RwV3d pos,dir;
	float radius, red, green, blue;
	BYTE type; /*
						0  Omni/Point   Using position and radius.
						1  Directional  Using position, radius and direction.
						2  Shadow       It's black!
						4  Global       Using only position.
						*/
	BYTE subType; /*
								0 Default.
								1 Car headlights.
								2 Car backlights.
								3 Car LED 
								*/
	BYTE fogType, generateShadows;
};