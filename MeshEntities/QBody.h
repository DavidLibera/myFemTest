// QBody.h: interface for Display.
//
//////////////////////////////////////////////////////////////////////

#ifndef DEF_QBody
#define DEF_QBody

#define num_of_select_pt 70
#define num_of_SP num_of_select_pt*num_of_select_pt

#include "../GLKernel/GLEntity.h"
#include "../GLKLib/GLKObList.h"
#include "../GLKLib/GLKMatrixLib.h"
#include "QMeshPatch.h"
#include "QMeshEdge.h"
#include "QMeshNode.h"
#include "QMeshFace.h"
#include "QMeshTetra.h"
#include "QMeshHexa.h"

class CMeshWorksView;

class QBody : public GLEntity
{
public:
	QBody();
	//CMeshWorksView* view;
	
	QBody(void* v);
	CMeshWorksView* getView(){return (CMeshWorksView*)view;};
	virtual ~QBody();
	public:
	void ClearAll();
	
	BOOL GetAttribFlag (const int whichBit);
	void SetAttribFlag (const int whichBit, const BOOL toBe=TRUE);

	UINT GetIndexNo() const; // from 1 to n
	void SetIndexNo( const UINT _index = 1 );

	int GetTrglFaceNumber() const;
	QMeshFace* GetTrglFaceRecordPtr(int No) const;//from 1 to n
    GLKObList& GetTrglFaceList();
    const GLKObList& GetTrglFaceList() const;

    int GetTrglEdgeNumber() const;
	QMeshEdge* GetTrglEdgeRecordPtr(int No) const;//from 1 to n
    GLKObList& GetTrglEdgeList();
    const GLKObList& GetTrglEdgeList() const;

    int GetTrglNodeNumber() const;
	QMeshNode* GetTrglNodeRecordPtr(int No) const;//from 1 to n
    GLKObList& GetTrglNodeList();
    const GLKObList& GetTrglNodeList() const;

	int GetTrglTetraNumber() const;
	QMeshTetra* GetTrglTetraRecordPtr(int No) const;//from 1 to n
    GLKObList& GetTrglTetraList();
    const GLKObList& GetTrglTetraList() const;

	int GetTrglHexaNumber() const;
	QMeshHexa* GetTrglHexaRecordPtr(int No) const;//from 1 to n
    GLKObList& GetTrglHexaList();
    const GLKObList& GetTrglHexaList() const;

	         
	// Implement the virtual method which draw this entity
	//		TRUE - draw the shading mode
	//		FALSE - draw the mesh mode
	int shadenormal; //0-node normal, 1-face normal
	virtual void drawShade();
	virtual void drawShade2();
	virtual void drawProfile();
	virtual void drawMesh();
	virtual void drawPreMesh();
	virtual void drawNode();
	virtual void drawNode2();
	virtual void drawNodeNormal(const double &length);

	
	//GDBWovenModel *m_wovenModel;
	// Implement the maximum distance to the original point of this entity 
	virtual float getRange();
	float m_range;
    double m_maxCurvature,m_minCurvature;
    QMeshPatch* GetMeshPatch(); 
	void SetMeshPatch(QMeshPatch* patch);

	//added for VSA
	void ChangeValueToColor(int nType, float & nRed, float & nGreen, float & nBlue);
	float m_ptSize;

	//added for denoising
	double m_poly[5];
	double m_p[3], m_n[3];

	bool m_drawPolynomial;
	double **m_R1;
	
private:
    QMeshPatch* Patch;
	void ChangeValueToColor(double maxValue, double minValue, double Value, 
											 float & nRed, float & nGreen, float & nBlue);

	void* view;

	GLKObList** SP;
	//BOOL* showSP;


//	void drawWovenModel();
	void drawFaceNormal();

	void drawPolynomial();
};
#endif
