#include "..\MeshWorks\StdAfx.h"

#include "..\MeshWorks\MainFrm.h"
#include "..\MeshWorks\MeshWorksDoc.h"


#include "SelectNodeTool.h"
#include "InterativeNode.h"


//#include "..\MeshWorks\MeshWorksDoc.h"
//#include "..\MeshWorks\MainFrm.h"

#include "..\MeshEntities\QBody.h"

#include "..\GLKernel\CW_Geometry.h"
#include "..\GLKernel\GLKernelView.h"
#include "..\GLKernel\position_array.h"
#include "..\GLKLib\GLKObList.h"



SelectNodeTool::SelectNodeTool(CGLKernelView *cview3d, QMeshPatch *patch, MouseTool* tool)
{
	pview3d = cview3d;	
	m_patch = patch;	
	mouseflag1 = false; 
	selectedNode=NULL;
	nPhase = 0;
	
	MTool = tool;

	AddNodeToDrawlist();

	pview3d->RedrawAll();
	pview3d->refresh();
}

SelectNodeTool::~SelectNodeTool()
{
	nPhase = 0;
	DelNodeFromDrawlist();

	if (MTool) delete MTool;

	pview3d->RedrawAll();
//	pview3d->refresh();
}


int SelectNodeTool::process_event(mouse_event_type even_type, const pick_event& pe)
{
	GLEntity *ent;

	if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
	{
		if (pview3d->pick_entity(pe,100,ent)) 
		{
			CMainFrame* pFrame = (CMainFrame*)(AfxGetMainWnd());

			InterativeNode* selected=(InterativeNode*)ent;

			double x, y, z;
			selected->m_node->GetCoord3D(x, y, z);

			// Added for FEA
			int NodeNumber = selected->m_node->GetIndexNo();

			CString str;
			str.Format("Selected node, index number %d, coordinate: %f, %f, %f", NodeNumber,x, y, z);
			AfxMessageBox(str);


			return 1;
		}
	}

	if (MTool)
		return MTool->process_event(even_type, pe);
	else return 1;
}

int SelectNodeTool::process_done_event()
{
	if (!selectedNode) 
		return 0;

	selectedNode->m_ID;
	m_patch->selectedNode = selectedNode->m_node;

	return 1;
}

void SelectNodeTool::AddNodeToDrawlist()
{
	m_totalNodeNum = m_patch->GetTrglNodeNumber();
	m_node = new InterativeNode*[m_totalNodeNum];

	int i = 0;
	for (GLKPOSITION PosNode = m_patch->GetTrglNodeList().GetHeadPosition(); PosNode!= NULL;  ++i)
	{
		QMeshNode *node = (QMeshNode *)(m_patch->GetTrglNodeList().GetNext(PosNode));

		m_node[i] = new InterativeNode(node);
		m_node[i]->entityType = 100;
		m_node[i]->m_ID = i;

		pview3d->AddDisplayObj(m_node[i]);
	}

}

void SelectNodeTool::DelNodeFromDrawlist()
{
	if (m_node)
	{
		for (int i = 0; i <m_totalNodeNum; ++i)
		{
			pview3d->DelDisplayObj2(m_node[i]);
		
			delete m_node[i];
			
		}
		delete [] m_node;
	}
	
}