// GeodesicDistanceField.h: interface for the GeodesicDistanceField class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEODESICDISTANCEFIELD_H__A96FC67E_B9DC_4F22_87A8_98DC643EC150__INCLUDED_)
#define AFX_GEODESICDISTANCEFIELD_H__A96FC67E_B9DC_4F22_87A8_98DC643EC150__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MinWndHeap.h"

//#include "ShareLibSrc/Base/Types.h"
#include "ShareLibSrc/Geometry/Point3D.h"
#include "ShareLibSrc/Geometry/Point2D.h"
#include "ShareLibSrc/Storage/Array.h"
#include "ShareLibSrc/Storage/minheap.h"
#include "ShareLibSrc/Storage/Queue.h"
//#include "ShareLibSrc/Surface/Surface.h"

enum SORTORDER {UPSORT, DOWNSORT};
#define MIN_EPSILON 1.0E-10
#define MAX_EPSILON 1.0E-5

class Window // 
{
public:
	void updateWindow(double vb, int flag);
	boolean mergeWindow(Window *w);
	boolean dividePoint(double *px, Window &w, double vb0, double vb1);
	boolean addressOverlapWindows(Window *w);
	boolean isIntersection(Window &w);
	double b0,b1; //define the endpoints of w (parametric coordinates)[0,|e|].
	double d0,d1; //the corresponding distances to the pseudosource s.
	double ds;  // D(s) is the geodesic distance from s to the source vs.
	int tao;  // encodes the direction of s from the directed edge.
	int edgeID; // belonging to the edge's ID
	Point2D pseudosource;
	int heap_location; //记录在最小堆中的位置
	int edge_location;
	Window()
	{
		b0 = b1 = d0 = d1 = ds = 0.0; tao = -1; edgeID = -1;
		heap_location = -1; edge_location = -1; pseudosource.assign(0.0,0.0);
	}
	virtual ~Window(){}
	void operator = (Window w)
	{
		b0 = w.b0; b1 = w.b1; d0 = w.d0; d1 = w.d1;
		ds = w.ds; tao = w.tao; edgeID = w.edgeID;
		pseudosource.assign(w.pseudosource);
	}
	boolean operator <= (Window w)
	{
		double minD,minDw;
		minD = d0 < d1 ? (d0+ds) : (d1+ds);
		minDw = w.d0 < w.d1 ? (w.d0+w.ds) : (w.d1+w.ds);
		return minD<=minDw;
	}
	boolean operator == (Window w)
	{
		if(fabs(b0 - w.b0)<EPSILON && fabs(b1 - w.b1)<EPSILON && fabs(d0-w.d0)<EPSILON 
			&& fabs(d1 - w.d1)<EPSILON && fabs(ds -w.ds)<EPSILON  
			&& edgeID == w.edgeID)//&& tao == w.tao
			return true;
		return false;
	}
protected:

private:
};

class Edge
{
public:
	Edge()
	{ 
		p1 = p2 = f1 = f2 = f1e2id = f1e3id = f2e2id = f2e3id = f1pid = f2pid = -1; 
		len = -1.0;	wndNum = 0;windows.resize(20);
	}
	virtual ~Edge(){}

	bool deleteWindow(int i)
	{
		if(i<wndNum && i>=0)
		{
			wndNum--;
			if(i<wndNum) //i在wndNum之前
			{
				windows.index(i)= windows.index(wndNum);
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	void insertWindow(int winID)
	{
		int maxWinNum = windows.getSize();
		if(wndNum>=maxWinNum)
		{
			windows.resize(2*maxWinNum);
		}
//		OUTPUTINFO("Edge insertWindow id=%d\n",winID);
		windows.index(wndNum) = winID;
		wndNum++;
	}

	int p1,p2; // p1,p2's index in vertices
	int f1,f2; // f1,f2's index in triangles
//	int o1,o2; // the order of p1,p2 in triangles. o1->f1; o2->f2. {o1,o2} = {0,1}.
//				// Eg: if the order is p1p2 in triangle f1, then o1=0,o2=1; 
//				// if the order is p2p1, o1 = 1,o2=0; 
	double len;  // length of the edge
//	Array<Window> windows;
	Array<int> windows;
	int wndNum;
	// for speed
	int f1pid, f2pid;
	Point2D f1p,f2p;  //the third points of triangles f1 and f2 in 2D
	int f1e2id, f1e3id, f2e2id, f2e3id;
};

//double edgeid[3];
struct EDGEID
{
	int e[3];
};

struct FIDPERVERTEX
{
	Array<int> fids;
	int fNum;
	__inline void addSurfItem(int fID)
	{
		if(fNum>=fids.getSize())
		{
			fids.resize(2*fNum);
		}
		fids.index(fNum) = fID;
		fNum++;
	}

};

/********************************************************************************


						The SOURCE Class


*********************************************************************************/
enum SOURCE_TYPE {S_POINT, S_LINE, S_FACE, S_NONE};

//	定义 源 的类
/*
// if type == S_POINT There is only one element----- the point id
// if type == S_LINE then the elements of the array are the points' id on the line
// if type == S_FACE then the elements of the array are the triangles' id in the face
*/
class SOURCE
{
public:
	void display(Surface *pSurf);
	Array<int> s_index;   // array
	SOURCE_TYPE type;	
};

//--------------------------------------------------------------------------------


class GeodesicDistanceField  
{
public:
	void initQueue(int sID);
	void getFieldValueVertex();
	void setupInfoForEdge(Edge &e, int eID, int fID, int *pID, int *e1ID, int *e2ID, Point2D *p);
	void thirdPointFrom3DTo2D(Edge &e, int fID, int pID, Point2D *p);
	void showDebugInfo();
	void deleteWindow(int wid, int flag=-1);
	Window & addWindow();
	void divideWindow(Window *w1, Window *w2, Point2D &s1, Point2D &s2);
	void addressOverlapWindows(Window *w1,Window *w2, Window *w3, Window *w4,int *flag);
	boolean createAndModifyWindow(Window &oldWnd, int eID, int fID, 
		int p0ID, Point2D p0 , Point2D &pb0, Point2D &pb1, Point2D &ps, double dsVal); // dsVal --- 过鞍点的window的源点到鞍点的距离
	boolean createWindow(Window *newWnd, Window &oldWnd, int eID, int fID, int p0ID,
		Point2D &p0,Point2D &pb0, Point2D &pb1, Point2D &ps, double dsVal); // 同上
	void createPotentialWindow(Window &oldWin, Point2D &p0, Point2D &p1, 
		Point2D &p2, int p2ID, int eID2, int eID3, int fID);
	void solveOverlapWindows(Edge *e,Window *w);
	void solveOverlapWindows(int eID,Window *w, int startID = 0);
	void solveOverlapWindows(Window *w);
	void propagation();
	void initializeQueue();

	int isInEdges(Edge *e);  // return value declare if the edge e is in array edges and 
							// the direction
	int isInEdges(Edge &e, Array<Edge> &eArray);
	void setupEdge();
	Point3D source;
	int sourceID;  Array<int> sIDs;
	Surface *surface;
	Array<Edge> edges;
	int edgeNum;
	
	Array<EDGEID> tri_edgeids;
	
	Array<FIDPERVERTEX> ver_fids;  // 每个顶点相连的三角片的序号

	Array<FIDPERVERTEX> ver_eids;  // 每个顶点相连的边的序号

	int steps,max_steps;  // for debug

//	MinHeap<Window> *winQueue;
	MinObjHeap<Window> *winQueue;

	Array<Window> windows; //Window 集合
	int wndNum;  //window个数
	
	// The distance of each vertex
	Array<double> distField;

	GeodesicDistanceField();
	GeodesicDistanceField(Surface &surf, int pID);
	GeodesicDistanceField(Surface &surf, Point3D p);
	GeodesicDistanceField(Surface &surf, SOURCE s);
	virtual ~GeodesicDistanceField();

	
};

#endif // !defined(AFX_GEODESICDISTANCEFIELD_H__A96FC67E_B9DC_4F22_87A8_98DC643EC150__INCLUDED_)
