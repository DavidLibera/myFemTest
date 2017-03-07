#include "InterBoard.h"

InterBoard::InterBoard(void)
{
	m_ID=-1;
	entityType = 8;
	inList=false;
}

InterBoard::~InterBoard(void)
{
}

void InterBoard::drawShade()
{
	//printf("drawboard\n");
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);

	glColor4f(0.5f,0.5f,0.6f,0.7f);
	glBegin(GL_QUADS);

	for (int i=0; i<4; i++){
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f((float)(x[i]),(float)(y[i]),(float)(z[i]));
	}
	
	glEnd();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void InterBoard::drawHighLight()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);

	glColor4f(1.0f,1.0f,0.0f,0.7f);
	glBegin(GL_QUADS);

	for (int i=0; i<4; i++){
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f((float)(x[i]),(float)(y[i]),(float)(z[i]));
	}
	
	glEnd();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}