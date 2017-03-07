
//#include "..\MeshWorks\stdafx.h"
//#include "..\MeshWorks\MeshWorks.h"

#include "../MeshEntities/QBody.h"
//#include "../GLKernel/CW_Geometry.h"
#include "../GLKernel/GLKernelView.h"
//#include "../GLKLib/GLKGeometry.h"
//
#include "..\MeshWorks\MeshWorksDoc.h"
#include "..\MeshWorks\MeshWorksView.h"
//#include "..\MeshWorks\MainFrm.h"

#include "..\GLKernel\CameraTool.h"

#include "InterBoard.h"
#include "SelectBoardTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SelectBoardTool::SelectBoardTool(CMeshWorksView *_cview, QMeshPatch *patch)
{
	cview = _cview;
	pview3d = cview->GetGLKernelView();	
	m_patch = patch;
	selectedBoard=NULL;
	board=NULL;
	cameratool = new CameraTool(pview3d, ORBITPAN);


	AddBoardToDrawlist();

	pview3d->RedrawAll();
	pview3d->refresh();
}

SelectBoardTool::~SelectBoardTool(void)
{
	DelBoardFromDrawlist();

	delete cameratool;

	//---------------------------------------------------------------------
	//	Fill in the flags
	for (GLKPOSITION Pos = m_patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
		QMeshFace* face = (QMeshFace*)m_patch->GetTrglFaceList().GetNext(Pos);
		if (face->GetLeftTetra() && face->GetRightTetra()) {face->inner=true;} else face->inner=false;
	}
	for(GLKPOSITION Pos=m_patch->GetTrglEdgeList().GetHeadPosition();Pos!=NULL;) {
		QMeshEdge *edge=(QMeshEdge *)(m_patch->GetTrglEdgeList().GetNext(Pos));
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {edge->inner=false; break;}
		}
	}
	for(GLKPOSITION Pos=m_patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode *)(m_patch->GetTrglNodeList().GetNext(Pos));
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace *)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner) {node->inner=false; break;}
		}
	}

	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(GLKPOSITION Pos=m_patch->GetTrglFaceList().GetHeadPosition();Pos!=NULL;) {
		QMeshFace *face=(QMeshFace*)(m_patch->GetTrglFaceList().GetNext(Pos));
		if (face->inner) continue;
		face->CalPlaneEquation();
		//face->CalCenterPos();
	}
	for(GLKPOSITION Pos=m_patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;) {
		QMeshNode* node=(QMeshNode*)(m_patch->GetTrglNodeList().GetNext(Pos));
		node->CalNormal();
	}

	pview3d->RedrawAll();
	//pview3d->refresh();
}


int SelectBoardTool::process_event(mouse_event_type even_type, const pick_event& pe){
	//move and highlight
	if (even_type==MOUSE_MOVE && !pe.nFlags){
		GLEntity *ent;
		if (pview3d->pick_entity(pe,8,ent)) 
		{
			//InterBoard* selected=(InterBoard*)ent;
			//selected->drawHighLight();
			pview3d->HighLightObj(ent);
		} else pview3d->HighLightObj(NULL);
	}
	//left click to select
	if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags == MK_LBUTTON))
	{
		GLEntity *ent;
		if (pview3d->pick_entity(pe,8,ent)) 
		{
			//CMainFrame* pFrame = (CMainFrame*)(AfxGetMainWnd());
			selectedBoard = (InterBoard*)ent;
			pview3d->HighLightObj(ent);
			old_x = pe.x; old_y = pe.y; new_x = pe.x; new_y = pe.y;
		}
	}
	//deselect
	if ((even_type==MOUSE_BUTTON_UP) && !(pe.nFlags & MK_LBUTTON) && selectedBoard){
		pview3d->HighLightObj(NULL);
		selectedBoard = NULL;
		UpdateMesh();
	}
	//move the board
	if (even_type==MOUSE_MOVE && (pe.nFlags == MK_LBUTTON) && selectedBoard){
		new_x = pe.x;
		new_y = pe.y;
		MoveModel(new_x,new_y,old_x,old_y);
		pview3d->refresh();
		old_x = new_x;
		old_y = new_y;
		return 1;
	}

	if (even_type==KEY_PRESS)
	{

	}

	return cameratool->process_event(even_type, pe);
}

void SelectBoardTool::MoveModel(double x, double y,double oldx,double oldy)
{
	if (!selectedBoard)
	{
		return;
	}

	int direction = -1;

	if (selectedBoard->n[0] == 1)
		direction = 1;
	else if (selectedBoard->n[0] == -1)
		direction = 3;
	else if (selectedBoard->n[1] == 1)
		direction = 4;
	else if (selectedBoard->n[1] == -1)
		direction = 5;
	else if (selectedBoard->n[2] == 1)
		direction = 0;
	else if (selectedBoard->n[2] == -1)
		direction = 2;

	if (direction == -1){
		printf("(MoveModel)Error: direction == -1\n");
		return;
	}

	double pos[3],pos1[3];
	pview3d->screen_to_wcl(x,y,pos[0],pos[1],pos[2]);
	pview3d->screen_to_wcl(oldx,oldy,pos1[0],pos1[1],pos1[2]);

	double change[3];
	for (int i=0; i<3; i++){
		change[i] = (pos[i]-pos1[i]);
	}

	switch (direction){
		case 0:
			max_z+=change[2];
			if (max_z>maxZ) max_z=maxZ;
			if (max_z<min_z) max_z=min_z;
			break;
		case 1:
			max_x+=change[0];
			if (max_x>maxX) max_x=maxX;
			if (max_x<min_x) max_x=min_x;
			break;
		case 2:
			min_z+=change[2];
			if (min_z<minZ) min_z=minZ;
			if (min_z>max_z) min_z=max_z;
			break;
		case 3:
			min_x+=change[0];
			if (min_x<minX) min_x=minX;
			if (min_x>max_x) min_x=max_x;
			break;
		case 4:
			max_y+=change[1];
			if (max_y>maxY) max_y=maxY;
			if (max_y<min_y) max_y=min_y;
			break;
		case 5:
			min_y+=change[1];
			if (min_y<minY) min_y=minY;
			if (min_y>max_y) min_y=max_y;
			break;
		default:
			break;
	}

	UpdateBoards();

	//UpdateMesh();

	return;
}

void SelectBoardTool::UpdateMesh(){
	if (m_patch->isTET){
		for (GLKPOSITION Pos = m_patch->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
			QMeshTetra* tetra = (QMeshTetra*)m_patch->GetTrglTetraList().GetNext(Pos);
			tetra->inner=false;
		}
		for (GLKPOSITION Pos = m_patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
			QMeshNode* node = (QMeshNode*)m_patch->GetTrglNodeList().GetNext(Pos);
			double x, y, z;
			node->GetCoord3D(x, y, z);
			if (x>max_x || x<min_x || y>max_y || y<min_y || z>max_z || z<min_z){
				for (GLKPOSITION Pos1 = node->GetTrglTetraList().GetHeadPosition(); Pos1!=NULL;){
					QMeshTetra *tetra = (QMeshTetra*)node->GetTrglTetraList().GetNext(Pos1);
					tetra->inner=true;
				}
			}
		}
		for (GLKPOSITION Pos = m_patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
			QMeshFace* face = (QMeshFace*)m_patch->GetTrglFaceList().GetNext(Pos);
			face->inner = false;
			if (face->GetLeftTetra() && face->GetRightTetra() && (face->GetLeftTetra()->inner == face->GetRightTetra()->inner)){
					face->inner=true;
			}
			if (face->GetLeftTetra() && !face->GetRightTetra() && face->GetLeftTetra()->inner){
				face->inner=true;
			}
			if (face->GetRightTetra() && !face->GetLeftTetra() && face->GetRightTetra()->inner){
				face->inner=true;
			}
		}
	} else {
		for (GLKPOSITION Pos = m_patch->GetTrglHexaList().GetHeadPosition(); Pos!=NULL;){
			QMeshHexa* Hexa = (QMeshHexa*)m_patch->GetTrglHexaList().GetNext(Pos);
			Hexa->inner=false;
		}
		for (GLKPOSITION Pos = m_patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
			QMeshNode* node = (QMeshNode*)m_patch->GetTrglNodeList().GetNext(Pos);
			double x, y, z;
			node->GetCoord3D(x, y, z);
			if (x>max_x || x<min_x || y>max_y || y<min_y || z>max_z || z<min_z){
				for (GLKPOSITION Pos1 = node->GetTrglHexaList().GetHeadPosition(); Pos1!=NULL;){
					QMeshHexa *Hexa = (QMeshHexa*)node->GetTrglHexaList().GetNext(Pos1);
					Hexa->inner=true;
				}
			}
		}
		for (GLKPOSITION Pos = m_patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
			QMeshFace* face = (QMeshFace*)m_patch->GetTrglFaceList().GetNext(Pos);
			face->inner = false;
			if (face->GetLeftHexa() && face->GetRightHexa() && (face->GetLeftHexa()->inner == face->GetRightHexa()->inner)){
					face->inner=true;
			}
			if (face->GetLeftHexa() && !face->GetRightHexa() && face->GetLeftHexa()->inner){
				face->inner=true;
			}
			if (face->GetRightHexa() && !face->GetLeftHexa() && face->GetRightHexa()->inner){
				face->inner=true;
			}
		}
	}



	for (GLKPOSITION Pos = m_patch->GetTrglEdgeList().GetHeadPosition(); Pos!=NULL;){
		QMeshEdge* edge = (QMeshEdge*)m_patch->GetTrglEdgeList().GetNext(Pos);
		edge->inner=true;
		for (GLKPOSITION Pos1 = edge->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace*)edge->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner){
				edge->inner = false;
				break;
			}
		}
	}
	for (GLKPOSITION Pos = m_patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
		QMeshNode *node = (QMeshNode*)m_patch->GetTrglNodeList().GetNext(Pos);
		node->inner=true;
		for (GLKPOSITION Pos1 = node->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace *face = (QMeshFace*)node->GetTrglFaceList().GetNext(Pos1);
			if (!face->inner){
				node->inner = false;
				break;
			}
		}
	}
	//---------------------------------------------------------------------
	//	Step 2: compute the normal
	for(GLKPOSITION Pos=m_patch->GetTrglFaceList().GetHeadPosition();Pos!=NULL;) {
		QMeshFace *face=(QMeshFace*)(m_patch->GetTrglFaceList().GetNext(Pos));
		if (face->inner) continue;
		face->CalPlaneEquation();
		face->CalCenterPos();
	}
	for(GLKPOSITION Pos=m_patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;) {
		QMeshNode *node=(QMeshNode*)(m_patch->GetTrglNodeList().GetNext(Pos));
		node->CalNormal();
	}
	pview3d->RedrawAll();
}


void SelectBoardTool::AddBoardToDrawlist()
{
	if (m_patch->GetTrglNodeList().GetCount()==0) return;

	min_x=9e+15; max_x=-9e+15; min_y=9e+15; max_y=-9e+15; min_z=9e+15; max_z=-9e+15;
	
	for (GLKPOSITION Pos = m_patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
		QMeshNode * node = (QMeshNode*)m_patch->GetTrglNodeList().GetNext(Pos);
		double x, y, z;
		node->GetCoord3D(x, y, z);
		if (min_x>x) min_x=x;
		if (min_y>y) min_y=y;
		if (min_z>z) min_z=z;
		if (max_x<x) max_x=x;
		if (max_y<y) max_y=y;
		if (max_z<z) max_z=z;
	}

	min_x-=1e-10; min_y-=1e-10; min_z-=1e-10;
	max_x+=1e-10; max_y+=1e-10; max_z+=1e-10;

	maxX=max_x; minX=min_x;
	maxY=max_y; minY=min_y;
	maxZ=max_z; minZ=min_z;

	board = new InterBoard*[6];
	for (int i=0; i<6; i++){
		board[i]=new InterBoard();
		board[i]->m_ID = i;
		pview3d->AddDisplayObj(board[i]);
	}

	UpdateBoards(/*min_x, max_x, min_y, max_y, min_z, max_z*/);

}

void SelectBoardTool::UpdateBoards(/*float min_x, float max_x, float min_y, float max_y, float min_z, float max_z*/){
	int b = 0; //front board
	board[b]->n[0] = 0;	board[b]->n[1] = 0;	board[b]->n[2] = 1;
	board[b]->x[0] = min_x;	board[b]->x[1] = max_x;	board[b]->x[2] = max_x;	board[b]->x[3] = min_x;
	board[b]->y[0] = min_y; board[b]->y[1] = min_y;	board[b]->y[2] = max_y;	board[b]->y[3] = max_y;
	board[b]->z[0] = max_z; board[b]->z[1] = max_z;	board[b]->z[2] = max_z;	board[b]->z[3] = max_z;

	b = 1; //right board
	board[b]->n[0] = 1;	board[b]->n[1] = 0;	board[b]->n[2] = 0;
	board[b]->x[0] = max_x;	board[b]->x[1] = max_x;	board[b]->x[2] = max_x;	board[b]->x[3] = max_x;
	board[b]->y[0] = min_y; board[b]->y[1] = min_y;	board[b]->y[2] = max_y;	board[b]->y[3] = max_y;
	board[b]->z[0] = max_z; board[b]->z[1] = min_z;	board[b]->z[2] = min_z;	board[b]->z[3] = max_z;

	b = 2; //back board
	board[b]->n[0] = 0;	board[b]->n[1] = 0;	board[b]->n[2] = -1;
	board[b]->x[0] = max_x;	board[b]->x[1] = min_x;	board[b]->x[2] = min_x;	board[b]->x[3] = max_x;
	board[b]->y[0] = min_y; board[b]->y[1] = min_y;	board[b]->y[2] = max_y;	board[b]->y[3] = max_y;
	board[b]->z[0] = min_z; board[b]->z[1] = min_z;	board[b]->z[2] = min_z;	board[b]->z[3] = min_z;

	b = 3; //left board
	board[b]->n[0] = -1;	board[b]->n[1] = 0;	board[b]->n[2] = 0;
	board[b]->x[0] = min_x;	board[b]->x[1] = min_x;	board[b]->x[2] = min_x;	board[b]->x[3] = min_x;
	board[b]->y[0] = min_y; board[b]->y[1] = min_y;	board[b]->y[2] = max_y;	board[b]->y[3] = max_y;
	board[b]->z[0] = min_z; board[b]->z[1] = max_z;	board[b]->z[2] = max_z;	board[b]->z[3] = min_z;

	b = 4; //top board
	board[b]->n[0] = 0;	board[b]->n[1] = 1;	board[b]->n[2] = 0;
	board[b]->x[0] = min_x;	board[b]->x[1] = max_x;	board[b]->x[2] = max_x;	board[b]->x[3] = min_x;
	board[b]->y[0] = max_y; board[b]->y[1] = max_y;	board[b]->y[2] = max_y;	board[b]->y[3] = max_y;
	board[b]->z[0] = max_z; board[b]->z[1] = max_z;	board[b]->z[2] = min_z;	board[b]->z[3] = min_z;

	b = 5; //bottom board
	board[b]->n[0] = 0;	board[b]->n[1] = -1;	board[b]->n[2] = 0;
	board[b]->x[0] = max_x;	board[b]->x[1] = min_x;	board[b]->x[2] = min_x;	board[b]->x[3] = max_x;
	board[b]->y[0] = min_y; board[b]->y[1] = min_y;	board[b]->y[2] = min_y;	board[b]->y[3] = min_y;
	board[b]->z[0] = max_z; board[b]->z[1] = max_z;	board[b]->z[2] = min_z;	board[b]->z[3] = min_z;

	cview->UpdateCrossSectionDialog(min_x, max_x, min_y, max_y, min_z, max_z);
}

void SelectBoardTool::DelBoardFromDrawlist()
{
	if (board)
	{
		for (int i = 0; i <6; ++i)
		{
			pview3d->DelDisplayObj2(board[i]);
		
			delete board[i];
			
		}
		delete [] board;
	}
	
}