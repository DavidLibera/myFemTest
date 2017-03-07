#pragma once
#include "..\glkernel\mousetool.h"

class CGLKernelView;
class CMeshWorksView;
class QBody;
class QMeshPatch;
class InterBoard;
class CameraTool;

class SelectBoardTool :
	public MouseTool
{
public:
	SelectBoardTool(CMeshWorksView *_cview, QMeshPatch *patch);
	~SelectBoardTool(void);

	virtual int process_event(mouse_event_type even_type, const pick_event& pe);
	//virtual int process_done_event();


	InterBoard **board;
	//0 - front
	//1 - right
	//2 - back
	//3 - left
	//4 - top
	//5 - bottom
	float min_x, max_x, min_y, max_y, min_z, max_z; //bounding box
	float minX, maxX, minY, maxY, minZ, maxZ; //initial bounding box

private:
	CMeshWorksView* cview;
	CGLKernelView* pview3d;
	QMeshPatch* m_patch;

	CameraTool* cameratool;

	InterBoard *selectedBoard;

	void AddBoardToDrawlist();
	void DelBoardFromDrawlist();

private:
	double new_x, new_y, old_x, old_y;
	void MoveModel(double x, double y,double oldx,double oldy);

public:
	void UpdateBoards(/*float min_x, float max_x, float min_y, float max_y, float min_z, float max_z*/);
	void UpdateMesh();
};
