// GLList.h: interface for the GLList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CW_GLLIST
#define CW_GLLIST

#include <afx.h>

class CGLKernelView;

class GLList : public CObject  
{
public:
	GLList() {};
	virtual ~GLList() {};
	virtual void draw(CGLKernelView *view) {};
};

#endif 
