#pragma once
#include "..\glkernel\glentity.h"

class InterBoard :
	public GLEntity
{
public:
	InterBoard(void);
	~InterBoard(void);

	virtual void drawShade();
	virtual void drawHighLight();

	int m_ID;

	float x[4], y[4], z[4];
	float n[3]; 
};
