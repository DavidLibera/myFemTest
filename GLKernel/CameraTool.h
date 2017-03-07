/////////////////////////////////////////////////////////////////////////////
//
//	CameraTool - the kernel class of OpenGL display & manipulation
//
//		written by Charlie Wang (Jan 30th, 2000)
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CW_CAMERATOOL
#define CW_CAMERATOOL

#include "MouseTool.h"
#include <math.h>

class CGLKernelView;

typedef enum camera_type {ORBIT,PAN,ZOOM,ORBITPAN,ZOOMWINDOW};

class CameraTool : public MouseTool
{
public:
	CameraTool(CGLKernelView *cView, camera_type ct)
	{
		pView=cView;
		m_ct=ct;
		oldX = -9e+13;
	}

	virtual ~CameraTool() {};

private:
	CGLKernelView *pView;
	camera_type m_ct;
	double oldX,oldY;	double xxxx,yyyy;

public:
	// Implement the virtual method which processes the button events
	// The default implementation maps the pick_event into a position
	// and then calls the process_position_event method
	virtual int process_event(mouse_event_type even_type, const pick_event& pe)
	{
		switch(m_ct)
		{
		case ORBITPAN:{
					if (even_type==MOUSE_BUTTON_UP)
						oldX=-9e+13;
					if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
					{	oldX=pe.x;	oldY=pe.y;	}
					if ((even_type==MOUSE_MOVE) && (pe.nFlags==MK_LBUTTON))
					{
						if (oldX==-9e+13) break;
						float xR,yR;

						pView->GetRotation(xR,yR);

						double sx,sy;
						double cx,cy;
						pView->wcl_to_screen(0.0,0.0,0.0,sx,sy);
						pView->wcl_to_screen(0.0,1.0,0.0,cx,cy);
						if (cy>=sy)
							yR += (float)(oldX - pe.x)/2;
						else
							yR -= (float)(oldX - pe.x)/2;

						xR -= (float)(oldY - pe.y)/2;
						pView->SetRotation(xR,yR);
						oldX=pe.x;	oldY=pe.y;

						pView->refresh();
					}
					if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_RBUTTON))
					{	oldX=pe.x;	oldY=pe.y;	}
					if ((even_type==MOUSE_MOVE) && (pe.nFlags==MK_RBUTTON))
					{
						if(oldX==-9e+13) break;
						float xR,yR,zR;
						float mappingScale=pView->m_MappingScale;

						pView->GetTranslation(xR,yR,zR);
						xR -= (float)(oldX - pe.x)/mappingScale;
						yR += (float)(oldY - pe.y)/mappingScale;
						pView->SetTranslation(xR,yR,zR);
						oldX=pe.x;	oldY=pe.y;

						pView->refresh();
					}
					if (even_type==KEY_PRESS)
					{
						if (pe.nChar==VK_LEFT)	//	LEFT_KEY
						{
							float xR,yR;
							pView->GetRotation(xR,yR);
							pView->SetRotation(xR,yR-10);
							pView->refresh();
						}
						if (pe.nChar==VK_RIGHT)	//	RIGHT_KEY
						{
							float xR,yR;
							pView->GetRotation(xR,yR);
							pView->SetRotation(xR,yR+10);
							pView->refresh();
						}
						if (pe.nChar==VK_UP)	//	UP_KEY
						{
							float xR,yR;
							pView->GetRotation(xR,yR);
							pView->SetRotation(xR-10,yR);
							pView->refresh();
						}
						if (pe.nChar==VK_DOWN)	//	DOWN_KEY
						{
							float xR,yR;
							pView->GetRotation(xR,yR);
							pView->SetRotation(xR+10,yR);
							pView->refresh();
						}
					}
				   }break;
		case ORBIT:{
					if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
					{	oldX=pe.x;	oldY=pe.y;	}
					if ((even_type==MOUSE_MOVE) && (pe.nFlags==MK_LBUTTON))
					{
						float xR,yR;

						pView->GetRotation(xR,yR);

						double sx,sy;
						double cx,cy;
						pView->wcl_to_screen(0.0,0.0,0.0,sx,sy);
						pView->wcl_to_screen(0.0,1.0,0.0,cx,cy);
						if (cy>=sy)
							yR += (float)(oldX - pe.x)/2;
						else
							yR -= (float)(oldX - pe.x)/2;

						xR -= (float)(oldY - pe.y)/2;
						pView->SetRotation(xR,yR);
						oldX=pe.x;	oldY=pe.y;

						pView->refresh();
					}
				   }break;
		case PAN:  {
					if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
					{	oldX=pe.x;	oldY=pe.y;	}
					if ((even_type==MOUSE_MOVE) && (pe.nFlags==MK_LBUTTON))
					{
						float xR,yR,zR;
						float mappingScale=pView->m_MappingScale;

						pView->GetTranslation(xR,yR,zR);
						xR -= (float)(oldX - pe.x)/mappingScale;
						yR += (float)(oldY - pe.y)/mappingScale;
						pView->SetTranslation(xR,yR,zR);
						oldX=pe.x;	oldY=pe.y;

						pView->refresh();
					}
				   }break;
		case ZOOM: {
					if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
						oldY=pe.y;
					if ((even_type==MOUSE_MOVE) && (pe.nFlags==MK_LBUTTON))
					{
						float scale;

						pView->GetScale(scale);
						scale = scale + ((float)(oldY - pe.y)/400.0f);
						if (scale<0.0001) scale=0.0001f;
						pView->SetScale(scale);
						oldY=pe.y;
						pView->refresh();
					}
					if (even_type==MOUSE_BUTTON_UP) m_ct=ORBITPAN;
				   }break;
		case ZOOMWINDOW: {
					if ((even_type==MOUSE_BUTTON_DOWN) && (pe.nFlags==MK_LBUTTON))
					{oldX=pe.x;oldY=pe.y;xxxx=pe.x;yyyy=pe.y;}
					if ((even_type==MOUSE_MOVE) && (pe.nFlags==MK_LBUTTON))
					{
						pView->set_foreground_color(0.65f,0.65f,0.65f);
						float pnts[10];

						pnts[0]=(float)oldX;	pnts[1]=(float)oldY;
						pnts[2]=(float)xxxx;	pnts[3]=(float)oldY;
						pnts[4]=(float)xxxx;	pnts[5]=(float)yyyy;
						pnts[6]=(float)oldX;	pnts[7]=(float)yyyy;
						pnts[8]=(float)oldX;	pnts[9]=(float)oldY;
						pView->draw_polyline_2d(5,pnts);
						xxxx=pe.x;	yyyy=pe.y;

						pnts[0]=(float)oldX;	pnts[1]=(float)oldY;
						pnts[2]=(float)xxxx;	pnts[3]=(float)oldY;
						pnts[4]=(float)xxxx;	pnts[5]=(float)yyyy;
						pnts[6]=(float)oldX;	pnts[7]=(float)yyyy;
						pnts[8]=(float)oldX;	pnts[9]=(float)oldY;
						pView->draw_polyline_2d(5,pnts);
					}					
					if (even_type==MOUSE_BUTTON_UP)
					{
						double cx,cy,xx,yy;	int sx,sy;
						float xR,yR,zR;	float scale,sc;

						cx = fabs(oldX - pe.x);		cy = fabs(oldY - pe.y);
						if ((cx>0) && (cy>0))
						{
							pView->GetSize(sx,sy);
							scale=(float)(sx/cx);		sc=(float)(sy/cy);
							if (sc<scale) scale=sc;
							pView->GetScale(sc);	sc=sc*scale;	pView->SetScale(sc);

							float mappingScale=pView->m_MappingScale;

							cx = (oldX + pe.x)/2.0;		cy = (oldY + pe.y)/2.0;
							pView->GetTranslation(xR,yR,zR);
							pView->wcl_to_screen(0.0,0.0,0.0,xx,yy);
							xR -= (float)((cx-xx)*scale+xx-sx/2.0f)/mappingScale;
							yR += (float)((cy-yy)*scale+yy-sy/2.0f)/mappingScale;
							pView->SetTranslation(xR,yR,zR);

							pView->refresh();
							m_ct=ORBITPAN;
						}
					}
				}break;
		}

		return 0;
	}
};

#endif
