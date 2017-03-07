// QMeshPatch.h: interface for the QMeshPatch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CW_QMESHPATCH
#define _CW_QMESHPATCH

#include "../GLKLib/GLKObList.h"

class QMeshFace;
class QMeshEdge;
class QMeshNode;
class QMeshTetra;
class QMeshHexa;
class CCH_ATTRIB_EDGENODE;
class CCH_ATTRIB_FACENODE;


class QMeshPatch : public GLKObject
{
public:
	QMeshPatch();
	virtual ~QMeshPatch();

public:
	//------------------------------------------------------------------------------
	//file IO
	bool inputOFFFile(const char *filename);
	bool inputOBJFile(const char *filename);
	bool inputASCFile(const char *filename);
	bool inputBSFFile(const char *filename);
	bool inputPWNFile(const char *filename);
	bool inputBinaryFile(const char *filename);
	bool inputTETFile(const char *filename);
	bool inputELEFile(const char *filename);
	bool inputMESHFile(const char *filename);
	bool inputHEXFile(const char *filename);
	//------------------------------------------------------------------------------

	BOOL GetAttribFlag (const int whichBit);
	void SetAttribFlag (const int whichBit, const BOOL toBe=TRUE);

	UINT GetIndexNo(); // from 1 to n
	void SetIndexNo( const UINT _index = 1 );

	int GetTrglFaceNumber();
	QMeshFace* GetTrglFaceRecordPtr(int No);//from 1 to n
    GLKObList& GetTrglFaceList();

    int GetTrglEdgeNumber();
	QMeshEdge* GetTrglEdgeRecordPtr(int No);//from 1 to n
    GLKObList& GetTrglEdgeList();

    int GetTrglNodeNumber();
	QMeshNode* GetTrglNodeRecordPtr(int No);//from 1 to n
    GLKObList& GetTrglNodeList();

	int GetTrglTetraNumber();
	QMeshTetra* GetTrglTetraRecordPtr(int No);//from 1 to n
    GLKObList& GetTrglTetraList();

	int GetTrglHexaNumber(){return GetTrglHexaList().GetCount();};
	QMeshHexa* GetTrglHexaRecordPtr(int No);//from 1 to n
	GLKObList& GetTrglHexaList(){return trglHexaList;};
   

	void AdjustOrientation(float rx, float ry);
	void ScaleToUnitHeight();

	void ClearAll();

	//for screen interactive select
	QMeshNode *selectedNode;

	//for volume mesh
	bool isVolume;
	bool isTET;

private:
	UINT indexno;			// start from 1 to n  

	BOOL flags[9];			// bit 0 -- TRUE for developed pattern
							// bit 1 -- TRUE for display 2D pattern
							//       -- FALSE for not display 2D pattern
							// bit 2 -- TRUE for the base mesh of template garment
							// bit 3 --	TRUE for display the nodes
							// bit 4 -- TRUE for display the Gausian curvature map
							// bit 5 -- TRUE for display the 
							// bit 6 -- TRUE for Human Model
							// bit 7 -- TRUE for display the woven model
							// bit 8 -- TRUE for display node normals

	GLKObList trglFaceList;	// a list of triangle's faces (TRGLFACE)
	GLKObList trglEdgeList;	// a list of triangle's edges (TRGLEDGE)
	GLKObList trglNodeList;	// a list of triangle's nodes (TRGLNODE)
	GLKObList trglTetraList; //a list of tetrahedrals (TRGLTETRA)
	GLKObList trglHexaList; //a list of hexahedrals (TRGLHEXA)
	GLKObList clusterList;	// a list of clusters

//operations to modify topology
public:
	void deleteTetra(QMeshTetra* tetra);
	void deleteFace(QMeshFace *face);
	void RecomputeAllNormals();

};

#endif 

