// CH_ATTRIB_FACENODE.h: interface for the CCH_ATTRIB_FACENODE class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CCH_ATTRIB_FACENODE
#define _CCH_ATTRIB_FACENODE

#include "CH_ATTRIB_NODE.h"

class QMeshFace;

class CCH_ATTRIB_FACENODE : public CCH_ATTRIB_NODE  
{
public:
	CCH_ATTRIB_FACENODE();
	virtual ~CCH_ATTRIB_FACENODE();

public:
	void SetTrglFace(QMeshFace *_face) {trglFace=_face;};
	QMeshFace *GetTrglFace() {return trglFace;};

	void SetParameter(double _u, double _v, double _w) {u=_u; v=_v; w=_w;};
	void GetParameter(double &_u, double &_v, double &_w) {_u=u; _v=v; _w=w;};

private:
	double	u, v, w;		// Areal coordinate of this node
	QMeshFace *trglFace;		// TRGLFACE contain this node


public:
	void GetCoord3D(double &x, double &y, double &z);
};

#endif
