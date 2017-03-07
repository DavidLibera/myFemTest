/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR 
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE 
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF 
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED 
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY 
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/

/*

    Surface representation consisting of triangles

      #define USE_RHINO_IO      tells the code to support loading of Rhino .3DM files
*/
#ifndef _SURFACE_H_
#define _SURFACE_H_

//#define USE_RHINO_IO

#ifdef USE_RHINO_IO
#include "c:/devel/inc/RhinoIO/RhinoIO.h"
#pragma comment( lib, "RhinoIO_vc60" )
#endif

#include "../Storage/Array.h"
#include "../Geometry/Point3D.h"
#include "../Geometry/Point2D.h"
#include "../Geometry/Vector3D.h"
#include "../Geometry/Sphere.h"


#define BLOCK_NUM 10

class Surface{
public:
	struct Point
	{
		public:
			Point3D p;      //  point in 3D space
			Vector3D n;     //  surface normal
			Array<int> fids;
			Array<int> eids;
			int fNum,eNum; 

			double curvaValue;
			
			int vIndex;  // the point index in a point array
//			bool isInTile;
			bool isVisible;  // if visible or not
			double distToStroke;  // the distance to the stroke

			double weight;  // 

		__inline void addFID(int fid)
		{
			if(fNum>=fids.getSize())
			{
				fids.resize(fNum+BLOCK_NUM);
			}
			fids.index(fNum) = fid;
			fNum++;
		}
		__inline void addEID(int eid)
		{
			if(eNum>=eids.getSize())
			{
				eids.resize(eNum+BLOCK_NUM);
			}
			eids.index(eNum) = eid;
			eNum++;
		}
	};

	struct VERTEX_EXTRA_PROPERTY
	{
	public:
		Array<int> f_e3_ids;  //每个顶点相连的每个三角形中与顶点相对的那条边的序号，个数与三角形的个数相同
		Array<int> f_v_id; // 与f_e3_ids[i]边相连的另外一个三角形中所对顶点的序号
		Array<double> f_v_distance; //该点与f_v_id[i]之间的测地距离
	};


	struct MESH_EDGE
	{
		public:
			int v[2], f[2];
			double cost;
			double length;
			int eIndex;		 // the index of the edge in an array
//			bool isInTile;   // is in the tile
			int isVisible;	// if visible or not
	};
	
	struct EDGE_EXTRA_PROPERTY
	{
	public:
		Point2D f_v3_2D[2];  // the third vertex's 2D coordinate in triangle f[i];		
		int f_v3_id[2];

		int f_v_eid[2][2];	// 在三角形f[i]中与该边的端点v[j]相邻的边序号f_v_eid[i][j]
	};

	struct Triangle
	{
		public:
		  int v[3],e[3],f[3];
		  Vector3D n;     //  surface normal

		  int fIndex;		// the index of the face in an array
//		  bool isInTile;  // is in the tile
		  bool isVisible;	// if visible or not
	};
private:
	int isInEdges(MESH_EDGE *e);
public:
	bool setupExtraProperty(int flag = 3);
	bool transform3Dto2D(int triID, int eID, double tv1, double tv2, int vtID, Point2D *vt);
	int findEdge(int id1,int id2, int flag);
	Surface(void);
	~Surface(void);

	void outputMeshInfo(int pID, int pType);
	void initialStoreSpace();
	void setupMeshGeoInfo();
	int findNearestVertex(Point3D &v);
	

    //  bounding box
    Point3D pMin, pMax;

    //  elements
    Array<Point> vertices;
    Array<Triangle> triangles;
	Array<MESH_EDGE> edges;

	Array<VERTEX_EXTRA_PROPERTY> v_ex_properties;
	Array<EDGE_EXTRA_PROPERTY> e_ex_properties;
	
#ifdef USE_RHINO_IO
    //  loading from Rhino file
    bool loadMesh(const CRhinoMesh &mesh);
#endif
    bool loadRhinoSurface(const char *fileName, char **err, float boxSize = -1);

    //  fitting into unit box
    float fitIntoBox(float boxSize = 1000);

    //  our own format with edge adjacency
    bool loadSurface(const char *fileName, float boxSize = -1);
    bool loadMinimalSurface(const char *fileName, float boxSize = -1);
    bool saveSurface(const char *fileName, float scale = 1);
    bool saveHavokSurface(const char *fileName);

    //  normals
    void getTriangleNormal(Vector3D *n, int tri) const;
    void getVertexNormal(Vector3D *n, int vertex) const;
    void getNormal(Vector3D *n, const Point3D &p, int tri) const;
    void getEdgeNormal(Vector3D *n, int tri, int edge) const;

    //  neighbours of a point
    void findNeighbours(Array<int> *tris, int vertex, int startTri = -1) const;

    //  bounding sphere by Ritter's algorithm
    void getBoundingSphere(Sphere *sph) const;

    //  create copy
    void copy(const Surface &other);

    //  tester
    void testSurface(int *badVerts, int *badNeighs, int *openEdges, int *multEdges, int *badFaces) const;

    //  mass properties
    void computeMassProperties(REAL *mass, REAL com[3], REAL it[3][3]) const;

    //  compute bounding box 
    void setupBoundingBox();

  //protected:
    //  compute normals of faces
    void setupNormals(int mn, int mx, int tn, int tx);

    //  joining triangles
    void setupAdjacent(int mn, int mx);
    int findTriangle(int tri, int v1, int v2, int mn, int mx) const;
    void stitchTriangles();
    void remapVertex(int s, int d);
    bool vertexHasRef(int s);
    static bool hasEdge(const Triangle *tri, int e[2]);
    static bool hasVertex(const Triangle *tri, int v);
};

#endif
