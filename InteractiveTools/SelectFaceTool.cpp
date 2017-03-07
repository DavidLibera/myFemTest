// SelectFaceTool.cpp: implementation of the SelectFaceTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\MeshWorks\StdAfx.h"

#include "..\MeshWorks\MainFrm.h"


#include "../MeshEntities/QBody.h"
#include "../GLKernel/CW_Geometry.h"
#include "../GLKernel/GLKernelView.h"
#include "../GLKLib/GLKGeometry.h"
#include "SelectFaceTool.h"

#include "..\GLKernel\CameraTool.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SelectFaceTool::SelectFaceTool(CGLKernelView *cview3d, QMeshPatch *body, MouseTool* tool, select_face_type ft)
{
   pview3d = cview3d;
   meshbody = body;

   m_ft = ft;

   MTool = tool;
}

SelectFaceTool::~SelectFaceTool()
{
	for (GLKPOSITION Pos = meshbody->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
		QMeshTetra *tmp = (QMeshTetra*) meshbody->GetTrglTetraList().GetNext(Pos);
		if (tmp->selected){
			tmp->selected=false;
		}
	}
	for (GLKPOSITION Pos = meshbody->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
		QMeshFace *tmp = (QMeshFace*) meshbody->GetTrglFaceList().GetNext(Pos);
		tmp->selected=false;
	}

	if (MTool) delete MTool;
}

int SelectFaceTool::process_event(mouse_event_type even_type, const pick_event& pe)
{   
	switch (m_ft){
		case FACE:
			return process_event_face(even_type, pe);
		case FACENODE:
			break;
		case TETRA:
			return process_event_tetra(even_type, pe);
			break;
		default:
			break;
	}
	return 1;
}


QMeshFace* SelectFaceTool::CalProjectedTrgl(double xx, double yy, double zz, 
										  double nx, double ny, double nz, double& t,double& u,double& v,double& w)
{
	CCW_Geometry geo;
	double tx,ty,tz,V,W;
	double p[3],n[3],v0[3],v1[3],v2[3],maxT;
	bool bFlag;		QMeshFace* activeFace=NULL;
	GLKPOSITION Pos;
	
	p[0]=xx;	p[1]=yy;	p[2]=zz;	n[0]=nx;	n[1]=ny;	n[2]=nz;	bFlag=false;
	for(Pos=(meshbody->GetTrglFaceList()).GetHeadPosition();Pos!=NULL;)
	{
       QMeshFace *face=(QMeshFace *)((meshbody->GetTrglFaceList()).GetNext(Pos));
	   if (face->inner) continue;
	    /*if(!(face->GetAttribFlag(6)))
		{
			continue;
		}*/
		face->GetNodePos(1,tx,ty,tz);
		v0[0]=tx;	v0[1]=ty;	v0[2]=tz;
	
		face->GetNodePos(2,tx,ty,tz);
		v1[0]=tx;	v1[1]=ty;	v1[2]=tz;

		face->GetNodePos(3,tx,ty,tz);
		v2[0]=tx;	v2[1]=ty;	v2[2]=tz;

		if (geo.CalLineFacetIntersection(p,n,v0,v1,v2,t,v,w))
		{
			if (bFlag)
				{if (t>maxT) {maxT=t; V = v; W = w;	activeFace=face;}}
			else
				{bFlag=true;	maxT=t;	V = v; W = w;activeFace=face;}
		}

	}

	t=maxT;
	v = V;
	w = W;
	u = 1.0-V-W;
	
	return activeFace;
}

int SelectFaceTool::process_event_face(mouse_event_type even_type, const pick_event& pe){
	//left click
	if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags & MK_LBUTTON))
	{
        double xxxx,yyyy,xx,yy,zz,t,uu,vv,ww;
		double pp[3],nv[3],p1[3],p2[3],p3[3];
		QMeshFace* trglFace;
		xxxx = pe.x;
		yyyy = pe.y;
		
		pview3d->screen_to_wcl(xxxx,yyyy,pp[0],pp[1],pp[2]);
		pview3d->GetViewVector(nv[0],nv[1],nv[2]);
		trglFace=CalProjectedTrgl(pp[0],pp[1],pp[2],nv[0],nv[1],nv[2],t,uu,vv,ww);

		if (trglFace){
			switch (pe.nFlags){
				case (MK_LBUTTON):
					for (GLKPOSITION Pos = meshbody->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
						QMeshFace *tmp = (QMeshFace*) meshbody->GetTrglFaceList().GetNext(Pos);
						tmp->selected=false;
					}
					trglFace->selected=true;
					pview3d->RedrawAll();
					pview3d->refresh();
					break;
				default:
					break;
			}
			return 1;
		}

	}

	if (even_type==KEY_PRESS)
	{
		if (pe.nChar == VK_DELETE){ //DEL 
			for (GLKPOSITION Pos = meshbody->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
				QMeshFace *face = (QMeshFace*) meshbody->GetTrglFaceList().GetNext(Pos);
				if(face->selected){
					bool t1 = face->GetLeftTetra(), t2 = face->GetRightTetra();
					if (t1) meshbody->deleteTetra(face->GetLeftTetra());
					if (t2) meshbody->deleteTetra(face->GetRightTetra());
					break;
				}
			}
			pview3d->RedrawAll();
			pview3d->refresh();
		}

	}

	if (MTool)
		return MTool->process_event(even_type, pe);
	else return 1;
}

int SelectFaceTool::process_event_tetra(mouse_event_type even_type, const pick_event& pe){
	//left click
	if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags & MK_LBUTTON))
	{
        double xxxx,yyyy,xx,yy,zz,t,uu,vv,ww;
		double pp[3],nv[3],p1[3],p2[3],p3[3];
		QMeshFace* trglFace;
		xxxx = pe.x;
		yyyy = pe.y;
		
		pview3d->screen_to_wcl(xxxx,yyyy,pp[0],pp[1],pp[2]);
		pview3d->GetViewVector(nv[0],nv[1],nv[2]);
		trglFace=CalProjectedTrgl(pp[0],pp[1],pp[2],nv[0],nv[1],nv[2],t,uu,vv,ww);

		if (trglFace){
			switch (pe.nFlags){
				case (MK_LBUTTON):{
					for (GLKPOSITION Pos = meshbody->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
						QMeshTetra *tmp = (QMeshTetra*) meshbody->GetTrglTetraList().GetNext(Pos);
						if (tmp->selected){
							tmp->selected=false;
							for (int i=1; i<=4; i++)
								tmp->GetFaceRecordPtr(i)->selected=false;
						}
					}
					QMeshTetra* tetra[2] = {trglFace->GetLeftTetra(), trglFace->GetRightTetra()};

					for (int k=0; k<2; k++)
						if (tetra[k]){
							tetra[k]->selected=true;
							for (int i=1; i<=4; i++)
								tetra[k]->GetFaceRecordPtr(i)->selected=true;
						}
					pview3d->RedrawAll();
					pview3d->refresh();
					break;
				}
				case (MK_SHIFT|MK_LBUTTON):{
					QMeshTetra* tetra[2] = {trglFace->GetLeftTetra(), trglFace->GetRightTetra()};
					for (int k=0; k<2; k++)
						if (tetra[k]){
							tetra[k]->selected=true;
							for (int i=1; i<=4; i++)
								tetra[k]->GetFaceRecordPtr(i)->selected=true;
						}
					pview3d->RedrawAll();
					pview3d->refresh();
					break;
				}
				default:
					break;
			}
			return 1;
		}

	}

	if (even_type==KEY_PRESS)
	{
		if (pe.nChar == VK_DELETE){ //DEL 
			for (GLKPOSITION Pos = meshbody->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
				QMeshTetra *tetra = (QMeshTetra*) meshbody->GetTrglTetraList().GetNext(Pos);
				if(tetra->selected){
					for (int i=1; i<=4; i++)
						tetra->GetFaceRecordPtr(i)->selected=false;
					meshbody->deleteTetra(tetra);
				}
			}
			pview3d->RedrawAll();
			pview3d->refresh();
		}

	}

	if (MTool)
		return MTool->process_event(even_type, pe);
	else return 1;
}

int SelectFaceTool::process_event_facenode(mouse_event_type even_type, const pick_event& pe){
	//left click
	if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags & MK_LBUTTON))
	{
        double xxxx,yyyy,xx,yy,zz,t,uu,vv,ww;
		double pp[3],nv[3],p1[3],p2[3],p3[3];
		QMeshFace* trglFace;
		xxxx = pe.x;
		yyyy = pe.y;
		
		pview3d->screen_to_wcl(xxxx,yyyy,pp[0],pp[1],pp[2]);
		pview3d->GetViewVector(nv[0],nv[1],nv[2]);
		trglFace=CalProjectedTrgl(pp[0],pp[1],pp[2],nv[0],nv[1],nv[2],t,uu,vv,ww);

		//select face node
		if (trglFace)
		{   
			trglFace->GetNodePos(1,xx,yy,zz);		p1[0]=xx;	p1[1]=yy;	p1[2]=zz;
			trglFace->GetNodePos(2,xx,yy,zz);		p2[0]=xx;	p2[1]=yy;	p2[2]=zz;
			trglFace->GetNodePos(3,xx,yy,zz);		p3[0]=xx;	p3[1]=yy;	p3[2]=zz;

			pp[0] = uu*p1[0]+vv*p2[0]+ww*p3[0];
			pp[1] = uu*p1[1]+vv*p2[1]+ww*p3[1];
			pp[2] = uu*p1[2]+vv*p2[2]+ww*p3[2];
		}

		if (trglFace){
			switch (pe.nFlags){
				case (MK_LBUTTON):
					
					pview3d->RedrawAll();
					pview3d->refresh();
					break;
				//case (MK_SHIFT|MK_LBUTTON):
				//	pview3d->RedrawAll();
				//	pview3d->refresh();
				//	break;
				default:
					break;
			}
			return 1;
		}

	}


	//right click
	if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags & MK_RBUTTON))
	{
	}



	if (even_type==KEY_PRESS)
	{
		//if (pe.nChar == VK_ESCAPE) // ESC
		//{
		//	return 2;
		//}

		if (pe.nChar == VK_DELETE){ //DEL 
			pview3d->RedrawAll();
			pview3d->refresh();
		}

	}

	if (MTool)
		return MTool->process_event(even_type, pe);
	else return 1;
}