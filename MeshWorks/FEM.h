#pragma once
#include <iostream>

class FEM {

public:
	// Members
	float t, E, v; // thickness, modulus, poisson

	// Functions 
	FEM() { std::cout << "FEM constructor called" << std::endl; 

			// Initializing Sizes
			nNodes = getNumberOfNodes();
			nFaces = getNumberOfFaces();
			nDOF = nNodes * 2;

			std::cout << "nNodes: " << nNodes << std::endl;
			std::cout << "nFaces: " << nFaces << std::endl;
			std::cout << "nDOF: " << nDOF << std::endl;



			//Initializing Matrices/Vectors
			matNodesRow = nNodes; matNodesCol = 2;
			matConnRow = nFaces; matConnCol = 3;
			Drow = 3; Dcol = 3;
			Brow = 3; Bcol = 6;
			frow = nDOF;
			drow = nDOF;
			Krow = nDOF; Kcol = nDOF;
			Kerow = 6; Kecol = 6;

			//Initializing for solver
			vars = nNodes * 2 - 5; // unknowns (DEPENDS ON BCS) ************************************************
			Isolrow = vars;
			Kmodrow = vars; Kmodcol = vars;
			fmodrow = vars;
			dmodrow = vars;

			std::cout << "vars: " << vars << std::endl;

			//Initialize for each element
			sigerow = 3;
			derow = 6;

			//Initialize 
			BTrow = Bcol; BTcol = Brow;
			trow = 6; tcol = 3;
			t2row = 3; t2col = 6;
			vonMisVecrow = nFaces;

			//Allocate memory
			Create(); 
			};

	~FEM() { std::cout << "FEM destructor called" << std::endl; Destroy(); };
	
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
	int nNodes;// = getNumberOfNodes();
	int nFaces; // = getNumberOfFaces();
	int nDOF;// = nNodes * 2;

	// Matrices/Vectors
	double** matNodes; int matNodesRow,matNodesCol;
	double** matConn;  int matConnRow,matConnCol;
	double** D;  int Drow, Dcol;
	double** B;  int Brow, Bcol;
	double* f;   int frow;
	double* d;   int drow;
	double** K;  int Krow, Kcol;
	double** Ke; int Kerow, Kecol;

	// Matrices/Vectors for solving systems

	// NOTE: WILL NEED TO MODIFY WHEN INTERACTIVE USER DEFINDED more than 2 or 5 BCS
	//DANGER (vars = nDOF - 5 because 5 BCS are imposed otherwise must change for more general case)
	int vars;// = getNumberOfNodes() * 2 - 6; // unknowns
	int* Isol;     int Isolrow;
	double** Kmod; int Kmodrow, Kmodcol;
	double* fmod;  int fmodrow;
	double* dmod;  int dmodrow;

	//Matrices/Vectors for stress strain elemental
	double* sige; int sigerow; //= 3; // sige = stress element 
	double* de; int derow; //= 6;     // de = displacement element  

	//miscallenous matrices
	double** BT;			int BTrow,BTcol;
	double**temp;			int trow,tcol;
	double**temp2;          int t2row, t2col;
	double* vonMisVec;      int vonMisVecrow; 

};

