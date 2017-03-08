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

	// Getting functions
	int getNumberOfNodes();
	int getNumberOfFaces();

	// Setting memory
	void Create();
	void Destroy();

	// Calculating
	void MainFunction();

public:
	// Matrices
	double** matNodes; int matNodesRow = getNumberOfNodes(), matNodesCol = 2;
	double** matConn;  int matConnRow = getNumberOfFaces(), matConnCol = 3;
	double** B; int Brow = 3, Bcol = 6;

};

