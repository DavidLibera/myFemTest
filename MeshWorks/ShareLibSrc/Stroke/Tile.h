#pragma once
#include "..\geometry\point3d.h"
#include "..\surface\surface.h"
//#include "face.h"
//#include "edge.h"
#include "hole.h"
#include <vector>
#include <set>
using namespace std;
class CTile
{
public:
	CTile(void);
	~CTile(void);
	void AddFace(int fid);
	vector<int>* m_pFaceVect;
	vector<int>* m_pInnerV;
private:

	vector<int>* m_pEdges;
	vector<int>* m_pBoundV;

	vector<bool> * m_pFaceInTile;
	vector<bool> * m_pEdgeInTile;
	vector<bool> * m_pVertexInTile;
	vector<bool> * m_pVertexInBound;

	Surface *m_pSurf;
public:
	bool isInTile(int fID);
	void AddInnerEdge(int eid);
	void initialTile(Surface *surf);
	void AddBoundV(int vid);
	void AddInnerV(int vid);
	void Display(void);
	void Dijkstra(int vInID, int vOutID, CHole* pBound);
	void SetFlag(bool flag);
};
