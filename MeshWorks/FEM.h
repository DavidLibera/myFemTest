#pragma once
#include <iostream>

class FEM {
private:
	float t, E, v; // thickness, modulus, poisson
	double** B;
public:
	FEM() { std::cout << "FEM constructor called" << std::endl; };
	~FEM() { std::cout << "FEM destructor called" << std::endl; };
	void setProps();
	void setNodeMatrix();
	void printNodeCoord();
};

