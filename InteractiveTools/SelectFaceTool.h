// SelectFaceTool.h: interface for the SelectFaceTool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SelectFaceTool_
#define _SelectFaceTool_



#include <afx.h>
#include "../GLKernel/MouseTool.h"
#include "../GLKLib/GLKObList.h" 

class CGLKernelView;
class QBody;
class QMeshPatch;
class QMeshFace;
class CCH_ATTRIB_EDGE;
class CCH_Curve;
class CCH_ATTRIB_NODE;


typedef enum select_face_type {FACE,FACENODE,TETRA};

class SelectFaceTool : public MouseTool 
{

public:
	SelectFaceTool(CGLKernelView *cview3d, QMeshPatch *body, MouseTool* tool, select_face_type ft);
	virtual ~SelectFaceTool();

	virtual int process_event(mouse_event_type even_type, const pick_event& pe);
	//virtual int process_done_event();
private:
    CGLKernelView* pview3d;
	QMeshPatch* meshbody;

	MouseTool* MTool;

	select_face_type m_ft;

	CCH_Curve * EditCurve;
	void GetSubdivisionLines();
	void CopyEditCurveToEdge();
	QMeshFace* CalProjectedTrgl(double xx, double yy, double zz, 
		             double nx, double ny, double nz, double& t,double& u,double& v,double& w);
	void FindClosedPointForAll();
	double Distance_to_Triangle(double p[], QMeshFace* Face,double closePnt[],double rValue[]);

	void GetIntersectionPoint(double p1[],double p2[],double& x,double& y,double& z,QMeshFace*RFace);

	void ExamineEdgeNodeOrNot(CCH_ATTRIB_EDGENODE* &Node,CCH_ATTRIB_NODE *attrNode);

	GLKObList& GetShortestEdgePath(QMeshPatch*meshbody,QMeshNode*StartNode, QMeshNode*EndNode);



	int process_event_face(mouse_event_type even_type, const pick_event& pe);
	int process_event_facenode(mouse_event_type even_type, const pick_event& pe);
	int process_event_tetra(mouse_event_type even_type, const pick_event& pe);
    	

};

#endif
