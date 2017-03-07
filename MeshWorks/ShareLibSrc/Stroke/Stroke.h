#pragma once
#ifndef _C_STROKE_H_
#define _C_STROKE_H_
#include "../base/defs.h"
#include <vector>

using namespace std;


class CStroke
{
public:
	CStroke(void);
	~CStroke(void);
private:
	struct Point3D{
		float x,y,z;
	};
	struct Point2D{
		float x,y;
	};
	struct BOX
	{
		Point2D pt[4];
	};
	vector<BOX*> * m_pStrokeBoxVect;
	vector <Point3D*> *m_pStrokeVert3DVect;
	vector <Point3D*> *m_pStrokeNormVect;
	vector <Point3D*> *m_pStrokeDisVertVect;
	vector <Point2D*> *m_pStrokeVert2DVect;
	float m_fWidth;
	float m_fLineColor[3];
	float m_fPointColor[3];

public:
	void AddPoint3D(double x, double y, double z,bool dis=true);
	void Display(bool offset=false);
	int GetStrokePointsNum(void);
	void GetStrokePoint(int index, double* x, double* y, double* z);
	void AddPoint2D(float x, float y);
	void BuildStrokeBox(void);
	bool IsPointInStroke(float x, float y,float* vtos);
	bool IsPointInBox(float x, float y,BOX* box);
	float GetWidth(void);
	void EraseLast(void);
private:
	bool m_bFinished;
public:
	void SetFinished(bool flag);
	void BSplineFitting(void);
	void SolveTridiagonal(int n, Point3D* Q, double* U, Point3D* P);
	void BasisFuns(int i, double t, int p, double* U, double* N);
	void Resample(void);
	void CurvePoint(double t , Point3D* pPt);
	int FindSpan(double t, int n, int p, double* U);
private:
	Point3D* m_Q;
	int m_n;
	int m_p;
	double* m_U;
	Point3D* m_P;
	double m_TotLen;

public:
	void ExportV(FILE* file);
	void ExportF(FILE* file, int *offPos);
	void GetStrokePoint2D(int index, float* x, float* y);
	void GetStroke2S(float* x, float* y);
	void GetStroke2E(float* x, float* y);
	void SetStrokePoint(int index, double x, double y, double z);
	void GetStrokeNorm(int index, float* x, float* y, float* z);
	void SetStrokeNorm(int index, float x, float y, float z);
	void AddDisPoint(double x, double y, double z);
	void SetLineColor(float r, float g, float b);
	void SetPointColor(float r, float g, float b);
};

#endif