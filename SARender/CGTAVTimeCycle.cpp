#include "CGTAVTimeCycle.h"
using namespace rapidxml;
TCRegion CGTAVTimeCycle::currRegion;
int CGTAVTimeCycle::m_nCurrentTimeID;
int CGTAVTimeCycle::m_nNextTimeID;
float CGTAVTimeCycle::A;
float CGTAVTimeCycle::B;
void CGTAVTimeCycle::ParseTC(){
	file<> TimeCycle("w_clear.xml");
	xml_document<> doc;
	doc.parse<parse_declaration_node | parse_no_data_nodes>(TimeCycle.data());
	FILE *f;
	xml_node<>* cycles = doc.first_node("timecycle_keyframe_data");
	xml_node<>* regions = cycles->first_node("cycle");
	xml_node<>* regionsdata = regions->first_node("region");
	xml_node<>* regionsdata1 = regionsdata->first_node();
	
	fopen_s(&f, "log.log", "wt");
	// DirectScan
	sscanf_s(regionsdata1[0].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.LightDirCol[0].x, &currRegion.LightDirCol[1].x, &currRegion.LightDirCol[2].x, 
																			&currRegion.LightDirCol[3].x, &currRegion.LightDirCol[4].x, &currRegion.LightDirCol[5].x,
																			&currRegion.LightDirCol[6].x, &currRegion.LightDirCol[7].x, &currRegion.LightDirCol[8].x,
																			&currRegion.LightDirCol[9].x, &currRegion.LightDirCol[10].x, &currRegion.LightDirCol[11].x);

	sscanf_s(regionsdata1[1].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.LightDirCol[0].y, &currRegion.LightDirCol[1].y, &currRegion.LightDirCol[2].y,
		&currRegion.LightDirCol[3].y, &currRegion.LightDirCol[4].y, &currRegion.LightDirCol[5].y,
		&currRegion.LightDirCol[6].y, &currRegion.LightDirCol[7].y, &currRegion.LightDirCol[8].y,
		&currRegion.LightDirCol[9].y, &currRegion.LightDirCol[10].y, &currRegion.LightDirCol[11].y);
	sscanf_s(regionsdata1[2].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.LightDirCol[0].z, &currRegion.LightDirCol[1].z, &currRegion.LightDirCol[2].z,
		&currRegion.LightDirCol[3].z, &currRegion.LightDirCol[4].z, &currRegion.LightDirCol[5].z,
		&currRegion.LightDirCol[6].z, &currRegion.LightDirCol[7].z, &currRegion.LightDirCol[8].z,
		&currRegion.LightDirCol[9].z, &currRegion.LightDirCol[10].z, &currRegion.LightDirCol[11].z);
	// AmbientScan
	sscanf_s(regionsdata1[11].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.NaturAmbColDown[0].x, &currRegion.NaturAmbColDown[1].x, &currRegion.NaturAmbColDown[2].x,
		&currRegion.NaturAmbColDown[3].x, &currRegion.NaturAmbColDown[4].x, &currRegion.NaturAmbColDown[5].x,
		&currRegion.NaturAmbColDown[6].x, &currRegion.NaturAmbColDown[7].x, &currRegion.NaturAmbColDown[8].x,
		&currRegion.NaturAmbColDown[9].x, &currRegion.NaturAmbColDown[10].x, &currRegion.NaturAmbColDown[11].x);
	sscanf_s(regionsdata1[12].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.NaturAmbColDown[0].y, &currRegion.NaturAmbColDown[1].y, &currRegion.NaturAmbColDown[2].y,
		&currRegion.NaturAmbColDown[3].y, &currRegion.NaturAmbColDown[4].y, &currRegion.NaturAmbColDown[5].y,
		&currRegion.NaturAmbColDown[6].y, &currRegion.NaturAmbColDown[7].y, &currRegion.NaturAmbColDown[8].y,
		&currRegion.NaturAmbColDown[9].y, &currRegion.NaturAmbColDown[10].y, &currRegion.NaturAmbColDown[11].y);
	sscanf_s(regionsdata1[13].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.NaturAmbColDown[0].z, &currRegion.NaturAmbColDown[1].z, &currRegion.NaturAmbColDown[2].z,
		&currRegion.NaturAmbColDown[3].z, &currRegion.NaturAmbColDown[4].z, &currRegion.NaturAmbColDown[5].z,
		&currRegion.NaturAmbColDown[6].z, &currRegion.NaturAmbColDown[7].z, &currRegion.NaturAmbColDown[8].z,
		&currRegion.NaturAmbColDown[9].z, &currRegion.NaturAmbColDown[10].z, &currRegion.NaturAmbColDown[11].z);
	// AmbientScan2
	sscanf_s(regionsdata1[15].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.NaturAmbColUp[0].x, &currRegion.NaturAmbColUp[1].x, &currRegion.NaturAmbColUp[2].x,
		&currRegion.NaturAmbColUp[3].x, &currRegion.NaturAmbColUp[4].x, &currRegion.NaturAmbColUp[5].x,
		&currRegion.NaturAmbColUp[6].x, &currRegion.NaturAmbColUp[7].x, &currRegion.NaturAmbColUp[8].x,
		&currRegion.NaturAmbColUp[9].x, &currRegion.NaturAmbColUp[10].x, &currRegion.NaturAmbColUp[11].x);
	sscanf_s(regionsdata1[16].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.NaturAmbColUp[0].y, &currRegion.NaturAmbColUp[1].y, &currRegion.NaturAmbColUp[2].y,
		&currRegion.NaturAmbColUp[3].y, &currRegion.NaturAmbColUp[4].y, &currRegion.NaturAmbColUp[5].y,
		&currRegion.NaturAmbColUp[6].y, &currRegion.NaturAmbColUp[7].y, &currRegion.NaturAmbColUp[8].y,
		&currRegion.NaturAmbColUp[9].y, &currRegion.NaturAmbColUp[10].y, &currRegion.NaturAmbColUp[11].y);
	sscanf_s(regionsdata1[17].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.NaturAmbColUp[0].z, &currRegion.NaturAmbColUp[1].z, &currRegion.NaturAmbColUp[2].z,
		&currRegion.NaturAmbColUp[3].z, &currRegion.NaturAmbColUp[4].z, &currRegion.NaturAmbColUp[5].z,
		&currRegion.NaturAmbColUp[6].z, &currRegion.NaturAmbColUp[7].z, &currRegion.NaturAmbColUp[8].z,
		&currRegion.NaturAmbColUp[9].z, &currRegion.NaturAmbColUp[10].z, &currRegion.NaturAmbColUp[11].z);
	// FarClip
	sscanf_s(regionsdata1[347].value(), " %f %f %f %f %f %f %f %f %f %f %f %f", &currRegion.FarClip[0], &currRegion.FarClip[1], &currRegion.FarClip[2],
		&currRegion.FarClip[3], &currRegion.FarClip[4], &currRegion.FarClip[5],
		&currRegion.FarClip[6], &currRegion.FarClip[7], &currRegion.FarClip[8],
		&currRegion.FarClip[9], &currRegion.FarClip[10], &currRegion.FarClip[11]);
	fclose(f);
}
void CGTAVTimeCycle::UptadteTime(){
	float fTime = nGameClockSecs*0.00027777778f + nGameClockMins*0.016666668f + nGameClockHours;

	m_nCurrentTimeID = (int)floor(fTime / 2);
	B = (fTime / 2) - m_nCurrentTimeID;
	A = 1.0f - B;
	m_nNextTimeID = m_nCurrentTimeID + 1;
	if (m_nNextTimeID >= 12) m_nNextTimeID = 0;
}
D3DXVECTOR4 CGTAVTimeCycle::GetCurrentDirectLightColor(){
	return D3DXVECTOR4(currRegion.LightDirCol[m_nCurrentTimeID].x*A + currRegion.LightDirCol[m_nNextTimeID].x*B,
		currRegion.LightDirCol[m_nCurrentTimeID].y*A + currRegion.LightDirCol[m_nNextTimeID].y*B,
		currRegion.LightDirCol[m_nCurrentTimeID].z*A + currRegion.LightDirCol[m_nNextTimeID].z*B, 1.0);
}
D3DXVECTOR4 CGTAVTimeCycle::GetCurrentNatAmbLightColorDown(){
	return D3DXVECTOR4(currRegion.NaturAmbColDown[m_nCurrentTimeID].x*A + currRegion.NaturAmbColDown[m_nNextTimeID].x*B,
		currRegion.NaturAmbColDown[m_nCurrentTimeID].y*A + currRegion.NaturAmbColDown[m_nNextTimeID].y*B,
		currRegion.NaturAmbColDown[m_nCurrentTimeID].z*A + currRegion.NaturAmbColDown[m_nNextTimeID].z*B, 1.0);
}
D3DXVECTOR4 CGTAVTimeCycle::GetCurrentNatAmbLightColorUp(){
	return D3DXVECTOR4(currRegion.NaturAmbColUp[m_nCurrentTimeID].x*A + currRegion.NaturAmbColUp[m_nNextTimeID].x*B,
		currRegion.NaturAmbColUp[m_nCurrentTimeID].y*A + currRegion.NaturAmbColUp[m_nNextTimeID].y*B,
		currRegion.NaturAmbColUp[m_nCurrentTimeID].z*A + currRegion.NaturAmbColUp[m_nNextTimeID].z*B, 1.0);
}
float CGTAVTimeCycle::GetCurrentFarClip(){
	return currRegion.FarClip[m_nCurrentTimeID] * A + currRegion.FarClip[m_nNextTimeID] * B;
}