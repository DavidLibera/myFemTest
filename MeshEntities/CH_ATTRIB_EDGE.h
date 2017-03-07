// CH_ATTRIB_EDGE.h: interface for the CCH_ATTRIB_EDGE class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CCH_ATTRIB_EDGE
#define _CCH_ATTRIB_EDGE

#include <afx.h>
#include "..\GLKernel\GLEntity.h"
#include "../GLKLib/GLKObList.h"

#include "CH_ATTRIB_NODE.h"
#include "CH_ATTRIB_EDGENODE.h"
#include "CH_ATTRIB_FACENODE.h"

class CCH_ATTRIB_EDGE : public GLEntity  
{
public:
	CCH_ATTRIB_EDGE();
	virtual ~CCH_ATTRIB_EDGE();

public:
    int GetAttrNodeNumber() {return attrNodeList.GetCount();};
    GLKObList& GetAttrNodeList() {return attrNodeList;};

	virtual void drawProfile();
	virtual void drawShade();
	//virtual void drawHighLight();

	int GetIndexNo() const;		//from 1 to n
	void SetIndexNo( const int _index = 1 );

	CCH_ATTRIB_NODE* GetAttib_NodeRecordPtr(int No);//from 1 to n
    bool GetSystemFlag();
    void SetSystemFlag(bool Flag);

	bool GetDrawingFlag();
    void SetDrawingFlag(bool Flag);
	bool ShowOrNot;
	bool IsNotch;
	bool IsActive;
	bool isVisited;

private:
    int		indexno;
	bool SystemFlag;
	bool DrawingFlag;
	
	GLKObList attrNodeList;	// a list of triangle's nodes (TRGLNODE)

	void drawBox(float xx, float yy, float zz, float r);
};

#endif 
