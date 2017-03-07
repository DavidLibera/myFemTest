// HighLightTool.h: interface for the HighLightTool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CW_HIGHLIGHTTOOL
#define CW_HIGHLIGHTTOOL

#include "MouseTool.h"

class CGLKernelView;

class HighLightTool : public MouseTool  
{
public:
	HighLightTool(CGLKernelView *cView) {pView=cView;};
	virtual ~HighLightTool() {};

private:
	CGLKernelView *pView;

public:
	// Implement the virtual method which processes the button events
	// The default implementation maps the pick_event into a position
	// and then calls the process_position_event method
	virtual int process_event(mouse_event_type even_type, const pick_event& pe)
	{
		GLEntity* ent;

		if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
			if (pView->pick_entity(pe,0,ent)) pView->HighLightObj(ent);

		return 0;
	};

};

#endif
