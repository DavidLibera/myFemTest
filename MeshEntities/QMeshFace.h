// QMeshFace.h: interface for the QMeshFace class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DEF_TRGLFACE
#define DEF_TRGLFACE

#include "../GLKLib/GLKObList.h"
#define MAX_EDGE_NUM	4
class QMeshEdge;
class QMeshPatch;
class QMeshNode;
class QMeshTetra;
class QMeshHexa;

class QMeshFace : public GLKObject
{
public:
	QMeshFace();
	virtual ~QMeshFace();

public:
	BOOL GetAttribFlag( const int whichBit );
	void SetAttribFlag( const int whichBit, const BOOL toBe = TRUE );

	UINT GetIndexNo();		//from 1 to n
	void SetIndexNo( const UINT _index = 1 );

	int GetEdgeNum();
	void SetEdgeNum(int num);

	BOOL IsNormalDirection( const UINT whichEdge );
	void SetDirectionFlag( const UINT whichEdge, const BOOL toBe = TRUE );

	QMeshEdge * GetEdgeRecordPtr( const UINT whichEdge );
	void SetEdgeRecordPtr( const UINT whichEdge, QMeshEdge * _edge = NULL );
		
	void GetNodePos( const UINT whichNode, double &xx, double &yy, double &zz);
	QMeshNode * GetNodeRecordPtr( const UINT whichNode );

	void GetPlaneEquation( double & A, double & B, double & C, double & D ); 
	//void CalPlaneEquation( double & A, double & B, double & C, double & D ); 
	void CalPlaneEquation( ){return;}; 
						// to calculate the plane equation parameter
	                    // Plane equation:  Ax + By + Cz + D = 0, and
                        // Vector(A,B,C) is positive unit normal vector of this trangle plane

	void SetMeshSurfacePtr(QMeshPatch* _mesh);
	QMeshPatch* GetMeshSurfacePtr();

	GLKObList attachedList;

	//added for VSA
	int m_nIdentifiedPatchIndex;
	GLKObList& GetAttachedList() {return attachedList;};	
	void CalCenterPos(double &xx, double &yy, double &zz);
	void CalCenterPos(){};

	void CalBoundingBox(double &xmin, double &ymin, double &zmin,
						double &xmax, double &ymax, double &zmax);
	double CalArea();
	double GetArea() {return m_area;};

	double m_area;

	//private b4
	//double  abcd[4];	// plane equation

	bool selected;

	GLKPOSITION Pos;


private:
	UINT indexno;
	BOOL flags[4];
		// bit 0 -- TRUE for 1st edge normal direction
		//			FALSE for 1st edge inverse direction
		// bit 1 -- TRUE for 2nd edge normal direction
		//			FALSE for 2nd edge inverse direction
		// bit 2 -- TRUE for 3rd edge normal direction
		//			FALSE for 3rd edge inverse direction
	    // bit 3 -- TRUE for 4th edge normal direction
		//			FALSE for 4tj edge inverse direction


		// 1st, 2nd, 3rd edges construct an anti-clockwise closed loop
                                      //**********************************
                                      //             point2              *
                                      //              /\                 *
                                      //  2nd edge  /    \ _             *
                                      //         |_       |\ 1st edge    *
                                      //        /            \           *
                                      //      /                \         *
                                      //  point3 ------>-------- point1  *
	                                  //              3rd edge           *
	                                  //**********************************
	QMeshEdge * edges[MAX_EDGE_NUM];		//	3 edges

	QMeshPatch *meshSurface;	// MESHSURFACE contain this triangle	
	
	int edgeNum;

	//for volume mesh
	QMeshTetra *pLeftTetra;
	QMeshTetra *pRightTetra;

	//for hexahedral mesh
	QMeshHexa *pLeftHexa;
	QMeshHexa *pRightHexa;



public:
	//for volume mesh
	bool inner, i_inner;

	QMeshTetra * GetLeftTetra();
	void SetLeftTetra( QMeshTetra * _pLeftTetra = NULL );
	QMeshTetra * GetRightTetra();
	void SetRightTetra( QMeshTetra * _pRightTetra = NULL );


	QMeshHexa * GetLeftHexa(){return pLeftHexa;};
	void SetLeftHexa( QMeshHexa * _pLeftHexa = NULL ){pLeftHexa=_pLeftHexa;};
	QMeshHexa * GetRightHexa(){return pRightHexa;};
	void SetRightHexa( QMeshHexa * _pRightHexa = NULL ){pRightHexa=_pRightHexa;};


};

#endif 
