// OPENGL support head files
#include <afxwin.h>
#include <gl/gl.h>
//-----------------
#include ".\tile.h"

CTile::CTile(void)
{
	m_pFaceVect=new vector<int>;

	m_pEdges=new vector<int>;
	m_pInnerV=new vector<int>;
    m_pBoundV=new vector<int>;
	
	m_pFaceInTile = new vector<bool>;
	m_pEdgeInTile = new vector<bool>;
	m_pVertexInTile = new vector<bool>;
	m_pVertexInBound = new vector<bool>;
}

CTile::~CTile(void)
{
//	int i;
//	for(i=0;i<m_pFaceVect->size();i++)
//	{
//		(m_pFaceVect->at(i))->SetInTile(false);
//	}
	m_pFaceVect->clear();
	m_pFaceInTile->clear();
//	for(i=0;i<m_pEdges->size();i++)
//	{
//		(m_pEdges->at(i))->SetInTile(false);
//		m_pEdges->at(i)->GetHe1()->SetPathEdge(false);
//		m_pEdges->at(i)->GetHe2()->SetPathEdge(false);
//	}
	m_pEdges->clear();
	m_pEdgeInTile->clear();
//	for(i=0;i<m_pInnerV->size();i++)
//	{
//		(m_pInnerV->at(i))->SetInTile(false);
//	}
	m_pInnerV->clear();
	m_pVertexInTile->clear();
//	for(i=0;i<m_pBoundV->size();i++)
//	{
//		(m_pBoundV->at(i))->SetInTile(false);
//	}
	m_pBoundV->clear();
	m_pVertexInBound->clear();
}

void CTile::AddFace(int fid)
{
	Surface::Triangle *tri = &m_pSurf->triangles.index(fid);
	if(!m_pFaceInTile->at(fid))
	{
		m_pFaceVect->push_back(fid);
		m_pFaceInTile->at(fid) = true;
	}
	AddInnerV(tri->v[0]);
	AddInnerV(tri->v[1]);
	AddInnerV(tri->v[2]);

//	CEdge* pEdge = pHe1->GetEdge();
//	m_pEdges->push_back(pEdge);
//	pEdge->SetInTile (true);
//
//	pEdge = pHe2->GetEdge();
//	m_pEdges->push_back(pEdge);
//	pEdge->SetInTile (true);
//
//	pEdge = pHe3->GetEdge();
//	m_pEdges->push_back(pEdge);
//	pEdge->SetInTile (true);

}

void CTile::AddInnerEdge(int eid)
{
	if(!m_pEdgeInTile->at(eid))
	{
		m_pEdges->push_back(eid);
		m_pEdgeInTile->at(eid) = true;
	}
}

void CTile::AddBoundV(int vid)
{
	if(!m_pVertexInBound->at(vid))
	{
		m_pBoundV->push_back(vid);
		m_pVertexInBound->at(vid) = true;
	}
}

void CTile::AddInnerV(int vid)
{
	
	if(!m_pVertexInTile->at(vid))
	{
		m_pInnerV->push_back(vid);
		m_pVertexInTile->at(vid) = true;
	}
}

void CTile::Display(void)
{
	vector<int>::iterator it=m_pFaceVect->begin();
	int pFaceID;
	Surface::Triangle *pFace;
	Surface::Point *pVertex;
	GLfloat * pNorm;
	glColor3f(0.63,0.63,1.0);
	glBegin(GL_TRIANGLES);
	for(;it!=m_pFaceVect->end();++it){
		pFaceID = *it;
		pFace = &m_pSurf->triangles.index(pFaceID);
		
		for(int j=0;j<3;j++)
		{
			pVertex = &m_pSurf->vertices.index(pFace->v[j]);
			glNormal3f(pVertex->n.x, pVertex->n.y, pVertex->n.z);
			glVertex3f(pVertex->p.x, pVertex->p.y, pVertex->p.z);
		}

	}
	glEnd();
//	glPointSize(5);
//	glColor3f(1.0,0.0,0.0);
//	glBegin(GL_POINTS);
//	it=m_pInnerV->begin();
//	for(;it!=m_pInnerV->end();++it){
//		pFaceID = *it;
//		pVertex = &m_pSurf->vertices.index(pFaceID);
//		
//		glNormal3f(pVertex->n.x, pVertex->n.y, pVertex->n.z);
//		glVertex3f(pVertex->p.x, pVertex->p.y, pVertex->p.z);
//	}
//	glEnd();

}

void CTile::Dijkstra(int vInID, int vOutID, CHole* pBound)
{
	int i,j,count=0;
	int pFaceID;
	Surface::Triangle *pFace;
	Surface::Point *pVertex;
	int pVertID;
	int num=m_pInnerV->size();
	for(i=0;i<num;i++)
	{
		pVertID = m_pInnerV->at(i);
		pVertex = &m_pSurf->vertices.index(pVertID);
		
		pVertex->vIndex = i;
	}
	/*
	OUTPUTINFO("****************vertex*****************\n");
	for(i = 0; i<num; i++)
	{
		pVertID = m_pInnerV->at(i);
		m_pSurf->outputMeshInfo(pVertID,1);
	}
	OUTPUTINFO("****************Triangle*****************\n");
	for(i = 0; i<m_pFaceVect->size(); i++)
	{
		pFaceID = m_pFaceVect->at(i);
		pFace = &m_pSurf->triangles.index(pFaceID);
		m_pSurf->outputMeshInfo(pFaceID,3);
		OUTPUTINFO("*******Edge******\n");
		for(int j = 0; j<3; j++)
		{
			int pEdgeID = pFace->e[j];
			m_pSurf->outputMeshInfo(pEdgeID,2);
		}
		OUTPUTINFO("\n");
	}
	return;
*/
	float *distance=new float[num];
	float *cost=new float[num*num];
	int *found=new int[num];
	int* predecessor=new int[num];
	double ct=1.0E+7;
	/// initialize the cost
	for(i=0;i<num;i++)
	{
		for(j=0;j<num;j++)
		{
			if(i==j)
				cost[i*num+j]=0;
			else
				cost[i*num+j]=ct;
		}
	}
	
	// get the cost between the vertices in the tile
	for(i=0;i<m_pFaceVect->size();i++)
	{
		pFaceID=m_pFaceVect->at(i);
		pFace = &m_pSurf->triangles.index(pFaceID);

		int eID;
		Surface::MESH_EDGE *pEdge;
		int vid1,vid2;
		for(int j = 0; j<3; j++)
		{
			eID = pFace->e[j];
			pEdge = &m_pSurf->edges.index(eID);
			ct=pEdge->cost;
			vid1 = m_pSurf->vertices.index(pEdge->v[0]).vIndex;
			vid2 = m_pSurf->vertices.index(pEdge->v[1]).vIndex;
			cost[vid1*num+vid2] = ct;
			cost[vid2*num+vid1] = ct;
		}
	}
	
	pVertex = &m_pSurf->vertices.index(vInID);
	for(i=0;i<num;i++)
	{
		found[i]=0;
		distance[i]=cost[pVertex->vIndex*num+i];
		predecessor[i]=-1;
	}
	found[pVertex->vIndex]=1;
	distance[pVertex->vIndex]=0;
	int u;
	for(i=0;i<num-2;i++)
	{
		{
			int k,minpos;
			double min=1.0E+7;
			minpos=-1;
			for(k=0;k<num;k++)
			{
				if(distance[k]<min&&!found[k]){
					min=distance[k];
					minpos=k;
				}
			}
				u=minpos;
		}
		found[u]=1;
		for(j=0;j<num;j++)
			if(!found[j])
				if(distance[u]+cost[u*num+j]<distance[j])
				{
					distance[j]=distance[u]+cost[u*num+j];
					predecessor[j]=u;
				}
	}
	for(i=0;i<num;i++)
	{
		if(predecessor[i]==-1&&i!=pVertex->vIndex) predecessor[i]=pVertex->vIndex;
	}

	Surface::Point * pVert1,*pVert2;
	int pVertID1,pVertID2;
	pVertID1 = vOutID;
	pVert1=&m_pSurf->vertices.index(vOutID);
	pBound->AddVertex(vOutID);
	while(predecessor[pVert1->vIndex]!=-1)
	{
		pVertID2=m_pInnerV->at(predecessor[pVert1->vIndex]);
		pVert2 = &m_pSurf->vertices.index(pVertID2);
		pBound->AddVertex(pVertID2);
		pVertID1 = pVertID2;
		pVert1=pVert2;
	}
	if(pVertID1 != vInID) AfxMessageBox("WRONG BOUNDARY!");

}


void CTile::SetFlag(bool flag)
{
	int i;
	int pid;
	for(i=0;i<m_pFaceVect->size();i++)
	{
		pid = m_pFaceVect->at(i);
		m_pFaceInTile->at(pid) = flag;
	}
	for(i=0;i<m_pEdges->size();i++)
	{
		pid = m_pEdges->at(i);
		m_pEdgeInTile->at(pid) = flag;
	}
	for(i=0;i<m_pInnerV->size();i++)
	{
		pid = m_pInnerV->at(i);
		m_pVertexInTile->at(pid) = flag;
	}
	for(i=0;i<m_pBoundV->size();i++)
	{
		pid = m_pBoundV->at(i);
		m_pVertexInBound->at(pid) = flag;
	}
}

void CTile::initialTile(Surface *surf)
{
	m_pSurf = surf;

	int i;
	for(i = 0; i< m_pSurf->triangles.getSize();i++)
	{
		m_pFaceInTile->push_back(false);
	}
	for(i = 0; i< m_pSurf->edges.getSize();i++)
	{
		m_pEdgeInTile->push_back(false);
	}
	for(i = 0; i< m_pSurf->vertices.getSize();i++)
	{
		m_pVertexInTile->push_back(false);
		m_pVertexInBound->push_back(false);
	}

}


bool CTile::isInTile(int fID)
{
	if(fID>=0 && fID <m_pSurf->triangles.getSize())
	{
		return m_pFaceInTile->at(fID);
	}
	// error
	return true;
}
