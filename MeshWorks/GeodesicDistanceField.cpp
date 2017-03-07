// GeodesicDistanceField.cpp: implementation of the GeodesicDistanceField class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShareLibSrc/opengl/GLsupport.h"

#include "3dshop.h"
#include "GeodesicDistanceField.h"

#include "ShareLibSrc/Base/Defs.h"
#include "ShareLibSrc/Base/Types.h"
#include "ShareLibSrc/Geometry/Vector3D.h"
#include "ShareLibSrc/Geometry/Line2D.h"
#include "ShareLibSrc/Geometry/LineSeg.h"
#include "ShareLibSrc/Storage/minheap.h"
#include "ShareLibSrc/LinearAlgebra/Matrix.h"

#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GeodesicDistanceField::GeodesicDistanceField()
{

}

GeodesicDistanceField::~GeodesicDistanceField()
{

}

GeodesicDistanceField::GeodesicDistanceField(Surface &surf, Point3D p)
{
	source.assign(p);
	surface = &surf;
}

GeodesicDistanceField::GeodesicDistanceField(Surface &surf, int pID)
{
	sourceID = pID;
	source.assign(surf.vertices.index(pID).p);
	surface = &surf;
	max_steps = 100;
	setupEdge();
}


GeodesicDistanceField::GeodesicDistanceField(Surface &surf, SOURCE s)
{
	
}

void GeodesicDistanceField::setupEdge()
{
	int num = surface->triangles.getSize();
	int vertNum = surface->vertices.getSize();
//	edges.setSize(0);
//	tri_edgeids.setSize(0);
	OUTPUTINFO("ģ��Ԥ����...\n");
	OUTPUTINFO("���ٿռ�,��ʼ��(��,ÿ������Ƭ�ı����,ÿ���������š������)...\n");
	edges.resize(num+vertNum);
	edgeNum = 0;
	tri_edgeids.resize(num);
	ver_fids.resize(vertNum);
	ver_eids.resize(vertNum);
	distField.resize(vertNum);

	if(num <=0)
		return;
	for(int i = 0;i<vertNum;i++)
	{
		// ÿ���������������ʼ��
		FIDPERVERTEX *ver = &ver_fids.index(i);
		ver->fids.resize(10);
		ver->fNum=0;

		// ÿ����ı���������ʼ��
		ver = &ver_eids.index(i);
		ver->fids.resize(10);
		ver->fNum = 0;
	}
	OUTPUTINFO("ÿ������������Ϣ����...\n");
	for(i = 0; i<num; i++)
	{
		Surface::Triangle *tri = &surface->triangles.index(i);
		for(int j = 0; j<3; j++)
		{
			FIDPERVERTEX *ver = &ver_fids.index(tri->v[j]);
			ver->addSurfItem(i);
		}
	}

	OUTPUTINFO("ÿ������Ƭ�������Ϣ��ÿ���ߵĶ�����Ϣ�����������Ϣ����...\n");
	for(i = 0 ;i<num;i++) // for each triangle in the model
	{
		Surface::Triangle *tri = &surface->triangles.index(i);

		EDGEID *eid = &tri_edgeids.index(i);//addItem(); //

		for(int j = 0; j<3;j++) // for each edge of the triangle 
		{
			Edge e;
			e.p1 = tri->v[j]; e.p2 = tri->v[(j+1)%3];
			e.f1 = i;
			int temp = isInEdges(&e);
			if(temp == -1)  // is not in the array edges
			{
				// add the edge into the array edges

				Edge * newe = &edges.index(edgeNum);//addItem();//
				edgeNum++;
				newe->p1 = e.p1; newe->p2 = e.p2;
				newe->f1 = i; newe->f2 = e.f2;

				Vector3D vec;
				vec.difference(surface->vertices.index(e.p1).p,surface->vertices.index(e.p2).p);
				newe->len = vec.mag();

				eid->e[j] = edgeNum-1;
				
			}
			if(temp >=0)  // return the index of the edge in the array edges 
			{
				eid->e[j] = temp;
			}
		}
	}
	OUTPUTINFO("ÿ����������������Ϣ����...\n");
	for(i = 0; i<edgeNum; i++)
	{
		Edge *e = &edges.index(i);
//		if(e->p1>vertNum || e->p1<0 || e->p2>vertNum || e->p2<0)
//		{
//			OUTPUTINFO("e->p1=%d, e->p2=%d\n",e->p1,e->p2);
//			continue;
//		}
		FIDPERVERTEX *ver = &ver_eids.index(e->p1);
		ver->addSurfItem(i);
		ver = &ver_eids.index(e->p2);
		ver->addSurfItem(i);
	}
	// Ԥ�����������������������ƽ���еĶ�ά����, �����������Ϣ
	OUTPUTINFO("Ԥ�����������������������ƽ���еĶ�ά����, �����������Ϣ...\n");
	for(i = 0;i<edgeNum; i++)
	{
		Edge *e = &edges.index(i);
		int fpid = -1; //f?(1,2)����Ƭ�е�������������
		Point2D fp;
		int e2id, e3id; e2id = e3id = -1;
		//��һ������Ƭ
		if(e->f1>=0)
		{
			setupInfoForEdge(*e, i, e->f1, &fpid, &e2id, &e3id, &fp);
			e->f1pid = fpid; e->f1e2id= e2id; e->f1e3id = e3id;
			e->f1p.assign(fp);
		}
		// �ڶ�������Ƭ
		if(e->f2>=0)
		{
			setupInfoForEdge(*e, i, e->f2, &fpid, &e2id, &e3id, &fp);
			e->f2pid = fpid; e->f2e2id= e2id; e->f2e3id = e3id;
			e->f2p.assign(fp);
//			tri2 = &surface->triangles.index(f2id);
//			for(j = 0;j<3;j++)  // ȡ��f2�е�������������
//			{
//				f2pid = tri2->v[j];
//				if(f2pid!=p1id && f2pid!=p2id)
//					break;
//			}
//			e->f2pid = f2pid; 
//			// ͬ����f2e2id,f2e3id;
//			for(j = 0;j<3;j++)
//			{
//				int eid = tri_edgeids.index(f2id).e[j];
//				if(eid == i)
//				{
//					break;
//				}
//			}
//			e->f2e2id = tri_edgeids.index(f2id).e[(j+1)%3];
//			e->f2e3id = tri_edgeids.index(f2id).e[(j+2)%3];
//			if(!(p1id == edges.index(e->f2e2id).p1 || p1id == edges.index(e->f2e2id).p2))
//			{
//				int tempID;
//				tempID = e->f2e2id;	e->f2e2id = e->f2e3id;  e->f2e3id = tempID;			
//			}
//			// ��f2p
//			Point2D f2P;
//			thirdPointFrom3DTo2D(*e,f2id, f2pid, &f2P);
//			e->f2p.assign(f2P);
		}
	}

	OUTPUTINFO("����window���пռ���window����ռ�...\n");
	int maxheapsize = edgeNum; //MAXHEAPSIZE;
	winQueue = new MinObjHeap<Window>(maxheapsize,&windows);

	int maxwndsize = 20*edgeNum;
	windows.resize(maxwndsize);
	wndNum=0;

	
//	OUTPUTINFO("%d,%d",edgeNum,vertNum);
//	for(i = 0 ;i< edgeNum ;i++)
//	{
////		OUTPUTINFO("p1=%d, p2=%d, f1 = %d, f2= %d, f1: %d %d %d, f2:%d %d %d \n",
////			edges.index(i).p1, edges.index(i).p2,edges.index(i).f1,edges.index(i).f2,
////			surface->triangles.index(edges.index(i).f1).v[0],
////			surface->triangles.index(edges.index(i).f1).v[1],
////			surface->triangles.index(edges.index(i).f1).v[2],
////			surface->triangles.index(edges.index(i).f2).v[0],
////			surface->triangles.index(edges.index(i).f2).v[1],
////			surface->triangles.index(edges.index(i).f2).v[2]);
//		OUTPUTINFO("eid = %d:p1=%d, p2=%d, f1 = %d, f2= %d , len = %f \n",
//			i,edges.index(i).p1, edges.index(i).p2,edges.index(i).f1,edges.index(i).f2 , edges.index(i).len);
//	}
//	for(i = 0;i< tri_edgeids.getSize();i++)
//	{
//		OUTPUTINFO("Triangle %d: e1id=%d, e2id=%d, e3id = %d \n",
//			i,tri_edgeids.index(i).e[0],tri_edgeids.index(i).e[1],tri_edgeids.index(i).e[2]);
//	}
}


int GeodesicDistanceField::isInEdges(Edge *e)
{
//	int eNum = edges.getSize();
	FIDPERVERTEX *ver = &ver_fids.index(e->p1);
	int triNum = ver->fNum;//getSize();//
	for(int i = 0; i< triNum; i++)  //���ڱߵ�һ������������ÿ��������
	{
		int fid = ver->fids.index(i);
		if(fid != e->f1)  // �����һ�������β��ǵ�ǰ��������
		{
			Surface::Triangle *tri = &surface->triangles.index(fid); //ȡ����������
			for(int j = 0; j< 3; j++)  //���������ε�ÿ������
			{
				if(tri->v[j]==e->p2) //�����һ�������Ǳߵ�����һ�����㣬����������ΰ����ñߣ���¼Ϊ�ñߵĵڶ���������
				{
					e->f2 = fid; //��¼Ϊ�ñߵĵڶ���������
					if(fid<e->f1) //�����ε����С�ڵ�ǰ�����ε���ţ��������ε������߶������
					{
						//��ñ��Ѿ�������
						//�õ��ñ��������е�����
						for(int k = 0; k<3;k++)
						{
							int edgeid = tri_edgeids.index(fid).e[k];
							Edge *edge = &edges.index(edgeid);
							if((edge->p1 == e->p1 && edge->p2 == e->p2) || (edge->p2== e->p1 && edge->p1 == e->p2))
							{
								return edgeid;
							}
						}
					}
					else  // ������û�иñ�
					{
						//����ñߣ����øñߵ���Ϣ
						return -1;
					}
				}
			}
		}
	}
}

int GeodesicDistanceField::isInEdges(Edge &e, Array<Edge> &eArray)
{
	int num = eArray.getSize();
	for(int i = 0;i<num;i++)
	{
		Edge edge;
		edge = eArray.index(i);

		if((edge.p1== e.p1 && edge.p2 == e.p2) || (edge.p2== e.p1 && edge.p1 == e.p2))
		{
			return i;
		}
	}
	return -1;
}
/*
	The priority queue is initialized with a window for each edge adjacent to the source vs.
	The distance fields for these initial edges are trivial, e.g. vertices adjacent to vs 
	are assigned distances D equal to the corresponding edge lengths.
	Here, we add the window for the each edge of triangles adjacent to the source vs into
	the initial queue.
*/
void GeodesicDistanceField::initializeQueue()
{
//	for(int k = 0; k<5; k++)
//	{

	int triNum = ver_fids.index(sourceID).fNum;  //��Դ�������������εĸ���
	for(int i = 0; i< triNum; i++)
	{
		int fid = ver_fids.index(sourceID).fids.index(i); // �����ε����
		for(int j = 0; j< 3; j++)
		{
			int eid = tri_edgeids.index(fid).e[j];
			Edge *edge = &edges.index(eid);
			Window wnd;
			wnd.edgeID = eid;
			wnd.b0 = 0.0;
			wnd.b1 = edge->len;
			wnd.ds = 0.0;  wnd.tao = fid;
			if(edge->p1 == sourceID || edge->p2 == sourceID)  // ��Դ�������ı�
			{
				if(edge->p2 == sourceID)
				{
					int temp;
					temp = edge->p1;
					edge->p1 = edge->p2; edge->p2 = temp;
				}
				wnd.d0 = 0.0;  wnd.d1 = edge->len;				
			
				wnd.pseudosource.assign(0.0,0.0);

			}
			else  // ��Դ����Եı�
			{
				wnd.d0 = surface->vertices.index(sourceID).p.distance(surface->vertices.index(edge->p1).p);
				wnd.d1 = surface->vertices.index(sourceID).p.distance(surface->vertices.index(edge->p2).p);
				Point2D s;
				s.x = 0.5* (wnd.b1+wnd.b0+(wnd.d0*wnd.d0-wnd.d1*wnd.d1)/(wnd.b1-wnd.b0));
				if(fabs(s.x)<EPSILON)
					s.x = 0.0;
				s.y = wnd.d0 * wnd.d0 - (s.x-wnd.b0)*(s.x-wnd.b0);
				if(fabs(s.y)<EPSILON)
					s.y = 0.0;
				s.y = sqrt(s.y);
				wnd.pseudosource.assign(s);
			}

			solveOverlapWindows(eid, &wnd);
			Window *newWnd = &addWindow();
			*newWnd = wnd;
			edge->insertWindow(wndNum-1);
			newWnd->edge_location = edge->wndNum - 1;
			winQueue->Insert(wndNum-1);
		}		
	}
//		sourceID = sourceID + 1; 
//		}

//	for(i = 0; i<eNum; i++)
//	{
//		Edge *edge = &edges.index(i);
//		if(edge->p1 == sourceID || edge->p2 == sourceID)
//		{
//			Window *wnd = &addWindow();
//			edge->insertWindow(wndNum-1);
//			wnd->edge_location = edge->wndNum-1;
//			wnd->edgeID = i;
//			if(edge->p2 == sourceID)
//			{
//				int temp;
//				temp = edge->p1;
//				edge->p1 = edge->p2; edge->p2 = temp;
//			}
//			wnd->b0 = 0.0;
//			wnd->b1 = edge->len;
//			wnd->d0 = 0.0;  wnd->d1 = edge->len;
//			wnd->ds = 0.0;  wnd->tao = edge->f1;
//			
//		
//			winQueue->Insert(wndNum-1);
//			Point2D s;
//			s.x = 0.5* (wnd->b1+wnd->b0+(wnd->d0*wnd->d0-wnd->d1*wnd->d1)/(wnd->b1-wnd->b0));
//			if(fabs(s.x)<EPSILON)
//				s.x = 0.0;
//			s.y = wnd->d0 * wnd->d0 - (s.x-wnd->b0)*(s.x-wnd->b0);
//			if(fabs(s.y)<EPSILON)
//				s.y = 0.0;
//
//			s.y = sqrt(s.y);
//			wnd->pseudosource.assign(s);
//			
//		}
//		else
//		{
//			// ����Դ�������ӵ������εĵ��������ҳ���
//			Surface::Triangle *tri1,*tri2;
//			tri1 = &surface->triangles.index(edge->f1);
//			tri2 = &surface->triangles.index(edge->f2);
//			if(tri1->v[0] == sourceID || tri1->v[1] == sourceID || tri1->v[2] == sourceID
//				|| tri2->v[0] == sourceID|| tri2->v[1] == sourceID|| tri2->v[2] == sourceID)
//			{
//				Window *wnd = &addWindow();
//				edge->insertWindow(wndNum-1);
//				wnd->edge_location = edge->wndNum-1;
//
//				wnd->edgeID = i;				
//
//				wnd->b0 = 0.0;
//				wnd->b1 = edge->len;
//
//				Vector3D vec;
//				vec.difference(surface->vertices.index(edge->p1).p,surface->vertices.index(sourceID).p);
//
//				wnd->d0 = vec.mag();  
//				
//				vec.difference(surface->vertices.index(edge->p2).p,surface->vertices.index(sourceID).p);
//				wnd->d1 = vec.mag();
//				wnd->ds = 0.0;  
//				if(tri1->v[0] == sourceID || tri1->v[1] == sourceID || tri1->v[2] == sourceID)
//					wnd->tao = edge->f1;
//				else
//					wnd->tao = edge->f2;
//				winQueue->Insert(wndNum-1);
//				Point2D s;
//				s.x = 0.5* (wnd->b1+wnd->b0+(wnd->d0*wnd->d0-wnd->d1*wnd->d1)/(wnd->b1-wnd->b0));
//				if(fabs(s.x)<EPSILON)
//					s.x = 0.0;
//				s.y = wnd->d0 * wnd->d0 - (s.x-wnd->b0)*(s.x-wnd->b0);
//				if(fabs(s.y)<EPSILON)
//					s.y = 0.0;
//
//				s.y = sqrt(s.y);
//				
//				wnd->pseudosource.assign(s);
//			}
//		}
//	}
	// Test--- 0k
//	int qnum = winQueue->GetValidSize();
//	for(i = 0 ; i< qnum;i++)
//	{
//		int node;
//		winQueue->RemoveMin(&node);
////		winQueue->GetNode(&node,i);
//		Window *window = &windows.index(node);
//		OUTPUTINFO("b0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d\n",
//			window->b0,window->b1,window->d0,window->d1,window->ds,window->tao);
////		delete node;
//	}
//	for(i = 0; i<wndNum; i++)
//	{
//		Window *window = &windows.index(i);
//		OUTPUTINFO("b0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d\n",
//			window->b0,window->b1,window->d0,window->d1,window->ds,window->tao);
//	}
}

void GeodesicDistanceField::initQueue(int sID)
{

}


Window & GeodesicDistanceField::addWindow()
{
	if(wndNum>=windows.getSize())
	{
		windows.resize(2*wndNum);				
	}
//	OUTPUTINFO("addWindow id = %d\n",wndNum);
	wndNum++;
	return windows.index(wndNum-1);
}

void GeodesicDistanceField::deleteWindow(int wid, int flag)
{
	// ��ɾ��windows�����е�Ԫ��
	if(wid<0 || wid >= wndNum) // wid������Ч��Χ
		return;
//	OUTPUTINFO("**************\n");
	if(flag!=-1)
		winQueue->deleteNode(flag);
//	OUTPUTINFO("delete window id = %d\n",wid);
	Window *tmpW = &windows.index(wid);
	int eid = tmpW->edgeID;  int edge_index = tmpW->edge_location; // ����Ҫɾ����window�ıߵ����������ڱߵ�window�����е�λ��
	if(wid < (wndNum-1)) // wid��Ϊ���һ��Ԫ��
	{
		Window *lastW = &windows.index(wndNum-1);
		*tmpW = *lastW; //���������һ��Ԫ�طŵ�widλ��
		tmpW->heap_location = lastW->heap_location; tmpW->edge_location = lastW->edge_location;
		
		Edge *mE = &edges.index(tmpW->edgeID); //ȡ�����������һ��window���ڵı�
		mE->windows.index(tmpW->edge_location)=wid; //�޸��ڱ��ϸ�window��window�����е�����		
		
		int heap_index = windows.index(wid).heap_location;
		if(heap_index!=-1)  
		{		 
			winQueue->setNode(wid,heap_index);
		}
	}
	// ��Ӧ��ɾ��edges�����ж�Ӧ�ߵ�window��¼
	Edge *tmpE = &edges.index(eid);
	if(tmpE->deleteWindow(edge_index)) //���wid�����һ��Ԫ��֮ǰ
	{
		windows.index(tmpE->windows.index(edge_index)).edge_location = edge_index; //�޸��ڱߵ�window�����е�����
	}	
	
	wndNum--;
}

/*
	The basic step is to select(and remove) a window from the queue and propagate it as
	in Section 2.2 of the paper(Hoppe). Note that the propagation step can add, modify, 
	or remove existing windows, and the queue is updated accordingly. We repeat the process
	until the queue is empty.
*/
void GeodesicDistanceField::propagation()
{
//	float times = clock();
	initializeQueue();
//	times = (clock()-times)/CLOCKS_PER_SEC;
//	OUTPUTINFO("times = %f", times);
//	return;
//	int 
//	steps=0;

	for(;;)
	{
//		steps++;
//		if(steps>max_steps)
//		{
//			OUTPUTINFO("Failure!\n");
//			break;
//		}
		if(winQueue->Is_Empty())
		{
//			OUTPUTINFO("winQueue is empty!\n");
			break;
		}

		// pick out a window
		int minWinID; 
		winQueue->RemoveMin(&minWinID);
		Window *window = &windows.index(minWinID);
			
		int e1id;

		e1id = window->edgeID;

		Edge *e = &edges.index(e1id);

		window->heap_location = -1;

		
		if(e->f1==-1 || e->f2==-1)
		{
			continue;
		}
//			(!(window->b0<EPSILON && window->pseudosource.x<EPSILON) || 
//			!(fabs(e->len-window->b1)<EPSILON && (e->len-window->pseudosource.x)<EPSILON))))// 
//			fabs(window->b0 - window->b1)<EPSILON || 
		if(((fabs(window->d0)<EPSILON || fabs(window->d1)<EPSILON) && fabs(window->ds)<EPSILON) ||
			(fabs(window->pseudosource.y)<EPSILON && (window->b0>EPSILON && (e->len-window->b1)>EPSILON )))//!(window->b0<EPSILON && fabs(e->len-window->b1)<EPSILON)))
		{
			continue;
		}

		
		int e2id, e3id, fid, p2ID, eid;

		fid = (window->tao == e->f1) ? e->f2 : e->f1;
		
		Point2D newp0,newp1,newp2;
		
		newp0.assign(0.0,0.0); newp1.assign(e->len,0.0); 

		if(fid == e->f1)
		{
			newp2.assign(e->f1p); p2ID = e->f1pid; e2id = e->f1e2id ; e3id = e->f1e3id;
		}
		else
		{
			newp2.assign(e->f2p); p2ID = e->f2pid; e2id = e->f2e2id ; e3id = e->f2e3id;
		}
		
		//  �����µ�window
		createPotentialWindow(*window, newp0, newp1, newp2, p2ID, e2id, e3id, fid);

	}
//	// Test--- 0k
	int qnum = winQueue->GetValidSize();
//	OUTPUTINFO("The all steps: %d ;The Window Queue number:%d; Windows number:%d\n", steps-1, qnum, wndNum);
}


void GeodesicDistanceField::createPotentialWindow(Window &oldWin, Point2D &p0, 
							Point2D &p1, Point2D &p2, int p2ID, int eID2, int eID3, int fID)
{
	int edgeID, p0ID;
	Point2D s;
	Window oldWnd = oldWin;
	
	s.assign(oldWnd.pseudosource);

	Point2D pb0,pb1;
	Line2D sb0,sb1;
	Line2D p0p2,p1p2;

	pb0.assign(oldWnd.b0,0.0); pb1.assign(oldWnd.b1,0.0);
	sb0.assign(s,pb0);  sb1.assign(s,pb1);
	p0p2.assign(p0,p2); p1p2.assign(p1,p2);

	LineSeg p0p2Seg, p1p2Seg;
	p0p2Seg.assign(p0,p2); p1p2Seg.assign(p1,p2);

	Point2D pnewb0,pnewb1;
	
	if(sb0.intersect(&pnewb0,p0p2) && p0p2Seg.isInSegment(pnewb0)) 
	{
		// ����sb0������p0p2�ཻ������ֻ��һ���������߶�p0p2��
		if(sb1.intersect(&pnewb1,p0p2) && p0p2Seg.isInSegment(pnewb1)) 
		{
			// ����sb1������p0p2�ཻ������ֻ��һ���������߶�p0p2��
			// ���sb1������p1p2Ҳ�ཻ���ҽ������߶�p1p2�� ; ��������pb1��p1�Ƿ���ͬһ�����ж�
			// ��ʱp1���ǰ��㡣�������µ�window����
			// �����ʱ��һ���µ�window����
			Point2D tempb1;
			if(sb1.intersect(&tempb1,p1p2) && p1p2Seg.isInSegment(tempb1))
			{
				double tempDs=0.0;
//				if(fabs(oldWnd.ds)<EPSILON)
				if(s.x<=0.0 )
				{
					if(pb0.equals(pnewb0))
					{
						tempDs = oldWnd.d0 + oldWnd.ds;
						edgeID = eID3;
						p0ID = p2ID;
						if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,p1,p0,tempDs))
						{
		//					OUTPUTINFO("p0 is a saddle point! \n");
						}
						
						edgeID = eID2;
						p0ID = edges.index(oldWnd.edgeID).p1;

						if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb1,p2,p0,tempDs))
						{
		//					OUTPUTINFO("p0 is a saddle point! \n");
						}
					}
				}
				else
				{
					if(pb1.equals(tempb1))
					{
						tempDs = oldWnd.d1 + oldWnd.ds;
						edgeID = eID3;
						p0ID = p2ID;
						if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,p1,p1,tempDs))
						{
		//					OUTPUTINFO("p1 is a saddle point! \n");
						}
						
						edgeID = eID2;
						p0ID = edges.index(oldWnd.edgeID).p1;

						if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb1,p2,p1,tempDs))
						{
		//					OUTPUTINFO("p1 is a saddle point! \n");
						}
					}
				}
				
				edgeID = eID2;
				p0ID = edges.index(oldWnd.edgeID).p1;

				if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb0,pnewb1,s,oldWnd.ds))
				{
//					OUTPUTINFO("p1 is a saddle point! \n");
				}				
			}
			else
			{
				if(p1.equals(pb1)) // ����sb1���߶�p1p2����
				{

					// ��ʱ�������µ�window����
					edgeID = eID2;
					p0ID = edges.index(oldWnd.edgeID).p1;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb0,p2,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 and p1p2 on same line\n");
					}
					
					edgeID = eID3;
					p0ID = p2ID;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,p1,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 and p1p2 on same line\n");
					}				
				}
				else
				{
//					OUTPUTINFO("sb0 X p0p2 && sb1 // p1p2 \n");
					edgeID = eID2;
					p0ID = edges.index(oldWnd.edgeID).p1;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb0,pnewb1,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 X p0p2\n");
 					}
				}
			}
		}
		else // ����sb1���߶�p0p2ƽ�л��߹��ߣ�������p0p2�ཻ���㲻���߶�p0p2��
		{
			// ��ʱ����sb1��������p0p2����---pb0��pb1�ǲ�ͬ��
			// ����sb1��������p1p2�ཻ���
			// ʵ���ϴ�ʱ����sb1Ҫ�����߶�p1p2��һ�����㣬Ҫ������
			if(sb1.intersect(&pnewb1,p1p2) && p1p2Seg.isInSegment(pnewb1))
			{
				// ����sb1������p1p2�ཻ������ֻ��һ���������߶�p1p2��
				// ���sb0������p0p1�ཻ�Ľ���Ϊp0, �����ж�sb0��p1p2�Ƿ��ཻ; ��������pb0��p0�Ƿ���ͬһ�����ж�
				// ����ཻ��������p1p2�ϣ�����ʱp0���ǰ��㡣�������µ�window����
				//
				// �����ʱ�������µ�window����
				Point2D tempb0;
				if(sb0.intersect(&tempb0,p1p2) && p1p2Seg.isInSegment(tempb0))
				{
//					if(fabs(oldWnd.ds<EPSILON))
					{
					edgeID = eID2;
					p0ID = edges.index(oldWnd.edgeID).p1;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,p0,p2,p0,oldWnd.d0+oldWnd.ds))
					{
//						OUTPUTINFO("p0 is a saddle point! \n");
					}
					
					edgeID = eID3;
					p0ID = p2ID;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,tempb0,p0,oldWnd.d0+oldWnd.ds))
					{
//						OUTPUTINFO("p0 is a saddle point! \n");
					}
					}
					edgeID = eID3;
					p0ID = p2ID;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,tempb0,pnewb1,s,oldWnd.ds))
					{
//						OUTPUTINFO("p0 is a saddle point! \n");
					}

				}
				else
				{
					edgeID = eID2;
					p0ID = edges.index(oldWnd.edgeID).p1;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb0,p2,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 X p1p2 \n");
					}
					
					edgeID = eID3;
					p0ID = p2ID;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,pnewb1,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 X p1p2 \n");
					}
				}
			}
			else // ����sb1���߶�p1p2ƽ�л��߹��ߣ�������p1p2�ཻ���㲻���߶�p1p2��
			{
				if(p1.equals(pb1)) // ����sb1���߶�p1p2����
				{

					// ��ʱ�������µ�window����
					edgeID = eID2;
					p0ID = edges.index(oldWnd.edgeID).p1;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,pnewb0,p2,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 and p1p2 on same line\n");
					}
					
					edgeID = eID3;
					p0ID = p2ID;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,p1,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p0p2 && sb1 and p1p2 on same line\n");
					}				
				}
				else
				{
//					OUTPUTINFO("sb0 X p0p2 && sb1 // p1p2 \n");
				}
			}

		}
	}
	else  // ����sb0���߶�p0p2ƽ�л��߹��ߣ�������p0p2�ཻ���㲻���߶�p0p2��
	{
		if(p0.equals(pb0))  // ����sb0���߶�p0p2����
		{
			// ʵ���ϣ���ʱsb1һ����p1p2�ཻ��һ��
			if(sb1.intersect(&pnewb1,p1p2) && p1p2Seg.isInSegment(pnewb1)) 
			{
				// ����sb1��������p1p2�ཻ������ֻ��һ���������߶�p1p2��
				// ��ʱ�������µ�window����
				edgeID = eID2;
				p0ID = edges.index(oldWnd.edgeID).p1;

				if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p0,p0,p2,s,oldWnd.ds))
				{
//					OUTPUTINFO("sb0 and p0p2 on same line && sb1 X p1p2 \n");
				}
				
				edgeID = eID3;
				p0ID = p2ID;

				if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,p2,pnewb1,s,oldWnd.ds))
				{
//					OUTPUTINFO("sb0 and p0p2 on same line && sb1 X p1p2 \n");
				}				
			}
			else // ����sb1���߶�p1p2ƽ�л��߹��ߣ�������p1p2�ཻ���㲻���߶�p1p2��
			{
				if(p1.equals(pb1)) // ����sb1���߶�p1p2����
				{
//					OUTPUTINFO("sb0 and p0p2 on same line && sb1 and p1p2 on same line\n");
				}
				else
				{
//					OUTPUTINFO("sb0 and p0p2 on same line && sb1 // p1p2 \n");
				}
			}
		}
		else  // ����sb0���߶�p0p2ƽ�У�������p0p2�ཻ���㲻���߶�p0p2��
		{
			// ��ʱ��������sb0���߶�p1p2���ཻ���
			// ����sb0���߶�p1p2��һ���ཻ������ֻ��һ���������佻��
			if(sb0.intersect(&pnewb0,p1p2) && p1p2Seg.isInSegment(pnewb0))
			{
				// ����sb0��������p1p2�ཻ������ֻ��һ���������߶�p1p2��
				if(sb1.intersect(&pnewb1,p1p2) && p1p2Seg.isInSegment(pnewb1)) 
				{
					// ����sb1��������p1p2�ཻ������ֻ��һ���������߶�p1p2��
					edgeID = eID3;
					p0ID = p2ID;

					if(createAndModifyWindow(oldWnd,edgeID,fID,p0ID,p2,pnewb0,pnewb1,s,oldWnd.ds))
					{
//						OUTPUTINFO("sb0 X p1p2 && sb1 X p1p2 \n");
 					}

				}
				else // ����sb1���߶�p1p2ƽ�л��߹��ߣ�������p1p2�ཻ���㲻���߶�p1p2��
				{
					if(p1.equals(pb1)) // ����sb1���߶�p1p2����
					{
//						OUTPUTINFO("sb0 X p1p2 && sb1 and p1p2 on same line\n");

					}
					else
					{
//						OUTPUTINFO("sb0 X p1p2 && sb1 // p1p2 \n");
						
					}
				}
			}
		}
	}
}
// �������޸�window
boolean GeodesicDistanceField::createAndModifyWindow(Window &oldWnd, int eID, int fID, 
													 int p0ID, Point2D p0, Point2D &pb0, 
													 Point2D &pb1, Point2D &ps, double dsVal)
{
	Window wnd;// = &windows.index(wndNum);//,tempWnd;
	if(createWindow(&wnd,oldWnd,eID,fID,p0ID,p0,pb0,pb1,ps,dsVal))
	{
//		if(steps == 78720 || steps == 78754 || steps == 78821 || steps == 78860 ||
//			steps == 78943 || steps == 78944 || steps == 78971 || steps == 79068 ||
//			steps == 79072 || steps == 79091 || steps == 79119 || steps == 79132 ||
//			steps == 79138 || steps == 79147 || steps == 79176 || steps == 79177 ||
//			steps == 79178 || steps == 79179 || steps == 79180 || steps == 79182)
//		OUTPUTINFO("+++++++++++++++++++++++++++++++\nThe new window: \nb0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d, edgeID=%d, pseudosource=(%f,%f)\n",
//			wnd.b0,wnd.b1,wnd.d0,wnd.d1,wnd.ds,wnd.tao,wnd.edgeID,wnd.pseudosource.x,wnd.pseudosource.y);
		// ���window�ص�����
//		if(eID == 7519 )
//		{
//			OUTPUTINFO("eID= %d , steps = %d\n",eID,steps);
//		}
		solveOverlapWindows(eID,&wnd);

//		OUTPUTINFO("After once overlapped, The new window: \nb0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d, edgeID=%d\n",
//			wnd.b0,wnd.b1,wnd.d0,wnd.d1,wnd.ds,wnd.tao,wnd.edgeID);

 		if(fabs(wnd.b0-wnd.b1)>EPSILON)
		{
			Window *window = &addWindow();
			*window = wnd;
			Edge *tmpE;
			tmpE = &edges.index(eID);				
			tmpE->insertWindow(wndNum-1);
			window->edge_location = tmpE->wndNum-1;
			winQueue->Insert(wndNum-1);
			return true;
		}
		return false;
	}
	return false;
}

// �����µ�window
boolean GeodesicDistanceField::createWindow(Window *newWnd, Window &oldWnd, int eID, 
							int fID, int p0ID, Point2D &p0, Point2D &pb0, Point2D &pb1, 
							Point2D &ps, double dsVal)
{
	double vb0,vb1;
	vb0 = p0.distance(pb0);  vb1 = p0.distance(pb1);
	if(fabs(vb0)<EPSILON)
		vb0 = 0.0;
	if(fabs(vb1)<EPSILON)
		vb1 = 0.0;
	if(fabs(vb0-vb1)>EPSILON)
	{
		Edge *edge = &edges.index(eID);
		
		newWnd->edgeID = eID;
		if(edge->p1==p0ID)
		{
			newWnd->b0 = vb0; newWnd->b1 = vb1;
			
			newWnd->d0 = ps.distance(pb0);
			
			newWnd->d1 = ps.distance(pb1);
		}
		else if(edge->p2==p0ID)
		{
			newWnd->b0 = edge->len-vb1; newWnd->b1 = edge->len-vb0;
			if(fabs(newWnd->b0)<EPSILON)
				newWnd->b0 = 0.0;
			if(fabs(newWnd->b1)<EPSILON)
				newWnd->b1 = 0.0;
			newWnd->d0 = ps.distance(pb1);
			
			newWnd->d1 = ps.distance(pb0);
		}
		
//		newWnd->d0 = ps.distance(pb0);  			
//		newWnd->d1 = ps.distance(pb1);//vec.mag();
		if(fabs(newWnd->d0)<EPSILON)
			newWnd->d0 = 0.0;
		if(fabs(newWnd->d1)<EPSILON)
			newWnd->d1 = 0.0;

		Point2D s;
		s.x = 0.5* (newWnd->b1+newWnd->b0+(newWnd->d0*newWnd->d0-newWnd->d1*newWnd->d1)/(newWnd->b1-newWnd->b0));
		if(fabs(s.x)<EPSILON)
			s.x = 0.0;
		s.y = newWnd->d0 * newWnd->d0 - (s.x-newWnd->b0)*(s.x-newWnd->b0);
	//		SQR(0.5*(oldWnd.b1-oldWnd.b0+(oldWnd.d0*oldWnd.d0-oldWnd.d1*oldWnd.d1)/(oldWnd.b1-oldWnd.b0)));//*(s.x-oldWnd.b0)

		if(fabs(s.y)<EPSILON)
		{
			s.y = 0.0;
		}
		else
		{
			if(s.y<0.0)
			{
				if(fabs(fabs(newWnd->b1-newWnd->b0)-fabs(newWnd->d0-newWnd->d1))<EPSILON)
				{
					s.y = 0.0;
				}
				else
				{
					OUTPUTINFO("createWindow: s Error! \n steps = %d, edgeID= %d, s.y = %.8f\n", steps, newWnd->edgeID, s.y);
					OUTPUTINFO("e1len = %.8f, e2len-e3len = %.8f\n", fabs(newWnd->b1-newWnd->b0), fabs(newWnd->d0-newWnd->d1));
					return false;
				}
			}
		}
		s.y = sqrt(s.y);
		newWnd->pseudosource.assign(s);


		newWnd->ds = dsVal;  
		newWnd->tao = fID;
		return true;
	}
	return false;
}

// ����ڱ�edge�����е�window���Ƿ����ཻ�Ĳ��֣��޸ĸ���
void GeodesicDistanceField::solveOverlapWindows(Edge *e,Window *w)
{
//	int wndNum = e->windows.getSize();
//	for(int k = 0;k<wndNum;k++)
//	{
//		Window *tempW = &e->windows.index(k);
//		w->addressOverlapWindows(tempW);
//		if(tempW->b0==tempW->b1)
//		{
//			e->windows.removeItem(k);
//			k--;
//			wndNum--;
//		}
//	}
}
void GeodesicDistanceField::solveOverlapWindows(int eID,Window *w, int startID)
{
	Edge *e = &edges.index(eID);
	int e_wndNum = e->wndNum;
	int winID;
	int startWndID = startID;
	for(int k = startWndID;k<e_wndNum;k++)
	{
		int wid1 = e->windows.index(k);
		Window *tempW1 = &windows.index(wid1);
		// �ҳ�window�����ж�Ӧ��window
		winID = tempW1->heap_location;
		if(*w == *tempW1)
		{
			w->b1 = w->b0;
			continue;
		}
		if((tempW1->b1-w->b0)<EPSILON || (w->b1-tempW1->b0)<EPSILON )
			continue;

		Window subw1,subw2;
		int flag=0;
		
		Window tempW2 = *tempW1;
		addressOverlapWindows(w, tempW1, &subw1, &subw2, &flag);
		
		if(fabs(tempW1->b0-tempW1->b1)<EPSILON)
		{
			deleteWindow(wid1,winID);			
			k--;
		}
		else
		{
			if(winID!=-1)
			{
				if(!(tempW2<=*tempW1))
				{
					winQueue->FilterUp(winID);
				}
				else 
				{
					if(!(*tempW1<=tempW2))
						winQueue->FilterDown(winID,winQueue->GetValidSize()-1);				
				}
			}
		}
		
		switch(flag)
		{
		case 0:
			{
				break;
			}
		case 1:
			{
 				if(fabs(subw1.b0-subw1.b1)>EPSILON)
				{
					Window *window = &addWindow();
					*window = subw1;
					Edge *tmpE;
					tmpE = &edges.index(eID);				
					tmpE->insertWindow(wndNum-1);
					window->edge_location = tmpE->wndNum-1;
					winQueue->Insert(wndNum-1);
				}
				break;
			}
		case 2:
			{
 				if(fabs(subw2.b0-subw2.b1)>EPSILON)
				{
					Window *window = &addWindow();
					*window = subw2;
					Edge *tmpE;
					tmpE = &edges.index(eID);				
					tmpE->insertWindow(wndNum-1);
					window->edge_location = tmpE->wndNum-1;
					if(winID!=-1)// && )
						winQueue->Insert(wndNum-1);
					else
						window->heap_location = -1; 
				}
				break;
			}
		case 3:
			{
 				if(fabs(subw1.b0-subw1.b1)>EPSILON)//EPSILON
				{
					Window *window = &addWindow();
					*window = subw1;
					Edge *tmpE;
					tmpE = &edges.index(eID);				
					tmpE->insertWindow(wndNum-1);
					window->edge_location = tmpE->wndNum-1;
					winQueue->Insert(wndNum-1);
				}
 				if(fabs(subw2.b0-subw2.b1)>EPSILON)//EPSILON
				{
					Window *window = &addWindow();
					*window = subw2;
					Edge *tmpE;
					tmpE = &edges.index(eID);				
					tmpE->insertWindow(wndNum-1);
					window->edge_location = tmpE->wndNum-1;
					if(winID!=-1)// && )
						winQueue->Insert(wndNum-1);
					else
						window->heap_location = -1;

				}
				break;
			}
		case 4:
			{
				solveOverlapWindows(eID,&subw1, k+1);
 				if(fabs(subw1.b0-subw1.b1)>EPSILON)
				{
					Window *window = &addWindow();
					*window = subw1;
					Edge *tmpE;
					tmpE = &edges.index(eID);				
					tmpE->insertWindow(wndNum-1);
					window->edge_location = tmpE->wndNum-1;
					winQueue->Insert(wndNum-1);
				}
				break;
			}
		default:
			break;  // no this case
		}

		e_wndNum = e->wndNum;

		if(fabs(w->b1-w->b0)<EPSILON )
			break;
	}
}
// �����window���ȶ������Ƿ����ཻ�Ĳ��֣��޸ĸ���
void GeodesicDistanceField::solveOverlapWindows(Window *w)
{
//	for(int k = 0;k<winQueue->GetValidSize();k++)
//	{
//		Window tempW;
//		winQueue->GetNode(&tempW, k);						
//		if(w->addressOverlapWindows(&tempW))
//		{
//			winQueue->UpdateNode(tempW,k);
//		}
//	}
}
/****************************************************************************************

	Function:  addressOverlapWindows()
	
	// ����window�ཻ�ص�������֮����ܳ������������������ĸ�window,  
	// w1----�����ɵ�window,
	// w2----���е�window,
	// w3----�洢��w1���ѳ�����window, 
	// w4----�洢��w2���ѳ�����window,
	// flag---����Ƿ�w3,w4�Ƿ�ʹ�ã�
	// 0 �� ��ʹ�ã�1 - ʹ��w3; 2 �� ʹ��w4��3��ʹ��w3,w4
	// 4 - ʹ��w3,��ʱw3����Ҫ�����ص�����,���ڴ����ص������У��������������账���ص�����window.

*/
void GeodesicDistanceField::addressOverlapWindows(Window *w1, Window *w2, Window *w3, Window *w4, int *flag)
{
	// w1Ϊ��window��w2Ϊ����window
	//��������window��Դ������
	//Ӧ�ÿ�����ÿ��window�м���Դ������ĳ�Ա����
	Window tempW1,tempW2,tempW3,tempW4;
	double px;

//	if(fabs(w1->b1-w1->b0)<EPSILON || fabs(w2->b1-w2->b0)<EPSILON)
//		return;
	Point2D s1,s2,s11,s22;
	s1.assign(w1->pseudosource);
	s2.assign(w2->pseudosource);

//	if((w2->b0-w1->b1)>=EPSILON )
//	{
//		return;
//	}
//	if((w1->b0-w2->b1)>=EPSILON )
//	{
//		// two window no intersection
//		return;
//	}
	if((w2->b0-w1->b0)>EPSILON && (w1->b1-w2->b0)>EPSILON && (w2->b1-w1->b1)>EPSILON)
	{
		tempW1 = *w1; tempW2 = *w1; tempW3 = *w2; tempW4 = *w2;
		Point2D p;
		p.assign(w2->b0,0.0); 
		double dis = s1.distance(p);
		tempW1.b1 = w2->b0; tempW1.d1 = dis; 
		tempW2.b0 = w2->b0; tempW2.d0 = dis;
		p.assign(w1->b1,0.0);
		dis = s2.distance(p);
		tempW3.b1 = w1->b1; tempW3.d1 = dis; 
		tempW4.b0 = w1->b1; tempW4.d0 = dis; 

		divideWindow(&tempW2,&tempW3,s1,s2);
		if(tempW1.mergeWindow(&tempW2))
		{
			if(tempW4.mergeWindow(&tempW3))
			{
				*flag=0;
			}
			else  //
			{
				*flag=2;
				*w4 = tempW3;
			}			
		}
		else
		{
			*w3 = tempW2;
			if(tempW4.mergeWindow(&tempW3))
			{
				*flag = 1;
			}
			else
			{
				*flag=3;
				*w4 = tempW3;
			}
		}
		*w1 = tempW1;
		*w2 = tempW4;		
		return;
	}
	if((w2->b0-w1->b0)>EPSILON  && (w1->b1-w2->b1)>EPSILON )
	{
//		OUTPUTINFO("b0<w->b0<w->b1<b1");
		tempW1 = *w1; tempW2 = *w1; tempW3 = *w1; tempW4 = *w2;
		Point2D p;
		p.assign(w2->b0,0.0); 
		double dis = s1.distance(p);
		tempW1.b1 = w2->b0; tempW1.d1 = dis; 
		tempW2.b0 = w2->b0; tempW2.d0 = dis; 
		p.assign(w2->b1,0.0);
		dis = s1.distance(p);
		tempW2.b1 = w2->b1; tempW2.d1 = dis;
		tempW3.b0 = w2->b1; tempW3.d0 = dis; 

		divideWindow(&tempW2,&tempW4,s1,s2);
		if(tempW1.mergeWindow(&tempW2))
		{
			if(tempW1.mergeWindow(&tempW3))
			{
				*flag = 0;
			}
			else
			{
				*flag = 4;
				*w3 = tempW3;
			}
		}
		else
		{
			if(tempW3.mergeWindow(&tempW2))
			{
				*flag = 4;
				*w3 = tempW3;
			}
			else
			{
				//������Ϊ������ֵ����
			}
		}
		*w1 = tempW1;				
		*w2 = tempW4;	
		return;
	}
	if((w1->b0 - w2->b0)>EPSILON && (w2->b1-w1->b0)>EPSILON && (w1->b1 -w2->b1)>EPSILON)
	{
		tempW1 = *w1; tempW2 = *w1; tempW3 = *w2; tempW4 = *w2;
		Point2D p;
		p.assign(w2->b1,0.0); 
		double dis = s1.distance(p);
		tempW1.b1 = w2->b1; tempW1.d1 = dis; 
		tempW2.b0 = w2->b1; tempW2.d0 = dis;
		p.assign(w1->b0,0.0);
		dis = s2.distance(p);
		tempW3.b1 = w1->b0; tempW3.d1 = dis; 
		tempW4.b0 = w1->b0; tempW4.d0 = dis; 

		divideWindow(&tempW1,&tempW4,s1,s2);
		if(tempW2.mergeWindow(&tempW1))
		{
			if(tempW3.mergeWindow(&tempW4))
			{
				*flag = 0;
			}
			else
			{
				*flag = 2;
				*w4 = tempW4;
			}
		}
		else
		{
			*w3 = tempW1;
			if(tempW3.mergeWindow(&tempW4))
			{
				*flag = 1;
			}
			else
			{
				*flag = 3;
				*w4 = tempW4;
			}
		}
		
		*w1 = tempW2;
		*w2 = tempW3;
		return;
	}
	if((w1->b0-w2->b0)>EPSILON && (w2->b1-w1->b1)>EPSILON )
	{
//		OUTPUTINFO("w->b0<b0<b1<w->b1");
		tempW1 = *w1; tempW2 = *w2; tempW3 = *w2; tempW4 = *w2;
		Point2D p;
		p.assign(w1->b0,0.0); 
		double dis = s2.distance(p);
		tempW2.b1 = w1->b0; tempW2.d1 = dis; 
		tempW3.b0 = w1->b0; tempW3.d0 = dis; 
		p.assign(w1->b1,0.0);
		dis = s2.distance(p);
		tempW3.b1 = w1->b1; tempW3.d1 = dis;
		tempW4.b0 = w1->b1; tempW4.d0 = dis; 

		divideWindow(&tempW1,&tempW3,s1,s2);
		if(tempW2.mergeWindow(&tempW3))
		{
			if(tempW2.mergeWindow(&tempW4))
			{
				*flag = 0;
			}
			else
			{
				*flag = 2;
				*w4 = tempW4;
			}
		}
		else
		{
			if(tempW3.mergeWindow(&tempW4))
			{
				*flag = 2;
				*w4 = tempW3;
			}
			else
			{
				// ������������������֣�������֣������д���
			}
		}
		*w1 = tempW1;				
		*w2 = tempW2;
		return;
	}
	
	if(fabs(w1->b0-w2->b0)<=EPSILON && (w1->b1-w2->b1)>EPSILON )
	{
		w1->b0 = w2->b0;
		tempW1 = *w1; tempW2 = *w1; tempW3 = *w2; 
		Point2D p;
		p.assign(w2->b1,0.0);
		double dis = s1.distance(p);
		tempW1.b1 = w2->b1; tempW1.d1 = dis;
		tempW2.b0 = w2->b1; tempW2.d0 = dis; 

		divideWindow(&tempW1,&tempW3,s1,s2);
		if(tempW2.mergeWindow(&tempW1))
		{
			*flag = 0;
		}
		else
		{
			*flag = 1;
			*w3 = tempW1;
		}
		*w1 = tempW2;				
		*w2 = tempW3;	
		return;
	}
	if(fabs(w1->b0-w2->b0)<=EPSILON  && (w2->b1-w1->b1)>EPSILON )
	{
		w1->b0 = w2->b0;
		tempW1 = *w1; tempW2 = *w2; tempW3 = *w2; 
		Point2D p;
		p.assign(w1->b1,0.0);
		double dis = s2.distance(p);
		tempW2.b1 = w1->b1; tempW2.d1 = dis;
		tempW3.b0 = w1->b1; tempW3.d0 = dis; 

		divideWindow(&tempW1,&tempW2,s1,s2);
		if(tempW3.mergeWindow(&tempW2))
		{
			*flag = 0;
		}
		else
		{
			*flag = 2;
			*w4 = tempW2;
		}
		*w1 = tempW1;				
		*w2 = tempW3;
		return;
	}
	if((w2->b0-w1->b0)>EPSILON && fabs(w1->b1 - w2->b1)<=EPSILON)
	{
//		if(fabs(w1->b0-0.167933)<1.0E-6 && fabs(w1->b1-0.1682)<1.0E-6 && fabs(w2->b0-0.16817)<1.0E-6 )
//			w1->b1=w2->b1;
		w1->b1=w2->b1;
		tempW1 = *w1; tempW2 = *w1; tempW3 = *w2; 
		Point2D p;
		p.assign(w2->b0,0.0);
		double dis = s1.distance(p);
		tempW1.b1 = w2->b0; tempW1.d1 = dis;
		tempW2.b0 = w2->b0; tempW2.d0 = dis; 

		divideWindow(&tempW2,&tempW3,s1,s2);
		if(tempW1.mergeWindow(&tempW2))
		{
			*flag = 0;
		}
		else
		{
			*flag = 1;
			*w3 = tempW2;
		}
		*w1 = tempW1;				
		*w2 = tempW3;
		return;
	}
	if((w1->b0-w2->b0)>EPSILON && fabs(w1->b1 - w2->b1)<=EPSILON)
	{
		w1->b1=w2->b1;
		tempW1 = *w1; tempW2 = *w2; tempW3 = *w2; 
		Point2D p;
		p.assign(w1->b0,0.0);
		double dis = s2.distance(p);
		tempW2.b1 = w1->b0; tempW2.d1 = dis;
		tempW3.b0 = w1->b0; tempW3.d0 = dis; 

		divideWindow(&tempW1,&tempW3,s1,s2);
		if(tempW2.mergeWindow(&tempW3))
		{
			*flag = 0;
		}
		else
		{
			*flag = 2;
			*w4 = tempW3;
		}
		*w1 = tempW1;				
		*w2 = tempW2;
		return;
	}
	if(fabs(w1->b0-w2->b0) <= EPSILON && fabs(w1->b1-w2->b1)<=EPSILON)
	{
		w1->b0 = w2->b0; w1->b1 = w2->b1;
		divideWindow(w1,w2,s1,s2);
		*flag = 0;
		return;
	}

}

void GeodesicDistanceField::divideWindow(Window *w1, Window *w2, Point2D &s1, Point2D &s2)
{
	double A,B,C;
	double a,b,c;
	A=B=C=0.0;a=b=c=0.0;
	double b0 = w1->b0; double b1 = w1->b1;
	double w1d0 = w1->d0 + w1->ds;
	double w1d1 = w1->d1 + w1->ds;
	double w2d0 = w2->d0 + w2->ds;
	double w2d1 = w2->d1 + w2->ds;
	//�������window�����������ڵ�ֱ���ϵ�s1,s2������ȵĵ㣻
	a = s2.x-s1.x; b = w2->ds-w1->ds;  b=b*b;
	c = (s1.x*s1.x+s1.y*s1.y)-(s2.x*s2.x+s2.y*s2.y)-b;

	A = a*a-b; B = c*a+2*s2.x*b; C = 0.25*c*c-(s2.x*s2.x+s2.y*s2.y)*b;
	double px=0.0;
	if(fabs(A)<EPSILON)
	{
		px = -C/B;
	}
	else
	{	
	double delta = B*B-4.0*A*C;
	if(fabs(delta)<=EPSILON)
		delta = 0.0;
	if(delta >= 0.0)
	{
//		if(delta > 0.0 && fabs(w2->ds)<EPSILON && fabs(w1->ds)<EPSILON)
//			OUTPUTINFO("delta > 0.0   ----  ERROR! steps = %d\n",steps);
		double x1,x2;
		double x12;
		delta = sqrt(delta);
		x1 = (-B+delta)/(2.0*A);  x2 = -B/A-x1;
		if(fabs(x1)<EPSILON )
			x1 = 0.0;
		if(fabs(x2)<EPSILON )
			x2 = 0.0;
		if((b0-x1)<EPSILON && (x1-b1)<EPSILON)
		{
			if(fabs(b0-x1)<EPSILON)
				x1 = b0;
			if(fabs(b1-x1)<EPSILON)
				x1 = b1;
			px = x1;
		}
		else
		{
			if((b0-x2)<EPSILON && (x2-b1)<EPSILON)
			{
				if(fabs(b0-x2)<EPSILON)
					x2 = b0;
				if(fabs(b1-x2)<EPSILON)
					x2 = b1;
				px = x2;
			}
			else
			{
				px = x1;
			}
		}
	}
	else
	{
//		OUTPUTINFO("delta<0 \n");
		if(((w2d0-w1d0)>EPSILON && (w2d1 - w1d1)>EPSILON) || ((w2d1-w1d0)>EPSILON 
			&& (w2d0 - w1d1)>EPSILON))
		{
			w2->b0 = b1; w2->d0 = w2->d1;
			return;
		}
		else 
		{
			if(((w1d0-w2d0)>EPSILON && (w1d1 - w2d1)>EPSILON) || ((w1d1-w2d0)>EPSILON 
			&& (w1d0 - w2d1)>EPSILON))
			{
				w1->b0 = b1; w1->d0 = w1->d1;
				return;
			}
			else
			{
// 				OUTPUTINFO("delta<0  ----- ERROR!\n");
				px = -1.0;
			}
		}
	}
	}
	// ����ָ�
	if((b0-px)>EPSILON)
	{
		if((w1d0-w2d0)>EPSILON)
		{
			w1->b1 = b0;  w1->d1 = w1->d0;
			// w2�����䲻�䡣
		}
		else
		{
			w2->b1 = b0;  w2->d1 = w2->d0;
		}
		return;
	}
	if((px-b1)>EPSILON)
	{

		if((w1d1-w2d1)>EPSILON )
		{
			w1->b0 = b1;  w1->d0 = w1->d1;
		}
		else
		{
			w2->b0 = b1;  w2->d0 = w2->d1;
		}
		return;
	}
	if(fabs(px-b0)<=EPSILON)
	{
		if((w1d1-w2d1)>EPSILON )
		{
			w1->b1 = b0;  w1->d1 = w1->d0;
		}
		else
		{
			w2->b1 = b0;  w2->d1 = w2->d0;
		}
		return;
	}
	if(fabs(px-b1)<=EPSILON)
	{
		if((w1d0-w2d0)>EPSILON)
		{
			w1->b0 = b1;  w1->d0 = w1->d1;
		}
		else
		{
			w2->b0 = b1;  w2->d0 = w2->d1;
		}
		return;
	}
	if((px-b0)>EPSILON && (b1-px)>EPSILON)
	{
		Point2D p_px;
		p_px.assign(px,0.0);
		double dis1 = s1.distance(p_px);
		double dis2 = s2.distance(p_px);
		if((w1d0>=w2d0) && (w1d1>=w2d1))
		{
			w1->b0 = w1->b1;  w1->d0 = w1->d1; // ��ȥw1 , w2 ����
			return;
		}
		if((w1d0>=w2d0) && (w2d1>=w1d1))
		{
			w1->b0 = px;  w1->d0 = dis1;
			w2->b1 = px;  w2->d1 = dis2;
			return;
		}
		if((w2d0>=w1d0) && (w2d1>=w1d1))
		{
			w2->b0 = w2->b1;  w2->d0 = w2->d1; // ��ȥw1 , w2 ����
			return;
		}
		if((w2d0>=w1d0) && (w1d1>=w2d1))
		{
			w1->b1 = px;  w1->d1 = dis1;
			w2->b0 = px;  w2->d0 = dis2;
			return;
		}
//		if((w1d0-w2d0)>EPSILON)
//		{
//			w1->b0 = px;  w1->d0 = dis1;
//			w2->b1 = px;  w2->d1 = dis2;
//		}
//		else
//		{
//			w1->b1 = px;  w1->d1 = dis1;
//			w2->b0 = px;  w2->d0 = dis2;
//		}
//		return;
	}
}

//***************************************************************************************//
//  window function
boolean Window::isIntersection(Window &w)
{

	return true;
}

boolean Window::addressOverlapWindows(Window *w)
{
	boolean isOverlap=false;
	double px;

	if(edgeID!=w->edgeID)
		return isOverlap;
	
//	OUTPUTINFO("The overlap window: \nb0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d, edgeID=%d\n",
//		w->b0,w->b1,w->d0,w->d1,w->ds,w->tao,w->edgeID);


	if((b0-w->b0)<=EPSILON)
	{
		if((b1-w->b1)<=EPSILON)
		{
			if((w->b0-b1)<EPSILON) 
			{
				// the intersection internal is (w->b0,b1)
				isOverlap = true;
				dividePoint(&px, *w, w->b0, b1 );
				if(b0==w->b0 && b1 == w->b1)
				{
					if(fabs(px-b1)<EPSILON)
					{
						if(((d0+ds)-(w->d0+w->ds))>EPSILON)
						{
							updateWindow(px,0);
							w->updateWindow(px,1);
						}
						else
						{
							updateWindow(px,1);
							w->updateWindow(px,0);

						}
					}
					if(fabs(px-b0)<EPSILON)
					{
						if(((d1+ds)-(w->d1+w->ds))>EPSILON)
						{
							updateWindow(px,1);
							w->updateWindow(px,0);							
						}
						else
						{
							updateWindow(px,0);
							w->updateWindow(px,1);
						}
					}
					if((px-b1)<EPSILON && (px-b0)>EPSILON)
					{
						if(((d1+ds)-(w->d1+w->ds))>EPSILON)
						{
							updateWindow(px,1);
							w->updateWindow(px,0);
						}
						else
						{
							updateWindow(px,0);
							w->updateWindow(px,1);

						}
					}
				}
				else
				{
//					if(d1<w->d1)
//					{
//					}
//					if(b0==w->b0 && b1<w->b1)
//					{
//						if(px==b0)
//						{
//							updateWindow(px,1);
//							w->updateWindow(px,0);
//						}
//					}
//					else
//					{
//						updateWindow(px,1);
//						w->updateWindow(px,0);
//					}
//					if(b1==w->b1)
//					{
//						
//					}
					{
						updateWindow(px,1);
						w->updateWindow(px,0);
					}
				}

			}
			else  // b1<w->b0  
			{
				isOverlap = false;  // two window no intersection
			}
		}
		else //b1>w->b1 
		{	
			// window w is in this window
			// the intersection internal is (w->b0,w->b1)
			isOverlap = true;
			dividePoint(&px, *w, w->b0, w->b1 );
			updateWindow(w->b1,0);
// 			w->updateWindow(px,1);
			
		}
	}
	else //b0>w->b0
	{
		if((b1-w->b1)<=EPSILON)
		{
			// this window is in window w
			// the intersection internal is (b0,b1)
			isOverlap = true;
			dividePoint(&px, *w, b0, b1 );
			updateWindow(px,0);
			w->updateWindow(px,1);

		}
		else //b1>w->b1 
		{	
			if((b0-w->b1)<EPSILON) 
			{
				// the intersection internal is (b0,w->b1)
				isOverlap = true;
				dividePoint(&px, *w, b0, w->b1 );
				updateWindow(px,0);
				w->updateWindow(px,1);

			}
			else  // w->b1<b0  
			{
				isOverlap = false;  // two window no intersection
			}
		}

	}
//	OUTPUTINFO("After overlap, The overlap window: \nb0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d, edgeID=%d\n",
//		w->b0,w->b1,w->d0,w->d1,w->ds,w->tao,w->edgeID);
	return isOverlap;

}


boolean Window::dividePoint(double *px, Window &w, double vb0, double vb1)
{
	double A,B,C;
	double a,b,c;
	Point2D s0,s1;
	s0.x = 0.5* (b1*b1-b0*b0+d0*d0-d1*d1)/(b1-b0);
	if(fabs(s0.x)<EPSILON)
		s0.x = 0.0;
	s0.y = sqrt(d0 * d0 - (s0.x-b0)*(s0.x-b0));
	if(fabs(s0.y)<EPSILON)
		s0.y = 0.0;
	s1.x = 0.5* (w.b1*w.b1-w.b0*w.b0+w.d0*w.d0-w.d1*w.d1)/(w.b1-w.b0);
	if(fabs(s1.x)<EPSILON)
		s1.x = 0.0;
	s1.y = sqrt(w.d0 * w.d0 - (s1.x-w.b0)*(s1.x-w.b0));
	if(fabs(s1.y)<EPSILON)
		s1.y = 0.0;

	a = s1.x-s0.x; b = w.ds-ds;  b=b*b;
	c = (s0.x*s0.x+s0.y*s0.y)-(s1.x*s1.x+s1.y*s1.y)-b;

	A = a*a-b; B = c*a+2*s1.x*b; C = 0.25*c*c-(s1.x*s1.x+s1.y*s1.y)*b;

	Point2D mp;
	mp.assign((vb0+vb1)/2.0,0.0);
	double smp0 = s0.distance(mp)+ds;
	double smp1 = s1.distance(mp)+w.ds;
	
	double delta = B*B-4.0*A*C;
	if(fabs(delta)<EPSILON)
		delta = 0.0;
	*px = vb0;
	
	if(delta >= 0.0)
	{
		if(delta>0.0)
			delta=delta;
		double x1,x2;
		double x12;
		delta = sqrt(delta);
//		if(fabs(A)<1.0E-5)
//		{
//			x12 = -C/B; 
//		}
//		else
		{
			x1 = (-B+delta)/(2.0*A);  x2 = -B/A-x1;
			x12 = (x1+x2)*0.5;
		}
//		if(x12 < vb0)
//		{
//			*px = b0;
//			return false;
//		}
		if(x12>=vb0 && x12<=vb1)
		{
			*px = x12;
			return true;
		}
		else
		{
//			OUTPUTINFO("+++++The impossible Value++++ \n");
		}
//		if(x12>vb1)
//		{
//			*px = vb1;
//			return true;
//		}
	}
//	else
//	{
		if(smp0 < smp1)
		{
			if(b1>vb1)
				*px = vb0;
			else
				*px = vb1;
		}
		else //if(smp0 > smp1)
		{
			if(b1>vb1)
				*px = vb1;
			else
				*px = vb0;
		}
		return false;
//	}
}

void Window::updateWindow(double vb, int flag)
{
	Point2D s0,s1;
	s0.x = 0.5* (b1*b1-b0*b0+d0*d0-d1*d1)/(b1-b0);
	if(fabs(s0.x)<EPSILON)
		s0.x = 0.0;
	s0.y = sqrt(d0 * d0 - (s0.x-b0)*(s0.x-b0));
	if(fabs(s0.y)<EPSILON)
		s0.x = 0.0;

	Point2D tempP;
	tempP.assign(vb,0.0);
	if(flag==0) // left vertex b0
	{
		b0 = vb;
		d0 = s0.distance(tempP);
	}
	else  // right vertex b1
	{
		b1 = vb;
		d1 = s0.distance(tempP);
	}

}


boolean Window::mergeWindow(Window *w)
{
	if(fabs(b0-w->b1)<EPSILON || fabs(b1-w->b0)<EPSILON)
	{
		if(tao == w->tao && fabs(ds - w->ds)<EPSILON && (fabs(d0-w->d1)<EPSILON || fabs(d1-w->d0)<EPSILON))
		{
			if(fabs(b0-w->b1)<EPSILON)
			{
				b0= w->b0;  d0 = w->d0;
			}
			else
			{
				b1 = w->b1; d1 = w->d1;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}



void GeodesicDistanceField::showDebugInfo()
{
//	int qnum = winQueue->GetValidSize();
//	for(int i = 0 ; i< qnum;i++)
//	{
//		int index;
//		winQueue->GetNode(&index,i);
//		Window *node = &windows.index(index);
//		OUTPUTINFO("The minimal node in queue:\n b0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d, edgeID=%d\n\n",
//			node->b0,node->b1,node->d0,node->d1,node->ds,node->tao,node->edgeID);
//		
//		OUTPUTINFO("heap_index=%d, node_index = %d, heap_location=%d, edgeID=%d, edge_location=%d\n", 
//			i,index,node->heap_location,node->edgeID, node->edge_location);
////		delete node;
//	}
//	for(int i = 0 ; i< wndNum; i++)
//	{
//		Window *node = &windows.index(i);
//		int index=-1;
//		int heap_index;
//		heap_index = node->heap_location ;
//		winQueue->GetNode(&index,heap_index);
//		if(heap_index>0)
//		if(i!=index)
//		{
//			OUTPUTINFO("********\nsteps=%d, winID =%d, heap_location = %d, winID from heap = %d, edgeID = %d \n",steps,i,heap_index,index,node->edgeID);
//		}
//	}
//	int i = 52;
	for(int i = 0;i<edgeNum;i++)
	{
		if(i==419 || i==420 || i==427 || i==417 || i == 418||
			i==599 || i==35 || i==36)
		{
		Edge *edge = &edges.index(i);
		if(edge->wndNum<=0)
			continue;
		if(edge->wndNum>0)
		{
		OUTPUTINFO("************************************\n");
		OUTPUTINFO("steps = %d, edge %d, edgelen = %f, p1id= %d, p2id = %d\n", steps, i, edge->len, edge->p1, edge->p2);
		OUTPUTINFO("------------------------------------\n");
//		OUTPUTINFO("step %d %d\n///////////////////////////////////////////////////\n\n",
//			steps, winQueue->GetValidSize());
		}
		for(int j = 0; j<edge->wndNum;j++)
		{
			Window *window = &windows.index(edge->windows.index(j));
//			if(window->ds<0.0)
//			{	
//				OUTPUTINFO("\n\nSteps= %d, eid = %d \n\n",steps, i);
//				break;
//			}
			OUTPUTINFO("winID = %d, b0=%f, b1= %f, d0=%f, d1=%f, ds=%f, tao=%d, winID in edge = %d, heap_location = %d\n",
				edge->windows.index(j),window->b0,window->b1,window->d0,window->d1,window->ds,
				window->tao,window->edge_location,window->heap_location);
		}
		}

	}
}

void GeodesicDistanceField::thirdPointFrom3DTo2D(Edge &e, int fID, int pID, Point2D *p)
{
	//  ����άת������άƽ��
	Vector3D newX,newY,newZ;
	Point3D p0,p1,p2;
	int p2ID,fid;
	p2ID = pID; fid = fID;
	p0.assign(surface->vertices.index(e.p1).p);
	p1.assign(surface->vertices.index(e.p2).p);
	p2.assign(surface->vertices.index(p2ID).p);
	
	newX.difference(p1,p0);		newX.norm();
	newZ.assign(surface->triangles.index(fid).n);		newZ.norm();
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

void GeodesicDistanceField::setupInfoForEdge(Edge &e, int eID, int fID, int *pID, int *e1ID, int *e2ID, Point2D *p)
{
	Surface::Triangle *tri;
	tri = &surface->triangles.index(fID);
	for(int j = 0;j<3;j++)  // ȡ��f1�е�������������
	{
		*pID = tri->v[j];
		if(*pID!=e.p1 && *pID!=e.p2)
			break;
	}
	// ��ȡe1id,e2id; ��ӦEdge�ṹ�е�f?e2id,f?e3id;
	for(j = 0;j<3;j++)
	{
		int eid = tri_edgeids.index(fID).e[j];
		if(eID == eid)
		{
			break;
		}
	}
	*e1ID = tri_edgeids.index(fID).e[(j+1)%3];
	*e2ID = tri_edgeids.index(fID).e[(j+2)%3];
	if(!(e.p1 == edges.index(*e1ID).p1 || e.p1 == edges.index(*e1ID).p2))
	{
		int tempID;
		tempID = *e1ID;	*e1ID = *e2ID;  *e2ID = tempID;			
	}
	// ��fp
	thirdPointFrom3DTo2D(e,fID, *pID, p);
}

void GeodesicDistanceField::getFieldValueVertex()
{
	int vertNum = surface->vertices.getSize();

	int i ;
//	for(i = 0; i<vertNum; i++) //����ÿһ������
//	{
//		FIDPERVERTEX * ver = &ver_eids.index(i);  //�õ�ÿ�������Ӧ����Ϣ����ĵ�ַ
//		for(int j = 0; j< ver->fNum; j++)  //����ÿһ���붥�������ı�
//		{
//			int eid = ver->fids.index(j);  //ȡ��ÿ�������ߵ����
//			Edge *edge = &edges.index(eid); //ȡ��ÿ����������Ϣ�ĵ�ַ
//			
//		}
//	}
	for(i = 0; i<vertNum; i++)
	{
		double *fieldvalue = &distField.index(i);
		*fieldvalue = 0.0;
	}
	for(i = 0; i<edgeNum; i++)
	{
		Edge *e = &edges.index(i);
		int winNum = e->wndNum;
		Window *win1,*win2;
		int pointNum = 0;
		int winID1=-1,winID2=-1;
		
		for(int j = 0; j<winNum ;j++)
		{
			int wid = e->windows.index(j);
			Window *win  = &windows.index(wid);
			
			if(win->b0<EPSILON )
			{
				winID1 = j;
				win1 = win;
			}
			if(fabs(win->b1-e->len)<EPSILON)
			{
				winID2 = j;
				win2 = win;
			}
			if(winID1>=0 && winID2>=0)
			{
				break;
			}
		}
		if(winID1<0 || winID2<0)
		{
			OUTPUTINFO("{winID1<0 || winID2<0}, eid = %d \n", i);
			continue;
		}
		double *fieldvalue = &distField.index(e->p1);
		*fieldvalue = *fieldvalue + win1->d0 + win1->ds;
		fieldvalue = &distField.index(e->p2);
		*fieldvalue = *fieldvalue + win2->d1 + win2->ds;		
	}

	for(i = 0; i<vertNum; i++)  //��ƽ��ֵ
	{
		double *fieldvalue = &distField.index(i);
		int eNum = ver_eids.index(i).fNum ;
		if(eNum>0)
			*fieldvalue = *fieldvalue/eNum;
	}
}

/********************************************************************
	created:	2006/01/17
	created:	17:1:2006   16:20
	funcname: 	display
	author:		Liu Shengjun
	
	purpose:	display sources 

*********************************************************************/
void SOURCE::display(Surface *pSurf)
{
	switch(type)
	{
	case S_POINT:
		{
			glDisable(GL_LIGHTING);
			glColor3f(0,0,1);
			glPointSize(5);
			glBegin(GL_POINTS);
			for(int i = 0; i<s_index.getSize();i++)
			{
				Surface::Point *pt = &pSurf->vertices.index(s_index.index(i));
				glNormal3f(pt->n.x, pt->n.y, pt->n.z);
				glVertex3f(pt->p.x, pt->p.y, pt->p.z);
			}
			glEnd();
			glPointSize(3);
		break;
		}
	case S_LINE:
		{
			glDisable(GL_LIGHTING);
			glColor3f(0,0,1);
			glLineWidth(3);
			glBegin(GL_LINE_LOOP);
			for(int i = 0; i<s_index.getSize();i++)
			{
				Surface::Point *pt = &pSurf->vertices.index(s_index.index(i));
				glNormal3f(pt->n.x, pt->n.y, pt->n.z);
				glVertex3f(pt->p.x, pt->p.y, pt->p.z);
			}
			glEnd();
			glLineWidth(1);
		break;
		}
	case S_FACE:
		{
			glDisable(GL_LIGHTING);
			glColor3f(0,0,1.0);
			for(int i = 0; i<s_index.getSize();i++){
			glBegin(GL_TRIANGLES);
			Surface::Triangle *tri = &pSurf->triangles.index(s_index.index(i));
				for(int j=0;j<3;j++)
				{
					Surface::Point *pVertex = &pSurf->vertices.index(tri->v[j]);
					glNormal3f(pVertex->n.x, pVertex->n.y, pVertex->n.z);
					glVertex3f(pVertex->p.x, pVertex->p.y, pVertex->p.z);
				}

			}
			glEnd();
		break;
		}
	}
}
