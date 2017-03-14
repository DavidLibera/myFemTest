#include "stdafx.h"
#include "QBody.h"
#include <math.h>
#include "CH_ATTRIB_EDGENODE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


QBody::QBody() : m_ptSize(3.0f), m_drawPolynomial(false)
{
  Patch = new QMeshPatch;
  GLKMatrixLib::CreateMatrix(m_R1,3,3);
  shadenormal=0;
}

QBody::QBody(void* v) : m_ptSize(3.0f), m_drawPolynomial(false)
{
	view=v;
  Patch = new QMeshPatch;
  GLKMatrixLib::CreateMatrix(m_R1,3,3);
  shadenormal=0;

}

QBody::~QBody()
{
	ClearAll();
	if(Patch)
		delete Patch;

	GLKMatrixLib::DeleteMatrix(m_R1,3,3);
}

QMeshPatch* QBody::GetMeshPatch() 
{
	return Patch;
}

void QBody::SetMeshPatch(QMeshPatch* patch)
{
   Patch= patch;  
}
BOOL QBody::GetAttribFlag( const int whichBit )
{
	return Patch->GetAttribFlag(whichBit);
}

void QBody::SetAttribFlag( const int whichBit, const BOOL toBe )
{
	Patch->SetAttribFlag(whichBit, toBe );
}

UINT QBody::GetIndexNo() const
{
	return Patch->GetIndexNo();
}
void QBody::SetIndexNo( const UINT _index )
{
	Patch->SetIndexNo( _index );
}
int QBody::GetTrglFaceNumber() const
{
	return Patch->GetTrglFaceNumber();
}

QMeshFace* QBody::GetTrglFaceRecordPtr(int No) const //from 1 to n
{
	return Patch->GetTrglFaceRecordPtr(No);
}

GLKObList& QBody::GetTrglFaceList()
{
	return Patch->GetTrglFaceList();
}

const GLKObList& QBody::GetTrglFaceList() const
{
	return Patch->GetTrglFaceList();
}

int QBody::GetTrglEdgeNumber() const
{
	return Patch->GetTrglEdgeNumber();
}

QMeshEdge* QBody::GetTrglEdgeRecordPtr(int No) const //from 1 to n
{
	return Patch->GetTrglEdgeRecordPtr(No);
}

GLKObList& QBody::GetTrglEdgeList()
{
	return Patch->GetTrglEdgeList();
}

const GLKObList& QBody::GetTrglEdgeList() const
{
	return Patch->GetTrglEdgeList();
}

int QBody::GetTrglNodeNumber() const
{
	return Patch->GetTrglNodeNumber();
}

QMeshNode* QBody::GetTrglNodeRecordPtr(int No) const //from 1 to n
{
	return Patch->GetTrglNodeRecordPtr(No);
}

GLKObList& QBody::GetTrglNodeList()
{
	return Patch->GetTrglNodeList();
}

const GLKObList& QBody::GetTrglNodeList() const
{
	return Patch->GetTrglNodeList();
}

int QBody::GetTrglTetraNumber() const
{
	return Patch->GetTrglTetraNumber();
}

QMeshTetra* QBody::GetTrglTetraRecordPtr(int No) const //from 1 to n
{
	return Patch->GetTrglTetraRecordPtr(No);
}

GLKObList& QBody::GetTrglTetraList()
{
	return Patch->GetTrglTetraList();
}

const GLKObList& QBody::GetTrglTetraList() const
{
	return Patch->GetTrglTetraList();
}

int QBody::GetTrglHexaNumber() const
{
	return Patch->GetTrglHexaNumber();
}

QMeshHexa* QBody::GetTrglHexaRecordPtr(int No) const //from 1 to n
{
	return Patch->GetTrglHexaRecordPtr(No);
}

GLKObList& QBody::GetTrglHexaList()
{
	return Patch->GetTrglHexaList();
}

const GLKObList& QBody::GetTrglHexaList() const
{
	return Patch->GetTrglHexaList();
}

void QBody::ClearAll()
{
     Patch->ClearAll();
}

void QBody::drawShade()
{
	if (GetAttribFlag(5)) 
		return;


	for (GLKPOSITION Pos=Patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;)
	{
		QMeshFace *temp=(QMeshFace *)(Patch->GetTrglFaceList().GetNext(Pos));

		//if (!temp->selected) continue;
		if (temp->inner) continue;

        int i = temp->GetEdgeNum();
		glPolygonMode(GL_FRONT, GL_FILL); 
		//glEnable(GL_BLEND);
		//glEnable(GL_CULL_FACE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);

		float rr = 0.75f, gg = 0.75f, bb = 0.75f, alpha=0.9f;

		//rr=0.75f; gg=0.592f; bb=0.498f;

		//added for color display
		if (temp->m_nIdentifiedPatchIndex>=0) {
			ChangeValueToColor(temp->m_nIdentifiedPatchIndex,rr,gg,bb);
			//alpha=1.0;
		}

		if (temp->selected){
			rr=0.5; gg=0.5; bb=1;
		}

		//glColor4f(rr, gg, bb, alpha);
		//end color

		glBegin(GL_POLYGON);
		QMeshNode *node[MAX_EDGE_NUM];
		for(int j = 0; j < i; j++)
		{   			
			double x,y,z;
			double nx,ny,nz;
			double dd;
			node[j]=temp->GetNodeRecordPtr(j+1);
			//double x,y,z;
			//double nx,ny,nz;

			//node[j]->GetNormal(nx,ny,nz);
			node[j]->GetCoord3D(x,y,z);

			if (temp->i_inner)
				temp->GetPlaneEquation(nx,ny,nz,dd);
			else {
				if (shadenormal==0)
					node[j]->GetNormal(nx,ny,nz);
				else temp->GetPlaneEquation(nx,ny,nz,dd);
			}
			// for flat shading
			glColor4f(rr, gg, bb, alpha);
			glNormal3d(nx,ny,nz);
			glVertex3d(x,y,z);
		}  
		glEnd();
	}
	//glDisable(GL_BLEND);
	//glDepthMask(GL_TRUE);
}

void QBody::drawShade2()
{
	if (GetAttribFlag(5))
		return;

	float rr = 0.2f, gg = 0.1f, bb = 0.2f, alpha = 0.9f;

	for (GLKPOSITION Pos = Patch->GetTrglFaceList().GetHeadPosition(); Pos != NULL;)
	{
		QMeshFace *temp = (QMeshFace *)(Patch->GetTrglFaceList().GetNext(Pos));
		
		//if (!temp->selected) continue;
		if (temp->inner) continue;

		int i = temp->GetEdgeNum();
		glPolygonMode(GL_FRONT, GL_FILL);
		//glEnable(GL_BLEND);
		//glEnable(GL_CULL_FACE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);

		//rr=0.75f; gg=0.592f; bb=0.498f;

		//added for color display
		if (temp->m_nIdentifiedPatchIndex >= 0) {
			ChangeValueToColor(temp->m_nIdentifiedPatchIndex, rr, gg, bb);
			//alpha=1.0;
		}

		if (temp->selected) {
			rr = 0.5; gg = 0.5; bb = 1;
		}

		//glColor4f(rr, gg, bb, alpha);
		//end color

		glBegin(GL_POLYGON);
		QMeshNode *node[MAX_EDGE_NUM];
		for (int j = 0; j < i; j++)
		{
			double x, y, z;
			double nx, ny, nz;
			double dd;
			node[j] = temp->GetNodeRecordPtr(j + 1);
			//double x,y,z;
			//double nx,ny,nz;

			//node[j]->GetNormal(nx,ny,nz);
			node[j]->GetCoord3D(x, y, z);

			if (temp->i_inner)
				temp->GetPlaneEquation(nx, ny, nz, dd);
			else {
				if (shadenormal == 0)
					node[j]->GetNormal(nx, ny, nz);
				else temp->GetPlaneEquation(nx, ny, nz, dd);
			}
			// for flat shading
			glColor4f(rr, gg, bb, alpha);
			glNormal3d(nx, ny, nz);
			glVertex3d(x, y, z);
		}
		glEnd();
	}
	//glDisable(GL_BLEND);
	//glDepthMask(GL_TRUE);

	rr = rr + 0.1; gg = gg + .05; bb = bb + 0.1;

}

void QBody::drawProfile()
{
//	if (m_wovenModel && GetAttribFlag(7)) drawWovenModel();

	/////////////////////////////////////////////////////////////////////////////
	//	Drawing Profile of Object
	GLKPOSITION pos;
	glDisable(GL_LIGHTING);
	glLineWidth(3.0);
	double x1,y1,z1,x2,y2,z2;

	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_LINES);
	for(pos=Patch->GetTrglEdgeList().GetHeadPosition();pos!=NULL;)
	{
		QMeshEdge *edge=(QMeshEdge *)(Patch->GetTrglEdgeList().GetNext(pos));
		if (!(edge->GetAttribFlag(0))) continue;
		if (edge->GetAttribFlag(6)) continue;

		QMeshNode *startNode,*endNode;
		startNode=edge->GetStartPoint();
		endNode=edge->GetEndPoint();
		startNode->GetCoord3D(x1,y1,z1);
		endNode->GetCoord3D(x2,y2,z2);
		glVertex3d(x1,y1,z1);
		glVertex3d(x2,y2,z2);
	}
	glEnd();

	glColor3f(0.0,0.0,1.0);
	glBegin(GL_LINES);
	for(pos=Patch->GetTrglEdgeList().GetHeadPosition();pos!=NULL;)
	{
		QMeshEdge *edge=(QMeshEdge *)(Patch->GetTrglEdgeList().GetNext(pos));
		if (!(edge->GetAttribFlag(6))) continue;
		QMeshNode *startNode,*endNode;
		startNode=edge->GetStartPoint();
		endNode=edge->GetEndPoint();
		startNode->GetCoord3D(x1,y1,z1);
		endNode->GetCoord3D(x2,y2,z2);
		glVertex3d(x1,y1,z1);
		glVertex3d(x2,y2,z2);
	}
	glEnd();

	if (m_drawPolynomial)
		drawPolynomial();
}

void QBody::drawPreMesh()
{
	if (GetAttribFlag(5)) return;


	GLKPOSITION Pos;
	for(Pos=Patch->GetTrglFaceList().GetHeadPosition();Pos!=NULL;)
	{	
		glColor3f(1.0f,1.0f,1.0f);
	    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); 
		glBegin(GL_POLYGON);
		QMeshFace *temp=(QMeshFace *)(Patch->GetTrglFaceList().GetNext(Pos));
		int i = temp->GetEdgeNum();
		       
		QMeshNode *node[MAX_EDGE_NUM];
		for(int j=0;j<i;j++)
		{
			node[j]=temp->GetNodeRecordPtr(j+1);
			double x,y,z;
			double nx,ny,nz;

			node[j]->GetNormal(nx,ny,nz);
			node[j]->GetCoord3D(x,y,z);
			glNormal3d(nx,ny,nz);
			glVertex3d(x,y,z);
		}
		glEnd();
	}
}

void QBody::drawMesh()
{
	if (GetAttribFlag(5)) return;

	GLKPOSITION Pos;
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glColor3f(0.0,0.0,0.0);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(1.0,1.0);
	double x1,y1,z1,x2,y2,z2;
	glBegin(GL_LINES);
	for(Pos=Patch->GetTrglEdgeList().GetHeadPosition();Pos!=NULL;)
	{
		QMeshEdge *edge=(QMeshEdge *)(Patch->GetTrglEdgeList().GetNext(Pos));

		if (edge->inner) continue;

		//if (!edge->storedList.IsEmpty())
		//	glColor3f(1.0,0.0,0.0);
		//else glColor3f(0.0,0.0,0.0);

		//if (edge->GetIndexNo()==8489)
		//	glColor3f(1.0,1.0,0.0);

		//bool show = false;
		//for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
		//	QMeshFace *face = (QMeshFace*)edge->GetTrglFaceList().GetNext(Pos1);
		//	if (face->selected){
		//		show=true;
		//		break;
		//	}
		//}
		//if (!show) continue;


		QMeshNode *startNode,*endNode;
		startNode=edge->GetStartPoint();
		endNode=edge->GetEndPoint();
		startNode->GetCoord3D(x1,y1,z1);
		endNode->GetCoord3D(x2,y2,z2);
		glVertex3d(x1,y1,z1);
		glVertex3d(x2,y2,z2);
	}
	glEnd();


}

void QBody::drawNode()
{
	if (GetAttribFlag(5)) return;

	//glDisable(GL_LIGHTING);
	glPointSize(m_ptSize);
	//glPointSize(2.0);
	//glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	//glColor3f(0.75,0.75,0.75);
	//glEnable(GL_POINT_SMOOTH);
	double x, y, z, nx, ny, nz;

//	bool test=false; int tom=0;

	glEnable( GL_POINT_SMOOTH );
	glBegin(GL_POINTS);
	for(GLKPOSITION Pos=Patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;)
	{
		float rr = 0, gg=0, bb=0;
		QMeshNode *node=(QMeshNode *)(Patch->GetTrglNodeList().GetNext(Pos));

		if (node->inner) continue;

		//if (!test){
		//	for (GLKPOSITION Pos1 = this->GetTrglEdgeList().GetHeadPosition(); Pos1!=NULL;){
		//		QMeshEdge *edge = (QMeshEdge *)this->GetTrglEdgeList().GetNext(Pos1);
		//		edge->inner=true;
		//	}
		//	for (GLKPOSITION Pos1 = node->GetTrglTetraList().GetHeadPosition(); Pos1!=NULL;){
		//		QMeshTetra *tetra = (QMeshTetra *)node->GetTrglTetraList().GetNext(Pos1);
		//		for (int i=1; i<=6; i++)
		//			tetra->GetEdgeRecordPtr(i)->inner=false;
		//	}
		//	if (tom==2) test=true; else tom++;
		//}

	
		if (node->m_nIdentifiedPatchIndex >= 0) 
			ChangeValueToColor(node->m_nIdentifiedPatchIndex, rr, gg, bb);

		glColor3f(rr,gg,bb);

		node->GetNormal(nx, ny, nz);
		node->GetCoord3D(x,y,z);
		glNormal3d(nx,ny,nz);
		glVertex3d(x,y,z);
	}
	glEnd();
}

void QBody::drawNode2()
{
	if (GetAttribFlag(5)) return;

	//glDisable(GL_LIGHTING);
	glPointSize(m_ptSize);
	//glPointSize(2.0);
	//glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	//glColor3f(0.75,0.75,0.75);
	//glEnable(GL_POINT_SMOOTH);
	double x, y, z, nx, ny, nz;

	//	bool test=false; int tom=0;
	
	float rr = 0, gg = 0, bb = 0;

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	for (GLKPOSITION Pos = Patch->GetTrglNodeList().GetHeadPosition(); Pos != NULL;)
	{
		QMeshNode *node = (QMeshNode *)(Patch->GetTrglNodeList().GetNext(Pos));

		if (node->inner) continue;

		//if (!test){
		//	for (GLKPOSITION Pos1 = this->GetTrglEdgeList().GetHeadPosition(); Pos1!=NULL;){
		//		QMeshEdge *edge = (QMeshEdge *)this->GetTrglEdgeList().GetNext(Pos1);
		//		edge->inner=true;
		//	}
		//	for (GLKPOSITION Pos1 = node->GetTrglTetraList().GetHeadPosition(); Pos1!=NULL;){
		//		QMeshTetra *tetra = (QMeshTetra *)node->GetTrglTetraList().GetNext(Pos1);
		//		for (int i=1; i<=6; i++)
		//			tetra->GetEdgeRecordPtr(i)->inner=false;
		//	}
		//	if (tom==2) test=true; else tom++;
		//}


		if (node->m_nIdentifiedPatchIndex >= 0)
			ChangeValueToColor(node->m_nIdentifiedPatchIndex, rr, gg, bb);

		glColor3f(rr, gg, bb);

		node->GetNormal(nx, ny, nz);
		node->GetCoord3D(x, y, z);
		glNormal3d(nx, ny, nz);
		glVertex3d(x, y, z);

		rr = rr + 0.1; gg = gg + 0.2; bb = bb + 0.1;

	}
	glEnd();
}

void QBody::drawNodeNormal(const double &length)
{
	if (GetAttribFlag(5)) return;

	glColor3f(1.0, 0.0, 0.0);

	glLineWidth(1.0);
	glBegin(GL_LINES);
	for(GLKPOSITION Pos=Patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;)
	{
		QMeshNode *node=(QMeshNode *)(Patch->GetTrglNodeList().GetNext(Pos));
		if (node->inner) continue;
		
		double x, y, z, nx, ny, nz;
		node->GetCoord3D(x, y, z);
		node->GetNormal(nx, ny, nz);
		glVertex3d(x, y, z);
		glVertex3d(x+nx*length, y+ny*length, z+nz*length);
	}
	glEnd();
}

void QBody::drawFaceNormal()
{
	if (GetAttribFlag(5)) return;

	glColor3f(0.0, 1.0, 0.0);

	glLineWidth(1.0);
	glBegin(GL_LINES);
	for(GLKPOSITION Pos=Patch->GetTrglFaceList().GetHeadPosition();Pos!=NULL;)
	{
		QMeshFace *face=(QMeshFace *)(Patch->GetTrglFaceList().GetNext(Pos));
		
		double x, y, z, nx, ny, nz, d;
		face->CalCenterPos(x, y, z);
		face->GetPlaneEquation(nx, ny, nz, d);
		glVertex3d(x, y, z);
		glVertex3d(x+nx*0.1, y+ny*0.1, z+nz*0.1);
	}
	
	glEnd();
}
void QBody::drawPolynomial()
{
	if (GetAttribFlag(5)) return;

	glColor4f(0.0, 1.0, 0.0, 0.7);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
	glEnable(GL_BLEND);
	//glLineWidth(1.0);
	//glBegin(GL_POINTS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	/*for(GLKPOSITION Pos=Patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;)
	{
		QMeshNode *node=(QMeshNode *)(Patch->GetTrglNodeList().GetNext(Pos));

		double x, y, z;

		node->GetCoord3D(x, y, z);
		const double zz = m_poly[0]*x*x+m_poly[1]*x*y+m_poly[2]*y*y+m_poly[3]*x+m_poly[4]*y;
		
		glVertex3d(x,y,zz);
	}*/

	double *b = new double[3];
	double *a = new double[3];
	double range = 10.0f;
	double div = 1.0f;

	for (double v = -range; v < range; v+=div)
	{
		for (double u = -range; u < range; u += div)
		{
			//glColor3f(0.0, fabs(range/5.0f), 0.0);
			double yy = v;
			for (double xx = u; xx <= u+div; xx+=div)
			{
				const double zz = m_poly[0]*xx*xx+m_poly[1]*xx*yy+m_poly[2]*yy*yy+m_poly[3]*xx+m_poly[4]*yy;
				//const double zz = m_poly[0]*xx*xx+m_poly[1]*xx*yy+m_poly[2]*yy*yy;
				b[0] = xx; b[1] = yy; b[2] = zz;
				GLKMatrixLib::Mul(m_R1, b, 3, 3, a);
				for (int i = 0; i < 3; ++i)
					a[i] += m_p[i];
				glVertex3d(a[0],a[1],a[2]);
			}

			yy += div;
			for (double xx = u+div; xx >= u; xx-=div)
			{
				const double zz = m_poly[0]*xx*xx+m_poly[1]*xx*yy+m_poly[2]*yy*yy+m_poly[3]*xx+m_poly[4]*yy;
				//const double zz = m_poly[0]*xx*xx+m_poly[1]*xx*yy+m_poly[2]*yy*yy;
				b[0] = xx; b[1] = yy; b[2] = zz;
				GLKMatrixLib::Mul(m_R1, b, 3, 3, a);
				for (int i = 0; i < 3; ++i)
					a[i] += m_p[i];
				glVertex3d(a[0],a[1],a[2]);
			}
		}
	}
	
	delete []a;
	delete []b;
	//CString str;
	//str.Format("%d", i);
	//AfxMessageBox(str);
	glEnd();

	//draw the centroid point
	glColor4f(0.0, 0.0, 1.0, 0.7);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(m_ptSize*2.0f);
	glBegin(GL_POINTS);
	glVertex3d(m_p[0],m_p[1],m_p[2]);
	glEnd();
	glPointSize(m_ptSize);

	//draw the normal of the centroid point
	glLineWidth(2.0);
	glBegin(GL_LINES);	
	glVertex3d(m_p[0],m_p[1],m_p[2]);
	glVertex3d(m_p[0]+m_n[0]*1.5, m_p[1]+m_n[1]*1.5, m_p[2]+m_n[2]*1.5);
	glEnd();
	glLineWidth(1.0);
	glDisable(GL_BLEND);
}





float QBody::getRange() 
{
	float max=0;
	GLKPOSITION pos;
	double cx,cy,cz;

	for(pos=Patch->GetTrglNodeList().GetHeadPosition();pos!=NULL;)
	{
		QMeshNode *node=(QMeshNode *)(Patch->GetTrglNodeList().GetNext(pos));
		node->GetCoord3D(cx,cy,cz);

		double distance=sqrt(cx*cx+cy*cy+cz*cz);
		if (distance>max) max=(float)distance;
	}

	return max;
}

void QBody::ChangeValueToColor(double maxValue, double minValue, double Value, 
								  float & nRed, float & nGreen, float & nBlue)
{
	if ((maxValue-minValue)<0.000000000001)
	{
		nRed=0.0;
		nGreen=0.0;
		nBlue=1.0;
		return;
	}

	double temp=(Value-minValue)/(maxValue-minValue);

	if (temp>0.75)
	{
		nRed=1;
		nGreen=(float)(1.0-(temp-0.75)/0.25);	
		if (nGreen<0) nGreen=0.0f;
		nBlue=0;
		return;
	}
	if (temp>0.5)
	{
		nRed=(float)((temp-0.5)/0.25);
		nGreen=1;
		nBlue=0;
		return;
	}
	if (temp>0.25)
	{
		nRed=0;
		nGreen=1;
		nBlue=(float)(1.0-(temp-0.25)/0.25);
		return;
	}
	else
	{
		nRed=0;
		nGreen=(float)(temp/0.25);
		nBlue=1;
	}
}

//added for VSA
void QBody::ChangeValueToColor(int nType, float & nRed, float & nGreen, float & nBlue)
{
	float color[][3]={ 
		{255,255,5},
		{0,255,255},
		{128,255,0},
		{128,128,64},
		{255,0,128},
		{128,128,192},
		{255,128,192},
		{255,128,0},
		{255,128,255},
		{128,128,192},
		{255,0,255},
		{23,128,192},
		{255,255,128},
		{128,128,64},
		{0,128,255},
		{12,255,34},
		{255,23,25},
		{255,214,202},
		{255,23,0},
		{128,67,123},
		{255,56,234},
		{123,34,5},
		{234,56,123},
		{78,5,23},
		{255,0,0},
		{0,234,34},
		{128,67,12},
		{255,214,202},
		{23,156,1},
		{21,234,134},
		{255,255,0},
		{26,89,34},
		{128,67,123},
		{255,255,128},
		{15,134,6},
		{24,87,45},
		{187,34,31},
		{344,2,143},
		{32,44,54},
		{11,233,4}
	};

	nRed=color[nType%40][0]/255.0f;
	nGreen=color[nType%40][1]/255.0f;
	nBlue=color[nType%40][2]/255.0f;
}