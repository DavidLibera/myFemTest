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
	void PrintVector(double* &a, int row);

	// Setting
	void setProps();
	void setNodeMatrix();
	void setConnMatrix();
	void setDMatrix(); // strain-stress matrix
	void setBCs();

	// Computing
	void computeKMatrix();
	void scatter(double** &Ke, double** &K, int n1, int n2, int n3);
	void scatterKmod(double** &K, double** &Kmod);
	void scatterfmod(double* &f, double* &fmod);
	void scatterBackDisplacements(double* &dmod, double* &d);
	void computeStress();
	double computeVonMises();

	// Getting functions
	int getNumberOfNodes();
	int getNumberOfFaces();

	// Setting memory
	void Create();
	void Destroy();
	void CreateVector(double* &ptr, int ptrsize);
	void DeleteVector(double* &ptr);
	void CreateVectorIsol(int* &ptr, int ptrsize);
	void DeleteVectorIsol(int* &ptr);

	// PostProcessing
	void colorFaces();

	// Main function
	void MainFunction();

public:
	// Sizes
	int nNodes = getNumberOfNodes();
	int nFaces = getNumberOfFaces();
	int nDOF = nNodes * 2;

	// Matrices/Vectors
	double** matNodes; int matNodesRow = nNodes, matNodesCol = 2;
	double** matConn;  int matConnRow = nFaces, matConnCol = 3;
	double** D; int Drow = 3, Dcol = 3;
	double** B; int Brow = 3, Bcol = 6;
	double* f; int frow = nDOF;
	double* d; int drow = nDOF;
	double** K; int Krow = nDOF, Kcol = nDOF;
	double** Ke; int Kerow = 6, Kecol = 6;

	// Matrices/Vectors for solving systems
	int vars = nDOF-5; // unknowns
	int* Isol; int Isolrow = vars; // THESE DIMENSIONS AFFECT THE NEXT MOD MATRICES for general case
	double** Kmod; int Kmodrow = vars, Kmodcol = vars;
	double* fmod; int fmodrow = vars;
	double* dmod; int dmodrow = vars;

	//Matrices/Vectors for stress strain elemental
	double* sige; int sigerow = 3; // sige = stress element 
	double* de; int derow = 6;     // de = displacement element  

	//miscallenous matrices
	double** BT;			int BTrow = Bcol, BTcol = Brow;
	double**temp;			int trow = 6, tcol = 3;
	double**temp2;          int t2row = 3, t2col = 6;

};

