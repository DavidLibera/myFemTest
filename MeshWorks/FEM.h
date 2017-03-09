#pragma once
#include <iostream>

class FEM {

public:
	// Members
	float t, E, v; // thickness, modulus, poisson

	// Functions 
	FEM() { std::cout << "FEM constructor called" << std::endl; };
	~FEM() { std::cout << "FEM destructor called" << std::endl; };
	
	// printing 
	void printNodeCoord();
	void PrintMatrix(double** &a, int row, int col);

	// Setting
	void setProps();
	void setNodeMatrix();
	void setConnMatrix();
	void setDMatrix(); // strain-stress matrix

	// Computing
	void computeKMatrix();
	void scatter(double** &Ke, double** &K, int n1, int n2, int n3);

	// Getting functions
	int getNumberOfNodes();
	int getNumberOfFaces();

	// Setting memory
	void Create();
	void Destroy();

	// PostProcessing
	void colorFaces();

	// Main function
	void MainFunction();

public:
	// Sizes
	int nNodes = getNumberOfNodes();
	int nFaces = getNumberOfFaces();
	int nDOF = nNodes * 2;

	// Matrices
	double** matNodes; int matNodesRow = nNodes, matNodesCol = 2;
	double** matConn;  int matConnRow = nFaces, matConnCol = 3;
	double** D; int Drow = 3, Dcol = 3;
	double** B; int Brow = 3, Bcol = 6;
	double** f; int frow = nDOF, fcol = 1; 
	double** d; int drow = nDOF, dcol = 1;
	double** K; int Krow = nDOF, Kcol = nDOF;
	double** Ke; int Kerow = 6, Kecol = 6;

	//miscallenous matrices
	double** BT;			int BTrow = Bcol, BTcol = Brow;
	double**temp;			int trow = 6, tcol = 3;

};

