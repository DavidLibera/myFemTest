// CW_Geometry.h: interface for the CCW_Geometry class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCW_GEOMETRY
#define CCW_GEOMETRY

#include <afx.h>

#define EPS		1.0E-8
#define PI		3.1415926
#define DEGREE_TO_ROTATE(x)		0.0174532922222*x
#define ROTATE_TO_DEGREE(x)		57.295780490443*x

//////////////////////////////////////////////////////////////////////
//	This class defines all geometry calculation functions needed

class CCW_Geometry : public CObject
{
public:

	//////////////////////////////////////////////////////////////////////
	// To transfer the point from wcl coordinate (p[0], p[1], p[2])
	//		to the given coordinate system with:
	//			X axis vector - (xA[0], xA[1], xA[2])	
	//			Y axis vector - (yA[0], yA[1], yA[2])
	//			Z axis vector - (zA[0], zA[1], zA[2])
	//			( they are unit vector )
	// Return value:
	//		new coordinate (xx, yy, zz)
	void CoordinateTransf(double xA[], double yA[], double zA[], double p[], 
						  double &xx, double &yy, double &zz);

	//////////////////////////////////////////////////////////////////////
	// To transfer the point from coordinate (p[0], p[1], p[2])
	//		in the given coordinate system with:
	//			X axis vector - (xA[0], xA[1], xA[2])	
	//			Y axis vector - (yA[0], yA[1], yA[2])
	//			Z axis vector - (zA[0], zA[1], zA[2])
	//			( they are unit vector )
	//		back to wcl coordinate system
	// Return value:
	//		wcl coordinate (xx, yy, zz)
	void InverseCoordinateTransf(double xA[], double yA[], double zA[], double p[], 
						  double &xx, double &yy, double &zz);

	//////////////////////////////////////////////////////////////////////
	// To normalize the vector (n[0],n[1],n[2])
	// Return value:
	//		TRUE	--	Has been normalized.
	//		FALSE	--	Length of the vector is zero
	BOOL Normalize(double n[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate plane equation parameter by three points
	// Plane equation:  Ax + By + Cz + D = 0, and
	// Vector(A,B,C) is positive unit normal vector of this trangle plane
	// Three points (x[0],y[0],z[0]), (x[1],y[1],z[1]) & (x[2],y[2],z[2])
	//		are in anti-clockwise direction
	// Return value:
	//		TRUE	--	3 points are not on the same line
	//		FALSE	--	3 points are on the same line
	BOOL CalPlaneEquation( double & A, double & B, double & C, double & D, 
		double x[], double y[], double z[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate plane equation parameter by three points
	// Plane equation:  Ax + By + Cz + D = 0, and
	// Vector(A,B,C) is positive unit normal vector of this trangle plane
	// Three points: p0, p1, p2
	//		are in anti-clockwise direction
	// Return value:
	//		TRUE	--	3 points are not on the same line
	//		FALSE	--	3 points are on the same line
	BOOL CalPlaneEquation( double p0[], double p1[], double p2[],
		double & A, double & B, double & C, double & D );

	//////////////////////////////////////////////////////////////////////
	// To approximate plane equation parameter by points ( p[][0], p[][1], p[][2])
	// Plane equation:  Ax + By + Cz + D = 0, and
	// Points number is: n, index from 0 to n-1
	// Return value:
	//		TRUE	--	has solution
	//		FALSE	--	no solution
	BOOL ApproximatePlaneEquation( int n, double** p,
		double & A, double & B, double & C, double & D );

	//////////////////////////////////////////////////////////////////////
	// To calculate line equation parameter by two points
	// Line equation:  Ax + By + C = 0 
	// Two points (x1,y1) & (x2,y2)
	//
	void CalLineEquation( double & A, double & B, double & C, double x1, double y1, double x2, double y2);

	//////////////////////////////////////////////////////////////////////
	// Line intersection test
	//
	// Line equation: a1 X + b1 Y + c1 = 0   &   a2 X + b2 Y + c2 = 0
	// Intersection point: (xx,yy)
	//
	// Return Value:
	//		TRUE	- Have Intersection
	//		FALSE	- No Intersection
	BOOL CalTwoLinesIntersection(double a1, double b1, double c1,
								 double a2, double b2, double c2,
								 double &xx, double &yy);

	//////////////////////////////////////////////////////////////////////
	// Line segment intersection test
	//
	// Line segment: (x1,y1)-(x2,y2)  &  (x3,y3)-(x4,y4)
	// Intersection point: (xx,yy)
	//
	// Return Value:
	//		TRUE	- Have Intersection
	//		FALSE	- No Intersection
	BOOL CalTwoLineSegmentsIntersection(double x1, double y1, double x2, double y2,
			double x3, double y3, double x4, double y4, double &xx, double &yy);

	//////////////////////////////////////////////////////////////////////
	// To calculate plane equation parameter by two points and one vector:
	//		Two points: (p1[0],p1[1],p1[2]) & (p2[0],p2[1],p2[2])
	//		Vector: (l,m,n)
	//		Plane equation:  Ax + By + Cz + D = 0
	BOOL CalPlaneEquation( double & A, double & B, double & C, double & D,
		double p1[], double p2[], double l, double m, double n);

	//////////////////////////////////////////////////////////////////////
	// To calculate the intersection point of a line and a facet
	// Facet:	(x[0],y[0],z[0]), (x[1],y[1],z[1]) & (x[2],y[2],z[2]) 
	//		in anti-clockwise direction with plane equation:  Ax + By + Cz + D = 0
	// Line:	Point=(p[0],p[1],p[2]) & Direction=(n[0],n[1],n[2])
	//
	// Return value:
	//		TRUE	--	Has an intersection point (p[0]+mu*n[0],p[1]+mu*n[1],p[2]+mu*n[2])
	//		FALSE	--	Has no intersection point
	BOOL CalLineFacetIntersection( double p[], double n[], double &mu,
		double x[], double y[], double z[],
		double A, double B, double C, double D);

	BOOL CalLineFacetIntersection( double p[], double n[], 
		double v0[], double v1[], double v2[],
		double& t, double& u, double& v);

	//////////////////////////////////////////////////////////////////////
	// To calculate the intersection point of a line and a plane
	// Plane equation:  Ax + By + Cz + D = 0
	// Line:	Points (p1[0],p1[1],p1[2]) & Direction (n[0],n[1],n[2])
	//
	// Return value:
	//		TRUE	--	Has an intersection point : 
	//									(p[0]+mu*n[0],p[1]+mu*n[1],p[2]+mu*n[2])
	//		FALSE	--	Has no intersection point
	BOOL CalPlaneLineIntersection( double p[], double n[],
		double A, double B, double C, double D, double &mu);

	//////////////////////////////////////////////////////////////////////
	// To calculate the intersection point of a linesegment and a plane
	// Plane equation:  Ax + By + Cz + D = 0
	// Line Segment:	Points (p1[0],p1[1],p1[2]) & (p1[0],p1[1],p1[2])
	//
	// Return value:
	//		TRUE	--	Has an intersection point : (p[0],p[1],p[2])
	//							p[0]=p1[0]+mu*(p2[0]-p1[0]);
	//							p[1]=p1[1]+mu*(p2[1]-p1[1]);
	//							p[2]=p1[2]+mu*(p2[2]-p1[2]);
	//		FALSE	--	Has no intersection point
	BOOL CalPlaneLineSegIntersection( double p1[], double p2[],
		double A, double B, double C, double D, double &mu);

	//////////////////////////////////////////////////////////////////////
	// To calculate the Areal Coordinate of point pp[] in triangle
	//			(p1[],p2[],p3[])
	//
	// Return value:
	//		areal coordinate ( u, v, w )
	void CalArealCoordinate(double p1[], double p2[], double p3[], double pp[], 
		double &u, double &v, double &w);

	//////////////////////////////////////////////////////////////////////
	// To calculate distance between two points:
	//			(p1[0],p1[1],p1[2]) and (p2[0],p2[1],p2[2])
	//
	// Return value:
	//		The distance value
	double Distance_to_Point(double p1[], double p2[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate distance between Point (p[0],p[1],p[2])
	//			and Line (p1[0],p1[1],p1[2])-(p2[0],p2[1],p2[2])
	//
	// Return value:
	//		The distance value
	double Distance_to_LineSegment(double p[], double p1[], double p2[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate area of triangle:
	//			(p0[0],p0[1],p0[2]), (p1[0],p1[1],p1[2]) & (p2[0],p2[1],p2[2])
	//
	// Return value:
	//		The area value,
	double SpatialTriangleArea(double p0[], double p1[], double p2[]);

	//////////////////////////////////////////////////////////////////////
	// To do discretization of polyline:
	//			(x[0],y[0],z[0]), (x[0],y[0],z[0]) ... (x[n-1],y[n-1],z[n-1])
	//	with the Chordal Thickness = Chordal
	//
	// Return value:
	//		Polyline,
	//			(x[0],y[0],z[0]), (x[0],y[0],z[0]) ... (x[m-1],y[m-1],z[m-1])
	void DiscretizationByChordal(double *x, double *y, double *z, UINT n, 
								 double Chordal, UINT &m);

	//////////////////////////////////////////////////////////////////////
	// To do discretization of polyline:
	//			(x[0],y[0],z[0]), (x[0],y[0],z[0]) ... (x[n-1],y[n-1],z[n-1])
	//	with the edge length = Len
	//
	// Return value:
	//		Polyline,
	//			(x[0],y[0],z[0]), (x[0],y[0],z[0]) ... (x[m-1],y[m-1],z[m-1])
	void DiscretizationByLength(double* &x, double* &y, double* &z, UINT n, 
								 double Len, UINT &m);

	//////////////////////////////////////////////////////////////////////
	// To do edge flip by Thales's Theorem:
	//		Input four points: p1[],p2[],p3[],p4[]
	//
	//					P1------------p4
	//					 |				\
	//					 |				  \
	//					 |					\
	//					 |					  \
	//					p2---------------------p3
	//
	//		To decide whether should connect "p1[]-p3[]" or "p2[]-p4[]"
	//
	// Return value:
	//		TRUE	- Should connect "p1[]-p3[]"
	//		FALSE	- Should connect "p2[]-p4[]"
	BOOL EdgeFlipDetection(double p1[], double p2[], double p3[], double p4[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate the 3rd point by points P1, P2 and radius R1, R2 as following:
	//
	//								P2
	//							   /|
	//						  R2 /	|
	//						   /	^
	//						 /  	|
	//					   P3-------P1
	//							R1
	//
	//		P3 should lie on the right side of edge "P1-P2", 
	//								the edge is pointing from P1 to P2 
	//
	void Get3rdPointCoord(double x1,double y1,double r1,double x2,double y2,double r2,double &x3,double &y3);

	//////////////////////////////////////////////////////////////////////
	// Inside/outside polygon test
	//
	// Polygon: xp[], yp[]	(the first point and the last point must be the same point)
	// Polygon point Number: pNum
	// Jug point: (x,y)
	//
	// Return Value:
	//		TRUE	- Inside Polygon
	//		FALSE	- Outside Polygon
	BOOL JugPointInsideOrNot(int pNum, double xp[], double yp[], double x, double y);

	//////////////////////////////////////////////////////////////////////
	// Clockwise/anti-clockwise polygon test
	//
	// Polygon: xp[], yp[]
	// Polygon point Number: pNum
	//
	// Return Value:
	//		TRUE	- Clockwise Polygon
	//		FALSE	- Anti-clockwise Polygon
	BOOL JugClockwiseOrNot(int pNum, double xp[], double yp[]);

	//////////////////////////////////////////////////////////////////////
	// Calculate the value of determinant:
	//			| a0  a1  a2  a3  |
	//			| a4  a5  a6  a7  |
	//			| a8  a9  a10 a11 |
	//			| a12 a13 a14 a15 |
	double Determinant4(double a[]);

	//////////////////////////////////////////////////////////////////////
	// Calculate the value of determinant:
	//			| a0  a1  a2 |
	//			| a3  a4  a5 |
	//			| a6  a7  a8 |
	double Determinant3(double a[]);

	//////////////////////////////////////////////////////////////////////
	// Calculate sphere equation of four points:
	//		(x[0],y[0],z[0]), (x[1],y[1],z[1]), (x[2],y[2],z[2]) & (x[3],y[3],z[3])
	//
	// Return Value:
	//		TRUE:
	//			Center point - (x0,y0,z0)
	//			Radius - R
	//		FLASE: the four points are co-planar
	BOOL CalSphereEquation(double x[], double y[], double z[],
						   double& x0, double& y0, double& z0, double& R );

	//////////////////////////////////////////////////////////////////////
	// To calculate the angle detarmined by points P1, P, and P2
	//
	//								P1
	//							   /
	//						     /	
	//						   /	
	//						 /  	
	//					   P--------P2
	//
	// Return Value:
	//		Angle value
	double CalAngle(double p1[], double p[], double p2[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate the vector product n3 = n1 X n2
	//
	//		where n1, n2 and n3 are three dimensional vectors
	//
	// Return Value:
	//		vector n3
	void VectorProduct(double n1[], double n2[], double n3[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate the vector project n3 = n1 * n2
	//
	//		where n1, n2 are three dimensional vectors
	//
	// Return Value:
	//		project result
	double VectorProject(double n1[], double n2[]);

	//////////////////////////////////////////////////////////////////////
	// To calculate the new postion of point (px, py, pz) after rotating
	//		along X, Y, Z axis or arbitrary vector (x1, y1, z1)->(x2, y2, z2)
	//
	//		where angle is the rotate angle in degree
	//
	// Return Value:
	//		new point position (px1,py1,pz1)
	void RotatePointAlongX(double px, double py, double pz, double angle, 
				double &px1, double &py1, double &pz1);
	void RotatePointAlongY(double px, double py, double pz, double angle, 
				double &px1, double &py1, double &pz1);
	void RotatePointAlongZ(double px, double py, double pz, double angle, 
				double &px1, double &py1, double &pz1);
	void RotatePointAlongVector(double px, double py, double pz, 
				double x1, double y1, double z1, double x2, double y2, double z2,
				double angle, double &px1, double &py1, double &pz1);

	//////////////////////////////////////////////////////////////////////
	// To sort an array by the quick-sort algorithm 
	void QuickSort(int a[], int n) {QuickSort(a,0,n-1,true);}
	void QuickSort(int pArr[], int d, int h, bool bAscending);

private:
	//////////////////////////////////////////////////////////////////////
	// Solve linear equations by Gauss Method
	int agaus(double* a, double* &b, int n);
};
#endif
