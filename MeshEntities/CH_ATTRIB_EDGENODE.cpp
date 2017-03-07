// CH_ATTRIB_EDGENODE.cpp: implementation of the CCH_ATTRIB_EDGENODE class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CH_ATTRIB_EDGENODE.h"
#include "QMeshEdge.h"
#include "QMeshNode.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCH_ATTRIB_EDGENODE::CCH_ATTRIB_EDGENODE()
{
	CCH_ATTRIB_NODE::CCH_ATTRIB_NODE();

	trglEdge=NULL;	u=0.0;

	flags[0]=FALSE;
}

CCH_ATTRIB_EDGENODE::~CCH_ATTRIB_EDGENODE()
{
	CCH_ATTRIB_NODE::~CCH_ATTRIB_NODE();
}
