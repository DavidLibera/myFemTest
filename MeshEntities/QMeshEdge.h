// QMeshEdge.h: interface for the QMeshEdge class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CW_QMESHEDGE
#define _CW_QMESHEDGE

#include "../GLKLib/GLKObList.h"

class QMeshPatch;
class QMeshNode;
class QMeshFace;

class QMeshEdge : public GLKObject
{
public:
	QMeshEdge();
	virtual ~QMeshEdge();

public:
	BOOL GetAttribFlag( const int whichBit );
	void SetAttribFlag( const int whichBit, const BOOL toBe = TRUE );

	UINT GetIndexNo();		//from 1 to n
	void SetIndexNo( const UINT _index = 1 );

	BOOL IsBoundaryEdge();

	QMeshNode * GetStartPoint();
	void SetStartPoint( QMeshNode * _pStartPoint = NULL );

	QMeshNode * GetEndPoint();
	void SetEndPoint( QMeshNode * _pEndPoint = NULL );

	QMeshFace * GetLeftFace();
	void SetLeftFace( QMeshFace * _pLeftFace = NULL );

	QMeshFace * GetRightFace();
	void SetRightFace( QMeshFace * _pRightFace = NULL );

	void SetMeshSurfacePtr(QMeshPatch* _mesh);
	QMeshPatch* GetMeshSurfacePtr();

	void CalLength(){return;};
	double GetLength();

	//GLKObList AttachList;

	GLKPOSITION Pos;
	

private:
	UINT indexno;
	//BOOL flags[8];
				// bit 0 -- TRUE for boundary edge
				// bit 1 -- TRUR for pomegranate edge
				// bit 3 -- Temp use
				// bit 7 -- Temp use

                                    //*** Edge vector definition ***
                                    //                             *
		                            //         end point           *
	QMeshNode * pStartPoint;		//           /|\               *
	QMeshNode * pEndPoint;			//            |                *
                                    //  left face | right face     *
	QMeshFace * pLeftFace;			//            |                *
	QMeshFace * pRightFace;			//            |                *
		                            //       start point           *
		                            //                             *
		                            //******************************

	QMeshPatch *meshSurface;	// MESHSURFACE contain this edge

	//double edgeLength;

	//for volume mesh
	GLKObList trglFaceList;	// a list of triangle's faces (TRGLFACE)

//for volume mesh
public:
    GLKObList& GetTrglFaceList();
	bool inner;


};

#endif 
