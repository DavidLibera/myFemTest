
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
	std::cout << "-------------------------------" << std::endl;
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			std::cout << " " << a[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << "-------------------------------" << std::endl;
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

void FEM::setDMatrix() {
	// stress-strain matrix
	D[0][0] = 1; D[0][1] = v; D[0][2] = 0; 
	D[1][0] = v; D[1][1] = 1; D[1][2] = 0;
	D[2][0] = 0; D[2][1] = 0; D[2][2] = (1-v)/2;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			D[i][j] = E / (1 - v*v)*D[i][j];
		}
	}
}

// Computing

void FEM::computeKMatrix() {
	/*
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	double xx, yy, zz;
	int f = 0;
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		
		for (int i = 1; i <= 3; i++) {
			
			pFace->GetNodePos(i, xx, yy, zz);


		}
		f = f + 1;
	}
	*/
	int n1, n2, n3;
	double x1, y1, z1, x2, y2, z2, x3, y3, z3;
	double b1, b2, b3, c1, c2, c3, A;
	for (int element = 0; element < nFaces; element++) {

		// Extracting node number from matConn matrix
		n1 = matConn[element][0];
		n2 = matConn[element][1];
		n3 = matConn[element][2];

		// Using node number from above to access x,y coordinates (Note: index starts is 1 less for matrices, hence the -1)
		x1 = matNodes[n1-1][0]; y1 = matNodes[n1-1][1];
		x2 = matNodes[n2-1][0]; y2 = matNodes[n2-1][1];
		x3 = matNodes[n3-1][0]; y3 = matNodes[n3-1][1];

		// Computing area of element
		A = 0.5*((x2*y3 - x3*y2) + (x3*y1 - x1*y3) + (x1*y2 - x2*y1)); // needs an absolute sign

		// Computing elements of matrix B
		b1 = y2 - y3; b2 = y3 - y1; b3 = y1 - y2;
		c1 = x3 - x2; c2 = x1 - x3; c3 = x2 - x1;

		// Setting matrix B 
		B[0][0] = b1/(2*A); B[0][2] = b2/(2*A); B[0][4] = b3/(2*A); 
		B[1][1] = c1/(2*A); B[1][3] = c2/(2*A); B[1][5] = c3/(2*A);
		B[2][1] = b1 / (2 * A); B[2][3] = b2 / (2 * A); B[2][5] = b3 / (2 * A);
		B[2][0] = c1 / (2 * A); B[2][2] = c2 / (2 * A); B[2][4] = c3 / (2 * A);

		// Performing Ke computation 
		// Ke = A*t*BT*D*B 
		GLKMatrixLib::Transpose(B, Brow, Bcol, BT);
		GLKMatrixLib::Mul(BT, D, 6, 3, 3, temp);		// Matrix Multiplication seperate TEDIOUS
		GLKMatrixLib::Mul(temp, B, 6, 3, 6, Ke);

		for (int i = 0; i < Kerow; i++) {
			for (int j = 0; j < Kecol; j++) {
				Ke[i][j] = t*A*Ke[i][j];
			}
		}

		// scattering results into global matrix
		scatter(Ke, K, n1, n2, n3);

	}
	
	PrintMatrix(K, Krow, Kcol);

}

void FEM::scatter(double** &Ke, double** &K, int n1, int n2, int n3) {

	int row = 0, col =0;

	for (int i = 0; i < Kerow; i++) {

		switch (i) {
		case 1: row = 2 * n1 - 2; break;
		case 2: row = 2 * n1 - 1; break;
		case 3: row = 2 * n2 - 2; break;
		case 4: row = 2 * n2 - 1; break;
		case 5: row = 2 * n3 - 2; break;
		case 6: row = 2 * n3 - 1; break;
		}

		for (int j = 0; j < Kecol; j++) {

			switch (j) {
			case 1: col = 2 * n1 - 2; break;
			case 2: col = 2 * n1 - 1; break;
			case 3: col = 2 * n2 - 2; break;
			case 4: col = 2 * n2 - 1; break;
			case 5: col = 2 * n3 - 2; break;
			case 6: col = 2 * n3 - 1; break;
			}
			K[row][col] = K[row][col] + Ke[i][j];
			
			/* Checking entries.. Note: 8th row is empty? why?
			std::cout << "row:" << row << std::endl;
			std::cout << "col:" << col << std::endl;
			std::cout << "K[row][col]:" << K[row][col] << std::endl;
			std::cout << "---------------------------" << std::endl;
			*/
		}
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

	GLKMatrixLib::CreateMatrix(matNodes, matNodesRow, matNodesCol);
	GLKMatrixLib::CreateMatrix(matConn, matConnRow, matConnCol);
	GLKMatrixLib::CreateMatrix(B, Brow, Bcol);
	GLKMatrixLib::CreateMatrix(D, Drow, Dcol);
	GLKMatrixLib::CreateMatrix(f, frow, fcol);
	GLKMatrixLib::CreateMatrix(d, drow, dcol);
	GLKMatrixLib::CreateMatrix(K, Krow, Kcol);
	GLKMatrixLib::CreateMatrix(Ke, Kerow, Kecol);

	// Miscallenous
	GLKMatrixLib::CreateMatrix(BT, BTrow, BTcol);
	GLKMatrixLib::CreateMatrix(temp, trow, tcol);
}

void FEM::Destroy() {
	std::cout << "FEM::Destoy()" << std::endl;

	GLKMatrixLib::DeleteMatrix(matNodes, matNodesRow, matNodesCol);
	GLKMatrixLib::DeleteMatrix(matConn, matConnRow, matConnCol);
	GLKMatrixLib::DeleteMatrix(B, Brow, Bcol);
	GLKMatrixLib::DeleteMatrix(D, Drow, Dcol);
	GLKMatrixLib::DeleteMatrix(f, frow, fcol);
	GLKMatrixLib::DeleteMatrix(d, drow, dcol);
	GLKMatrixLib::DeleteMatrix(K, Krow, Kcol);
	GLKMatrixLib::DeleteMatrix(Ke, Kerow, Kecol);

	// Miscallenous
	GLKMatrixLib::DeleteMatrix(BT, BTrow, BTcol);
	GLKMatrixLib::DeleteMatrix(temp, trow, tcol);
}



// Main function 
void FEM::MainFunction() {
	
	// Allocate required memory  
	Create();
	
	// Do computations of matrix 
	setProps();
	setNodeMatrix();
	setConnMatrix();
	setDMatrix();
	
	//Compute K matrix
	computeKMatrix();

	// Print
	PrintMatrix(matNodes, matNodesRow, matNodesCol);
	PrintMatrix(matConn, matConnRow, matConnCol);
	PrintMatrix(D, Drow, Dcol);

	// Deallocate memory before exiting
	Destroy();

}


// Post-processing
void FEM::colorFaces() {

}