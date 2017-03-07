// CH_SpecifyHideTool.cpp: implementation of the CCH_SpecifyHideTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>

#include "../MeshEntities/QBody.h"

#include "../GLKernel/CW_Geometry.h"
#include "../GLKernel/GLKernelView.h"
#include "../GLKernel/position_array.h"
#include "../GLKLib/GLKObList.h" 

#include "RegionSelectionTool.h"

#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RegionSelectionTool::RegionSelectionTool(CGLKernelView *cview3d, QMeshPatch *body)
{
	pview3d=cview3d;	meshbody=body;
	m_positionArray=new position_array;

}

RegionSelectionTool::~RegionSelectionTool()
{
	delete m_positionArray;
	//pview3d->DelDrawAndControlArray();
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

int RegionSelectionTool::process_event(mouse_event_type even_type, const pick_event& pe)
{
	if( (even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON) )
	{
		pview3d->refresh();
		m_positionArray->Add(pe.x,pe.y,0.0);
		if ((m_positionArray->GetSize())>1) draw_position_array();
		oldX=pe.x;	oldY=pe.y;	xxxx=pe.x;	yyyy=pe.y;
	}

	if ((even_type==MOUSE_MOVE) && ((m_positionArray->GetSize())>0))
	{
		pview3d->set_foreground_color(0.65f,0.65f,0.65f);
		float pnts[4];

		pnts[0]=(float)oldX;	pnts[1]=(float)oldY;
		pnts[2]=(float)xxxx;	pnts[3]=(float)yyyy;
		pview3d->draw_polyline_2d(2,pnts);

		xxxx=pe.x;	yyyy=pe.y;

		pnts[0]=(float)oldX;	pnts[1]=(float)oldY;
		pnts[2]=(float)xxxx;	pnts[3]=(float)yyyy;
		pview3d->draw_polyline_2d(2,pnts);
	}

	if( (even_type==KEY_PRESS) && (pe.nChar==46) )  //  Key:  Delete
	{
		if ((m_positionArray->GetSize())>1)
		{
			double zzzz;	float pnts[4];
			m_positionArray->RemoveAt(m_positionArray->GetSize()-1);
			pview3d->refresh();
			if ((m_positionArray->GetSize())>1)	draw_position_array();
			m_positionArray->ElementAt(m_positionArray->GetSize()-1,oldX,oldY,zzzz);
			pnts[0]=(float)oldX;	pnts[1]=(float)oldY;
			pnts[2]=(float)xxxx;	pnts[3]=(float)yyyy;
			pview3d->draw_polyline_2d(2,pnts);
		}
	}

	if( (even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_RBUTTON) && (m_positionArray->GetSize()>0))
	{
		process_done_event();
		delete m_positionArray;
		m_positionArray=new position_array;
        pview3d->RedrawAll();
		pview3d->refresh();

	}
	if (even_type==KEY_PRESS)
	{
		if (pe.nChar==37)	//	LEFT_KEY
		{
			float xR,yR;
			pview3d->GetRotation(xR,yR);
			pview3d->SetRotation(xR,yR-10);
			pview3d->refresh();
		}
		if (pe.nChar == 27)
		{
		  return 2;
		}
		if (pe.nChar==39)	//	RIGHT_KEY
		{
			float xR,yR;
			pview3d->GetRotation(xR,yR);
			pview3d->SetRotation(xR,yR+10);
			pview3d->refresh();
		}
		if (pe.nChar==38)	//	UP_KEY
		{
			float xR,yR;
			pview3d->GetRotation(xR,yR);
			pview3d->SetRotation(xR-10,yR);
			pview3d->refresh();
		}
		if (pe.nChar==40)	//	DOWN_KEY
		{
			float xR,yR;
			pview3d->GetRotation(xR,yR);
			pview3d->SetRotation(xR+10,yR);
			pview3d->refresh();
		}

	    if (pe.nChar == 27)   //Esc
		{
		  return 2;
		}
	}
	return 1;
}

int RegionSelectionTool::process_done_event()
{
	int pnum=m_positionArray->GetSize()+1;
	double *xp,*yp;	
	xp=new double[pnum];
	yp=new double[pnum];
	double zzzz;
	for(int i=0;i<pnum-1;i++)
	{
		m_positionArray->ElementAt(i,xp[i],yp[i],zzzz);
	}
	m_positionArray->ElementAt(0,xp[pnum-1],yp[pnum-1],zzzz);

	int NodeNumber = meshbody->GetTrglNodeNumber();
	bool *nodeFlag=new bool[NodeNumber];
	double sx,sy; int i=0;
	for(GLKPOSITION Pos = meshbody->GetTrglNodeList().GetHeadPosition();Pos!=NULL;i++) 
	{
		QMeshNode* TempNode = (QMeshNode*)meshbody->GetTrglNodeList().GetNext(Pos);
		double xx,yy,zz;
		TempNode->GetCoord3D(xx,yy,zz);
		pview3d->wcl_to_screen(xx,yy,zz,sx,sy);
		CCW_Geometry geo;
		if (geo.JugPointInsideOrNot(pnum,xp,yp,sx,sy)) 
			nodeFlag[i]=true;
		else
			nodeFlag[i]=false;
	}

	for(GLKPOSITION Pos = meshbody->GetTrglFaceList().GetHeadPosition();Pos!=NULL;)
	{
		QMeshFace* TempFace = (QMeshFace*)meshbody->GetTrglFaceList().GetNext(Pos);
		TempFace->selected=false;
        int Index1 = TempFace->GetNodeRecordPtr(1)->GetIndexNo();
		int Index2 = TempFace->GetNodeRecordPtr(2)->GetIndexNo();
		int Index3 = TempFace->GetNodeRecordPtr(3)->GetIndexNo();
		if (nodeFlag[Index1-1] || nodeFlag[Index2-1] 
			|| nodeFlag[Index3-1]) TempFace->selected=true;
	}
    //meshbody->SpecifyHide = true;
	delete xp;	delete yp;	delete nodeFlag;
	//pview3d->DelDrawAndControlArray();
	return 1;
}

void RegionSelectionTool::draw_position_array()
{
	double cx,cy,cz;

	pview3d->set_foreground_color(0.65f,0.65f,0.65f);
	float *pts;
	int nCount=m_positionArray->GetSize();
	pts=new float[nCount*2];
	for(int i=0;i<nCount;i++)
	{
		m_positionArray->ElementAt(i,cx,cy,cz);
		pts[i*2]=(float)cx;	pts[i*2+1]=(float)cy;
	}
	pview3d->draw_polyline_2d(nCount,pts);
	delete pts;
}