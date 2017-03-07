// CH_ATTRIB_NODE.cpp: implementation of the CCH_ATTRIB_NODE class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CH_ATTRIB_NODE.h"
#include "CH_ATTRIB_EDGE.h"
#include "CH_ATTRIB_EDGENODE.h"
#include "CH_ATTRIB_FACENODE.h"
#include "QMeshFace.h"
#include "QMeshEdge.h"
#include "QMeshNode.h"
#include "Math.h"
#include "../GLKernel/CW_Geometry.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCH_ATTRIB_NODE::CCH_ATTRIB_NODE()
{
	indexno=0;
	Owner = NULL;
	for(UINT i=0;i<8;i++) flags[i]=FALSE;
	ForIdentifySeg = false;
	IsErrorNode = false;
	active = false;
	AttachList.RemoveAll();
}

CCH_ATTRIB_NODE::~CCH_ATTRIB_NODE()
{
  AttachList.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

int CCH_ATTRIB_NODE::GetIndexNo() const
{
	return indexno;
}

void CCH_ATTRIB_NODE::SetIndexNo( const int _index )
{
	indexno=_index;
}

bool CCH_ATTRIB_NODE::GetAttribFlag( const int whichBit )
{
	return flags[whichBit];
}

void CCH_ATTRIB_NODE::SetAttribFlag( const int whichBit, const bool toBe )
{
	flags[whichBit] = toBe;
}


void getNormal( CCH_ATTRIB_NODE* attrNode, double &nx, double &ny, double &nz )
{
	if (attrNode->GetAttribFlag(0))	//	ATTRIB_FACENODE
	{
		CCH_ATTRIB_FACENODE *faceNode=(CCH_ATTRIB_FACENODE *)attrNode;
		double aa,bb,cc,dd;
		faceNode->GetTrglFace()->GetPlaneEquation(aa,bb,cc,dd);
		dd=(float)sqrt(aa*aa+bb*bb+cc*cc);
		nx=aa/dd;	ny=bb/dd;	nz=cc/dd;
	}
	else	//	ATTRIB_EDGENODE
	{
		CCH_ATTRIB_EDGENODE *edgeNode=(CCH_ATTRIB_EDGENODE *)attrNode;
		QMeshFace *face;
		double nx1,ny1,nz1,nx2,ny2,nz2;
		double aa,bb,cc,dd;

		face=edgeNode->GetTrglEdge()->GetLeftFace();
		if (face) {
			face->GetPlaneEquation(aa,bb,cc,dd);
			dd=(float)sqrt(aa*aa+bb*bb+cc*cc);
			nx1=aa/dd;	ny1=bb/dd;	nz1=cc/dd;
		}
		else {
			nx1=0.0;	ny1=0.0;	nz1=0.0;
		}

		face=edgeNode->GetTrglEdge()->GetRightFace();
		if (face) {
			face->GetPlaneEquation(aa,bb,cc,dd);
			dd=(float)sqrt(aa*aa+bb*bb+cc*cc);
			nx2=aa/dd;	ny2=bb/dd;	nz2=cc/dd;
		}
		else {
			nx2=0.0;	ny2=0.0;	nz2=0.0;
		}

		nx=(nx1+nx2)/2.0;	ny=(ny1+ny2)/2.0;	nz=(nz1+nz2)/2.0;
		dd=(float)sqrt(nx*nx+ny*ny+nz*nz);
		nx=nx/dd;	ny=ny/dd;	nz=nz/dd;
	}
}

void position( CCH_ATTRIB_NODE* attrNode, double &x, double &y, double &z )
{
	if (attrNode->GetAttribFlag(0))	//	ATTRIB_FACENODE
	{
		CCH_ATTRIB_FACENODE *faceNode=(CCH_ATTRIB_FACENODE *)attrNode;
		double p1[3],p2[3],p3[3];
		double u,v,w;
		faceNode->GetParameter(u,v,w);
		
		faceNode->GetTrglFace()->GetNodePos(1,p1[0],p1[1],p1[2]);
		faceNode->GetTrglFace()->GetNodePos(2,p2[0],p2[1],p2[2]);
		faceNode->GetTrglFace()->GetNodePos(3,p3[0],p3[1],p3[2]);
		x=u*p1[0]+v*p2[0]+w*p3[0];
		y=u*p1[1]+v*p2[1]+w*p3[1];
		z=u*p1[2]+v*p2[2]+w*p3[2];
	

	}
	else	//	ATTRIB_EDGENODE
	{
		CCH_ATTRIB_EDGENODE *edgeNode=(CCH_ATTRIB_EDGENODE *)attrNode;
		double p0[3],p1[3];
		double u;
		edgeNode->GetParameter(u);
		edgeNode->GetTrglEdge()->GetStartPoint()->GetCoord3D(p0[0],p0[1],p0[2]);
		edgeNode->GetTrglEdge()->GetEndPoint()->GetCoord3D(p1[0],p1[1],p1[2]);
		x=p0[0]+(p1[0]-p0[0])*u;
		y=p0[1]+(p1[1]-p0[1])*u;
		z=p0[2]+(p1[2]-p0[2])*u;
	}
}
