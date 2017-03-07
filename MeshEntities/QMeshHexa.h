#pragma once
#include "../GLKLib/GLKObList.h"

class QMeshEdge;
class QMeshPatch;
class QMeshNode;
class QMeshFace;


class QMeshHexa :
	public GLKObject
{
public:
	QMeshHexa(void);
	~QMeshHexa(void);

		
//     1------4
//     /|    /|
//    / |   / |
//   2--+---3 |
//    |5---+-/8
//    |/   |/
//   6------7

//	e1 = 1->2
//	e2 = 2->3
//	e3 = 3->4
//	e4 = 4->1
//	e5 = 5->8
//	e6 = 8->7
//	e7 = 7->6
//	e8 = 6->5
//	e9 = 1->5
//	e10 = 2->6
//	e11 = 3->7
//	e12 = 4->6
//
//	f1 = 1234
//	f2 = 5876
//	f3 = 1562
//	f4 = 2673
//	f5 = 3784
//	f6 = 4851

	bool inner; //for cross section view
	int m_nIdentifiedPatchIndex;
	GLKPOSITION Pos;


private:
	UINT indexno;
	QMeshPatch *meshSurface;	// MESHSURFACE contain this triangle
	QMeshFace * faces[6];		//	4 faces
	bool isNormal[6];			// is normail direction

public:
	UINT GetIndexNo(){return indexno;};		//from 1 to n
	void SetIndexNo( const UINT _index = 1 ){indexno = _index;};

	void SetMeshSurfacePtr(QMeshPatch* _mesh){meshSurface=_mesh;};
	QMeshPatch* GetMeshSurfacePtr(){return meshSurface;};

	QMeshFace * GetFaceRecordPtr( const UINT whichFace ){return faces[whichFace-1];};
	void SetFaceRecordPtr( const UINT whichFace, QMeshFace * _face = NULL ){faces[whichFace-1] = _face;};
	int GetFaceIndex(QMeshFace* face);

	//QMeshEdge * GetEdgeRecordPtr( const UINT whichEdge );
	int GetEdgeIndex(QMeshEdge* edge);
		
	void GetNodePos( const UINT whichNode, double &xx, double &yy, double &zz);
	QMeshNode * GetNodeRecordPtr( UINT whichNode );
	int GetNodeIndex(QMeshNode* node);

	BOOL IsNormalDirection( const UINT whichFace ){return isNormal[whichFace-1];};
	void SetDirectionFlag( const UINT whichFace, const BOOL toBe = TRUE ){isNormal[whichFace-1]=toBe;};
};
