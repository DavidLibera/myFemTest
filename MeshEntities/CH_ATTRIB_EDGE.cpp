// CH_ATTRIB_EDGE.cpp: implementation of the CCH_ATTRIB_EDGE class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CH_ATTRIB_EDGE.h"
#include "CH_ATTRIB_NODE.h"
#include "CH_ATTRIB_EDGENODE.h"
#include "CH_ATTRIB_FACENODE.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCH_ATTRIB_EDGE::CCH_ATTRIB_EDGE()
{
	//entityType = 100;
	DrawingFlag = false;
	ShowOrNot = true;
	IsNotch = false;
	IsActive = false;
	isVisited = false;
}

CCH_ATTRIB_EDGE::~CCH_ATTRIB_EDGE()
{
	GLKPOSITION Pos;
 //   if(attrNodeList.GetCount()>0)
	//{
	//	for(Pos=attrNodeList.GetHeadPosition();Pos!=NULL;)
	//	{
	//	    CCH_ATTRIB_NODE *node=(CCH_ATTRIB_NODE *)(attrNodeList.GetNext(Pos));
	//	    if (node->GetAttribFlag(0))
	//		{
	//		   CCH_ATTRIB_FACENODE *facenode=(CCH_ATTRIB_FACENODE *)node;
	//		   delete facenode;
	//		}
	//	    else
	//		{
	//		   CCH_ATTRIB_EDGENODE *edgenode=(CCH_ATTRIB_EDGENODE *)node;
	//		   delete edgenode;
	//		}
	//	}
	//}
   attrNodeList.RemoveAll();
}

CCH_ATTRIB_NODE* CCH_ATTRIB_EDGE::GetAttib_NodeRecordPtr(int No)
{
			if( (No < 1) || (No > attrNodeList.GetCount()))    return  NULL;
    return (CCH_ATTRIB_NODE *)attrNodeList.GetAt(attrNodeList.FindIndex(No-1));
}

void CCH_ATTRIB_EDGE::drawProfile()
{
	GLKPOSITION Pos;	double xx,yy,zz;

	if (attrNodeList.GetCount()<2) return;

	

	glColor3f(0.0f,0.0f,0.8f);
	glLineWidth(1.0f);
	glBegin(GL_LINE_STRIP);

	for(Pos=attrNodeList.GetHeadPosition();Pos!=NULL;) {
		CCH_ATTRIB_NODE *node=(CCH_ATTRIB_NODE *)(attrNodeList.GetNext(Pos));
		position(node,xx,yy,zz);

		glVertex3d(xx,yy,zz);
	}

	glEnd();
	glEndList();

	
}

void CCH_ATTRIB_EDGE::drawShade()
{
	GLKPOSITION Pos;	double xx,yy,zz;

	glColor3f(1.0f,1.0f,0.0f);
	for(Pos=attrNodeList.GetHeadPosition();Pos!=NULL;)
	{
		CCH_ATTRIB_NODE *node=(CCH_ATTRIB_NODE *)(attrNodeList.GetNext(Pos));
		position(node,xx,yy,zz);
		drawBox((float)xx,(float)yy,(float)zz,0.2f);
	}
}

/*void CCH_ATTRIB_EDGE::drawHighLight()
{
        GLKPOSITION Pos;
		CCH_ATTRIB_NODE *node;
		double xx,yy,zz;
		glDisable(GL_LIGHTING);

		

		glColor3f(1.0f,1.0f,0.0f);
		glBegin(GL_LINE_STRIP);
		for(Pos=this->GetAttrNodeList().GetHeadPosition();Pos!=NULL;)
		{
	      node=(CCH_ATTRIB_NODE *)this->GetAttrNodeList().GetNext(Pos);
		  position(node,xx,yy,zz);

		glVertex3d(xx,yy,zz);
		}
		glEnd();


		glLineWidth(3.0);
	
		glLineWidth(1.0);

		glEnable(GL_LIGHTING);
}*/

void CCH_ATTRIB_EDGE::drawBox(float xx, float yy, float zz, float r)
{
	glBegin(GL_QUADS);

	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx-r,yy-r,zz-r);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx+r,yy-r,zz-r);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx+r,yy+r,zz-r);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx-r,yy+r,zz-r);

	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx-r,yy-r,zz+r);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx+r,yy-r,zz+r);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx+r,yy+r,zz+r);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xx-r,yy+r,zz+r);
		
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx-r,yy-r,zz-r);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx-r,yy+r,zz-r);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx-r,yy+r,zz+r);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx-r,yy-r,zz+r);
		
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx+r,yy-r,zz-r);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx+r,yy+r,zz-r);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx+r,yy+r,zz+r);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xx+r,yy-r,zz+r);
		
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx-r,yy-r,zz-r);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx+r,yy-r,zz-r);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx+r,yy-r,zz+r);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx-r,yy-r,zz+r);
		
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx-r,yy+r,zz-r);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx+r,yy+r,zz-r);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx+r,yy+r,zz+r);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(xx-r,yy+r,zz+r);

	glEnd();
}

int CCH_ATTRIB_EDGE::GetIndexNo() const
{
	return indexno;
}

void CCH_ATTRIB_EDGE::SetIndexNo( const int _index )
{
	indexno=_index;
}

bool CCH_ATTRIB_EDGE::GetSystemFlag()
{
   return SystemFlag;
}
void CCH_ATTRIB_EDGE::SetSystemFlag(bool Flag)
{
   SystemFlag = Flag;
}
bool CCH_ATTRIB_EDGE::GetDrawingFlag()
{
   return DrawingFlag;
}
void CCH_ATTRIB_EDGE::SetDrawingFlag(bool Flag)
{
  DrawingFlag = Flag;
}
