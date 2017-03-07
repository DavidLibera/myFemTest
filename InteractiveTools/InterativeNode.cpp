#include "StdAfx.h"
#include "InterativeNode.h"

#include "..\MeshEntities\QMeshNode.h"

InterativeNode::InterativeNode(QMeshNode *node)
{
	m_node = node;
	entityType = 100;
}

InterativeNode::~InterativeNode(void)
{
}

void InterativeNode::drawShade()
{
	double x, y, z;

	glBegin(GL_POINTS);
	
	glColor3f(1.0,1.0,1.0);

	m_node->GetCoord3D(x,y,z);
	glVertex3d(x,y,z);

	glEnd();
}

void InterativeNode::drawHighLight()
{
	double x, y, z;
	float ptSize;

	glGetFloatv(GL_POINT_SIZE, &ptSize);
	glPointSize(ptSize*1.5);
	//glPointSize(10);
	glBegin(GL_POINTS);

	glColor3f(0.0, 0.0, 1.0);

	m_node->GetCoord3D(x,y,z);
	glVertex3d(x,y,z);

	glEnd();

	glPointSize(ptSize);
}