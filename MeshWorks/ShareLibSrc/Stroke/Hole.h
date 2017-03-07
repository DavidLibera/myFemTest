#pragma once
#include"..\Geometry\point3d.h"
#include "..\Surface\surface.h"
#include <vector>
using namespace std;
class CHole
{
public:
	CHole(void);
	~CHole(void);
private:
	Surface *m_pSurf;
	float m_fColor[3];
	vector<int> * m_pVertexVec;
public:
	CHole(Surface *pSurf);
	void initialHole(Surface *pSurf);
	void AddVertex(int vid);
	int GetVertex(int index);
	int GetHoleSize(void);
	void Display(void);
	void SetColor(float r, float g, float b);
};
