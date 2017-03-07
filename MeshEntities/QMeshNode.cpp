// QMeshNode.cpp: implementation of the QMeshNode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <math.h>
#include "QMeshPatch.h"
#include "QMeshFace.h"
#include "QMeshEdge.h"
#include "QMeshNode.h"
#include "QMeshTetra.h"
#include "QMeshHexa.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QMeshNode::QMeshNode()
{
	type=0;

	indexno=0;
	//trglFaceList.RemoveAll();
	//trglEdgeList.RemoveAll();
//	trglNodeList.RemoveAll();
//	for(UINT i=0;i<8;i++) flags[i]=FALSE;

	attachedObj=NULL;
	Pos=NULL;

	//added for node VSA
//	attachedList.RemoveAll();	
	m_nIdentifiedPatchIndex = -1;
	boundNodes = NULL;
	//merged = false;

	//for volume mesh
	inner=false;
}

QMeshNode::~QMeshNode()
{
	//trglFaceList.RemoveAll();
	//trglEdgeList.RemoveAll();
	//trglNodeList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

BOOL QMeshNode::GetAttribFlag( const int whichBit )
{
	//return flags[whichBit];
	return 1;
}

void QMeshNode::SetAttribFlag( const int whichBit, const BOOL toBe )
{
	//flags[whichBit] = toBe;
}

UINT QMeshNode::GetIndexNo()
{
	return indexno;
}

void QMeshNode::SetIndexNo( const UINT _index )
{
	indexno=_index;
}
//void QMeshNode::GetCoord2D( double &x, double &y )
//{
//	x=coord2D[0];	y=coord2D[1];
//}

void QMeshNode::GetCoord3D(double &x, double &y, double &z)
{
	x=coord3D[0];	y=coord3D[1];	z=coord3D[2];
}

//void QMeshNode::GetCoord3D_last(double &x, double &y, double &z)
//{
//	x=coord3D_last[0];	y=coord3D_last[1];	z=coord3D_last[2];
//}

//void QMeshNode::SetCoord2D( double x, double y )
//{
//	coord2D[0]=x;	coord2D[1]=y;
//}

void QMeshNode::SetCoord3D( double x, double y, double z)
{
	coord3D[0]=x;	coord3D[1]=y;	coord3D[2]=z;
}

//void QMeshNode::SetCoord3D_last(double x, double y, double z)
//{
//	coord3D_last[0]=x;	coord3D_last[1]=y;	coord3D_last[2]=z;
//}


void QMeshNode::CalNormal()
{
	double nx,ny,nz,tt;
	nx=0.0;	ny=0.0;	nz=0.0;

	GLKPOSITION Pos;
	for(Pos=trglFaceList.GetHeadPosition();Pos!=NULL;)
	{
		double a,b,c,d;

		QMeshFace *temp=
			(QMeshFace *)(trglFaceList.GetNext(Pos));
		if (temp->inner) continue;
		temp->GetPlaneEquation(a,b,c,d);
		nx+=a;	ny+=b;	nz+=c;
	}
	tt=nx*nx+ny*ny+nz*nz;
	tt=sqrt(tt);

	normal[0]=(double)(nx/tt);	normal[1]=(double)(ny/tt);	normal[2]=(double)(nz/tt);
}

void QMeshNode::GetNormal(double &nx, double &ny, double &nz)
{
	nx=normal[0];	ny=normal[1];	nz=normal[2];
}

void QMeshNode::SetNormal(double nx, double ny, double nz)
{
	normal[0]=nx;	normal[1]=ny;	normal[2]=nz;
}

void QMeshNode::SetMeshSurfacePtr(QMeshPatch* _mesh)
{
	meshSurface=_mesh;
}

QMeshPatch* QMeshNode::GetMeshSurfacePtr()
{
	return meshSurface;
}

//void QMeshNode::AddSewNode(QMeshNode *trglNode)
//{
//	trglNodeList.AddTail(trglNode);
//}
//
//int QMeshNode::GetSewNodeNumber()
//{
//	return trglNodeList.GetCount();
//}
//
//QMeshNode* QMeshNode::GetSewNodeRecordPtr(int No)	//from 1 to n
//{
//	if( (No < 1) || (No > trglNodeList.GetCount()))    return  NULL;
//    return (QMeshNode *)trglNodeList.GetAt(trglNodeList.FindIndex(No-1));
//}
//
//GLKObList& QMeshNode::GetSewNodeList()
//{
//	return trglNodeList;
//}

void QMeshNode::AddEdge(QMeshEdge *trglEdge)
{
	trglEdgeList.AddTail(trglEdge);
}

int QMeshNode::GetTrglEdgeNumber()
{
	return trglEdgeList.GetCount();
}

QMeshEdge* QMeshNode::GetTrglEdgeRecordPtr(int No)  //from 1 to n
{
	if( (No < 1) || (No > trglEdgeList.GetCount()))    return  NULL;
    return (QMeshEdge *)trglEdgeList.GetAt(trglEdgeList.FindIndex(No-1));
}

GLKObList& QMeshNode::GetTrglEdgeList()
{
	return trglEdgeList;
}

void QMeshNode::ClearUpFaceConnection(QMeshPatch *meshsurface)
{
	trglFaceList.RemoveAll();
	GLKPOSITION Pos;
	for(Pos=(meshsurface->GetTrglFaceList()).GetHeadPosition();Pos!=NULL;)
	{
		QMeshFace *tempFace=
			(QMeshFace *)((meshsurface->GetTrglFaceList()).GetNext(Pos));

		if ( ((tempFace->GetNodeRecordPtr(1))==this) 
			|| ((tempFace->GetNodeRecordPtr(2))==this)
			|| ((tempFace->GetNodeRecordPtr(3))==this) )
			trglFaceList.AddTail(tempFace);
	}
}

void QMeshNode::AddFace(QMeshFace *trglFace)
{
	trglFaceList.AddTail(trglFace);
}

int QMeshNode::GetTrglFaceNumber()
{
	return trglFaceList.GetCount();
}

QMeshFace* QMeshNode::GetTrglFaceRecordPtr(int No) //from 1 to n
{
	if( (No < 1) || (No > trglFaceList.GetCount()))    return  NULL;
    return (QMeshFace *)trglFaceList.GetAt(trglFaceList.FindIndex(No-1));
}

GLKObList& QMeshNode::GetTrglFaceList()
{
	return trglFaceList;
}


void QMeshNode::ClearUpTetraConnection(QMeshPatch *meshsurface)
{
	trglTetraList.RemoveAll();
	GLKPOSITION Pos;
	for(Pos=(meshsurface->GetTrglTetraList()).GetHeadPosition();Pos!=NULL;)
	{
		QMeshTetra *tempTetra=
			(QMeshTetra *)((meshsurface->GetTrglTetraList()).GetNext(Pos));

		if ( ((tempTetra->GetNodeRecordPtr(1))==this) 
			|| ((tempTetra->GetNodeRecordPtr(2))==this)
			|| ((tempTetra->GetNodeRecordPtr(3))==this)
			|| ((tempTetra->GetNodeRecordPtr(4))==this) )
			trglTetraList.AddTail(tempTetra);
	}
}

void QMeshNode::AddTetra(QMeshTetra *trglTetra)
{
	trglTetraList.AddTail(trglTetra);
}

int QMeshNode::GetTrglTetraNumber()
{
	return trglTetraList.GetCount();
}

QMeshTetra* QMeshNode::GetTrglTetraRecordPtr(int No) //from 1 to n
{
	if( (No < 1) || (No > trglTetraList.GetCount()))    return  NULL;
    return (QMeshTetra *)trglTetraList.GetAt(trglTetraList.FindIndex(No-1));
}

GLKObList& QMeshNode::GetTrglTetraList()
{
	return trglTetraList;
}

void QMeshNode::AddHexa(QMeshHexa *trglHexa)
{
	trglHexaList.AddTail(trglHexa);
}

int QMeshNode::GetTrglHexaNumber()
{
	return trglHexaList.GetCount();
}

QMeshHexa* QMeshNode::GetTrglHexaRecordPtr(int No) //from 1 to n
{
	if( (No < 1) || (No > trglHexaList.GetCount()))    return  NULL;
    return (QMeshHexa *)trglHexaList.GetAt(trglHexaList.FindIndex(No-1));
}

GLKObList& QMeshNode::GetTrglHexaList()
{
	return trglHexaList;
}