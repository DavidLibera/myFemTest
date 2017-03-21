/////////////////////////////////////////////////////////////////////////////
//	CGLKernelView - the kernel class of OpenGL display & manipulation
//
//		written by Charlie Wang (Jan 28th, 2000)
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CW_CGLKERNELVIEW
#define CW_CGLKERNELVIEW

#include <afxwin.h>

#include "GLEntity.h"

/////////////////////////////////////////////////////////////////////////////
//	The following IDs are for the view direction
#define VD_FRONTVIEW			0
#define VD_LEFTVIEW				1
#define VD_RIGHTVIEW			2
#define VD_BACKVIEW				3
#define VD_TOPVIEW				4
#define	VD_BOTTOMVIEW			5
#define VD_ISOMETRICVIEW		6
#define VD_BACKISOMETRICVIEW	7

#include "MouseTool.h"

class GLEntity;
class GLList;
class position_array;
//class GLKObList;


class CGLKernelView : public CView
{
public:
	CGLKernelView(CWnd *pWnd);  // protected constructor used by dynamic creation
	virtual ~CGLKernelView();

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, int cx, int cy);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnMouseMove(UINT nFlags, CPoint point); 
	void OnLButtonUp(UINT nFlags, CPoint point); 
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point); 
	void OnRButtonDown(UINT nFlags, CPoint point);

//	void HighLightEdge(int index, GLKObList list);

private:
	void GLInit();
	void GLDraw();
	void GLDrawAxis();
	void GLDrawPolyCount();
	void GLDrawDisplayObjList();
	void GLDrawGLList();

	void GLPickInit();
	void GLPickDraw(UINT nType);
	void GLPickDrawDisplayObjList(UINT nType);
	GLEntity* GetPickObjByName(UINT nType,UINT name);

// Attributes
public:
	HGLRC m_hRC;
	float m_MappingScale;
	CWnd *m_pWnd;
	bool m_shadeDrawn;
	bool m_nodeDrawn;
	bool m_nodeNormalDrawn;
	bool m_meshDrawn;
	bool m_lightON;
	bool m_FEAON;

private:
	MouseTool *m_currentTool;
	CObList m_displayObjList;
	CObList m_glList;
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];

private:
	void InitValue();
	void GLEnableLight();
	void GLDisableLight();

	// Position, rotation ,scaling
	int m_SizeX;
	int m_SizeY;
	float m_xRotation;
	float m_yRotation;
	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;
	float m_Scaling;
	float m_Range;
	float m_red,m_green,m_blue;
	int m_lineWidth;

	// Colors
	float m_ClearColorRed;
	float m_ClearColorGreen;
	float m_ClearColorBlue;

	//	Flags
	BOOL m_axisDisplay;
	BOOL m_Shading;
	BOOL m_Mesh;
	BOOL m_Profile;
	BOOL m_Node;
	BOOL m_NodeNormal;
	BOOL m_polyCountDisplay;

	GLEntity* m_HighLightObj;
	

	


/*---------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////
//
//			The followings are command functions which can be used by user
//
/////////////////////////////////////////////////////////////////////////////////////
public:
	MouseTool* GetCurrentTool() {return m_currentTool;};

	void GetSize(int &sx,int &sy) {sx=m_SizeX;sy=m_SizeY;};

	void SetClearColor(float r, float g, float b) 
		{m_ClearColorRed=r;m_ClearColorGreen=g;m_ClearColorBlue=b;};
	void GetClearColor(float &r, float &g, float &b) 
		{r=m_ClearColorRed;g=m_ClearColorGreen;b=m_ClearColorBlue;};

	////////////////////////////////////////////////////////////
	//	Return the parent window of this view class
	CWnd* GetParentWnd() {return m_pWnd;};

	////////////////////////////////////////////////////////////
	//	Refreshes the view.
	void refresh();

	////////////////////////////////////////////////////////////
	//	Kill everything in the view.
	void ClearAll();

	////////////////////////////////////////////////////////////
	//	Reset all drawn parameters to renew the gllist.
	void RedrawAll();

	////////////////////////////////////////////////////////////
	//	Use this method for setting up standard views.
	//
	//	which include:
	//
	//		VD_FRONTVIEW		- front view
	//		VD_LEFTVIEW			- left view
	//		VD_RIGHTVIEW		- right view
	//		VD_BACKVIEW			- back view
	//		VD_TOPVIEW			- top view
	//		VD_BOTTOMVIEW		- bottom view
	//		VD_ISOMETRICVIEW	- isometric view
	void SetViewDirection(UINT nDirID);

	////////////////////////////////////////////////////////////
	//	Scales the view
	void zoom(double ratio);
	void zoom_all_in_view();

	////////////////////////////////////////////////////////////
	//	Get the out point vector from original point
	//			to the eye point
	//	The vector is: (x, y, z)
	void GetViewVector(double &x, double &y, double &z);

	////////////////////////////////////////////////////////////
	//	Get the upwards point vector from original point,
	//			perpendicular to the vew vector
	//	The vector is: (x, y, z)
	void GetUpVector(double &x, double &y, double &z);
	
	////////////////////////////////////////////////////////////
	//	Set & Get rotate angle of display objects
	void GetRotation(float &rx, float &ry) 
		{rx=m_xRotation;ry=m_yRotation;};
	void SetRotation(float rx, float ry) 
		{m_xRotation=rx;m_yRotation=ry;};

	////////////////////////////////////////////////////////////
	//	Set & Get translation of display objects
	void GetTranslation(float &rx, float &ry, float &rz) 
		{rx=m_xTranslation;ry=m_yTranslation;rz=m_zTranslation;};
	void SetTranslation(float rx, float ry, float rz) 
		{m_xTranslation=rx;m_yTranslation=ry;m_zTranslation=rz;};

	////////////////////////////////////////////////////////////
	//	Set & Get scale ratio.
	void GetScale(float &scale);
	void SetScale(float scale);

	////////////////////////////////////////////////////////////
	//	Set & Get the display status of the Axis
	void SetAxisDisplay(BOOL bDisplay) {m_axisDisplay=bDisplay;};
	BOOL GetAxisDisplay() {return m_axisDisplay;};

	////////////////////////////////////////////////////////////
	//	Set & Get the display status of the Poly Count
	void SetPolyCountDisplay(BOOL bDisplay) {m_polyCountDisplay=bDisplay;};
	BOOL GetPolyCountDisplay() {return m_polyCountDisplay;};

	////////////////////////////////////////////////////////////
	//	Set & Get the shading display status
	void SetShading(BOOL bState) {m_Shading=bState;};
	BOOL GetShading() {return m_Shading;};

	////////////////////////////////////////////////////////////
	//	Set & Get the node display status
	void SetNode(BOOL bState) {m_Node=bState;};
	BOOL GetNode() {return m_Node;};

	////////////////////////////////////////////////////////////
	//	Set & Get the mesh display status
	void SetMesh(BOOL bState) {m_Mesh=bState;};
	BOOL GetMesh() {return m_Mesh;};

	////////////////////////////////////////////////////////////
	//	Set & Get the node normal display status
	void SetNodeNormal(BOOL bState) {m_NodeNormal=bState;};
	BOOL GetNodeNormal() {return m_NodeNormal;};

	////////////////////////////////////////////////////////////
	//	Set & Get the profile display status
	void SetProfile(BOOL bState) {m_Profile=bState;};
	BOOL GetProfile() {return m_Profile;};

	////////////////////////////////////////////////////////////
	//	Clear the tool stack
	void clear_tools();

	////////////////////////////////////////////////////////////
	//	Add tool into the tool stack
	void set_tool(MouseTool *tool);

	////////////////////////////////////////////////////////////
	//	The coornidate mapping between screen & wcl
	void screen_to_wcl(double sx, double sy, double &cx, double &cy, double &cz);
	void wcl_to_screen(double cx, double cy, double cz, double &sx, double &sy);

	////////////////////////////////////////////////////////////
	//	Add display objects into the display object list
	void AddDisplayObj(GLEntity *entity, BOOL bRefresh=FALSE);

	////////////////////////////////////////////////////////////
	//	Delete display objects from the display object list
	void DelDisplayObj(GLEntity *entity);
	void DelDisplayObj2(GLEntity *entity);
	void DelDisplayObj3(GLEntity *entity);

	////////////////////////////////////////////////////////////
	//	Get the count of display objects
	UINT DisplayObjCount();

	////////////////////////////////////////////////////////////
	//	Get the record of Display Obj at nIndex, the index begins from 1
	GLEntity* GetDisplayObjAt(UINT nIndex);

	////////////////////////////////////////////////////////////
	//	Remove all display objects from the display object list,
	//		and delete each object list
	void ClearDisplayObjList();

	////////////////////////////////////////////////////////////
	//	Add one GLList into the GLList list
	void add_gllist(GLList *glList);

	////////////////////////////////////////////////////////////
	//	Remove all GLList, and delete each GLList
	void clear_gllist();

	////////////////////////////////////////////////////////////
	//	Get the count of GLList
	UINT gllistCount();

	////////////////////////////////////////////////////////////
	//	Get the nIndex GLList, the index begins from 1
	GLList* get_gllist(UINT nIndex);

	////////////////////////////////////////////////////////////
	//	Hight light object: entity
	void HighLightObj(GLEntity *entity);

	////////////////////////////////////////////////////////////
	//	Set the RGB value of the drawing color
	void set_foreground_color(float red, float green, float blue);

	////////////////////////////////////////////////////////////
	//	Set the line width of drawing
	void set_line_width(int width);

	////////////////////////////////////////////////////////////
	//	Draws a 3D polyline in world coordinates 
	//
	//		pointNum	-	Points Number
	//		pts[]		-	The coordinate of points
	//		bFill		-	Fill or not
	void draw_polyline_3d(UINT pointNum, const float pts[], BOOL bFill=FALSE);

	////////////////////////////////////////////////////////////
	//	Draws a 3D polyline in world coordinates 
	//
	//		pointNum	-	Points Number
	//		pa			-	The coordinate of points
	//		bFill		-	Fill or not
	void draw_polyline_3d(UINT pointNum, position_array *pa, BOOL bFill=FALSE);

	////////////////////////////////////////////////////////////
	//	Draws 2D polyline in display window coordinates
	//
	//		pointNum	-	Points Number
	//		pts[]		-	The coordinate of points
	//		bFill		-	Fill or not
	void draw_polyline_2d(UINT pointNum, const float pts[], BOOL bFill=FALSE);

	////////////////////////////////////////////////////////////
	//	Draws a string "s" by 3D coordinate (cx,cy)
	void draw_text_3d(double cx, double cy, double cz, char *s);

	////////////////////////////////////////////////////////////
	//	Draws a string "s" by 2D coordinate (cx,cy)
	void draw_text_2d(int cx, int cy, char *s);

	////////////////////////////////////////////////////////////
	//	refresh range by all entities in the entity list
	void refreshRange();

	////////////////////////////////////////////////////////////
	//	Picks a single, top level GLEntity
	//
	//	Return Value:	TRUE - ent is the picked GLEntity
	//					FALSE - no GLEntity is picked
	//
	//	pe			-	pick event 
	//	ent			-	returned GLEntity picked
	//	entityType	-	GLEntity type can be picked	(default=0)
	//
	BOOL pick_entity(const pick_event& pe, UINT entityType, GLEntity*& ent);  

	//for displaying words
	void renderBitmapCharacher(float x, float y, float z, void *font,char *string);
};

#endif
