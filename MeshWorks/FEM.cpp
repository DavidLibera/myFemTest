
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
#include <iostream>
#include <fstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	
	t = 0.5; // thickness
	E = 30e6; // modulus
	v = 0.25; // poissons
	
	//t = .001; //m 
	//E = 200e9; // 1 GPA = 1e9 Pa 
	//v = 0.3;
}

void FEM::setNodeMatrix() {

	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();

	std::cout << "(QBody*)(pDoc->m_meshList).GetHead():" << (QBody*)(pDoc->m_meshList).GetHead() << std::endl;
	std::cout << "body->GetTrglNodeList().GetHeadPosition():" << body->GetTrglNodeList().GetHeadPosition() << std::endl;

	/*
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
	*/
}

int FEM::getMeshType() {
	
	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	//CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	std::cout << "body->GetTrglFaceNumber()" << std::endl;
	std::cout << body->GetTrglFaceNumber() << std::endl;
	
	////vector stores all index numbers
	//std::vector<int> temp; 

	//// considering first face/element in linked list
	//GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition();
	//pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);

	//// initialize vector temp
	//int c = 1;
	//int index = (pFace->GetNodeRecordPtr(c))->GetIndexNo();
	//temp.push_back(index);

	//// create loop to count number of index before it repeats (max 7 node elements) 
	//for (int c = 2; c <=5 ; c++) { // should break before it reaches 7 (otherwise error, program crashes) 
	//	index = (pFace->GetNodeRecordPtr(c))->GetIndexNo();
	//	//temp.push_back(index);

	//	std::cout << index << std::endl; 

	//	// check if index is in the vector already and return if so
	//	//if (std::find(temp.begin(), temp.end(), index) != temp.end()) {
	//	//	return c;
	//	//	break;
	//	//}
	//}
	return 0;
}

void FEM::setConnMatrix() {

	// Allocating memory for matConn based on user input (Note: can't be in constructor) 
	matConnRow = nFaces;
	if (meshType == 0) { matConnCol = 3; }//meshType;
	if (meshType == 1) { matConnCol = 4; }//meshType;
	GLKMatrixLib::CreateMatrix(matConn, matConnRow, matConnCol);

	// Need pWnd to access GetActiveDocument and GetMainView unlike in CMainFrame:: functions 
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(pWnd->GetActiveDocument());
	CGLKernelView *cView = pWnd->GetMainView()->GetGLKernelView();

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	// Element Connectivity Matrix
	// Ref: Programming the FEM with Matlab - Jack Chessa - 3rd October 2002 - pg.4 

	// Construct matrix of node numbers where each row of the matrix contains 
	// the connectivity of an element 
	// Connectivities are all ordered in counter-clockwise fashion i.e 
	// Jacobians are not negative, thus causing Kmatrix to be singular 

	int c = 0; // counter
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		matConn[c][0] = (pFace->GetNodeRecordPtr(1))->GetIndexNo();
		matConn[c][1] = (pFace->GetNodeRecordPtr(2))->GetIndexNo();
		matConn[c][2] = (pFace->GetNodeRecordPtr(3))->GetIndexNo();
		if(meshType == 1 ) // Q4 element
			matConn[c][3] = (pFace->GetNodeRecordPtr(4))->GetIndexNo();
		c = c + 1;
	}
}

//void FEM::setMeshType(int a) {
//	meshType = a;
//}

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

	// CASE 1: DANGER (ONLY FOR SQUARE MESH) affected nodes and ONLY SELF MADE MESH will make sense

    //Y-displacement is zero at edge 
	int DOF1 = sqrt(nNodes) * 2;
	//int DOF2 = sqrt(nNodes) * 2 - 1;
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
		if (i != DOF1 && i != DOF3 && i != DOF4 && i != DOF5 && i != DOF6) {
			Isol[c] = i;
			c = c + 1;
		}
	}
	//NOTE: I need to indent this one -1
	f[FORCEDOF-1] = 1225; // Fy at node 3 is 1225
	//f[FORCEDOF - 2] = -1225; // Fx


	// CASE 2: FOR PLATE WITH HOLE 
	/*
	// Isol: an array that mimics vector Isol (required form for FEM) 
	std::vector<int> listNodesExclude = { 2, 5, 7, 3, 6, 4, 8, 1 }; 
	std::vector<int> listDOFInclude;
	
	// setting listDOFInclude 
	for (int i = 0; i < nDOF; i++) {
		listDOFInclude.push_back(i + 1);
	}

	// eliminating values from listDOFInclude from list listNodes Include (both x and y components) 
	int my_int; 
	for (int i = 0; i < listNodesExclude.size(); i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0)
				my_int = listNodesExclude[i] * 2;
			else
				my_int = listNodesExclude[i] * 2 - 1;

			auto it = std::find(listDOFInclude.begin(), listDOFInclude.end(), my_int);
			if (it != listDOFInclude.end()) {
				listDOFInclude.erase(it);
			}
		}
	}

	// Set Isol to vector
	for (int i = 0; i < listDOFInclude.size(); i++) {
		Isol[i] = listDOFInclude[i];
	}

	//set force vector
	f[2 * 251 - 2] = 1500;
	f[2 * 243 - 2] = 1500;
	f[2 * 245 - 2] = 1500;
	f[2 * 246 - 2] = 1500;
	f[2 * 248 - 2] = 1500;
	f[2 * 249 - 2] = 1500;
	f[2 * 247 - 2] = 1500;
	f[2 * 244 - 2] = 1500;
	f[2 * 242 - 2] = 1500;
	f[2 * 250 - 2] = 1500;
	*/
	// CASE 3: THIN BEAM IN TENSILE	 
	// case 3 by 10 and 10 by 50(using vectors)
	//std::vector<int> listNodesExclude = { 20,40,60,80,100,120 };
	/*
	std::vector<int> listNodesExclude = { 50,100,150,200,250,300,350,400,450,500 };
	//std::vector<int> listNodesExclude = { 100,200,300,400,500,600,700,800,900,,200,250,300,350,400,450,500 }; (TOOO BIG)
	std::vector<int> listDOFInclude;

	// setting listDOFInclude 
	for (int i = 0; i < nDOF; i++) {
		listDOFInclude.push_back(i + 1);
	}

	// eliminating values from listDOFInclude from list listNodes Include (both x and y components) 
	int my_int;
	for (int i = 0; i < listNodesExclude.size(); i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0)
				my_int = listNodesExclude[i] * 2;
			else
				my_int = listNodesExclude[i] * 2 - 1;

			auto it = std::find(listDOFInclude.begin(), listDOFInclude.end(), my_int);
			if (it != listDOFInclude.end()) {
				listDOFInclude.erase(it);
			}
		}
	}

	// Copy content of vector to Isol[]
	
	//std::cout << "printing Isol" << std::endl;
	for (int i = 0; i < listDOFInclude.size(); i++) {
		Isol[i] = listDOFInclude[i];
	}

	double Force = 2000 / 2; //listNodesExclude.size();  //total force divided equally between # nodes at edge

	//std::cout << "Force: " << Force << std::endl;

	//f[1 * 2 - 2] = -Force;
	//f[21 * 2 - 2] =-Force;
	//f[41 * 2 - 2] =-Force;
	//f[61 * 2 - 2] =-Force;
	//f[81 * 2 - 2] =-Force;
	//f[101 * 2 - 2] =-Force;

	//f[1 * 2 - 2]   =-Force;
	//f[51 * 2 - 2]  =-Force;
	//f[101 * 2 - 2] = Force; 
	//f[151 * 2 - 2] =-Force;
	f[201 * 2 - 2] =-Force;
	f[251 * 2 - 2] =-Force;
	//f[301 * 2 - 2] =-Force;
	//f[351 * 2 - 2] =-Force;
	//f[401 * 2 - 2] =-Force;
	//f[451 * 2 - 2] =-Force;

	// 20 by 100 case (TOO BIG) 
	//f[1 * 2 - 2]   =-Force;
	//f[51 * 2 - 2]  =-Force;
	//f[101 * 2 - 2] = Force; 
	//f[151 * 2 - 2] =-Force;
	//f[201 * 2 - 2] = -Force;
	//f[251 * 2 - 2] = -Force;
	//f[301 * 2 - 2] =-Force;
	//f[351 * 2 - 2] =-Force;
	//f[401 * 2 - 2] =-Force;
	//f[451 * 2 - 2] =-Force;
	//f[501 * 2 - 2] = -Force;
	//f[551 * 2 - 2] = -Force;
	//f[601 * 2 - 2] = Force;
	//f[651 * 2 - 2] = -Force;
	//f[701 * 2 - 2] = -Force;
	//f[751 * 2 - 2] = -Force;
	//f[801 * 2 - 2] = -Force;
	//f[851 * 2 - 2] = -Force;
	//f[901 * 2 - 2] = -Force;
	//f[951 * 2 - 2] = -Force;
	*/

	// CASE 4: Euler Bernouilli 
	/*
	std::vector<int> listNodesExclude = { 50,100,150,200,250,300,350,400,450,500 };//{20,40,60,80,100,120};
	std::vector<int> listDOFInclude;

	// setting listDOFInclude 
	for (int i = 0; i < nDOF; i++) {
		listDOFInclude.push_back(i + 1);
	}

	// eliminating values from listDOFInclude from list listNodes Include (both x and y components) 
	int my_int;
	for (int i = 0; i < listNodesExclude.size(); i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0)
				my_int = listNodesExclude[i] * 2;
			else
				my_int = listNodesExclude[i] * 2 - 1;

			auto it = std::find(listDOFInclude.begin(), listDOFInclude.end(), my_int);
			if (it != listDOFInclude.end()) {
				listDOFInclude.erase(it);
			}
		}
	}

	// Copy content of vector to Isol[]

	//std::cout << "printing Isol" << std::endl;
	for (int i = 0; i < listDOFInclude.size(); i++) {
		Isol[i] = listDOFInclude[i];
	}

	double Force = 2000 / listNodesExclude.size();  //total force divided equally between # nodes at edge

	// Shear force on side of beam (distributed equally on each node)
	f[1 * 2 - 1] = -Force;
	f[51 * 2 - 1] = -Force;
	f[101 * 2 - 1] = Force;
	f[151 * 2 - 1] = -Force;
	f[201 * 2 - 1] = -Force;
	f[251 * 2 - 1] = -Force;
	f[301 * 2 - 1] = -Force;
	f[351 * 2 - 1] = -Force;
	f[401 * 2 - 1] = -Force;
	f[451 * 2 - 1] = -Force;
	*/

}


// Computing

double FEM::integral(double(*f)(double x), double a, double b, int n) {
	double step = (b - a) / n;  // width of each small rectangle
	double area = 0.0;  // signed area
	for (int i = 0; i < n; i++) {
		area += f(a + (i + 0.5) * step) * step; // sum up each small rectangle
	}
	return area;
}

void FEM::computeKMatrixQ4() {

	std::cout << "Computing KMatrixQ4()" << std::endl;

	int n1, n2, n3, n4;
	double x1, y1, x2, y2, x3, y3, x4, y4;

	for (int element = 0; element < nFaces; element++) {
		// Extracting node number from matConn matrix
		n1 = matConn[element][0];
		n2 = matConn[element][1];
		n3 = matConn[element][2];
		n4 = matConn[element][3];

		// Using node number from above to access x,y coordinates (Note: index starts is 1 less for matrices, hence the -1)
		x1 = matNodes[n1 - 1][0]; y1 = matNodes[n1 - 1][1];
		x2 = matNodes[n2 - 1][0]; y2 = matNodes[n2 - 1][1];
		x3 = matNodes[n3 - 1][0]; y3 = matNodes[n3 - 1][1];
		x4 = matNodes[n4 - 1][0]; y4 = matNodes[n4 - 1][1];

		// Compute area for integral
		double A; 

		// compute Jacobian 
		double dxds, dxdt, dyds, dydt;
		dxds = -0.25*x1 + 0.25*x2 + 0.25*x3 - 0.25*x4; // sum (dNi/ds xi) 
		dxdt = -0.25*x1 - 0.25*x2 + 0.25*x3 + 0.25*x4; // ^--> t terms cancel i.e constant
		dyds = -0.25*y1 + 0.25*y2 + 0.25*y3 - 0.25*y4;
		dydt = -0.25*y1 - 0.25*y2 + 0.25*y3 + 0.25*y4;
		J[0][0] = dxds; J[0][1] = dyds;
		J[1][0] = dxdt; J[1][1] = dydt;
		
		double detJ = J[0][0] * J[1][1] - J[0][1] * J[1][0];

		GLKMatrixLib::Inverse(J, Jrow); 
		//Note: does not need to be dynamic memory, but then will Inverse function work (**a) parameter ?  
		PrintMatrix(J, Jrow, Jcol);

		// Computing table Me Matrix (compute dNi/dx dNi/dy for B matrix (Ke is 8x8 matrix))
		setShapeFunctionTable();

		// Compute top half of elemental stiffness matrix (1 element at a time)
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				// summation loop
				double sum = 0.0, fi = 0.0;
				for (int k = 0; k < 4; k++) {

					selectB1B2(i, j, k,B1,B2);

					//std::cout << "____________________________________"<< std::endl;
					//std::cout << "for i =" << i << "j = " << j << std::endl;
					//std::cout << "B1:" << B1[0] << " " << B1[1] << " " << B1[2] << std::endl;
					//std::cout << "B2:" << B2[0] << " " << B2[1] << " " << B2[2] << std::endl;
					//std::cout << "temp3:" << temp3[0] << " " << temp3[1] << " " << temp3[2] << std::endl;

					// Solve fi = B1.' D B2 (using GLKMatrixLib)
					GLKMatrixLib::Transpose(D, 3); // doesn't matter for symmetric D matrix
					GLKMatrixLib::Mul(D, B1,3,3,temp3);
					GLKMatrixLib::Transpose(D, 3); // transpose D back?

					//std::cout << "B1:" << B1[0] << B1[1] << B1[2] << std::endl;
					//std::cout << "B2:" << B2[0] << B2[1] << B2[2] << std::endl;
					//std::cout << "temp3:" << temp3[0] << temp3[1] << temp3[2] << std::endl;

					fi = temp3[0]*B2[0]+temp3[1]*B2[1]+temp3[2]*B2[2];

					sum = sum + fi;

				}

				Ke[i][j] = sum*detJ;

				//std::cout << "Ke for i:"<< i << "and j:" << j << " " << Ke[i][j] << std::endl;

			}
		}

		// Apply symmetry
		//for (int i = 0; i < 8; i++) {
		//	for (int j = 0; j < 8-i; j++) {
		//		Ke[j][i] = Ke[i][j];
		//	}
		//}

		// Scatter to global matrix (FOR LATER) (SEE SCATTER FUNCTIONS) 
		scatter(Ke, K, n1, n2, n3, n4);

	}
}

void FEM::setShapeFunctionTable() {
	// Me is a table (8by4) of shape functions at gaussian quadrature values 
	// Format: 
	//	    (1/sq(3), 1/sq(3) ) (-1/sq(3), 1/sq(3) ) (-1/sq(3), -1/sq(3) ) (1/sq(3), -1/sq(3) ) 
	// N1x
	// N1y
	// N2x
	// ...
	// N4y 

	std::cout << "Setting ShapeFunctionTable()" << std::endl;

	// Allocate memory (only if using Q4) 
	Merow = 8; Mecol = 4;
	GLKMatrixLib::CreateMatrix(Me, Merow, Mecol);
	
	double tt, ss; // local variables whereas x,y are global

	// Shape functions of parent element in isoparametric coordinates
	// Ref: MANE 4240 & CIVL 4230 Intro to Finite Elements - Mapped element geometries and shape functions : the isoparametric formulationpg.6

	// Shape functions expressed in (s,t) coord system
	// N1 = 0.25*(1-ss)*(1-tt)
	// N2 = 0.25*(1+ss)*(1-tt)
	// N3 = 0.25*(1+ss)*(1+tt)
	// N4 = 0.25*(1-ss)*(1+tt)

	// It is laborious to find the inverse map s(x,y) and t(x,y)
	// Instead we compute the integrals in the domain of parent element
	//		Nx,
	//		Ny, Nx, 
	//		  , Ny, 
	//
	//Me = 
	//
	//
	//

	double Nx, Ny;
	// N1x N1y terms ONLY = invJ * N1s N1t (eval at 4 points/cases)
	for (int i = 1; i <= 4; i++) {
		switch (i) {
		case 1: 
				tt = 1 / sqrt(3); ss = 1 / sqrt(3);
				Nx = J[0][0] * (-0.25*(1 - tt)) + J[0][1] * (-0.25*(1 - ss));
				Ny = J[1][0] * (-0.25*(1 - tt)) + J[1][1] * (-0.25*(1 - ss));
				Me[0][0] = Nx; Me[1][0] = Ny; 
				break;
		case 2:	
				tt = -1 / sqrt(3); ss = 1 / sqrt(3);
				Nx = J[0][0] * (-0.25*(1 - tt)) + J[0][1] * (-0.25*(1 - ss)); 
				Ny = J[1][0] * (-0.25*(1 - tt)) + J[1][1] * (-0.25*(1 - ss));
				Me[0][1] = Nx; Me[1][1] = Ny; 
				break;
		case 3:	
				tt = -1 / sqrt(3); ss = -1 / sqrt(3);
				Nx = J[0][0] * (-0.25*(1 - tt)) + J[0][1] * (-0.25*(1 - ss));
				Ny = J[1][0] * (-0.25*(1 - tt)) + J[1][1] * (-0.25*(1 - ss));
				Me[0][2] = Nx; Me[1][2] = Ny;
				break;
		case 4:
				tt = 1 / sqrt(3); ss = -1 / sqrt(3);
				Nx = J[0][0] * (-0.25*(1 - tt)) + J[0][1] * (-0.25*(1 - ss));
				Ny = J[1][0] * (-0.25*(1 - tt)) + J[1][1] * (-0.25*(1 - ss));
				Me[0][3] = Nx; Me[1][3] = Ny;
				break;
		}
	}

	// N2x N2y terms ONLY = invJ * N2s N2t (eval at 4 points/cases)
	for (int i = 1; i <= 4; i++) {
		switch (i) {
		case 1:
			tt = 1 / sqrt(3); ss = 1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 - tt)) + J[0][1] * (-0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 - tt)) + J[1][1] * (-0.25*(1 + ss));
			Me[2][0] = Nx; Me[3][0] = Ny;
			break;
		case 2:
			tt = -1 / sqrt(3); ss = 1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 - tt)) + J[0][1] * (-0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 - tt)) + J[1][1] * (-0.25*(1 + ss));
			Me[2][1] = Nx; Me[3][1] = Ny;
			break;
		case 3:
			tt = -1 / sqrt(3); ss = -1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 - tt)) + J[0][1] * (-0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 - tt)) + J[1][1] * (-0.25*(1 + ss));
			Me[2][2] = Nx; Me[3][2] = Ny;
			break;
		case 4:
			tt = 1 / sqrt(3); ss = -1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 - tt)) + J[0][1] * (-0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 - tt)) + J[1][1] * (-0.25*(1 + ss));
			Me[2][3] = Nx; Me[3][3] = Ny;
			break;
		}
	}

	// N3x N3y terms ONLY = invJ * N3s N3t (eval at 4 points/cases)
	for (int i = 1; i <= 4; i++) {
		switch (i) {
		case 1:
			tt = 1 / sqrt(3); ss = 1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 + tt)) + J[0][1] * (0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 + tt)) + J[1][1] * (0.25*(1 + ss));
			Me[4][0] = Nx; Me[5][0] = Ny;
			break;
		case 2:
			tt = -1 / sqrt(3); ss = 1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 + tt)) + J[0][1] * (0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 + tt)) + J[1][1] * (0.25*(1 + ss));
			Me[4][1] = Nx; Me[5][1] = Ny;
			break;
		case 3:
			tt = -1 / sqrt(3); ss = -1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 + tt)) + J[0][1] * (0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 + tt)) + J[1][1] * (0.25*(1 + ss));
			Me[4][2] = Nx; Me[5][2] = Ny;
			break;
		case 4:
			tt = 1 / sqrt(3); ss = -1 / sqrt(3);
			Nx = J[0][0] * (0.25*(1 + tt)) + J[0][1] * (0.25*(1 + ss));
			Ny = J[1][0] * (0.25*(1 + tt)) + J[1][1] * (0.25*(1 + ss));
			Me[4][3] = Nx; Me[5][3] = Ny;
			break;
		}
	}

	// N4x N4y terms ONLY = invJ * N4s N4t (eval at 4 points/cases)
	for (int i = 1; i <= 4; i++) {
		switch (i) {
		case 1:
			tt = 1 / sqrt(3); ss = 1 / sqrt(3);
			Nx = J[0][0] * (-0.25*(1 + tt)) + J[0][1] * (0.25*(1 - ss));
			Ny = J[1][0] * (-0.25*(1 + tt)) + J[1][1] * (0.25*(1 - ss));
			Me[6][0] = Nx; Me[7][0] = Ny;
			break;
		case 2:
			tt = -1 / sqrt(3); ss = 1 / sqrt(3);
			Nx = J[0][0] * (-0.25*(1 + tt)) + J[0][1] * (0.25*(1 - ss));
			Ny = J[1][0] * (-0.25*(1 + tt)) + J[1][1] * (0.25*(1 - ss));
			Me[6][1] = Nx; Me[7][1] = Ny;
			break;
		case 3:
			tt = -1 / sqrt(3); ss = -1 / sqrt(3);
			Nx = J[0][0] * (-0.25*(1 + tt)) + J[0][1] * (0.25*(1 - ss));
			Ny = J[1][0] * (-0.25*(1 + tt)) + J[1][1] * (0.25*(1 - ss));
			Me[6][2] = Nx; Me[7][2] = Ny;
			break;
		case 4:
			tt = 1 / sqrt(3); ss = -1 / sqrt(3);
			Nx = J[0][0] * (-0.25*(1 + tt)) + J[0][1] * (0.25*(1 - ss));
			Ny = J[1][0] * (-0.25*(1 + tt)) + J[1][1] * (0.25*(1 - ss));
			Me[6][3] = Nx; Me[7][3] = Ny;
			break;
		}
	}

}

void FEM::selectB1B2(int i, int j, int k, double B1[3], double B2[3]) {
	//B1 and B2 will depend on row and col (i,j)
	// and also will be summed over 4 points (k =1,...4) from table Me 
	switch (i) {
	case 0: B1[0] = Me[0][k];	B1[1] = 0.0;		B1[2] = Me[1][k]; break;
	case 1: B1[0] = 0.0;		B1[1] = Me[1][k];	B1[2] = Me[0][k]; break;
	case 2:	B1[0] = Me[2][k];	B1[1] = 0.0;		B1[2] = Me[3][k]; break;
	case 3:	B1[0] = 0.0;		B1[1] = Me[3][k];	B1[2] = Me[2][k]; break;
	case 4:	B1[0] = Me[4][k];	B1[1] = 0.0;		B1[2] = Me[5][k]; break;
	case 5:	B1[0] = 0.0;		B1[1] = Me[5][k];	B1[2] = Me[4][k]; break;
	case 6:	B1[0] = Me[6][k];	B1[1] = 0.0;		B1[2] = Me[7][k]; break;
	case 7:	B1[0] = 0.0;		B1[1] = Me[7][k];	B1[2] = Me[6][k]; break;
	}
	switch (j) {
	case 0: B2[0] = Me[0][k];	B2[1] = 0.0;		B2[2] = Me[1][k]; break;
	case 1: B2[0] = 0.0;		B2[1] = Me[1][k];	B2[2] = Me[0][k]; break;
	case 2:	B2[0] = Me[2][k];	B2[1] = 0.0;		B2[2] = Me[3][k]; break;
	case 3:	B2[0] = 0.0;		B2[1] = Me[3][k];	B2[2] = Me[2][k]; break;
	case 4:	B2[0] = Me[4][k];	B2[1] = 0.0;		B2[2] = Me[5][k]; break;
	case 5:	B2[0] = 0.0;		B2[1] = Me[5][k];	B2[2] = Me[4][k]; break;
	case 6:	B2[0] = Me[6][k];	B2[1] = 0.0;		B2[2] = Me[7][k]; break;
	case 7:	B2[0] = 0.0;		B2[1] = Me[7][k];	B2[2] = Me[6][k]; break;
	}
}


void FEM::computeKMatrixT3() {

	int n1, n2, n3;
	double x1, y1, x2, y2, x3, y3;
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
		scatter(Ke, K, n1, n2, n3,0);

	}

}

void FEM::computeStress()
{

	int n1, n2, n3;
	double b1, b2, b3, c1, c2, c3, x1, x2, x3, y1, y2, y3, A;

	//openfile for extracting data for MATLAB
	std::ofstream myfile;
	myfile.open("stresses.txt");

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
		A = abs(0.5*((x2*y3 - x3*y2) + (x3*y1 - x1*y3) + (x1*y2 - x2*y1))); // needs an absolute sign

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

		double xavg = (x1 + x2 + x3) / 3;
		double yavg = (y1 + y2 + y3) / 3;

		// Writing to file
		toFile(element, xavg,yavg,sige[0], sige[1], sige[2], vonMises,myfile);

		//Print stresses
		
		//if (element % 200 == 0) {
		//	std::cout << "Sige:" << std::endl;
		//	PrintVector(sige, sigerow);
		//	std::cout << "vonMises of element:" << element << "is " << vonMises << std::endl;
		//}
		
	}

}

double FEM::computeVonMises() {

	// For 2D problems only
	return sqrt((pow((sige[0] - sige[1]), 2) + pow(sige[0], 2) + pow(sige[1], 2) + 6 * pow(sige[2], 2)) / 2);
}



// Scattering

void FEM::scatter(double** &Ke, double** &K, int n1, int n2, int n3, int n4) {

	int row = 0, col =0;

	for (int i = 0; i < Kerow; i++) {

		switch (i) {
		case 0: row = 2 * n1 - 2; break;
		case 1: row = 2 * n1 - 1; break;
		case 2: row = 2 * n2 - 2; break;
		case 3: row = 2 * n2 - 1; break;
		case 4: row = 2 * n3 - 2; break;
		case 5: row = 2 * n3 - 1; break;
		case 6: row = 2 * n4 - 2; break;
		case 7: row = 2 * n4 - 1; break;
		}

		for (int j = 0; j < Kecol; j++) {

			switch (j) {
			case 0: col = 2 * n1 - 2; break;
			case 1: col = 2 * n1 - 1; break;
			case 2: col = 2 * n2 - 2; break;
			case 3: col = 2 * n2 - 1; break;
			case 4: col = 2 * n3 - 2; break;
			case 5: col = 2 * n3 - 1; break;
			case 6: col = 2 * n4 - 2; break;
			case 7: col = 2 * n4 - 1; break;
			}
			K[row][col] = K[row][col] + Ke[i][j];
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

	// Function: To distinguish between Triangular and Hex mesh for Stiffness Matrix (future work) 
	//int index = getMeshType();
	
	//std::cout << "PRETEST" << std::endl;
	//setDMatrix();
	//B1[0] = 1; B1[1] = 0; B1[2] = 0;
	//GLKMatrixLib::Mul(D, B1, 3, 1, temp3);
	//std::cout << "B1: " << B1[0] << B1[1] << B1[2] << std::endl;
	//std::cout << "temp3: " << temp3[0] << temp3[1] << temp3[2] << std::endl;


	// START OF PROGRAM

	//setting mesh to Triangular 
	meshType = 0;

	std::cout << "meshType is " << meshType << std::endl; 

	// Do computations of matrix 
	setProps();
	setNodeMatrix();
	setConnMatrix();
	setDMatrix();

	PrintMatrix(matNodes, matNodesRow, matNodesCol);
	PrintMatrix(matConn, matConnRow, matConnCol);

	bool debug = true;

	//Compute K matrix
	if (debug = false) {
		if (meshType == 0) {
			Kerow = 6; Kecol = 6;
			GLKMatrixLib::CreateMatrix(Ke, Kerow, Kecol);
			computeKMatrixT3();
		}
		if (meshType == 1) {
			Kerow = 8; Kecol = 8;
			GLKMatrixLib::CreateMatrix(Ke, Kerow, Kecol);
			computeKMatrixQ4();
		}

		//PrintMatrix(J, Jrow, Jcol);
		//PrintMatrix(Me, Merow, Mecol);
		//PrintMatrix(Ke, Kerow, Kecol);

		//Set BCS
		setBCs();

		//Modify System of Equations based on BCs
		scatterKmod(K, Kmod);
		scatterfmod(f, fmod);

		//PrintMatrix(Kmod, Kmodrow, Kmodcol);
		//PrintVector(fmod, fmodrow);

		//Solve system of equations
		bool doesSystemSolve = GLKMatrixLib::GaussJordanElimination(Kmod, vars, fmod);
		//bool doesSystemSolve = GLKMatrixLib::GaussSeidelSolver(Kmod, fmod, vars, dmod,1e-6);
		scatterBackDisplacements(fmod, d);

		std::cout << "bool doesSystemSolve =" << doesSystemSolve << std::endl;

		std::cout << "displacements:" << std::endl;
		PrintVector(d, drow);

		//Compute Stresses

		computeStress();

		// POST PROCESSING **************************************

		std::ofstream myfile;
		double dtot;
		myfile.open("displacements.txt");
		for (int i = 0, j = 0; i < nNodes; i++, j = j + 2) {
			dtot = sqrt(pow(d[j], 2) + pow(d[j + 1], 2));
			myfile << i << " " << matNodes[i][0] << " " << matNodes[i][1] << " " << d[j] << " " << d[j + 1] << " " << dtot << std::endl;
		}

	}


	/*
	//Normalize vonMisVec (first find max value, then divide all by it)
	//double maxVM = 0.0;
	//int maxVMindex = 0;
	//for (int i = 0; i < nFaces; i++) {
	//	if (vonMisVec[i] > maxVM) {
	//		maxVM = vonMisVec[i];
	//		maxVMindex = i;
	//	}

	//}


	//std::cout << "Max VM (Pa): " << maxVM << "at Face index" << maxVMindex << std::endl;

	//for (int i = 0; i < nFaces; i++) {
	//	vonMisVec[i] = 1.0 - (vonMisVec[i]/ maxVM);
	//}
	*/
	
	std::cout << "END OF FEM" << std::endl;

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

void FEM::openFile() {
	std::ofstream myfile;
	myfile.open("example.txt");
}

void FEM::toFile(int face, double xavg, double yavg, double sigx, double sigy, double tauxy, double vonMis, std::ofstream& myfile) {
	myfile << face << " " << xavg << " " << yavg << " " << sigx << " " << sigy << " " << tauxy << " " << vonMis << std::endl;
}


// Setting Memory Functions
void FEM::Create() {
	std::cout << "FEM::Create()" << std::endl;

	GLKMatrixLib::CreateMatrix(matNodes, matNodesRow, matNodesCol);
	//GLKMatrixLib::CreateMatrix(matConn, matConnRow, matConnCol); this is created in setConnMatrix() its based on user input
	GLKMatrixLib::CreateMatrix(B, Brow, Bcol);
	GLKMatrixLib::CreateMatrix(D, Drow, Dcol);
	CreateVector(f, frow);
	CreateVector(d, drow);
	GLKMatrixLib::CreateMatrix(K, Krow, Kcol);
	//GLKMatrixLib::CreateMatrix(Ke, Kerow, Kecol);
	GLKMatrixLib::CreateMatrix(J, Jrow, Jcol);
	CreateVector(B1, B1row);
	CreateVector(B2, B2row);
	CreateVector(temp3, t3row);

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
	if(matConn != NULL) // only if matConn was allocated memory, do we need to deallocate
		GLKMatrixLib::DeleteMatrix(matConn, matConnRow, matConnCol);
	GLKMatrixLib::DeleteMatrix(B, Brow, Bcol);
	GLKMatrixLib::DeleteMatrix(D, Drow, Dcol);
	DeleteVector(f);
	DeleteVector(d);
	GLKMatrixLib::DeleteMatrix(K, Krow, Kcol);
	if(Ke != NULL) //Ke value is set in analyze (because of meshtype choice)
		GLKMatrixLib::DeleteMatrix(Ke, Kerow, Kecol);
	GLKMatrixLib::DeleteMatrix(J, Jrow, Jcol);
	if(Me != NULL)
		GLKMatrixLib::DeleteMatrix(Me, Merow, Mecol);
	DeleteVector(B1);
	DeleteVector(B2);
	DeleteVector(temp3);

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