// CH_ATTRIB_NODE.h: interface for the CCH_ATTRIB_NODE class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CCH_ATTRIB_NODE
#define _CCH_ATTRIB_NODE

#include <afx.h>
#include "../GLKLib/GLKObList.h"

class CCH_ATTRIB_NODE : public GLKObject
{
public:
	CCH_ATTRIB_NODE();
	virtual ~CCH_ATTRIB_NODE();

public:
    CObject* Owner;
	int GetIndexNo() const;		//from 1 to n
	void SetIndexNo( const int _index = 1 );

	bool GetAttribFlag( const int whichBit );
	void SetAttribFlag( const int whichBit, const bool toBe = true );

	friend void position( CCH_ATTRIB_NODE* attrNode, double &x, double &y, double &z );
	friend void getNormal( CCH_ATTRIB_NODE* attrNode, double &nx, double &ny, double &nz );
    bool ForIdentifySeg;
	bool IsErrorNode;
	bool active;
	GLKObList AttachList;
protected:
	int		indexno;
	bool	flags[8];	// 0 -- true	CCH_ATTRIB_FACENODE
						//		false	CCH_ATTRIB_EDGENODE
						// 1 --	TRUE	Endpoint of CCH_ATTRIB_EDGE
						//		FALSE	not endpoint of CCH_ATTRIB_EDGE
						// 3 -- Temp use
};

#endif 
