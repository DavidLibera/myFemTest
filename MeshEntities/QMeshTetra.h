///////////////////////////////////////////////////////////////
////// written by ToM
///////////////////////////////////////////////////////////////
#pragma once

#ifndef DEF_TRGLTETRA
#define DEF_TRGLTETRA

#include "../GLKLib/GLKObList.h"

class QMeshEdge;
class QMeshPatch;
class QMeshNode;
class QMeshFace;

class QMeshTetra :
	public GLKObject
{
public:
	QMeshTetra(void);
	~QMeshTetra(void);

//           1
//			/\
//		 e1/ |\e4
//		  /e3| \
//		 /_e5___\
//		2 \  |  / 4
//		 e2 \|/ e6
//           3
//	in anti-clockwise order
//  face1: 1,2,3
//	face2: 2,4,3
//	face3: 3,4,1
//	face4: 4,2,1


	bool selected;
	bool inner; //for cross section view
	int m_nIdentifiedPatchIndex;

	GLKPOSITION Pos;

private:
	UINT indexno;
	QMeshPatch *meshSurface;	// MESHSURFACE contain this triangle
	QMeshFace * faces[4];		//	4 faces
	bool isNormal[4];			// is normail direction

	double volume;

	GLKObList attachedList;

	//BOOL flags[8];

public:
	UINT GetIndexNo();		//from 1 to n
	void SetIndexNo( const UINT _index = 1 );

	void SetMeshSurfacePtr(QMeshPatch* _mesh);
	QMeshPatch* GetMeshSurfacePtr();

	QMeshFace * GetFaceRecordPtr( const UINT whichFace );
	void SetFaceRecordPtr( const UINT whichFace, QMeshFace * _face = NULL );
	int GetFaceIndex(QMeshFace* face);

	QMeshEdge * GetEdgeRecordPtr( const UINT whichEdge );
	int GetEdgeIndex(QMeshEdge* edge);
		
	void GetNodePos( const UINT whichNode, double &xx, double &yy, double &zz);
	QMeshNode * GetNodeRecordPtr( UINT whichNode );
	int GetNodeIndex(QMeshNode* node);

	BOOL IsNormalDirection( const UINT whichFace );
	void SetDirectionFlag( const UINT whichFace, const BOOL toBe = TRUE );

	//BOOL GetAttribFlag( const int whichBit );
	//void SetAttribFlag( const int whichBit, const BOOL toBe = TRUE );

	GLKObList& GetAttachedList() {return attachedList;};

	void CalCenterPos(double &xx, double &yy, double &zz);

	double CalVolume(double t[][3]);
	double CalVolume();
	double GetVolume();

	double CalSolidAngle(const int whichNode);
	double CalSolidAngle(double p[3][3], double pp[3]);

	bool CalTetraBarycentry(double p[3], double t[4][3], double &_p, double &_q, double &_r, double &_s);
	bool CalTetraBarycentry(double p[3], double &_p, double &_q, double &_r, double &_s); //find barycentry in this tetra

	void BarycentryToPosition(double _p, double _q, double _r, double _s, double t[4][3], double p[3]);
	void BarycentryToPosition(double _p, double _q, double _r, double _s, double p[3]);


	//int nodeindex[4];
};

#endif 