// QMeshFace.cpp: implementation of the QMeshFace class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <math.h>

#include "..\GLKLib\GLKGeometry.h"

#include "QMeshFace.h"
#include "QMeshEdge.h"
#include "QMeshNode.h"
#include "QMeshTetra.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QMeshFace::QMeshFace()
{
	type = 2;
	UINT i;
	indexno=0;
	for(i=0;i<3;i++) flags[i]=FALSE;
	for(i=0;i<3;i++) edges[i]=NULL;
	meshSurface=NULL;

	selected=false;

	Pos=NULL;

	attachedList.RemoveAll();

	//added for VSA
	m_nIdentifiedPatchIndex=-1;

	//for volume mesh
	i_inner=inner=false;
	SetLeftTetra();
	SetRightTetra();

	SetLeftHexa();
	SetRightHexa();
}

QMeshFace::~QMeshFace()
{
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
BOOL QMeshFace::GetAttribFlag( const int whichBit )
{
	return flags[whichBit];
}

void QMeshFace::SetAttribFlag( const int whichBit, const BOOL toBe )
{
	flags[whichBit] = toBe;
}

UINT QMeshFace::GetIndexNo()
{
	return indexno;
}

void QMeshFace::SetIndexNo( const UINT _index )
{
	indexno = _index;
}

BOOL QMeshFace::IsNormalDirection( const UINT whichEdge )
{
	//ASSERT( (whichEdge==1) || (whichEdge==2) || (whichEdge==3) );
	UINT bitNumber = whichEdge - 1;
	return flags[bitNumber];
}

void QMeshFace::SetDirectionFlag( const UINT whichEdge, const BOOL toBe )
{
	//ASSERT( (whichEdge==1) || (whichEdge==2) || (whichEdge==3) );
	UINT bitNumber = whichEdge - 1;
	flags[bitNumber]=toBe;
}

QMeshEdge * QMeshFace::GetEdgeRecordPtr( const UINT whichEdge )
{
	return edges[whichEdge-1];
}

void QMeshFace::SetEdgeRecordPtr( const UINT whichEdge, QMeshEdge * _edge )
{
	edges[whichEdge-1] = _edge;
}

void QMeshFace::GetNodePos( const UINT whichNode, double &xx, double &yy, double &zz)
{
	//ASSERT( (whichNode==1) || (whichNode==2) || (whichNode==3) );
	if(IsNormalDirection(whichNode))
	    edges[whichNode-1]->GetStartPoint()->GetCoord3D(xx,yy,zz);
	else
	    edges[whichNode-1]->GetEndPoint()->GetCoord3D(xx,yy,zz);
}

QMeshNode * QMeshFace::GetNodeRecordPtr( const UINT whichNode )
{
	//ASSERT( (whichNode==1) || (whichNode==2) || (whichNode==3) );
	if(IsNormalDirection(whichNode))
	    return edges[whichNode-1]->GetStartPoint();
	else
	    return edges[whichNode-1]->GetEndPoint();
	return NULL;
}

//-------------------------------------------------------------------------
// to calculate plane equation parameter
// Plane equation:  Ax + By + Cz + D = 0, and
// Vector(A,B,C) is positive unit normal vector of this trangle plane

// void QMeshFace::CalPlaneEquation( double & A, double & B, double & C, double & D )
// {
// 	double x[3],y[3],z[3];
// 	GLKGeometry geometry;
// 
// 	for(UINT i=0;i<3;i++) GetNodePos(i+1,x[i],y[i],z[i]);
// 	
// 	geometry.CalPlaneEquation(A,B,C,D,x,y,z);
// 
// 	abcd[0] = A;   abcd[1] = B;   abcd[2] = C;   abcd[3] = D;   
//}

void QMeshFace::GetPlaneEquation(double & A, double & B, double & C, double & D )
{
	GLKGeometry geo;
	double** p;
	int i,nodeNum;
	double aa,bb,cc,dd;
	double abcd[4];

	nodeNum=edgeNum;
	p=(double**)new long[nodeNum];
	for(i=0;i<nodeNum;i++) p[i]=new double[3];

	for(i=0;i<nodeNum;i++) GetNodePos(i+1,p[i][0],p[i][1],p[i][2]);

	for(i=0;i<4;i++) abcd[i]=0.0;
	for(i=0;i<nodeNum;i++) {
		geo.CalPlaneEquation(p[(i-1+nodeNum)%nodeNum],p[i],p[(i+1)%nodeNum],aa,bb,cc,dd);
		abcd[0]+=aa;		abcd[1]+=bb;		abcd[2]+=cc;		abcd[3]+=dd;
	}
	for(i=0;i<4;i++) abcd[i]=abcd[i]/(double)nodeNum;

	for(i=0;i<nodeNum;i++) delete [](double*)(p[i]);
	delete [](double**)p;

	A=abcd[0]; B=abcd[1]; C=abcd[2]; D=abcd[3];
}

//void QMeshFace::GetPlaneEquation( double & A, double & B, double & C, double & D )
//{
//	A = abcd[0];   B = abcd[1];   C = abcd[2];   D = abcd[3];
//}

void QMeshFace::SetMeshSurfacePtr(QMeshPatch* _mesh)
{
	meshSurface=_mesh;
}

QMeshPatch* QMeshFace::GetMeshSurfacePtr()
{
	return meshSurface;
}

int QMeshFace::GetEdgeNum()
{
	return edgeNum;
}

void QMeshFace::SetEdgeNum(int num)
{
	edgeNum=num;
}

//added for VSA
void QMeshFace::CalCenterPos(double &xx, double &yy, double &zz)
{
	int pntNum=GetEdgeNum();
	double pp[3];
	xx = 0.0; yy = 0.0; zz = 0.0;

	for (int i=0;i<pntNum;i++) {
		GetNodePos(i+1,pp[0],pp[1],pp[2]);
		xx=xx+pp[0];	yy=yy+pp[1];	zz=zz+pp[2];
	}
	xx=xx/(double)pntNum;	yy=yy/(double)pntNum;	zz=zz/(double)pntNum;	
}

void QMeshFace::CalBoundingBox(double &xmin, double &ymin, double &zmin,
							   double &xmax, double &ymax, double &zmax)
{
	int pntNum=GetEdgeNum();
	double pp[3];

	GetNodePos(1,xmin,ymin,zmin);
	GetNodePos(1,xmax,ymax,zmax);
	for(int i=1;i<pntNum;i++) {
		GetNodePos(i+1,pp[0],pp[1],pp[2]);
		if (pp[0]<xmin) xmin=pp[0];
		if (pp[0]>xmax) xmax=pp[0];
		if (pp[1]<ymin) ymin=pp[1];
		if (pp[1]>ymax) ymax=pp[1];
		if (pp[2]<zmin) zmin=pp[2];
		if (pp[2]>zmax) zmax=pp[2];
	}
}

double QMeshFace::CalArea()
{
	double cp[3],p1[3],p2[3];
	double area;
	double x1,y1,z1,x2,y2,z2;
	double ii,jj,kk;
	int i;

	
	CalCenterPos(cp[0],cp[1],cp[2]);
	area=0.0;

	GetNodePos(1,p1[0],p1[1],p1[2]);
	for(i=0;i<edgeNum;i++) {
		GetNodePos((((i+1)%edgeNum)+1),p2[0],p2[1],p2[2]);

		x1=p1[0]-cp[0];	y1=p1[1]-cp[1];	z1=p1[2]-cp[2];
		x2=p2[0]-cp[0];	y2=p2[1]-cp[1];	z2=p2[2]-cp[2];

		ii=y1*z2-z1*y2;
		jj=x2*z1-x1*z2;
		kk=x1*y2-x2*y1;

		area+=sqrt(ii*ii+jj*jj+kk*kk)/2.0;

		p1[0]=p2[0]; p1[1]=p2[1]; p1[2]=p2[2];
	}

	m_area=area;

	return area;
}

QMeshTetra * QMeshFace::GetLeftTetra()
{
	return pLeftTetra;
}

void QMeshFace::SetLeftTetra( QMeshTetra * _pLeftTetra )
{
	pLeftTetra = _pLeftTetra;
}

QMeshTetra * QMeshFace::GetRightTetra()
{
	return pRightTetra;
}

void QMeshFace::SetRightTetra( QMeshTetra * _pRightTetra )
{
	pRightTetra = _pRightTetra;
}