// CH_SpecifyHideTool.h: interface for the CCH_SpecifyHideTool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _REGIONSELECTION_
#define _REGIONSELECTION_

#include <afx.h>
#include "../GLKernel/MouseTool.h"

class CGLKernelView;
class QBody;

class RegionSelectionTool : public MouseTool  
{
public:
	RegionSelectionTool(CGLKernelView *cview3d, QMeshPatch *body);
	virtual ~RegionSelectionTool();
 	virtual int process_event(mouse_event_type even_type, const pick_event& pe);
	virtual int process_done_event();

private:
	CGLKernelView* pview3d;
	QMeshPatch* meshbody;
	position_array *m_positionArray;
	double oldX,oldY,xxxx,yyyy;

	void draw_position_array();
};

#endif
