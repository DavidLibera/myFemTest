#pragma once
#include <iostream>

class FEM {

public:
	// Members
	float t, E, v; // thickness, modulus, poisson
	int meshType;  // 0 for T3
				   // 1 for Q4 

	// Functions 
	FEM() 
	{ 
		std::cout << "FEM constructor called" << std::endl;
		// Initializing Sizes
		nNodes = getNumberOfNodes();
		nFaces = getNumberOfFaces();
		nDOF = nNodes * 2;

		//Initializing Matrices/Vectors
		matNodesRow = nNodes; matNodesCol = 2;
		Drow = 3; Dcol = 3;
		Brow = 3; Bcol = 6;
		frow = nDOF;
		drow = nDOF;
		Krow = nDOF; Kcol = nDOF;
		//Kerow = 6; Kecol = 6;
		Jrow = 2; Jcol = 2;
		B1row = 3; B2row = 3;

		//Initializing for solver
		vars = nDOF - 5; // unknowns (DEPENDS ON BCS) ************************************************
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
		t3row = 3;
		vonMisVecrow = nFaces;

		// Initializing matConn Pointer to NULL (this determines whether to deallocate) 
		matConn = NULL;
		Ke = NULL;
		Me = NULL; 

		Create();
	};

	~FEM() 
	{ 
		std::cout << "FEM destructor called" << std::endl;  
		Destroy();
	};
	
	// printing 
	void printNodeCoord();
	void PrintMatrix(double** &a, int row, int col);
	void PrintVector(double* &a, int row);

	// Setting
	int getMeshType();
	void setProps();
	void setNodeMatrix();
	void setConnMatrix();
	//void setMeshType(int a);
	void setDMatrix(); // strain-stress matrix
	void setBCs();
	void setShapeFunctionTable(); // for Q4 gaussian-integration

	// Computing
	void computeKMatrixT3();
	void computeKMatrixQ4();
	void selectB1B2(int i, int j, int k, double B1[3], double B2[3]);
	double integral(double(*f)(double x), double a, double b, int n);
	void scatter(double** &Ke, double** &K, int n1, int n2, int n3,int n4);
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
	void openFile();
	void toFile(int node, double xavg, double yavg,
				double sigx, double sigy, double tauxy,
				double vonMis, std::ofstream& myfile);

	// Main function
	void MainFunction();

public:
	// Sizes
	int nNodes;
	int nFaces;
	int nDOF;

	// Matrices/Vectors
	double** matNodes; int matNodesRow,matNodesCol;
	double** matConn;  int matConnRow, matConnCol;
	double** D;  int Drow, Dcol;
	double** B;  int Brow, Bcol;
	double* f;   int frow;
	double* d;   int drow;
	double** K;  int Krow, Kcol;
	double** Ke; int Kerow, Kecol;
	double** J; int Jrow, Jcol;
	double** Me; int Merow, Mecol;
	double* B1; int B1row;
	double* B2; int B2row;

	// Matrices/Vectors for solving systems
	int vars;
	int* Isol;     int Isolrow;
	double** Kmod; int Kmodrow, Kmodcol;
	double* fmod;  int fmodrow;
	double* dmod;  int dmodrow;

	//Matrices/Vectors for stress strain elemental
	double* sige; int sigerow;
	double* de; int derow;

	//miscallenous matrices
	double** BT;			int BTrow,BTcol;
	double**temp;			int trow,tcol;
	double**temp2;          int t2row, t2col;
	double* temp3;			int t3row;
	double* vonMisVec;      int vonMisVecrow;

};

