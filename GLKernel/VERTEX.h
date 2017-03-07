// VERTEX.h: interface for the VERTEX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CW_VERTEX
#define CW_VERTEX

#include "GLEntity.h"
#include <math.h>

class VERTEX : public GLEntity  
{
public:
	VERTEX() {};
	virtual ~VERTEX() {};

	virtual float getRange() {return (float)(sqrt(x*x+y*y+z*z));};

	void SetPosition(double xx, double yy, double zz) {x=xx;y=yy;z=zz;};
	void GetPosition(double &xx, double &yy, double &zz) {xx=x;yy=y;zz=z;};

	virtual void drawShade() 
	{
		glColor3f(0.7f,0.7f,0.7f);
		glPushMatrix();
		glTranslated(x,y,z);
		auxSolidSphere(1.0);
		glPopMatrix();
	};

	virtual void drawProfile() 
	{
	};

	virtual void drawHighLight() 
	{
		glColor3f(1.0f,0.0f,1.0f);
		glPushMatrix();
		glTranslated(x,y,z);
		auxWireSphere(1.0);
		glPopMatrix();
	};

	virtual void drawMesh() 
	{
		glColor3f(0.7f,0.7f,0.7f);
		glPushMatrix();
		glTranslated(x,y,z);
		auxWireSphere(1.0);
		glPopMatrix();
	};


private:
	double x,y,z;
};

#endif
