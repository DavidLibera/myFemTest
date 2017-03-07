#include "..\LinearSolver\LinearSolver.h" //this is all you need to include for using
//default, TAUCS is using, if SuperLU is needed, go into LinerSolver.h to enable #define _SUPERLU


//a sample of setting values to CSpMatrix
void CopyGLKSparseMatrixToCSpMatrix(GLKSparseMatrix &ATA)
{
	CSpMatrix matA(vertNum, vertNum);
	int rowNum = (int)ATA->GetRowNumber();
	for(int i=0; i<rowNum; i++) {
		GLKSparseMatrixNode *currentNode=ATA->GetRowRoots()[i];
		for(;currentNode!=NULL;) {
			GLKSparseMatrixNode *nextNode=currentNode->pNext;
			int j=currentNode->nColIndex;

			double value = currentNode->data;
			currentNode=nextNode;
			matA.Set(i, j, value, false); //here
		}
	}
}

//demostrate the usage
void main()
{
	time=clock();
	printf("Factorizating......");
	void *FactoredMat=NULL;
	if (!LinearSolver::FactorA(matA, FactoredMat)){
		printf("Cannot factorize F!\n"); return false;
	}
	printf("(%dms)\n", clock()-time);

	time=clock();
	printf("Solving......");
	if (!LinearSolver::SolveX(FactoredMat, x, b)){
		printf("Cannot solve!\n"); return false;
	}
	printf("(%dms)\n", clock()-time);

	//free memory
	LinearSolver::DeleteF(FactoredMat);
}