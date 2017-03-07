// MeshDistanceField.h: interface for the CMeshDistanceField class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHDISTANCEFIELD_H__38A3D800_82AC_4878_80F1_ED945F663EC2__INCLUDED_)
#define AFX_MESHDISTANCEFIELD_H__38A3D800_82AC_4878_80F1_ED945F663EC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MinWndHeap.h"

//#include "sharelibsrc/Surface/surface.h"
#include "GeodesicDistanceField.h"

class LSJ_OBJECT
{
public:
	int m_id;  // 对应Mesh中的各个（Point, Edge, Triangle）数组中元素的序号
	int heap_location;  
	double m_weight; // if the primitive is edge or triangle, it is the minimal point weight
	LSJ_OBJECT()
	{
		m_id = -1; heap_location = -1; m_weight = 1.0E+7;
	}
	virtual ~LSJ_OBJECT()
	{
	}
	void operator = (LSJ_OBJECT w)
	{
		m_id = w.m_id;  heap_location = w.heap_location;  m_weight = w.m_weight;
	}
	boolean operator <= (LSJ_OBJECT w)
	{
		return m_weight<=w.m_weight;
	}
	boolean operator == (LSJ_OBJECT w)
	{
		if(fabs(m_weight - w.m_weight) < EPSILON && (m_id == w.m_id))
			return true;
		return false;
	}	
};

class EDGE_OBJECT : public LSJ_OBJECT
{
public:
	double m_weight1;

	double m_ds;
	
	Point2D m_pseudoV;	
	double m_b0, m_b1;

	Point2D m_fv[2];  // 边所在面对应三角形第三个顶点在该三角形所在平面的二维坐标
	int m_tao;// 0--1 生成这条边用过的三角面片是该边相连三角形序号数组的序号

	EDGE_OBJECT(){
		m_weight1 = 1.0E+7;	m_ds = 0.0;	m_pseudoV.assign(0.0,0.0);
		m_b0 = m_b1 = 0.0; m_tao = -1; m_fv[0].assign(0.0,0.0); 
		m_fv[1].assign(0.0,0.0);
	}
	virtual ~EDGE_OBJECT(){}
	void operator = (EDGE_OBJECT w)
	{
		m_id = w.m_id;  heap_location = w.heap_location;  m_weight = w.m_weight;
		m_weight1 = w.m_weight1;

		m_ds = w.m_ds; m_b0 = w.m_b0; m_b1 = w.m_b1; m_pseudoV.assign(w.m_pseudoV);
		m_tao = w.m_tao;
	}
	boolean operator <= (EDGE_OBJECT w)
	{
		double minV1,minV2;
		minV1 = (m_weight > m_weight1 ? (m_ds+m_weight1) : (m_ds+m_weight));
		minV2 = (w.m_weight > w.m_weight1 ? (w.m_ds+w.m_weight1) : (w.m_ds+w.m_weight));
		return minV1<=minV2;
	}
	boolean operator == (EDGE_OBJECT w)
	{
		if(fabs(m_ds -w.m_ds) < EPSILON && fabs(m_weight - w.m_weight) < EPSILON && 
			fabs(m_weight1 - w.m_weight1) < EPSILON	&& (m_id == w.m_id));
			return true;
		return false;
	}	
};

class CMeshDistanceField  
{
public:
	CMeshDistanceField();
	virtual ~CMeshDistanceField();

private:
	Surface *m_Mesh;
	Array<SOURCE> *m_Source;
	Array<int> m_SourcePoints;
	int m_SourceIndex;
	Array<double> m_VertexWeight;  // 测地距离计算结果


public:
	void ApproximatingMapGeneration2();
	void ExtendedAdaptiveMapGeneration2();
//	void EdgeBasedApproximatingMapGeneration();
//	void VertexBasedApproximatingMapGeneration();
	void ApproximatingMapGeneration();
	void EuclideanDistanceMapGeneration();
	void thirdPointFrom3DTo2d(int eID, int fID, int pID, Point2D *p);
	void ExactMapGeneration();
	void initialQueue(MinObjHeap<EDGE_OBJECT> *eQ);
	void initialQueue(MinObjHeap<LSJ_OBJECT> *vQ, int cls = 1);
	Array<double> * getDistanceField(); // 获取测地距离计算结果
	Surface * getMeshModel();
	void TransformSourceToPointArray(Array<SOURCE> *src, int sid = -1);
	void ExtendedAdaptiveMapGeneration();
	void AdaptiveMapGeneration();
	void MapGeneration();
	CMeshDistanceField(Surface *surf, Array<SOURCE> *src, int sid = -1);
	// When all vertices are processed and the array is updated, 
	// the distance field on the mesh is established.
	
protected:
	void TransformSourceToPointArray(SOURCE *src, Array<bool> *pf);
private:
	void propagate(int idx, int eID, MinObjHeap<LSJ_OBJECT> *eQ, int flag = 1);  // idx is the adjacent triangle index of edge's f[]
	// flag = 0 (based on vertex) ; 1 (based on edge);
};

#endif // !defined(AFX_MESHDISTANCEFIELD_H__38A3D800_82AC_4878_80F1_ED945F663EC2__INCLUDED_)
