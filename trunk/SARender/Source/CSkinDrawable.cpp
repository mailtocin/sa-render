#include "..\Headers\CSkinDrawable.h"

void CSkinDrawable::Patch()
{
	// FFP for skinned models
	CPatch::Nop(0x7C7CCA, 6);
	CPatch::Nop(0x7C7CD5, 6);
	CPatch::Nop(0x7C7CDD, 6);
	CPatch::SetChar(0x7C800F, 0xEB);
}