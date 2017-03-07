// CH_ATTRIB_FACENODE.cpp: implementation of the CCH_ATTRIB_FACENODE class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CH_ATTRIB_FACENODE.h"
#include "QMeshNode.h"
#include "QMeshFace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCH_ATTRIB_FACENODE::CCH_ATTRIB_FACENODE()
{
	CCH_ATTRIB_NODE::CCH_ATTRIB_NODE();

	trglFace=NULL;	u=0.0;	v=0.0;	w=0.0;

	flags[0]=TRUE;
}

CCH_ATTRIB_FACENODE::~CCH_ATTRIB_FACENODE()
{
	CCH_ATTRIB_NODE::~CCH_ATTRIB_NODE();
}

void CCH_ATTRIB_FACENODE::GetCoord3D(double &x, double &y, double &z){
	if (trglFace==NULL) {printf("ERROR: no edge is attached yet!\n"); return;}
	QMeshNode* n[3];
	for (int i=1; i<=3; i++){
		n[i-1] = trglFace->GetNodeRecordPtr(i);
	}
	
	double u, v, w;
	GetParameter(u,v,w);
		
	double p1[3], p2[3], p3[3];
	trglFace->GetNodePos(1,p1[0],p1[1],p1[2]);
	trglFace->GetNodePos(2,p2[0],p2[1],p2[2]);
	trglFace->GetNodePos(3,p3[0],p3[1],p3[2]);

	x=u*p1[0]+v*p2[0]+w*p3[0];
	y=u*p1[1]+v*p2[1]+w*p3[1];
	z=u*p1[2]+v*p2[2]+w*p3[2];
}