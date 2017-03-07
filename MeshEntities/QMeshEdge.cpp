// QMeshEdge.cpp: implementation of the QMeshEdge class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <math.h>
#include "QMeshEdge.h"
#include "QMeshFace.h"
#include "QMeshNode.h"
#include "CH_ATTRIB_EDGENODE.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QMeshEdge::QMeshEdge()
{
	type=1;
	indexno=0; 
	//edgeLength=-1;
	pLeftFace=NULL;	pRightFace=NULL;
//	for(UINT i=0;i<8;i++) flags[i]=FALSE;

//	AttachList.RemoveAll();
	Pos=NULL;
	
	//for volume mesh
	GetTrglFaceList().RemoveAll();
	inner=false;
}

QMeshEdge::~QMeshEdge()
{
	//AttachList.RemoveAll();
	GetTrglFaceList().RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
BOOL QMeshEdge::GetAttribFlag( const int whichBit )
{
	return 0;
	//return flags[whichBit];
}

void QMeshEdge::SetAttribFlag( const int whichBit, const BOOL toBe )
{
	//flags[whichBit] = toBe;
}

UINT QMeshEdge::GetIndexNo()
{
	return indexno;
}

void QMeshEdge::SetIndexNo( const UINT _index )
{
	indexno = _index;
}

BOOL QMeshEdge::IsBoundaryEdge()
{
	if ((pLeftFace==NULL) || (pRightFace==NULL))
		return TRUE;
	return FALSE;
}

QMeshNode * QMeshEdge::GetStartPoint()
{
	return pStartPoint;
}

void QMeshEdge::SetStartPoint( QMeshNode * _pStartPoint )
{
	pStartPoint = _pStartPoint;
}

QMeshNode * QMeshEdge::GetEndPoint()
{
	return pEndPoint;
}

void QMeshEdge::SetEndPoint( QMeshNode * _pEndPoint )
{
	pEndPoint = _pEndPoint;
}

QMeshFace * QMeshEdge::GetLeftFace()
{
	return pLeftFace;
}

void QMeshEdge::SetLeftFace( QMeshFace * _pLeftFace )
{
	pLeftFace = _pLeftFace;
}

QMeshFace * QMeshEdge::GetRightFace()
{
	return pRightFace;
}

void QMeshEdge::SetRightFace( QMeshFace * _pRightFace )
{
	pRightFace = _pRightFace;
}

void QMeshEdge::SetMeshSurfacePtr(QMeshPatch* _mesh)
{
	meshSurface=_mesh;
}

QMeshPatch* QMeshEdge::GetMeshSurfacePtr()
{
	return meshSurface;
}

double QMeshEdge::GetLength()
{
	double x1,y1,z1,x2,y2,z2;

	pStartPoint->GetCoord3D(x1,y1,z1);	pEndPoint->GetCoord3D(x2,y2,z2);
	double edgeLength=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));
	return edgeLength;
}

//double QMeshEdge::GetLength()
//{
//	if (edgeLength<0) CalLength();
//	return edgeLength;
//}

GLKObList& QMeshEdge::GetTrglFaceList()
{
	return trglFaceList;
}