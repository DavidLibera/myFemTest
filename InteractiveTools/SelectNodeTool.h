#pragma once
#include "..\glkernel\mousetool.h"

class CGLKernelView;
class QBody;
class QMeshPatch;

class InterativeNode;


class QMeshNode;
class GLKObList;


class SelectNodeTool :  public MouseTool
{
public:
	SelectNodeTool(CGLKernelView *cview3d, QMeshPatch *patch, MouseTool* tool);
	virtual ~SelectNodeTool(void);

	virtual int process_event(mouse_event_type even_type, const pick_event& pe);
	virtual int process_done_event();

private:
	int nPhase;
	CMeshWorksDoc* pDoc;
	CGLKernelView* pview3d;
	QMeshPatch* m_patch;
	bool mouseflag1;
	InterativeNode **m_node;
	InterativeNode *selectedNode;
	int m_totalNodeNum;

	//CameraTool* cameratool;
	MouseTool* MTool;

	void AddNodeToDrawlist();
	void DelNodeFromDrawlist();

};

