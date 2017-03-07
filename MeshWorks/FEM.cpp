
#include "stdafx.h"
#include "FEM.h"
#include "MainFrm.h" // to use CMainFrame* ptr

#include "MeshWorksDoc.h" // for pDoc -> m_meshlist 

#include "MeshWorksView.h"
#include "..\GLKernel\GLKernelView.h"

#include "..\MeshEntities\QBody.h"  // for body and mesh entities pFace etc...
#include "..\GLKLib\GLKMatrixLib.h" // for linear algebra

#include "..\ANN\ANN.h"


void FEM::setProps() {
	t = 0.5; // thickness
	E = 30e6; // modulus
	v = 0.25; // poissons
}

void FEM::setNodeMatrix() {

	//AfxGetApp()->BeginWaitCursor();

	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();


	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	double M[2] = { 1.0, 20.0 };

	std::cout << M[1] << std::endl;
	/*
	int f = 0;
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		double xx, yy, zz;
		std::cout << "FACE: " << f << std::endl;
		for (int i = 1; i <= 3; i++) {
			pFace->GetNodePos(i, xx, yy, zz);
			M[f][i][0] = xx; M[f][i][1] = yy; M[f][i][2] = zz;
			std::cout << "Node number: " << i << " " << xx << " " << yy << " " << zz << std::endl;

		}
		f = f + 1;


	}
	*/
	//AfxGetApp()->EndWaitCursor();
}

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