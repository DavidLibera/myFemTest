// QMeshNode.h: interface for the QMeshNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CW_QMESHNODE
#define _CW_QMESHNODE

//#define num_of_gp 70

#include "../GLKLib/GLKObList.h"

class QMeshPatch;
class QMeshFace;
class QMeshEdge;
class QMeshTetra;
class QMeshHexa;

//struct Stack_for_Path{
//	int index;
//	Stack_for_Path* next;
//	Stack_for_Path* prev;
//};
//typedef Stack_for_Path* SfP;



class QMeshNode : public GLKObject
{
public:
	QMeshNode();
	virtual ~QMeshNode();

	UINT GetIndexNo();		//from 1 to n
	void SetIndexNo( const UINT _index = 1 );

	BOOL GetAttribFlag( const int whichBit );
	void SetAttribFlag( const int whichBit, const BOOL toBe = TRUE );

	void AddFace(QMeshFace *trglFace);
	int GetTrglFaceNumber();
	QMeshFace* GetTrglFaceRecordPtr(int No);	//from 1 to n
    GLKObList& GetTrglFaceList();
	void ClearUpFaceConnection(QMeshPatch *meshsurface);

	void AddEdge(QMeshEdge *trglEdge);
	int GetTrglEdgeNumber();
	QMeshEdge* GetTrglEdgeRecordPtr(int No);	//from 1 to n
    GLKObList& GetTrglEdgeList();
	void ClearUpEdgeConnection(QMeshPatch *meshsurface);

	void AddTetra(QMeshTetra *trglTetra);
	int GetTrglTetraNumber();
	QMeshTetra* GetTrglTetraRecordPtr(int No);	//from 1 to n
    GLKObList& GetTrglTetraList();
	void ClearUpTetraConnection(QMeshPatch *meshsurface);

	void AddHexa(QMeshHexa *trglHexa);
	int GetTrglHexaNumber();
	QMeshHexa* GetTrglHexaRecordPtr(int No);	//from 1 to n
    GLKObList& GetTrglHexaList();
	void ClearUpHexaConnection(QMeshPatch *meshsurface);

	//void GetCoord2D( double &x, double &y );
	//void SetCoord2D( double x, double y );

	void GetCoord3D( double &x, double &y, double &z );
	void SetCoord3D( double x, double y, double z );

	//void GetCoord3D_last( double &x, double &y, double &z );
	//void SetCoord3D_last( double x, double y, double z );

	void CalNormal();
	void SetNormal(double nx, double ny, double nz );
	void GetNormal(double &nx, double &ny, double &nz);

	void SetMeshSurfacePtr(QMeshPatch* _mesh);
	QMeshPatch* GetMeshSurfacePtr();

	//void AddSewNode(QMeshNode *trglNode);
	//int GetSewNodeNumber();
	//QMeshNode* GetSewNodeRecordPtr(int No);	//from 1 to n
 //   GLKObList& GetSewNodeList();

	//void SetBoundaryDis(double value);
	//double GetBoundaryDis();

	//void SetCurvature(double value);
	//double GetCurvature();

	void* attachedObj;
	
	//added for node VSA
	//GLKObList attachedList;
	//GLKObList& GetAttachedList() {return attachedList;};
	int m_nIdentifiedPatchIndex;

	//added for remesh
	GLKObList* boundNodes;
	//bool isSeed;
	//bool isBound;

	//added for denoising
	//double residual;
	//bool Q;
	//bool S;
	//bool p;
	//bool noise;
	//double projectedCoord[3];

	//added for clustering
	//int nextClusterIndex;
	//bool merged;

	//for volume mesh
	bool inner;

	GLKPOSITION Pos;

private:
	//BOOL flags[8];
				// bit 0 -- TRUE for boundary points
				// bit 1 -- TRUE for points on spine
				// bit 2 -- TRUE for points on cutting edge
				// bit 3 -- TRUE for nodes on base mesh
				// bit 6 -- Temp use
				// bit 7 -- Temp use

	GLKObList trglFaceList;	// a list of triangles contained this node (TRGLFACE)
	GLKObList trglEdgeList;	// a list of edges contained this node (TRGLEDGE)
	//GLKObList trglNodeList;	// a list of sewing nodes of this node
	GLKObList trglTetraList; //a list of tetra of this node
	GLKObList trglHexaList; //a list of hexa of this node

	//double coord2D[2];
				// 2D space coordinate
	double coord3D[3];
				// 3D space coordinate
	//double coord3D_last[3];  // just for reset sewing operation for one step
                // 3D space coordinate in the last sewing step
	float normal[3];
				// Point normal on the mesh surface
	UINT  indexno;

	QMeshPatch *meshSurface;	// MESHSURFACE contain this node

	//double disToBoundary;	// by walking along the triangular edges
	//double curvature;		// this is the discrete Gausian curvature at this point
};

#endif 
