#pragma once
#include "..\glkernel\glentity.h"

class QMeshNode;

class InterativeNode :
	public GLEntity
{
public:
	InterativeNode(QMeshNode *node);
	~InterativeNode(void);

	virtual void drawShade();
	virtual void drawHighLight();

	int m_ID;
	QMeshNode *m_node;
};
