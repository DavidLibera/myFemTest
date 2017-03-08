
#include "stdafx.h"
#include "FEM.h"
#include "MainFrm.h" // to use CMainFrame* ptr

#include "MeshWorksDoc.h" // for pDoc -> m_meshlist 

#include "MeshWorksView.h"
#include "..\GLKernel\GLKernelView.h"

#include "..\MeshEntities\QBody.h"  // for body and mesh entities pFace etc...
#include "..\GLKLib\GLKMatrixLib.h" // for linear algebra

#include "..\ANN\ANN.h"


// Member functions of class FEM 


// Printing
void FEM::printNodeCoord() {

	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();


	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	int f = 0;
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		double xx, yy, zz;
		std::cout << "FACE: " << f << std::endl;
		for (int i = 1; i <= 3; i++) {
			pFace->GetNodePos(i, xx, yy, zz);
			std::cout << "Node number: " << i << " " << xx << " " << yy << " " << zz << std::endl;
		}
		f = f + 1;
	}
}

void FEM::PrintMatrix(double** &a, int row, int col)
{
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			std::cout << " " << a[i][j];
		}
		std::cout << std::endl;
	}
}

// Setting
void FEM::setProps() {
	std::cout << "FEM::setProps()" << std::endl;

	t = 0.5; // thickness
	E = 30e6; // modulus
	v = 0.25; // poissons
}

void FEM::setNodeMatrix() {

	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshNode* pNode = (QMeshNode*)body->GetTrglNodeList().GetHead();

	int c = 0; // counter
	for (GLKPOSITION Pos = body->GetTrglNodeList().GetHeadPosition(); Pos != NULL; ) {
		pNode = (QMeshNode*)body->GetTrglNodeList().GetNext(Pos);
		double xx, yy, zz;
		pNode->GetCoord3D(xx, yy, zz);
		matNodes[c][0] = xx; 
		matNodes[c][1] = yy;

		c = c + 1;
	}
}

void FEM::setConnMatrix() {
	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	int c = 0; // counter
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		matConn[c][0] = (pFace->GetNodeRecordPtr(1))->GetIndexNo();
		matConn[c][1] = (pFace->GetNodeRecordPtr(2))->GetIndexNo();
		matConn[c][2] = (pFace->GetNodeRecordPtr(3))->GetIndexNo();
		c = c + 1;
	}
}



// Getting 
int FEM::getNumberOfNodes() {
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	int n = 0;
	for (GLKPOSITION Pos = body->GetTrglNodeList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglNodeList().GetNext(Pos);
		n = n + 1;
	}
	return n;
}

int FEM::getNumberOfFaces() {
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	int f = 0;
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		f = f + 1;
	}
	return f;
}



// Setting Memory Functions
void FEM::Create() {
	std::cout << "FEM::Create()" << std::endl;
	std::cout << "matNodesRow = " << matNodesRow << std::endl;

	GLKMatrixLib::CreateMatrix(B, Brow, Bcol);
	GLKMatrixLib::CreateMatrix(matNodes, matNodesRow, matNodesCol);
	GLKMatrixLib::CreateMatrix(matConn, matConnRow, matConnCol);
}

void FEM::Destroy() {
	std::cout << "FEM::Destoy()" << std::endl;
	GLKMatrixLib::DeleteMatrix(B, Brow, Bcol);
	GLKMatrixLib::DeleteMatrix(matNodes, matNodesRow, matNodesCol);
	GLKMatrixLib::DeleteMatrix(matConn, matConnRow, matConnCol);
}



// Calculating 
void FEM::MainFunction() {
	
	// Allocate required memory  
	Create();
	
	// Do computations of matrix 
	setProps();
	setNodeMatrix();
	setConnMatrix();

	//PrintMatrix(matNodes, matNodesRow, matNodesCol);

	//PrintMatrix(matConn, matConnRow, matConnCol);

	// Deallocate memory before exiting
	Destroy();

}