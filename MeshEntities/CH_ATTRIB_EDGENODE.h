// CH_ATTRIB_EDGENODE.h: interface for the CCH_ATTRIB_EDGENODE class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CCH_ATTRIB_EDGENODE
#define _CCH_ATTRIB_EDGENODE

#include "CH_ATTRIB_NODE.h"
#include "../GLKLib/GLKObList.h"

class QMeshEdge;

class CCH_ATTRIB_EDGENODE : public CCH_ATTRIB_NODE  
{
public:
	CCH_ATTRIB_EDGENODE();
	virtual ~CCH_ATTRIB_EDGENODE();

public:
	void SetTrglEdge(QMeshEdge *_edge) {trglEdge=_edge;};
	QMeshEdge *GetTrglEdge() {return trglEdge;};

	void SetParameter(double _u) {u=_u;};
	void GetParameter(double &_u) {_u=u;};


private:
	double	u;				// Parameter coordinate of this node
	QMeshEdge *trglEdge;		// TRGLEDGE contain this node
};

#endif
