
#include "stdafx.h"
#include "FEM.h"
#include "MainFrm.h" // to use CMainFrame* ptr

#include "MeshWorksDoc.h" // for pDoc -> m_meshlist 

#include "MeshWorksView.h"
#include "..\GLKernel\GLKernelView.h"

#include "..\MeshEntities\QBody.h"  // for body and mesh entities pFace etc...
#include "..\GLKLib\GLKMatrixLib.h" // for linear algebra

#include "..\ANN\ANN.h"

#include <vector> // useful for BCs


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

void FEM::PrintVector(double* &a, int row) {
	std::cout << "-------------------------------" << std::endl;
	int i, j;
	for (i = 0; i < row; i++) {
		std::cout << a[i] << " " << std::endl;
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

void FEM::setBCs() {

	// FOR GENERAL CASE

	//DANGER (ONLY FOR SQUARE MESH) affected nodes and ONLY SELF MADE MESH will make sense
	
	//Y-displacement is zero at edge 
	int DOF1 = sqrt(nNodes) * 2;
	int DOF2 = sqrt(nNodes) * 2 - 1;
	// upper d = 0 
	int DOF3 = (nNodes - sqrt(nNodes) + 1) * 2;		// NOTE: DANGER I AM ASSUMING nNodes is a square number
	int DOF4 = (nNodes - sqrt(nNodes) + 1) * 2 - 1;
	// lower d = 0 
	int DOF5 = 1;
	int DOF6 = 2;
	int FORCEDOF = nNodes * 2;

	//std::cout << "DOF1:" << DOF1 << std::endl;
	//std::cout << "DOF2:" << DOF2 << std::endl;
	//std::cout << "DOF3:" << DOF3 << std::endl;
	//std::cout << "DOF4:" << DOF4 << std::endl;
	//std::cout << "DOF5:" << DOF5 << std::endl;
	//std::cout << "FORCE DOF:" << FORCEDOF << std::endl;

	int c = 0;
	for (int i = 1; i <= nDOF; i++) {
		if (i != DOF1 /*&& i!= DOF2*/ && i != DOF3 && i != DOF4 && i != DOF5 && i != DOF6) {
			Isol[c] = i;
			c = c + 1;
		}
	}
	//NOTE: I need to indent this one -1
	f[FORCEDOF-1] = 1225; // Fy at node 3 is 1225
	//f[FORCEDOF - 2] = -1225; // Fx

}


// Computing

void FEM::computeKMatrix() {

	int n1, n2, n3;
	double x1, y1, z1, x2, y2, z2, x3, y3, z3;
	double b1, b2, b3, c1, c2, c3, A;

	for (int element = 0; element < nFaces; element++) {

		// Extracting node number from matConn matrix
		n1 = matConn[element][0];
		n2 = matConn[element][1];
		n3 = matConn[element][2];

		// Using node number from above to access x,y coordinates (Note: index starts is 1 less for matrices, hence the -1)
		x1 = matNodes[n1 - 1][0]; y1 = matNodes[n1 - 1][1];

		x2 = matNodes[n2 - 1][0]; y2 = matNodes[n2 - 1][1];
		x3 = matNodes[n3 - 1][0]; y3 = matNodes[n3 - 1][1];

		// Computing area of element
		A = 0.5*((x2*y3 - x3*y2) + (x3*y1 - x1*y3) + (x1*y2 - x2*y1)); // needs an absolute sign

		// Computing elements of matrix B
		b1 = y2 - y3; b2 = y3 - y1; b3 = y1 - y2;
		c1 = x3 - x2; c2 = x1 - x3; c3 = x2 - x1;

		// Setting matrix B 
		B[0][0] = b1 / (2 * A); B[0][2] = b2 / (2 * A); B[0][4] = b3 / (2 * A);
		B[1][1] = c1 / (2 * A); B[1][3] = c2 / (2 * A); B[1][5] = c3 / (2 * A);
		B[2][1] = b1 / (2 * A); B[2][3] = b2 / (2 * A); B[2][5] = b3 / (2 * A);
		B[2][0] = c1 / (2 * A); B[2][2] = c2 / (2 * A); B[2][4] = c3 / (2 * A);

		// Performing Ke computation (Ke = A*t*BT*D*B )
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

}

void FEM::computeStress() {

	int n1, n2, n3;
	double b1, b2, b3, c1, c2, c3, x1, x2, x3, y1, y2, y3, A;

	for (int element = 0; element < nFaces; element++) {
		n1 = matConn[element][0];
		n2 = matConn[element][1];
		n3 = matConn[element][2];

		de[0] = d[2 * n1 - 2];
		de[1] = d[2 * n1 - 1];
		de[2] = d[2 * n2 - 2];
		de[3] = d[2 * n2 - 1];
		de[4] = d[2 * n3 - 2];
		de[5] = d[2 * n3 - 1];

		// RECOMPUTE B MATRIX 
		// Using node number from above to access x,y coordinates (Note: index starts is 1 less for matrices, hence the -1)
		x1 = matNodes[n1 - 1][0]; y1 = matNodes[n1 - 1][1];
		x2 = matNodes[n2 - 1][0]; y2 = matNodes[n2 - 1][1];
		x3 = matNodes[n3 - 1][0]; y3 = matNodes[n3 - 1][1];

		// Computing area of element
		A = 0.5*((x2*y3 - x3*y2) + (x3*y1 - x1*y3) + (x1*y2 - x2*y1)); // needs an absolute sign

																	   // Computing elements of matrix B
		b1 = y2 - y3; b2 = y3 - y1; b3 = y1 - y2;
		c1 = x3 - x2; c2 = x1 - x3; c3 = x2 - x1;

		// Setting matrix B 
		B[0][0] = b1 / (2 * A); B[0][2] = b2 / (2 * A); B[0][4] = b3 / (2 * A);
		B[1][1] = c1 / (2 * A); B[1][3] = c2 / (2 * A); B[1][5] = c3 / (2 * A);
		B[2][1] = b1 / (2 * A); B[2][3] = b2 / (2 * A); B[2][5] = b3 / (2 * A);
		B[2][0] = c1 / (2 * A); B[2][2] = c2 / (2 * A); B[2][4] = c3 / (2 * A);

		// Computing Stress
		GLKMatrixLib::Mul(D, B, 3, 3, 6, temp2);
		GLKMatrixLib::Mul(temp2, de, 3, 6, sige);

		// Store vonMises for drawShade function
		double vonMises = computeVonMises();
		vonMisVec[element] = vonMises;



		//Print stresses
		//std::cout << "Sige:" << std::endl;
		//PrintVector(sige, sigerow);
		std::cout << "vonMises of element:" << element << "is " << vonMises << std::endl;

	}

}

double FEM::computeVonMises() {

	// For 2D problems only

	return sqrt((pow((sige[0] - sige[1]), 2) + pow(sige[0], 2) + pow(sige[1], 2) + 6 * pow(sige[2], 2)) / 2);
}

// Scattering

void FEM::scatter(double** &Ke, double** &K, int n1, int n2, int n3) {

	int row = 0, col =0;

	for (int i = 0; i < Kerow; i++) {

		switch (i) {
		case 0: row = 2 * n1 - 2; break;
		case 1: row = 2 * n1 - 1; break;
		case 2: row = 2 * n2 - 2; break;
		case 3: row = 2 * n2 - 1; break;
		case 4: row = 2 * n3 - 2; break;
		case 5: row = 2 * n3 - 1; break;
		}

		for (int j = 0; j < Kecol; j++) {

			switch (j) {
			case 0: col = 2 * n1 - 2; break;
			case 1: col = 2 * n1 - 1; break;
			case 2: col = 2 * n2 - 2; break;
			case 3: col = 2 * n2 - 1; break;
			case 4: col = 2 * n3 - 2; break;
			case 5: col = 2 * n3 - 1; break;
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

void FEM::scatterKmod(double** &K, double** &Kmod) {

	// Entries of Isol represent rows and cols that will be extracted
	int n = vars;

	int row, col;

	//Creating Kmod 
	for (int i = 0; i < n; i++) {

		row = Isol[i] - 1;

		for (int j = 0; j < n; j++) {

			col = Isol[j] - 1;

			Kmod[i][j] = K[row][col];

		}

	}

}

void FEM::scatterfmod(double* &f, double* &fmod) {

	int n = vars;
	
	int row;

	for (int i = 0; i < n; i++) {

		row = Isol[i] - 1;

		fmod[i] = f[row];
	}
}

void FEM::scatterBackDisplacements(double* &dmod, double* &d) {

	int n = vars; // n is the number of columns in Isol 
	int row;

	for (int i = 0; i < n; i++) {

		row = Isol[i] - 1;

		d[row] = dmod[i];
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




// Main function 
void FEM::MainFunction() {

	// Do computations of matrix 
	setProps();
	setNodeMatrix();
	setConnMatrix();
	setDMatrix();

	//Compute K matrix
	computeKMatrix();

	//Set BCS
	setBCs();

	//Modify System of Equations based on BCs
	scatterKmod(K, Kmod);
	scatterfmod(f, fmod);


	//std::cout << "Kmod" << std::endl;
	//PrintMatrix(Kmod, Kmodrow, Kmodcol);
	//std::cout << "fmod" << std::endl;
	//PrintVector(fmod, fmodrow);
	

	//Solve system of equations

	//bool doesSystemSolve = GLKMatrixLib::GaussSeidelSolver(Kmod, fmod, vars, dmod,1e-6);
	bool doesSystemSolve = GLKMatrixLib::GaussJordanElimination(Kmod, vars, fmod);

	std::cout << "bool doesSystemSolve =" << doesSystemSolve << std::endl;
	//Note: in GaussJordan solver, b vector becomes the output vector hence fmod contains dmod solution
	scatterBackDisplacements(fmod, d);

	//std::cout << "d" << std::endl;
	//PrintVector(d, drow);

	//Compute Stresses

	computeStress();

	// POST PROCESSING **************************************

	//Normalize vonMisVec (first find max value, then divide all by it)

	double maxVM = 0.0;
	int maxVMindex = 0;
	for (int i = 0; i < nFaces; i++) {
		if (vonMisVec[i] > maxVM) {
			maxVM = vonMisVec[i];
			maxVMindex = i;
		}

	}


	//Print
	//std::cout << " Printing values passed to color map CASE: Normalized Von Mises" << std::endl;
	//PrintVector(vonMisVec, vonMisVecrow);


	std::cout << "Max VM : " << maxVM << "at Face index" << maxVMindex << std::endl;

	for (int i = 0; i < nFaces; i++) {
		vonMisVec[i] = 1.0 - (vonMisVec[i]/ maxVM);
	}

	//Print
	//std::cout << " Printing values passed to color map CASE: Normalized Von Mises" << std::endl;
	//PrintVector(vonMisVec, vonMisVecrow);

	/*
	double totald;
	int c = 0;
	std::cout << "displacements at nodes = sqrt(dx^2+dy^2)" << std::endl;
	for (int i = 0; i < nDOF; i = i+2) {
	//computing total displacement at each node
	totald = sqrt(pow(d[i], 2)+pow(d[i + 1], 2));
	std::cout << "total d for node" << c << " is " << totald << std::endl;


	// DANGER ?
	//vonMisVec[c] = d[i];


	//std::cout << "dx:" << d[i] << std::endl;
	//std::cout << "dy:" << d[i + 1] << std::endl;
	c = c + 1;
	// safety break
	if (c == 100) { break; }
	}
	*/



}


// Post-processing
void FEM::colorFaces() {
	std::cout << "colorFaces" << std::endl;

	AfxGetApp()->BeginWaitCursor();
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	pWnd->ChangeColor(vonMisVec,vonMisVecrow);

	AfxGetApp()->EndWaitCursor();

}


// Setting Memory Functions
void FEM::Create() {
	std::cout << "FEM::Create()" << std::endl;

	GLKMatrixLib::CreateMatrix(matNodes, matNodesRow, matNodesCol);
	GLKMatrixLib::CreateMatrix(matConn, matConnRow, matConnCol);
	GLKMatrixLib::CreateMatrix(B, Brow, Bcol);
	GLKMatrixLib::CreateMatrix(D, Drow, Dcol);
	CreateVector(f, frow);
	CreateVector(d, drow);
	GLKMatrixLib::CreateMatrix(K, Krow, Kcol);
	GLKMatrixLib::CreateMatrix(Ke, Kerow, Kecol);

	// Solving System of Equations
	CreateVectorIsol(Isol, Isolrow);
	GLKMatrixLib::CreateMatrix(Kmod, Kmodrow, Kmodcol);
	CreateVector(fmod, fmodrow);
	CreateVector(dmod, dmodrow);

	//Solving stresses
	CreateVector(sige, sigerow);
	CreateVector(de, derow);

	// Miscallenous
	GLKMatrixLib::CreateMatrix(BT, BTrow, BTcol);
	GLKMatrixLib::CreateMatrix(temp, trow, tcol);
	GLKMatrixLib::CreateMatrix(temp2, t2row, t2col);
	CreateVector(vonMisVec, vonMisVecrow);

}

void FEM::Destroy() {
	std::cout << "FEM::Destoy()" << std::endl;

	GLKMatrixLib::DeleteMatrix(matNodes, matNodesRow, matNodesCol);
	GLKMatrixLib::DeleteMatrix(matConn, matConnRow, matConnCol);
	GLKMatrixLib::DeleteMatrix(B, Brow, Bcol);
	GLKMatrixLib::DeleteMatrix(D, Drow, Dcol);
	DeleteVector(f);
	DeleteVector(d);
	GLKMatrixLib::DeleteMatrix(K, Krow, Kcol);
	GLKMatrixLib::DeleteMatrix(Ke, Kerow, Kecol);

	// Solving system of equations
	DeleteVectorIsol(Isol);
	GLKMatrixLib::DeleteMatrix(Kmod, Kmodrow, Kmodcol);
	DeleteVector(fmod);
	DeleteVector(dmod);

	//Solving stresses
	DeleteVector(sige);
	DeleteVector(de);

	// Miscallenous
	GLKMatrixLib::DeleteMatrix(BT, BTrow, BTcol);
	GLKMatrixLib::DeleteMatrix(temp, trow, tcol);
	GLKMatrixLib::DeleteMatrix(temp2, t2row, t2col);
	DeleteVector(vonMisVec);
}

void FEM::CreateVector(double* &ptr, int ptrsize) {

	ptr = NULL;

	ptr = new double[ptrsize];
	for (int i = 0; i < ptrsize; i++) {
		ptr[i] = 0.0;
	}
}

void FEM::DeleteVector(double* &ptr) {
	delete[] ptr;
	ptr = NULL;
}

void FEM::CreateVectorIsol(int* &ptr, int ptrsize) {

	ptr = NULL;

	ptr = new int[ptrsize];
	for (int i = 0; i < ptrsize; i++) {
		ptr[i] = 0;
	}
}

void FEM::DeleteVectorIsol(int* &ptr) {
	delete[] ptr;

	ptr = NULL;
}