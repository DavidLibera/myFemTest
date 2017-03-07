/////////////////////////////////////////////////////////////////////////////
//
//	MouseTool - the kernel class of OpenGL display & manipulation
//
//		written by Charlie Wang (Jan 30th, 2000)
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CW_MOUSETOOL
#define CW_MOUSETOOL

#include <afx.h>

typedef enum mouse_event_type { MOUSE_BUTTON_DOWN, MOUSE_BUTTON_UP, MOUSE_MOVE, KEY_PRESS};

/////////////////////////////////////////////////////////////////////////////
//
//	The following definition are for the "nFlag " in the pick_event. (Defined by MFC)
//
//		MK_CONTROL   //Set if the CTRL key is down.
//		MK_LBUTTON   //Set if the left mouse button is down.
//		MK_MBUTTON   //Set if the middle mouse button is down.
//		MK_RBUTTON   //Set if the right mouse button is down.
//		MK_SHIFT	 //Set if the SHIFT key is down.

typedef struct {
	double x,y;
	UINT nFlags;
	UINT nChar;
} pick_event;

class MouseTool : public CObject  
{
public:
	MouseTool() {};
	virtual ~MouseTool() {};

public:
	// Implement the virtual method which processes the button events
	// The default implementation maps the pick_event into a position
	// and then calls the process_position_event method
	virtual int process_event(mouse_event_type even_type, const pick_event& pe) {return 0;};	
	virtual	int process_done_event() {return 0;};
	virtual void draw() {};
};

#endif
