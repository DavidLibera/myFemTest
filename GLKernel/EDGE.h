// EDGE.h: interface for the EDGE class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CW_EDGE
#define CW_EDGE

#include "GLEntity.h"

class VERTEX;

class EDGE : public GLEntity  
{
public:
	EDGE() {};
	virtual ~EDGE() {};

	void SetStartPoint(VERTEX *v) {start=v;};
	void SetEndPoint(VERTEX *v) {end=v;};
	VERTEX* GetStartPoint() {return start;};
	VERTEX* GetEndPoint() {return end;};

	virtual void drawProfile() 
	{
		glColor3f(0.0f,0.0f,0.0f);
		glBegin(GL_LINES);
			double x1,y1,z1,x2,y2,z2;
			start->GetPosition(x1,y1,z1);
			end->GetPosition(x2,y2,z2);
			glVertex3d(x1,y1,z1);
			glVertex3d(x2,y2,z2);
		glEnd();
	}

	virtual void drawMesh() 
	{
		glColor3f(0.0f,0.0f,0.0f);
		glBegin(GL_LINES);
			double x1,y1,z1,x2,y2,z2;
			start->GetPosition(x1,y1,z1);
			end->GetPosition(x2,y2,z2);
			glVertex3d(x1,y1,z1);
			glVertex3d(x2,y2,z2);
		glEnd();
	}

	virtual void drawHighLight() 
	{
		glColor3f(1.0f,0.0f,1.0f);
		glBegin(GL_LINES);
			double x1,y1,z1,x2,y2,z2;
			start->GetPosition(x1,y1,z1);
			end->GetPosition(x2,y2,z2);
			glVertex3d(x1,y1,z1);
			glVertex3d(x2,y2,z2);
		glEnd();
	};

	virtual float getRange() 
	{
		float range=0.0;

		if (start)
			range=start->getRange();
		if (end)
			if ((end->getRange())>range) range=end->getRange();

		return range;
	}

private:
	VERTEX *start,*end;
};

#endif
