// MeshDistanceField.cpp: implementation of the CMeshDistanceField class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "3dshop.h"
#include "MeshDistanceField.h"
#include "ShareLibSrc/LinearAlgebra/Matrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshDistanceField::CMeshDistanceField()
{
	m_Mesh = NULL;
	m_Source = NULL;
	m_SourceIndex = -1;

	m_VertexWeight.setSize(0);
	m_SourcePoints.setSize(0);
}

CMeshDistanceField::~CMeshDistanceField()
{
	if(m_VertexWeight.getSize()>0)
		m_VertexWeight.free();
	if(m_SourcePoints.getSize()>0)
		m_SourcePoints.free();
}

CMeshDistanceField::CMeshDistanceField(Surface *surf, Array<SOURCE> *src, int sid)
{
	m_Mesh = surf;
	m_Source = src;
	m_SourceIndex = sid;
	if(sid < -1)
		m_SourceIndex = -1;	
	m_VertexWeight.setSize(0);
	m_SourcePoints.setSize(0);
}

void CMeshDistanceField::MapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);

	int vNum = m_Mesh->vertices.getSize();

	Array<bool> vertexflag;	
	vertexflag.setSize(0);
	vertexflag.resize(vNum);

	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vNum);

	int i;
	double maxValue = 1.0E+7;
	// 初始化
	for(i = 0; i<vNum; i++)
	{
		m_VertexWeight.index(i) = maxValue;
		vertexflag.index(i) = false;
	}
	
	Array<int> tempSource1,tempSource2;
	tempSource1.clone(m_SourcePoints);
	
	for(i = 0; i<m_SourcePoints.getSize();i++)
	{
		int pid = m_SourcePoints.index(i);
		m_VertexWeight.index(pid) = 0.0;
		vertexflag.index(pid) = true;
	}

	//  process
	tempSource2.setSize(0);

	float timeExp1= clock();
	
	do{
		for(i = 0; i< tempSource1.getSize();i++)
		{
			int vk_id = tempSource1.index(i);
			Surface::Point *vk = &m_Mesh->vertices.index(vk_id);
			for(int j = 0; j< vk->eNum; j++)
			{
				int ekj_id = vk->eids.index(j);
				Surface::MESH_EDGE *ekj = &m_Mesh->edges.index(ekj_id);
				int vj_id = (ekj->v[0]==vk_id ? ekj->v[1] : ekj->v[0] );
				double tempW;
				tempW = m_VertexWeight.index(vk_id) + ekj->length;
				if(tempW < m_VertexWeight.index(vj_id))
					m_VertexWeight.index(vj_id) = tempW;
				if(!vertexflag.index(vj_id))
				{
					int *vid = &tempSource2.addItem();
					*vid = vj_id;

					vertexflag.index(vj_id) = true;
				}
			}
		}
		if(tempSource1.getSize()>0) tempSource1.free();
		
		tempSource1.clone(tempSource2);
		if(tempSource2.getSize()>0) tempSource2.free();
	}while(tempSource1.getSize()>0);

	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n", timeExp1);

	if(vertexflag.getSize()>0)	vertexflag.free();
	if(tempSource1.getSize()>0)  tempSource1.free();
	if(tempSource2.getSize()>0)  tempSource2.free();
}

void CMeshDistanceField::AdaptiveMapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);

	int vNum = m_Mesh->vertices.getSize();

	Array<bool> vertexflag;	
	vertexflag.setSize(0);
	vertexflag.resize(vNum);

	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vNum);

	int i;
	double maxValue = 1.0E+7;
	// 初始化
	for(i = 0; i<vNum; i++)
	{
		m_VertexWeight.index(i) = maxValue;
		vertexflag.index(i) = false;
	}
	
	Array<int> tempSource1,tempSource2;
	tempSource1.clone(m_SourcePoints);
	
	for(i = 0; i<m_SourcePoints.getSize();i++)
	{
		int pid = m_SourcePoints.index(i);
		m_VertexWeight.index(pid) = 0.0;
		vertexflag.index(pid) = true;
	}
	
	// get the minimum edge length min_length;
	double min_length = maxValue;
	for(i = 0; i< m_Mesh->edges.getSize(); i++)
	{
		Surface::MESH_EDGE *edge = &m_Mesh->edges.index(i);
		if(edge->length < min_length)
		{
			min_length = edge->length;
		}
	}

	//  process
	tempSource2.setSize(0);
	double ramda = min_length;

	float timeExp1= clock();

	do{
		for(i = 0; i< tempSource1.getSize();i++)
		{
			int vk_id = tempSource1.index(i);
			Surface::Point *vk = &m_Mesh->vertices.index(vk_id);
			for(int j = 0; j< vk->eNum; j++)
			{
				int ekj_id = vk->eids.index(j);
				Surface::MESH_EDGE *ekj = &m_Mesh->edges.index(ekj_id);
				int vj_id = (ekj->v[0]==vk_id ? ekj->v[1] : ekj->v[0] );
				double tempW;
				tempW = m_VertexWeight.index(vk_id) + ekj->length;
				if(tempW < m_VertexWeight.index(vj_id))
					m_VertexWeight.index(vj_id) = tempW;
				if(!vertexflag.index(vj_id) && (m_VertexWeight.index(vj_id)<ramda))
				{
					int *vid = &tempSource2.addItem();
					*vid = vj_id;

					vertexflag.index(vj_id) = true;
				}
			}
		}
		for(i = 0; i< tempSource1.getSize(); i++)
		{
			int vk_id = tempSource1.index(i);
			Surface::Point *vk = &m_Mesh->vertices.index(vk_id);
			int flag = false;
			for(int j = 0; j< vk->eNum; j++)
			{
				int ekj_id = vk->eids.index(j);
				Surface::MESH_EDGE *ekj = &m_Mesh->edges.index(ekj_id);
				int vj_id = (ekj->v[0]==vk_id ? ekj->v[1] : ekj->v[0] );

				if(!vertexflag.index(vj_id))
				{
					flag = true; break;
				}
			}
			if(flag)
			{
				int *vid = &tempSource2.addItem();
				*vid = vk_id;
			}
		}
		tempSource1.clone(tempSource2);
		if(tempSource2.getSize()>0) tempSource2.free();
		
		ramda += min_length;
	}while(tempSource1.getSize()>0);
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);

	if(vertexflag.getSize()>0)	vertexflag.free();
	if(tempSource1.getSize()>0)  tempSource1.free();
	if(tempSource2.getSize()>0)  tempSource2.free();
}

void CMeshDistanceField::ExtendedAdaptiveMapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);
	int vertNum = m_Mesh->vertices.getSize();
	
	int i;
	double maxValue = 1.0E+7;
	// 初始化
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vertNum);

	MinObjHeap<LSJ_OBJECT> *vertQueue;
	Array<LSJ_OBJECT> vertArray;  // for the minimal heap ---- the primitive array
	vertArray.setSize(0);
	vertArray.resize(vertNum);
	for(i = 0; i< vertNum; i++)
	{
		vertArray.index(i).m_id = i;	
		vertArray.index(i).m_weight = maxValue;
		vertArray.index(i).heap_location = -1;

		m_VertexWeight.index(i) = maxValue;
	}
	int maxheapsize = vertNum;
	vertQueue = new MinObjHeap<LSJ_OBJECT>(maxheapsize,&vertArray);

	// 初始化最小堆
	initialQueue(vertQueue);
//	int times = 0;
	float timeExp1= clock();

	for(;;)
	{
//		times ++;
//		if(times > 5000)
//		{
//			OUTPUTINFO("times = 5000");
//			break;
//		}
		if(vertQueue->Is_Empty())
		{
//			OUTPUTINFO("winQueue is empty!\n");
			break;
		}
		// 取出堆中最小元素（即权值最小的点）

		// pick out a point
		int minVertID; 
		vertQueue->RemoveMin(&minVertID);
		LSJ_OBJECT *lsj_obj = &vertArray.index(minVertID);
		lsj_obj->heap_location = -1;
		// 得到与权值最小的点相邻的点
		Surface::Point *pt = &m_Mesh->vertices.index(lsj_obj->m_id);
		for(int j = 0; j< pt->eNum; j++)
		{
			int v2_id;
			Surface::MESH_EDGE *eg = &m_Mesh->edges.index(pt->eids.index(j));
			v2_id = (lsj_obj->m_id == eg->v[0] ? eg->v[1] : eg->v[0]);
//			Surface::Point *v2 = &m_Mesh->vertices.index(v2_id);
			LSJ_OBJECT *lsj_obj2 = &vertArray.index(v2_id);

			double newWeight = lsj_obj->m_weight + eg->length;

			if(lsj_obj2->m_weight > newWeight)
			{
			// 更新点的权值
				lsj_obj2->m_weight = newWeight;
				m_VertexWeight.index(v2_id) = newWeight;
			// 生成新的对象，并插入到最小堆中
				if(lsj_obj2->heap_location < 0)
				{
					vertQueue->Insert(v2_id);				
				}
				else
				{
					vertQueue->FilterUp(lsj_obj2->heap_location);
				}
			}
		}
	}
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);
//	for(i = 0; i< vertNum; i++)
//	{
//		m_VertexWeight.index(i) = vertArray.index(i).m_weight;
//	}
	vertArray.free();
	delete vertQueue;
}

void CMeshDistanceField::ExtendedAdaptiveMapGeneration2()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);
	int vertNum = m_Mesh->vertices.getSize();
	
	int i;
	double maxValue = 1.0E+7;
	// 初始化
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vertNum);

	MinObjHeap<LSJ_OBJECT> *vertQueue;
	Array<LSJ_OBJECT> vertArray;  // for the minimal heap ---- the primitive array
	vertArray.setSize(0);
	vertArray.resize(vertNum);
	for(i = 0; i< vertNum; i++)
	{
		vertArray.index(i).m_id = i;	
		vertArray.index(i).m_weight = maxValue;
		vertArray.index(i).heap_location = -1;

		m_VertexWeight.index(i) = maxValue;
	}
	int maxheapsize = vertNum;
	vertQueue = new MinObjHeap<LSJ_OBJECT>(maxheapsize,&vertArray);

	// 初始化最小堆
	initialQueue(vertQueue);
//	int times = 0;
	float timeExp1= clock();

	for(;;)
	{
//		times ++;
//		if(times > 5000)
//		{
//			OUTPUTINFO("times = 5000");
//			break;
//		}
		if(vertQueue->Is_Empty())
		{
//			OUTPUTINFO("winQueue is empty!\n");
			break;
		}
		// 取出堆中最小元素（即权值最小的点）

		// pick out a point
		int minVertID; 
		vertQueue->RemoveMin(&minVertID);
		LSJ_OBJECT *lsj_obj = &vertArray.index(minVertID);
//		if(m_VertexWeight.index(minVertID)>10.0)
//		{
//			continue;
//		}
		lsj_obj->heap_location = -1;
		// 得到与权值最小的点相邻的点
		Surface::Point *pt = &m_Mesh->vertices.index(lsj_obj->m_id);
		for(int j = 0; j< pt->eNum; j++)
		{
			int v2_id;
			Surface::MESH_EDGE *eg = &m_Mesh->edges.index(pt->eids.index(j));
			v2_id = (lsj_obj->m_id == eg->v[0] ? eg->v[1] : eg->v[0]);
//			Surface::Point *v2 = &m_Mesh->vertices.index(v2_id);
			LSJ_OBJECT *lsj_obj2 = &vertArray.index(v2_id);

			double newWeight = lsj_obj->m_weight + eg->length;

			if(lsj_obj2->m_weight > newWeight)
			{
			// 更新点的权值
				lsj_obj2->m_weight = newWeight;
				m_VertexWeight.index(v2_id) = newWeight;
			// 生成新的对象，并插入到最小堆中
				if(lsj_obj2->heap_location < 0)
				{
					vertQueue->Insert(v2_id);				
				}
				else
				{
					vertQueue->FilterUp(lsj_obj2->heap_location);
				}
			}
		}
		Surface::VERTEX_EXTRA_PROPERTY *pt_pt = &m_Mesh->v_ex_properties.index(lsj_obj->m_id);
		for(int j = 0; j< pt->fNum; j++)
		{
			int v2_id = pt_pt->f_v_id.index(j);
			if(v2_id>=0)
			{
				LSJ_OBJECT *lsj_obj2 = &vertArray.index(v2_id);
				double newWeight = lsj_obj->m_weight + pt_pt->f_v_distance.index(j);

				if(lsj_obj2->m_weight > newWeight)
				{
				// 更新点的权值
					lsj_obj2->m_weight = newWeight;
					m_VertexWeight.index(v2_id) = newWeight;
				// 生成新的对象，并插入到最小堆中
					if(lsj_obj2->heap_location < 0)
					{
						vertQueue->Insert(v2_id);				
					}
					else
					{
						vertQueue->FilterUp(lsj_obj2->heap_location);
					}
				}
			}
		}
	}
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);
//	for(i = 0; i< vertNum; i++)
//	{
//		m_VertexWeight.index(i) = vertArray.index(i).m_weight;
//	}
	vertArray.free();
	delete vertQueue;
}

/********************************************************************
	created:	2006/03/20
	created:	20:3:2006   9:36
	funcname: 	ApproximatingMapGeneration
	author:		Liu Shengjun
	
	purpose:	it is exacter than other MapGeneration() algorithms

*********************************************************************/
void CMeshDistanceField::ApproximatingMapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);

	int edgeNum = m_Mesh->edges.getSize();
	int vertNum = m_Mesh->vertices.getSize();
	int i;
	double maxValue = 1.0E+7;
	// 初始化
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
//	Array<bool> vertFlag;
//	vertFlag.setSize(0);
//	vertFlag.resize(vertNum);

	m_VertexWeight.resize(vertNum);
	for(i = 0; i< vertNum; i++)
	{
		m_VertexWeight.index(i) = maxValue;

//		vertFlag.index(i) = false;
	}

	MinObjHeap<LSJ_OBJECT> *edgeQueue;
	Array<LSJ_OBJECT> edgeArray;  // for the minimal heap ---- the primitive array
	edgeArray.setSize(0);
	edgeArray.resize(edgeNum);
	for(i = 0; i< edgeNum; i++)
	{
		edgeArray.index(i).m_id = i;	
		edgeArray.index(i).m_weight = maxValue;
		edgeArray.index(i).heap_location = -1;

	}
	int maxheapsize = edgeNum;
	edgeQueue = new MinObjHeap<LSJ_OBJECT>(maxheapsize,&edgeArray);

	// 初始化最小堆
	initialQueue(edgeQueue,2);

	float timeExp1= clock();
	
	for(;;)
	{
		if(edgeQueue->Is_Empty())
			break;
		
		// 取出堆中最小元素（即权值最小的边）

		int minEdgeID; 
		edgeQueue->RemoveMin(&minEdgeID);

		LSJ_OBJECT *edge_obj = &edgeArray.index(minEdgeID);		

		edge_obj->heap_location = -1;
		
//		if(edge_obj->m_weight > 15.0)
//			continue;

		Surface::MESH_EDGE *edge = &m_Mesh->edges.index(edge_obj->m_id);

		for(int j = 0; j<2; j++)
		{
			int fid = edge->f[j];
			if(fid < 0)
				continue;
			
			propagate(j, edge_obj->m_id, edgeQueue);
			
		}
	}
	
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);

	edgeArray.free();
	delete edgeQueue;	
	
}

void CMeshDistanceField::propagate(int idx, int eID, MinObjHeap<LSJ_OBJECT> *eQ, int flag)
{
	Surface::MESH_EDGE *edge = &m_Mesh->edges.index(eID);
	Surface::EDGE_EXTRA_PROPERTY *edge_ex_pt = &m_Mesh->e_ex_properties.index(eID);

	int vid1 = edge->v[0];
	int vid2 = edge->v[1];

	double tv1,tv2;
	tv1 = m_VertexWeight.index(vid1);
	tv2 = m_VertexWeight.index(vid2);

	// 计算第三点的权值
	Point2D O, v3; // v3的二维坐标可以预计算，在读入模型时
	
	int v3_id;

	int eid1, eid2;  // 取出另外两边的序号  可以预处理，在读入模型时，将其保存

	Surface::MESH_EDGE *e1,*e2;

	v3_id = edge_ex_pt->f_v3_id[idx];

	double *old_weight = &m_VertexWeight.index(v3_id);

	if(*old_weight < tv1 && *old_weight < tv2)
		return;

	Array<LSJ_OBJECT> *objArray = eQ->getObjectArray();

	int triID = edge->f[idx];
	Surface::Triangle *tri = &m_Mesh->triangles.index(triID);

	Surface::Point *vert3 = &m_Mesh->vertices.index(v3_id);


	v3.assign(edge_ex_pt->f_v3_2D[idx]);
	eid1 = edge_ex_pt->f_v_eid[idx][0];
	eid2 = edge_ex_pt->f_v_eid[idx][1];

	e1 = &m_Mesh->edges.index(eid1);
	e2 = &m_Mesh->edges.index(eid2);

	double w1,w2;
	w1 = tv1 + e1->length;	w2 = tv2 + e2->length;

	v3.y = -v3.y; // 将v3放置在X轴下方
	if((tv1+tv2) < edge->length)  // can't form a triangle
	{
		// 近似计算
		// 在三个值中选择一个最小的
		O.assign(edge_ex_pt->f_v3_2D[1-idx]);
//		OUTPUTINFO("(tv1+tv2) < edge->length\n");
	}
	else
	{		
		m_Mesh->transform3Dto2D(-1,eID,tv1,tv2,v3_id,&O);
	}

	double v3_weight;
	
	if(fabs(O.y)<EPSILON)
	{
		v3_weight = (w1<w2 ? w1 : w2);
//		OUTPUTINFO("fabs(O.y)<EPSILON\n");
	}
	else
	{
		if(O.y<0)
		{
			OUTPUTINFO("\"O.y<0\" Error!\n");
			return;
		}
		else
		{
			//　Ov1
			double x1,x2;
			x1 = v3.y*O.x/O.y;
			if(v3.x<=x1)
			{
				v3_weight = w1;
			}
			else
			{
				//  Ov2
				x2 = edge->length + v3.y * (O.x-edge->length)/O.y;
				if(v3.x >= x2)
				{
					v3_weight = w2;
				}
				else
				{
					v3_weight = v3.distance(O);
				}
			}
		}
	}		

	if((tv1+tv2) < edge->length)  // can't form a triangle
	{
		// 近似计算
		// 在三个值中选择一个最小的
		int O_id = edge_ex_pt->f_v3_id[1-idx];
		double tO = m_VertexWeight.index(O_id);
		double w3 = tO + v3_weight;

		v3_weight = (w1 < w2 ? w1 : w2);
		v3_weight = (v3_weight < w3 ? v3_weight : w3);
//		OUTPUTINFO("(tv1+tv2) < edge->length\n");
	}
	
	
	if(*old_weight>v3_weight)
	{
		*old_weight = v3_weight;
		
//		LSJ_OBJECT *edge_object;
//		edge_object = &objArray->index(eid1);
//		edge_object->m_weight = (tv1< v3_weight ? tv1 : v3_weight);
//		if(edge_object->heap_location < 0)
//		{
//			eQ->Insert(eid1);
//		}
//		else
//		{
//			eQ->FilterUp(edge_object->heap_location);
//		}
//		edge_object = &objArray->index(eid2);
//		edge_object->m_weight = (tv2< v3_weight ? tv2 : v3_weight);
//		if(edge_object->heap_location<0)
//		{
//			eQ->Insert(eid2);
//		}
//		else
//		{
//			eQ->FilterUp(edge_object->heap_location);
//		}	
		LSJ_OBJECT *edge_object;
		if(flag)
		{
			for(int i = 0; i< vert3->eNum; i++)
			{
				int eid = vert3->eids.index(i);
				
				Surface::MESH_EDGE *e = &m_Mesh->edges.index(eid);

				edge_object = &objArray->index(eid);

				double temp_w1, temp_w2;
				temp_w1 = m_VertexWeight.index(e->v[0]);
				temp_w2 = m_VertexWeight.index(e->v[1]);
				edge_object->m_weight = (temp_w1 < temp_w2 ? temp_w1 : temp_w2);
				if(fabs(temp_w1-temp_w2)> e->length)// || edge_object->m_weight < v3_weight)
					continue;
				if(edge_object->heap_location<0)
				{
					eQ->Insert(eid);
				}
				else
				{
					eQ->FilterUp(edge_object->heap_location);
				}				
			}
		}
		else
		{
			edge_object = &objArray->index(v3_id);
			edge_object->m_weight = v3_weight;
			if(edge_object->heap_location<0)
			{
				eQ->Insert(v3_id);
			}
			else
			{
				eQ->FilterUp(edge_object->heap_location);
			}				
		}

	}
		

	// （将与该点相邻接的所有边进行更新）
	// 先考虑三角形的两条边
	
	// 如果计算出的权值比当前权值小，则{
	// 更新边的权值
	
	// 如果边已经在最小堆中
	// 更新最小堆

	// 如果不在最小堆中
	// 插入到最小堆中
	// }



}

void CMeshDistanceField::ExactMapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);
	int edgeNum = m_Mesh->edges.getSize();
	int vertNum = m_Mesh->vertices.getSize();
	int triNum = m_Mesh->triangles.getSize();

	int i;
	double maxValue = 1.0E+7;
	// 初始化
	//---------------------------------------------------------------------------
	// 重新设置距离场
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vertNum);
	for(i = 0; i< vertNum; i++)
	{
		m_VertexWeight.index(i) = maxValue;
	}

	// 初始化边数组
	Array<EDGE_OBJECT> edgeArray;
	edgeArray.setSize(0);
	edgeArray.resize(edgeNum);
	for(i = 0; i<edgeNum; i++)
	{
		// 预处理。
		// 计算
		Surface::MESH_EDGE *edge = &m_Mesh->edges.index(i);
		EDGE_OBJECT *eg_obj = &edgeArray.index(i);
		// m_b0, m_b1
		eg_obj->m_b0 = 0; eg_obj->m_b1 = edge->length;
		// m_id
		eg_obj->m_id = i;
		// m_fv[0], m_fv[1]
		int vid;
		int fid = edge->f[0];
		Surface::Triangle *tri;
		if(fid>=0 && fid < triNum)
		{
			tri = &m_Mesh->triangles.index(fid);
			for(int j =0; j<3; j++)
			{
				vid = tri->v[j];
				if(vid != edge->v[0] && vid != edge->v[1])
					break;
			}
			thirdPointFrom3DTo2d(i,fid,vid,&eg_obj->m_fv[0]);
		}
		fid = edge->f[1];
		if(fid >=0 && fid < triNum)
		{
			tri = &m_Mesh->triangles.index(edge->f[1]);
			for(int j =0; j<3; j++)
			{
				vid = tri->v[j];
				if(vid != edge->v[0] && vid != edge->v[1])
					break;
			}
			thirdPointFrom3DTo2d(i,fid,vid,&eg_obj->m_fv[1]);
		}
	}
	//-----------------------------------------------------------------------------
	// 定义最小堆
	int maxheapsize = edgeNum;
	MinObjHeap<EDGE_OBJECT> *edgeQueue = new MinObjHeap<EDGE_OBJECT>(maxheapsize,&edgeArray);

	// 初始化最小堆
	initialQueue(edgeQueue);
	//------------------------------------------------------------------------------
	
	//	int times = 0;
	float timeExp1= clock();
	// 算法开始
	for(;;)
	{
//		times ++;
//		if(times > 5000)
//		{
//			OUTPUTINFO("times = 5000");
//			break;
//		}
		if(edgeQueue->Is_Empty())
		{
//			OUTPUTINFO("winQueue is empty!\n");
			break;
		}
		// 取出堆中最小元素（即权值最小的边）

		// pick out a edge
		int minEdgeID; 
		edgeQueue->RemoveMin(&minEdgeID);
		EDGE_OBJECT *edge_obj = &edgeArray.index(minEdgeID);
		edge_obj->heap_location = -1;
		// 得到与权值最小的点相邻的点
			// 更新点的权值
			// 生成新的对象，并插入到最小堆中

				EDGE_OBJECT *edge_obj2;
				int v2_id;
				if(edge_obj2->heap_location < 0)
				{
					edgeQueue->Insert(v2_id);				
				}
				else
				{
					edgeQueue->FilterUp(edge_obj2->heap_location);
				}
	}
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);
//	for(i = 0; i< vertNum; i++)
//	{
//		m_VertexWeight.index(i) = vertArray.index(i).m_weight;
//	}
	//释放空间
	edgeArray.free();
	delete edgeQueue;	
}


void CMeshDistanceField::TransformSourceToPointArray(Array<SOURCE> *src, int sid)
{
	int i;
	if(m_SourcePoints.getSize()>0)
	{
		m_SourcePoints.free();
	}
	m_SourcePoints.setSize(0);


	int vNum = m_Mesh->vertices.getSize();

	Array<bool> pointflag;
	pointflag.setSize(0);
	pointflag.resize(vNum);
	for(i = 0; i<vNum; i++)
	{
		pointflag.index(i) = false;
	}

	if(sid <= -1)  // all source data as one source
	{
		for(i = 0; i< src->getSize(); i++)
		{
			SOURCE *source = &m_Source->index(i);
			TransformSourceToPointArray(source, &pointflag);
		}
	}
	else
	{
		if(sid < src->getSize())
		{
			SOURCE *source = &m_Source->index(sid);
			TransformSourceToPointArray(source, &pointflag);
		}
	}
	pointflag.free();
}

void CMeshDistanceField::TransformSourceToPointArray(SOURCE *src, Array<bool> *pf)
{
	if(src->type == S_POINT || src->type == S_LINE)
	{
		if(src->s_index.getSize()>0)
		{
			for(int j = 0; j< src->s_index.getSize(); j++)
			{
				if(!pf->index(src->s_index.index(j)))
				{
					int *pid = &m_SourcePoints.addItem();
					*pid = src->s_index.index(j);

					pf->index(*pid) = true;
				}
			}
		}
	}
	if(src->type == S_FACE)
	{
		if(src->s_index.getSize()>0)
		{
			for(int j = 0; j< src->s_index.getSize(); j++)
			{
				int fid;
				fid = src->s_index.index(j);
				Surface::Triangle *tri = &m_Mesh->triangles.index(fid);
				for(int k = 0; k< 3; k++)
				{
					if(!pf->index(tri->v[k]))
					{
						int *pid = &m_SourcePoints.addItem();
						*pid = tri->v[k];

						pf->index(*pid) = true;
					}
				}
			}
		}
	}		
}

Surface * CMeshDistanceField::getMeshModel()
{
	return m_Mesh;
}

Array<double> * CMeshDistanceField::getDistanceField()
{
	return &m_VertexWeight;
}


void CMeshDistanceField::initialQueue(MinObjHeap<LSJ_OBJECT> *vQ, int cls)
{
	Array<LSJ_OBJECT> *objArray = vQ->getObjectArray();
	switch(cls)
	{
	case 1:  // vertex
		{
			for(int i = 0; i< m_SourcePoints.getSize(); i++)
			{
				int vid = m_SourcePoints.index(i);
				objArray->index(vid).m_weight = 0.0 ;
				vQ->Insert(vid);
				m_VertexWeight.index(vid) = 0.0;

				Surface::Point *v = &m_Mesh->vertices.index(vid);
				for(int j = 0; j< v->eNum; j++)
				{
					int eid = v->eids.index(j);
					Surface::MESH_EDGE *e = &m_Mesh->edges.index(eid);

					int v2_id = (vid == e->v[0] ? e->v[1] : e->v[0]);
					
					LSJ_OBJECT *obj = &objArray->index(v2_id);
					if(obj->m_weight > e->length)
					{
						obj->m_weight = e->length;
						m_VertexWeight.index(v2_id) = e->length;
						if(obj->heap_location == -1)
							vQ->Insert(v2_id);
						else
							vQ->FilterUp(obj->heap_location);
					}
				}
			}
			break;
		}
	case 2:	// edge
		{
			for(int i = 0; i< m_SourcePoints.getSize(); i++)  // process the adjacent edges
			{
				int vid = m_SourcePoints.index(i);
				m_VertexWeight.index(vid) = 0.0;
				
				Surface::Point *v = &m_Mesh->vertices.index(vid);

				for(int j = 0; j< v->eNum; j++)
				{
					int eid = v->eids.index(j);
					Surface::MESH_EDGE *e = &m_Mesh->edges.index(eid);
					
					int vid2 = (e->v[0] == vid ? e->v[1] : e->v[0]);
					double *vid2_weight = &m_VertexWeight.index(vid2);
					if(*vid2_weight > e->length)
						*vid2_weight = e->length;

					objArray->index(eid).m_weight = 0.0;
				}
			}
			for(int i = 0; i< m_SourcePoints.getSize(); i++) // process the opposite edges
			{
				int vid = m_SourcePoints.index(i);

				Surface::Point *v = &m_Mesh->vertices.index(vid);
				for(int j = 0; j<v->fNum; j++)
				{
					int fid = v->fids.index(j);
					Surface::Triangle *tri = &m_Mesh->triangles.index(fid);
					
					int eid;
					Surface::MESH_EDGE *e;
					for(int k = 0; k< 3; k++)  // find the opposite edge
					{
						eid = tri->e[k];

						e = &m_Mesh->edges.index(eid);
						if(e->v[0] != vid && e->v[1] != vid)
						{
							break;
						}
					}
					double vt1,vt2;
					vt1 = m_VertexWeight.index(e->v[0]);
					vt2 = m_VertexWeight.index(e->v[1]);
					objArray->index(eid).m_weight = (vt1<vt2 ? vt1 : vt2);
					if(vt1 > 0.0 && vt2 > 0.0)
						vQ->Insert(eid);
				}
			}

			break;
		}
	case 3:	// triangle
		{
			break;
		}
	}

}
/********************************************************************
	created:	2006/03/16
	created:	16:3:2006   11:00
	funcname: 	initialQueue
	author:		Liu Shengjun
	
	purpose:	For ExactMapGeneration()

*********************************************************************/
void CMeshDistanceField::initialQueue(MinObjHeap<EDGE_OBJECT> *eQ)
{
	Array<EDGE_OBJECT> *objArray = eQ->getObjectArray();
	int vid = -1;
	for(int i = 0; i< m_SourcePoints.getSize(); i++)
	{
		vid = m_SourcePoints.index(i);
		m_VertexWeight.index(vid) = 0.0;
	}

	for(int i = 0; i< m_SourcePoints.getSize(); i++)
	{
		vid = m_SourcePoints.index(i);
		Surface::Point *vert = &m_Mesh->vertices.index(vid);
		for(int j = 0; j< vert->fNum; j++)
		{
			int fid = vert->fids.index(j);
			Surface::Triangle *tri = &m_Mesh->triangles.index(fid);
			for(int k = 0; k<3;k++)
			{
				int eid = tri->e[k];
				Surface::MESH_EDGE *edge = &m_Mesh->edges.index(eid);
				EDGE_OBJECT *eg_obj = &objArray->index(eid);

				if(vid == edge->v[0]) //与源点相连的边
				{
					eg_obj->m_weight = 0.0; 
					if(m_VertexWeight.index(edge->v[1]) > edge->length)
					{
						eg_obj->m_weight1 = edge->length;
					}
				}
				else if( vid == edge->v[1]) // 与源点相连的边
				{
					eg_obj->m_weight1 = 0.0;
					if(m_VertexWeight.index(edge->v[0]) > edge->length)
					{
						eg_obj->m_weight = edge->length;
					}
				}
				else  // 与源点相对的边
				{
//					if(!(m_VertexWeight.index(edge->v[0]) == 0.0 || 
//						m_VertexWeight.index(edge->v[1] == 0.0)))
					{
						eg_obj->m_weight = vert->p.distance(m_Mesh->vertices.index(edge->v[0]).p);
						eg_obj->m_weight1 = vert->p.distance(m_Mesh->vertices.index(edge->v[1]).p);
						eg_obj->m_tao = fid;
						eQ->Insert(eid);
					}
				}
			}
		}
	}
}
/********************************************************************
	created:	2006/03/16
	created:	16:3:2006   11:01
	funcname: 	thirdPointFrom3DTo2d
	author:		Liu Shengjun
	
	purpose:	For ExactMapGeneration()

*********************************************************************/
void CMeshDistanceField::thirdPointFrom3DTo2d(int eID, int fID, int pID, Point2D *p)
{
	Surface::MESH_EDGE *edge = &m_Mesh->edges.index(eID);
	//  将三维转化到二维平面
	Vector3D newX,newY,newZ;
	Point3D p0,p1,p2;
	int p2ID,fid;
	p2ID = pID; fid = fID;
	p0.assign(m_Mesh->vertices.index(edge->v[0]).p);
	p1.assign(m_Mesh->vertices.index(edge->v[1]).p);
	p2.assign(m_Mesh->vertices.index(p2ID).p);
	
	newX.difference(p1,p0);		newX.norm();
	newZ.assign(m_Mesh->triangles.index(fid).n);		newZ.norm();
	newY.cross(newZ,newX);		newY.norm();
	
	Matrix mat(4,4);
	mat.index(0,0) = newX.x ; mat.index(0,1) = newX.y; mat.index(0,2) = newX.z; mat.index(0,3) = -(p0.x*newX.x + p0.y * newX.y + p0.z*newX.z);
	mat.index(1,0) = newY.x ; mat.index(1,1) = newY.y; mat.index(1,2) = newY.z; mat.index(1,3) = -(p0.x*newY.x + p0.y * newY.y + p0.z*newY.z);
	mat.index(2,0) = newZ.x ; mat.index(2,1) = newZ.y; mat.index(2,2) = newZ.z; mat.index(2,3) = -(p0.x*newZ.x + p0.y * newZ.y + p0.z*newZ.z);
	mat.index(3,0) = 0 ; mat.index(3,1) = 0; mat.index(3,2) = 0; mat.index(3,3) = 1;
	
	Matrix sp2(4,1);
	Matrix tp2(4,1);
	sp2.index(0,0) = p2.x; sp2.index(1,0) = p2.y; sp2.index(2,0) = p2.z; sp2.index(3,0) = 1;
	tp2.mult(mat,sp2);
	if(tp2.index(1,0)>0.0)
	{
		tp2.index(1,0) = -tp2.index(1,0);
	}

	p->x = tp2.index(0,0); p->y = tp2.index(1,0);

	mat.free(); sp2.free(); tp2.free();	
}
/********************************************************************
	created:	2006/03/16
	created:	16:3:2006   10:58
	funcname: 	EuclideanDistanceMapGeneration
	author:		Liu Shengjun
	
	purpose:	The Euclidean distance calculation

*********************************************************************/
void CMeshDistanceField::EuclideanDistanceMapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);
	int vertNum = m_Mesh->vertices.getSize();
	
	Array<bool> pointflag;
	pointflag.setSize(0);
	pointflag.resize(vertNum);
	
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vertNum);

	int i;
	double maxValue = 1.0E+7;
	// 初始化
	for(i = 0; i<vertNum; i++)
	{
		m_VertexWeight.index(i) = maxValue;
		pointflag.index(i) = false;
	}
	
	int sNum = m_SourcePoints.getSize();
	for(i = 0; i<sNum; i++)
	{
		int vid = m_SourcePoints.index(i);

		pointflag.index(vid) = true;
		m_VertexWeight.index(vid) = 0.0;
	}

	float timeExp1= clock();
	// calculation
	for(i = 0; i<vertNum; i++)  // for each vertex
	{
		if(pointflag.index(i))  // for each vertex except the source points
			continue;
		
		double vW = maxValue;
		Surface::Point *v = &m_Mesh->vertices.index(i);
		for(int j = 0; j < sNum; j++)  // for each source point
		{
			int vid = m_SourcePoints.index(j);
			Surface::Point *s = &m_Mesh->vertices.index(vid);

			double dis = v->p.distanceSQR(s->p);
			
			if(vW > dis)
			{
				vW = dis;
			}			
		}
		m_VertexWeight.index(i) = sqrt(vW);
	}
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);

	pointflag.free();
}

/*
void CMeshDistanceField::VertexBasedApproximatingMapGeneration()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);
	int vertNum = m_Mesh->vertices.getSize();
	
	int i;
	double maxValue = 1.0E+7;
	// 初始化
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vertNum);

	MinObjHeap<LSJ_OBJECT> *vertQueue;
	Array<LSJ_OBJECT> vertArray;  // for the minimal heap ---- the primitive array
	vertArray.setSize(0);
	vertArray.resize(vertNum);
	for(i = 0; i< vertNum; i++)
	{
		vertArray.index(i).m_id = i;	
		vertArray.index(i).m_weight = maxValue;
		vertArray.index(i).heap_location = -1;

		m_VertexWeight.index(i) = maxValue;
	}
	int maxheapsize = vertNum;
	vertQueue = new MinObjHeap<LSJ_OBJECT>(maxheapsize,&vertArray);

	// 初始化最小堆
	initialQueue(vertQueue);
//	int times = 0;
	float timeExp1= clock();

	for(;;)
	{
//		times ++;
//		if(times > 5000)
//		{
//			OUTPUTINFO("times = 5000");
//			break;
//		}
		if(vertQueue->Is_Empty())
		{
//			OUTPUTINFO("winQueue is empty!\n");
			break;
		}
		// 取出堆中最小元素（即权值最小的点）

		// pick out a point
		int minVertID; 
		vertQueue->RemoveMin(&minVertID);
		LSJ_OBJECT *lsj_obj = &vertArray.index(minVertID);
		lsj_obj->heap_location = -1;
		// 得到与权值最小的点相邻的点
		Surface::Point *pt = &m_Mesh->vertices.index(lsj_obj->m_id);
		for(int j = 0; j< pt->eNum; j++)
		{
			int v2_id;
			Surface::MESH_EDGE *eg = &m_Mesh->edges.index(pt->eids.index(j));
			v2_id = (lsj_obj->m_id == eg->v[0] ? eg->v[1] : eg->v[0]);
//			Surface::Point *v2 = &m_Mesh->vertices.index(v2_id);
			LSJ_OBJECT *lsj_obj2 = &vertArray.index(v2_id);

			double newWeight = lsj_obj->m_weight + eg->length;

			if(lsj_obj2->m_weight > newWeight)
			{
			// 更新点的权值
				lsj_obj2->m_weight = newWeight;
				m_VertexWeight.index(v2_id) = newWeight;
			// 生成新的对象，并插入到最小堆中
				if(lsj_obj2->heap_location < 0)
				{
					vertQueue->Insert(v2_id);				
				}
				else
				{
					vertQueue->FilterUp(lsj_obj2->heap_location);
				}
			}
		}
	}
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);
//	for(i = 0; i< vertNum; i++)
//	{
//		m_VertexWeight.index(i) = vertArray.index(i).m_weight;
//	}
	vertArray.free();
	delete vertQueue;
}

void CMeshDistanceField::EdgeBasedApproximatingMapGeneration()
{

}
*/

void CMeshDistanceField::ApproximatingMapGeneration2()
{
	TransformSourceToPointArray(m_Source, m_SourceIndex);
	int vertNum = m_Mesh->vertices.getSize();
	
	int i;
	double maxValue = 1.0E+7;
	// 初始化
	if(m_VertexWeight.getSize()>0)
	{
		m_VertexWeight.free();
		m_VertexWeight.setSize(0);
	}
	m_VertexWeight.resize(vertNum);

	MinObjHeap<LSJ_OBJECT> *vertQueue;
	Array<LSJ_OBJECT> vertArray;  // for the minimal heap ---- the primitive array

	Array<bool> vertFlag;
	
	vertArray.setSize(0);
	vertArray.resize(vertNum);
	vertFlag.setSize(0);
	vertFlag.resize(vertNum);

	for(i = 0; i< vertNum; i++)
	{
		vertArray.index(i).m_id = i;	
		vertArray.index(i).m_weight = maxValue;
		vertArray.index(i).heap_location = -1;

		vertFlag.index(i) = false;

		m_VertexWeight.index(i) = maxValue;
	}
	int maxheapsize = vertNum;
	vertQueue = new MinObjHeap<LSJ_OBJECT>(maxheapsize,&vertArray);

	// 初始化最小堆
	initialQueue(vertQueue);

//	int times = 0;
	float timeExp1= clock();

	for(;;)
	{
		if(vertQueue->Is_Empty())
		{
			break;
		}
		// 取出堆中最小元素（即权值最小的点）

		// pick out a point
		int minVertID; 
		vertQueue->RemoveMin(&minVertID);
		LSJ_OBJECT *lsj_obj = &vertArray.index(minVertID);
		lsj_obj->heap_location = -1;
		vertFlag.index(minVertID) = true;
		
//		if(lsj_obj->m_weight > 10.0)
//			continue;

		if(m_VertexWeight.index(minVertID) ==0.0)
			continue;
		// 得到与权值最小的点相邻的点
		Surface::Point *pt = &m_Mesh->vertices.index(lsj_obj->m_id);
		for(int j = 0; j< pt->eNum; j++)
		{
			int v2_id;
			int eid = pt->eids.index(j);
			Surface::MESH_EDGE *eg = &m_Mesh->edges.index(eid);
			v2_id = (lsj_obj->m_id == eg->v[0] ? eg->v[1] : eg->v[0]);
//			Surface::Point *v2 = &m_Mesh->vertices.index(v2_id);

			if(vertFlag.index(v2_id)==false || m_VertexWeight.index(v2_id) == 0.0)
				continue;
			
			Surface::EDGE_EXTRA_PROPERTY *edge_ex_pt = &m_Mesh->e_ex_properties.index(eid);
			
			if(edge_ex_pt->f_v3_id[0] >= 0 ) //非边界边
			{
				if(vertFlag.index(edge_ex_pt->f_v3_id[0])==false)
				{
					propagate(0, eid, vertQueue, 0);
				}
			}
			if(edge_ex_pt->f_v3_id[1] >= 0) //非边界边
			{
				if(vertFlag.index(edge_ex_pt->f_v3_id[1])==false)
				{
					propagate(1, eid, vertQueue, 0);
				}
			}

		}
	}
	timeExp1 = (clock()-timeExp1)/CLOCKS_PER_SEC;
	OUTPUTINFO("\n所用时间:%f秒\n",timeExp1);
	vertArray.free();
	vertFlag.free();
	delete vertQueue;
}
