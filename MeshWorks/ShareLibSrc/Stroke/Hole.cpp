// OPENGL support head files
#include <afxwin.h>
#include <gl/gl.h>
//-----------------
#include ".\hole.h"
#include <math.h>
CHole::CHole(void)
{
	m_fColor[0]=1.0;
	m_fColor[1]=0.0;
	m_fColor[2]=0.0;
	m_pVertexVec=new vector<int>;
}
CHole::~CHole(void)
{
}

void CHole::AddVertex(int vid)
{
	m_pVertexVec->push_back(vid);
}

int CHole::GetVertex(int index)
{
	return m_pVertexVec->at(index);
}

int CHole::GetHoleSize(void)
{
	return m_pVertexVec->size();
}

void CHole::Display(void)
{
	Point3D* pV;
	int pVID;
	glDisable(GL_LIGHTING);
	glLineWidth(3.0);
	glColor3fv(m_fColor);
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<m_pVertexVec->size();i++)
	{
		pVID=m_pVertexVec->at(i);
		pV = &m_pSurf->vertices.index(pVID).p;
		glVertex3f(pV->x,pV->y,pV->z);
	}
	glEnd();
	glLineWidth(1.0);
/*	glColor3f(0.0,0.0,1.0);
	glBegin(GL_LINES);
	float* norm, coord[3];
	for(int i=0;i<m_pVertexVec->size();i++)
	{
		pV=m_pVertexVec->at(i);
		glVertex3fv(pV->GetCoord());
		norm=pV->GetNormal();
		coord[0]=pV->GetX()+norm[0]*0.1;
		coord[1]=pV->GetY()+norm[1]*0.1;
		coord[2]=pV->GetZ()+norm[2]*0.1;
		glVertex3fv(coord);
	}
	glEnd();*/
	glEnable(GL_LIGHTING);
}


void CHole::SetColor(float r, float g, float b)
{
	m_fColor[0]=r;
	m_fColor[1]=g;
	m_fColor[2]=b;

}

void CHole::initialHole(Surface *pSurf)
{
	m_pSurf = pSurf;
}

CHole::CHole(Surface *pSurf)
{
	m_pSurf = pSurf;

	m_fColor[0]=1.0;
	m_fColor[1]=0.0;
	m_fColor[2]=0.0;
	m_pVertexVec=new vector<int>;
}
