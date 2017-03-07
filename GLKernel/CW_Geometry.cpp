// CW_Geometry.cpp: implementation of the CCW_Geometry class.
//
//	This class defines all geometry calculation functions needed
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <afxtempl.h>

#include "CW_Geometry.h"

#include "position_array.h"

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
BOOL CCW_Geometry::Normalize(double n[])
{
	double tt=sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);

	if (tt<EPS) 
		return FALSE;
	else{
		n[0]=n[0]/tt;	n[1]=n[1]/tt;	n[2]=n[2]/tt;
	}

	return TRUE;
}

BOOL CCW_Geometry::CalPlaneEquation( double & A, double & B, double & C, double & D, 
			double p1[], double p2[], double l, double m, double n)
{
	A = ( p2[1] - p1[1] ) * n - ( p2[2] - p1[2] ) * m;
	B = ( p2[2] - p1[2] ) * l - ( p2[0] - p1[0] ) * n;
	C = ( p2[0] - p1[0] ) * m - ( p2[1] - p1[1] ) * l;
	D = - ( p1[0] * A + p1[1] * B + p1[2] * C );

	double  tt = A*A + B*B + C*C;
	tt = sqrt(tt);
	if(tt < EPS)    return FALSE;
	A = A/tt;   B = B/tt;   C = C/tt;   D = D/tt;

	return TRUE;
}

void CCW_Geometry::CalArealCoordinate(double p1[], double p2[], double p3[], double pp[], 
									  double &u, double &v, double &w)
{
	double area=SpatialTriangleArea(p1,p2,p3);

	u=SpatialTriangleArea(pp,p2,p3)/area;
	v=SpatialTriangleArea(pp,p3,p1)/area;
	w=SpatialTriangleArea(pp,p1,p2)/area;
}

void CCW_Geometry::CalLineEquation( double & A, double & B, double & C, double x1, double y1, double x2, double y2)
{
	A=y2-y1;
	B=x1-x2;
	if (fabs(B)<EPS)
	{
		A=1;
		B=0;
		C=-x1;
		return;
	}
	C=-(B*y1+A*x1);
}

BOOL CCW_Geometry::ApproximatePlaneEquation( int n, double** p,
		double & A, double & B, double & C, double & D )
{
	double T[16], *s, r;
	int i,j,k;

	s=new double[4]; for(i=0;i<4;i++) s[i]=1.0;
	for(i=0;i<3;i++) {
		for(j=0;j<3;j++) {
			T[i*4+j]=0.0;
			for(k=0;k<n;k++) T[i*4+j]=T[i*4+j]+p[k][i]*p[k][j];
		}
	}
	for(i=0;i<3;i++) {T[i*4+3]=0.0; for(k=0;k<n;k++) T[i*4+3]=T[i*4+3]+p[k][i];}
	for(j=0;j<3;j++) {T[12+j]=0.0; for(k=0;k<n;k++) T[12+j]=T[12+j]+p[k][j];}	T[15]=n;

	if (!(agaus(T,s,4))) 
	{
		delete s; 
		if (CalPlaneEquation(p[0],p[1],p[2],A,B,C,D)) return TRUE;
		return FALSE;
	}

	A=s[0];	B=s[1];	C=s[2];	D=s[3]-1;
	delete s;

	r=sqrt(A*A+B*B+C*C);
	A=A/r;	B=B/r;	C=C/r;	D=D/r;

	return TRUE;
}

int CCW_Geometry::agaus(double* a, double* &b, int n)
{
	int *js,l,k,i,j,is,p,q;
    double d,t;
    js=new int[n];
    l=1;
    for (k=0;k<=n-2;k++)
      { d=0.0;
        for (i=k;i<=n-1;i++)
          for (j=k;j<=n-1;j++)
            { t=fabs(a[i*n+j]);
              if (t>d) { d=t; js[k]=j; is=i;}
            }
        if (d+1.0==1.0) l=0;
        else
          { if (js[k]!=k)
              for (i=0;i<=n-1;i++)
                { p=i*n+k; q=i*n+js[k];
                  t=a[p]; a[p]=a[q]; a[q]=t;
                }
            if (is!=k)
              { for (j=k;j<=n-1;j++)
                  { p=k*n+j; q=is*n+j;
                    t=a[p]; a[p]=a[q]; a[q]=t;
                  }
                t=b[k]; b[k]=b[is]; b[is]=t;
              }
          }
        if (l==0)
          { delete js; printf("fail\n");
            return 0;
          }
        d=a[k*n+k];
        for (j=k+1;j<=n-1;j++)
          { p=k*n+j; a[p]=a[p]/d;}
        b[k]=b[k]/d;
        for (i=k+1;i<=n-1;i++)
          { for (j=k+1;j<=n-1;j++)
              { p=i*n+j;
                a[p]=a[p]-a[i*n+k]*a[k*n+j];
              }
            b[i]=b[i]-a[i*n+k]*b[k];
          }
      }
    d=a[(n-1)*n+n-1];
    if (fabs(d)+1.0==1.0)
      { free(js); printf("fail\n");
        return 0;
      }
    b[n-1]=b[n-1]/d;
    for (i=n-2;i>=0;i--)
      { t=0.0;
        for (j=i+1;j<=n-1;j++)
          t=t+a[i*n+j]*b[j];
        b[i]=b[i]-t;
      }
    js[n-1]=n-1;
    for (k=n-1;k>=0;k--)
      if (js[k]!=k)
        { t=b[k]; b[k]=b[js[k]]; b[js[k]]=t;}
    delete js;
    return 1;
}

BOOL CCW_Geometry::CalPlaneEquation( double p0[], double p1[], double p2[], double & A, double & B, double & C, double & D )
{
	double x[3],y[3],z[3];
	x[0]=p0[0];	x[1]=p1[0];	x[2]=p2[0];
	y[0]=p0[1];	y[1]=p1[1];	y[2]=p2[1];
	z[0]=p0[2];	z[1]=p1[2];	z[2]=p2[2];

	return CalPlaneEquation(A,B,C,D,x,y,z);
}

BOOL CCW_Geometry::CalPlaneEquation( double & A, double & B, double & C, double & D, double x[], double y[], double z[])
{
	A =   y[0] * ( z[1] - z[2] )
		+ y[1] * ( z[2] - z[0] )
		+ y[2] * ( z[0] - z[1] );
	
	B =   z[0] * ( x[1] - x[2] )
		+ z[1] * ( x[2] - x[0] ) 
		+ z[2] * ( x[0] - x[1] );

	C =   x[0] * ( y[1] - y[2] )
		+ x[1] * ( y[2] - y[0] )
		+ x[2] * ( y[0] - y[1] );

	D = - x[0] * ( y[1]*z[2] - y[2]*z[1] )
		- x[1] * ( y[2]*z[0] - y[0]*z[2] )
		- x[2] * ( y[0]*z[1] - y[1]*z[0] );

	double  tt = A*A + B*B + C*C;
	tt = sqrt(tt);
	if(tt < EPS)    return FALSE;
	A = A/tt;   B = B/tt;   C = C/tt;   D = D/tt;

	return TRUE;
}

BOOL CCW_Geometry::CalPlaneLineIntersection( double p[], double n[],
		double A, double B, double C, double D, double &mu)
{
	double denom;

	denom=A*n[0]+B*n[1]+C*n[2];
	if (fabs(denom)<EPS)	return FALSE;

	mu=-(D+A*p[0]+B*p[1]+C*p[2])/denom;

	return TRUE;
}

BOOL CCW_Geometry::CalPlaneLineSegIntersection( double p1[], double p2[],
		double A, double B, double C, double D, double &mu)
{
	double denom;

	denom=A*(p2[0]-p1[0])+B*(p2[1]-p1[1])+C*(p2[2]-p1[2]);
	if (fabs(denom)<EPS)	return FALSE;

	mu=-(D+A*p1[0]+B*p1[1]+C*p1[2])/denom;
	if ((mu<0.0) || (mu>1.0)) return FALSE;

	return TRUE;
}

#define EPSILON 0.00000001

BOOL CCW_Geometry::CalLineFacetIntersection( double p[], double n[], 
		double v0[], double v1[], double v2[],
		double& t, double& u, double& v)
{
	double edge1[3],edge2[3],tvec[3],pvec[3],qvec[3];
	double det,inv_det;
    double delta = 0.00000001;
	for(int i =0;i<3;i++)
	{
		edge1[i] = v1[i]-v0[i];
		edge2[i] = v2[i]-v0[i];
	}


    pvec[0] = n[1]*edge2[2]-n[2]*edge2[1];
	pvec[1] = n[2]*edge2[0]-n[0]*edge2[2];
	pvec[2] = n[0]*edge2[1]-n[1]*edge2[0];

	det = edge1[0]*pvec[0]+edge1[1]*pvec[1]+edge1[2]*pvec[2];


	if((det>-EPSILON)&&(det<EPSILON))
		return FALSE;
	inv_det = 1.0/det;

	for(int j = 0;j<3;j++)
	{
       tvec[j] = p[j]-v0[j];
	}

	u = (tvec[0]*pvec[0]+tvec[1]*pvec[1]+tvec[2]*pvec[2])*inv_det;
	//if(u<0.0||u>1.0)
	if(u<((-1.0)*delta)||u>(1.0+delta))
		return FALSE;
	
	qvec[0] = tvec[1]*edge1[2]-tvec[2]*edge1[1];
	qvec[1] = tvec[2]*edge1[0]-tvec[0]*edge1[2];
	qvec[2] = tvec[0]*edge1[1]-tvec[1]*edge1[0];

	v = (n[0]*qvec[0]+n[1]*qvec[1]+n[2]*qvec[2])*inv_det;
	
	//if(v<0.0||u+v>1.0)
      if(v<((-1.0)*delta)||u+v>(1.0+(2*delta)))
		return FALSE;

	t = (edge2[0]*qvec[0]+edge2[1]*qvec[1]+edge2[2]*qvec[2])*inv_det;

	return TRUE;

	/*if(det<EPSILON)
	   return FALSE;


	for(int j = 0;j<3;j++)
	{
       tvec[j] = p[j]-v0[j];
	}

	u = tvec[0]*pvec[0]+tvec[1]*pvec[1]+tvec[2]*pvec[2];
	if(u<0.0||u>det)
	   return FALSE;

	qvec[0] = tvec[1]*edge1[2]-tvec[2]*edge1[1];
	qvec[1] = tvec[2]*edge1[0]-tvec[0]*edge1[2];
	qvec[2] = tvec[0]*edge1[1]-tvec[1]*edge1[0];

	
	v = n[0]*qvec[0]+n[1]*qvec[1]+n[2]*qvec[2];
	if(v<0.0||u+v>det)
	   return FALSE;


	t = edge2[0]*qvec[0]+edge2[1]*qvec[1]+edge2[2]*qvec[2];
	inv_det = 1.0/det;
	t *= inv_det;
	u *= inv_det;
    v *= inv_det;

	return true;*/


	/*denom=A*n[0]+B*n[1]+C*n[2];
	if (fabs(denom)<EPS)	return FALSE;
	mu=-(D+A*p[0]+B*p[1]+C*p[2])/denom;

	// Obtain the intersection point 
	for(UINT i=0;i<3;i++) sp[i]=p[i]+mu*n[i];

	// Determine whether or not the intersection point is bounded by x[],y[],z[]
	pa1[0]=x[0];	pa1[1]=y[0];	pa1[2]=z[0];
	pa2[0]=x[1];	pa2[1]=y[1];	pa2[2]=z[1];
	pa3[0]=x[2];	pa3[1]=y[2];	pa3[2]=z[2];

	a1=SpatialTriangleArea(sp,pa1,pa2);
	a2=SpatialTriangleArea(sp,pa2,pa3);
	a3=SpatialTriangleArea(sp,pa3,pa1);
	a=a1+a2+a3;
	total=SpatialTriangleArea(pa1,pa2,pa3);

	if (fabs(a-total)>1.0e-3) return FALSE;

	return TRUE;*/
}

BOOL CCW_Geometry::CalLineFacetIntersection( double p[], double n[], double &mu,
		double x[], double y[], double z[],
		double A, double B, double C, double D)
{
	double denom,sp[3],pa1[3],pa2[3],pa3[3],total;
	double a1,a2,a3,a;

	denom=A*n[0]+B*n[1]+C*n[2];
	if (fabs(denom)<EPS)	return FALSE;
	mu=-(D+A*p[0]+B*p[1]+C*p[2])/denom;

	// Obtain the intersection point 
	for(UINT i=0;i<3;i++) sp[i]=p[i]+mu*n[i];

	// Determine whether or not the intersection point is bounded by x[],y[],z[]
	pa1[0]=x[0];	pa1[1]=y[0];	pa1[2]=z[0];
	pa2[0]=x[1];	pa2[1]=y[1];	pa2[2]=z[1];
	pa3[0]=x[2];	pa3[1]=y[2];	pa3[2]=z[2];

	a1=SpatialTriangleArea(sp,pa1,pa2);
	a2=SpatialTriangleArea(sp,pa2,pa3);
	a3=SpatialTriangleArea(sp,pa3,pa1);
	a=a1+a2+a3;
	total=SpatialTriangleArea(pa1,pa2,pa3);

	if (fabs(a-total)>1.0e-3) return FALSE;

	return TRUE;
}

double CCW_Geometry::Distance_to_Point(double p1[], double p2[])
{
	double dis=(p1[0]-p2[0])*(p1[0]-p2[0])
		+(p1[1]-p2[1])*(p1[1]-p2[1])
		+(p1[2]-p2[2])*(p1[2]-p2[2]);
	dis=sqrt(dis);

	return dis;
}

double CCW_Geometry::Distance_to_LineSegment(double p[], double p1[], double p2[])
{
	double area=SpatialTriangleArea(p,p1,p2)*2.0;
	double dis=Distance_to_Point(p1,p2);

	if (area<EPS)  return 0.0;
	if (dis<EPS)  return 0.0;
	dis=area/dis;

	return dis;
}

double CCW_Geometry::SpatialTriangleArea(double p0[], double p1[], double p2[])
{
	double x1,y1,z1,x2,y2,z2;
	double ii,jj,kk;
	double area;

	x1=p1[0]-p0[0];	y1=p1[1]-p0[1];	z1=p1[2]-p0[2];
	x2=p2[0]-p0[0];	y2=p2[1]-p0[1];	z2=p2[2]-p0[2];

	ii=y1*z2-z1*y2;
	jj=x2*z1-x1*z2;
	kk=x1*y2-x2*y1;

	area=sqrt(ii*ii+jj*jj+kk*kk)/2.0;

	return area;
}

void CCW_Geometry::DiscretizationByLength(double* &x, double* &y, double* &z, UINT n, 
								 double Len, UINT &m)
{
	UINT *nIndex, i, j;

	nIndex=new UINT[n];
	m=0;

	int startNo,endNo;
	for(j=0;j<n;j++)
	{
		if ((j==0) || (j==(n-1)))
		{
			nIndex[m++]=j;
			startNo=j;
			continue;
		}
		endNo=j;

		double point1[3],point2[3];
		point1[0]=x[startNo];	point1[1]=y[startNo];	point1[2]=z[startNo];
		point2[0]=x[endNo];		point2[1]=y[endNo];		point2[2]=z[endNo];

		double distance=Distance_to_Point(point1,point2);

		if (distance>Len)
		{
			nIndex[m++]=endNo;
			startNo=endNo;
		}
	}

	for(j=0;j<m;j++)
	{
		x[j]=x[nIndex[j]];	y[j]=y[nIndex[j]];	z[j]=z[nIndex[j]];
	}

	delete nIndex;

	position_array  posArray;
	posArray.Add(x[0],y[0],z[0]);
	for(j=1;j<m;j++)
	{
		double p1[3],p2[3];
		p1[0]=x[j-1];	p1[1]=y[j-1];	p1[2]=z[j-1];
		p2[0]=x[j];		p2[1]=y[j];		p2[2]=z[j];
		double l=Distance_to_Point(p1,p2);
		if (l<=Len)
		{
			posArray.Add(x[j],y[j],z[j]);
		}
		else
		{
			double d[3];
			UINT num=(UINT)(l/Len+1);
			for(i=0;i<3;i++) d[i]=(p2[i]-p1[i])/((double)num);
			for(i=1;i<=num;i++)
				posArray.Add(p1[0]+d[0]*((double)i),
							 p1[1]+d[1]*((double)i),
							 p1[2]+d[2]*((double)i));
		}
	}

	delete x;	delete y;	delete z;
	m=posArray.GetSize();
	x=new double[m];	y=new double[m];	z=new double[m];
	for(j=0;j<m;j++) posArray.ElementAt(j,x[j],y[j],z[j]);
}

void CCW_Geometry::DiscretizationByChordal(double *x, double *y, double *z, UINT n, 
								  double Chordal, UINT &m)
{
	UINT *nIndex, j;

	nIndex=new UINT[n];
	m=0;

	int startNo,endNo;
	for(j=0;j<n;j++)
	{
		if ((j==0) || (j==(n-1)))
		{
			nIndex[m++]=j;
			startNo=j;
			continue;
		}
		endNo=j;

		double point1[3],point2[3];
		point1[0]=x[startNo];	point1[1]=y[startNo];	point1[2]=z[startNo];
		point2[0]=x[endNo];		point2[1]=y[endNo];		point2[2]=z[endNo];

		for(int k=startNo+1;k<endNo;k++)
		{
			double point[3];
			point[0]=x[k];	point[1]=y[k];	point[2]=z[k];

			double distance=Distance_to_LineSegment(point,point1,point2);;
			if (distance>Chordal)
			{
				nIndex[m++]=endNo-1;
				startNo=endNo-1;
				break;
			}
		}
	}

	for(j=0;j<m;j++)
	{
		x[j]=x[nIndex[j]];	y[j]=y[nIndex[j]];	z[j]=z[nIndex[j]];
	}

	delete nIndex;
}

BOOL CCW_Geometry::EdgeFlipDetection(double p1[], double p2[], double p3[], double p4[])
{
	double e[6],e2[6],a,minA[2];

	e[0]=Distance_to_Point(p1,p2);
	e[1]=Distance_to_Point(p2,p3);
	e[2]=Distance_to_Point(p3,p4);
	e[3]=Distance_to_Point(p4,p1);
	e[4]=Distance_to_Point(p1,p3);
	e[5]=Distance_to_Point(p2,p4);

	for(UINT i=0;i<6;i++) e2[i]=e[i]*e[i];

	a=acos((e2[0]+e2[1]-e2[4])/(2.0*e[0]*e[1]));
	minA[0]=a;
	a=acos((e2[1]+e2[4]-e2[0])/(2.0*e[1]*e[4]));
	if (a<minA[0]) minA[0]=a;
	a=acos((e2[0]+e2[4]-e2[1])/(2.0*e[0]*e[4]));
	if (a<minA[0]) minA[0]=a;
	a=acos((e2[3]+e2[4]-e2[2])/(2.0*e[3]*e[4]));
	if (a<minA[0]) minA[0]=a;
	a=acos((e2[2]+e2[4]-e2[3])/(2.0*e[2]*e[4]));
	if (a<minA[0]) minA[0]=a;
	a=acos((e2[2]+e2[3]-e2[4])/(2.0*e[2]*e[3]));
	if (a<minA[0]) minA[0]=a;

	a=acos((e2[0]+e2[5]-e2[3])/(2.0*e[0]*e[5]));
	minA[1]=a;
	a=acos((e2[5]+e2[3]-e2[0])/(2.0*e[5]*e[3]));
	if (a<minA[1]) minA[1]=a;
	a=acos((e2[0]+e2[3]-e2[5])/(2.0*e[0]*e[3]));
	if (a<minA[1]) minA[1]=a;
	a=acos((e2[1]+e2[5]-e2[2])/(2.0*e[1]*e[5]));
	if (a<minA[1]) minA[1]=a;
	a=acos((e2[1]+e2[2]-e2[5])/(2.0*e[1]*e[2]));
	if (a<minA[1]) minA[1]=a;
	a=acos((e2[2]+e2[5]-e2[1])/(2.0*e[2]*e[5]));
	if (a<minA[1]) minA[1]=a;
	
	if (minA[0]<minA[1]) return FALSE;

	return TRUE;
}

void CCW_Geometry::Get3rdPointCoord(double x1,double y1,double r1,double x2,double y2,double r2,double &x3,double &y3)
{
	long double r0=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	long double cs,sn,nx,ny,mx,my,temp;

	cs=(r1*r1+r0*r0-r2*r2)/(2.0*r1*r0);
	temp=1.0-cs*cs;

	if ((fabs(cs)>1.0) || (fabs(cs)<0.0))
	{
		cs=1.0;
		temp=0.0;
		r1=r0/2.0;
	}

	sn=sqrt(temp);
	nx=(x2-x1)/r0;	ny=(y2-y1)/r0;
	mx=nx*cs-ny*sn;	my=nx*sn+ny*cs;
	x3=mx*r1+x1;	y3=my*r1+y1;
}

BOOL CCW_Geometry::JugClockwiseOrNot(int pNum, double xp[], double yp[])
{
	double area=0.0;

	for(int i=1;i<pNum;i++)	area+=(xp[i-1]-xp[i])*(yp[i-1]+yp[i]);
	area+=(xp[pNum-1]-xp[0])*(yp[pNum-1]+yp[0]);

	if (area<0.0) return TRUE;

	return FALSE;
}

BOOL CCW_Geometry::JugPointInsideOrNot(int pNum, double xp[], double yp[], double x, double y)
{
	int i, j;
	BOOL c=FALSE;

	j=pNum-1;
	for(i=0;i<pNum;j=i++)
	{
		if ((((yp[i]<=y) && (y<yp[j])) ||
			((yp[j]<=y) && (y<yp[i]))) &&
			(x<(xp[j]-xp[i])*(y-yp[i])/(yp[j]-yp[i])+xp[i]))
			c=!c;
	}

	return c;
}

BOOL CCW_Geometry::CalTwoLinesIntersection(double a1, double b1, double c1,
										   double a2, double b2, double c2,
										   double &xx, double &yy)
{
	double d=a1*b2-a2*b1;

	if (fabs(d)<EPS) return FALSE;

	xx=-(c1*b2-c2*b1)/d;
	yy=(c1*a2-c2*a1)/d;

	return TRUE;
}

BOOL CCW_Geometry::CalTwoLineSegmentsIntersection(double x1, double y1, double x2, double y2,
										   double x3, double y3, double x4, double y4,
										   double &xx, double &yy)
{
	double a1,b1,c1,a2,b2,c2;

	CalLineEquation(a1,b1,c1,x1,y1,x2,y2);
	CalLineEquation(a2,b2,c2,x3,y3,x4,y4);
	
	if (!(CalTwoLinesIntersection(a1,b1,c1,a2,b2,c2,xx,yy))) return FALSE;

	double u1;
	if (x3==x4)
		u1=(yy-y3)/(y4-y3);
	else
		u1=(xx-x3)/(x4-x3);

	double u2;
	if (x1==x2)
		u2=(yy-y1)/(y2-y1);
	else
		u2=(xx-x1)/(x2-x1);

	if ((u1>=0.0) && (u1<=1.0) && (u2>=0.0) && (u2<=1.0)) return TRUE;

	return FALSE;
}

double CCW_Geometry::CalAngle(double p1[], double p[], double p2[])
{
	double angle;
	double a=Distance_to_Point(p,p1);
	double b=Distance_to_Point(p,p2);
	double c=Distance_to_Point(p1,p2);
	angle=acos((a*a+b*b-c*c)/(2.0*a*b));
		
	return ROTATE_TO_DEGREE(angle);
}

BOOL CCW_Geometry::CalSphereEquation(double x[], double y[], double z[],
						   double& x0, double& y0, double& z0, double& R )
{
	double a[16];
	double t1,t2,t3,t4;
	double M11,M12,M13,M14,M15;

	t1=x[0]*x[0]+y[0]*y[0]+z[0]*z[0];	t2=x[1]*x[1]+y[1]*y[1]+z[1]*z[1];
	t3=x[2]*x[2]+y[2]*y[2]+z[2]*z[2];	t4=x[3]*x[3]+y[3]*y[3]+z[0]*z[3];

	a[0]=x[0];	a[1]=y[0];	a[2]=z[0];	a[3]=1.0;
	a[4]=x[1];	a[5]=y[1];	a[6]=z[1];	a[7]=1.0;
	a[8]=x[2];	a[9]=y[2];	a[10]=z[2];	a[11]=1.0;
	a[12]=x[3];	a[13]=y[3];	a[14]=z[3];	a[15]=1.0;
	M11=Determinant4(a);

	a[0]=t1;	a[1]=y[0];	a[2]=z[0];	a[3]=1.0;
	a[4]=t2;	a[5]=y[1];	a[6]=z[1];	a[7]=1.0;
	a[8]=t3;	a[9]=y[2];	a[10]=z[2];	a[11]=1.0;
	a[12]=t4;	a[13]=y[3];	a[14]=z[3];	a[15]=1.0;
	M12=Determinant4(a);

	a[0]=t1;	a[1]=x[0];	a[2]=z[0];	a[3]=1.0;
	a[4]=t2;	a[5]=x[1];	a[6]=z[1];	a[7]=1.0;
	a[8]=t3;	a[9]=x[2];	a[10]=z[2];	a[11]=1.0;
	a[12]=t4;	a[13]=x[3];	a[14]=z[3];	a[15]=1.0;
	M13=Determinant4(a);

	a[0]=t1;	a[1]=x[0];	a[2]=y[0];	a[3]=1.0;
	a[4]=t2;	a[5]=x[1];	a[6]=y[1];	a[7]=1.0;
	a[8]=t3;	a[9]=x[2];	a[10]=y[2];	a[11]=1.0;
	a[12]=t4;	a[13]=x[3];	a[14]=y[3];	a[15]=1.0;
	M14=Determinant4(a);

	a[0]=t1;	a[1]=x[0];	a[2]=y[0];	a[3]=z[0];
	a[4]=t2;	a[5]=x[1];	a[6]=y[1];	a[7]=z[1];
	a[8]=t3;	a[9]=x[2];	a[10]=y[2];	a[11]=z[2];
	a[12]=t4;	a[13]=x[3];	a[14]=y[3];	a[15]=z[3];
	M15=Determinant4(a);

	if (M11<EPS) return FALSE;

	x0=M12/(2.0*M11);
	y0=-M13/(2.0*M11);
	z0=M14/(2.0*M11);
	R=sqrt((M12*M12+M13*M13+M14*M14-4.0*M15*M11)/(4.0*M11*M11));

	return TRUE;
}

double CCW_Geometry::Determinant3(double a[])
{
	double r;
	
	r=a[0]*a[4]*a[8]-a[0]*a[5]*a[7]
		+a[1]*a[5]*a[6]-a[1]*a[3]*a[8]
		+a[2]*a[3]*a[7]-a[2]*a[4]*a[6];

	return r;
}

double CCW_Geometry::Determinant4(double a[])
{
	double r;
	double b[9];

	b[0]=a[5];	b[1]=a[6];	b[2]=a[7];
	b[3]=a[9];	b[4]=a[10];	b[5]=a[11];
	b[6]=a[13];	b[7]=a[14];	b[8]=a[15];
	r=a[0]*Determinant3(b);

	b[0]=a[4];	b[1]=a[6];	b[2]=a[7];
	b[3]=a[8];	b[4]=a[10];	b[5]=a[11];
	b[6]=a[12];	b[7]=a[14];	b[8]=a[15];
	r=r-a[1]*Determinant3(b);

	b[0]=a[4];	b[1]=a[5];	b[2]=a[7];
	b[3]=a[8];	b[4]=a[9];	b[5]=a[11];
	b[6]=a[12];	b[7]=a[13];	b[8]=a[15];
	r=r+a[2]*Determinant3(b);

	b[0]=a[4];	b[1]=a[5];	b[2]=a[6];
	b[3]=a[8];	b[4]=a[9];	b[5]=a[10];
	b[6]=a[12];	b[7]=a[13];	b[8]=a[14];
	r=r-a[3]*Determinant3(b);

	return r;
}

void CCW_Geometry::VectorProduct(double n1[], double n2[], double n3[])
{
	n3[0]=n1[1]*n2[2]-n1[2]*n2[1];
	n3[1]=n1[2]*n2[0]-n1[0]*n2[2];
	n3[2]=n1[0]*n2[1]-n1[1]*n2[0];
}

double CCW_Geometry::VectorProject(double n1[], double n2[])
{
	double r=n1[0]*n2[0]+n1[1]*n2[1]+n1[2]*n2[2];

	return r;
}

void CCW_Geometry::CoordinateTransf(double xA[], double yA[], double zA[], double p[], 
						  double &xx, double &yy, double &zz)
{
	xx=VectorProject(p,xA);
	yy=VectorProject(p,yA);
	zz=VectorProject(p,zA);
}

void CCW_Geometry::InverseCoordinateTransf(double xA[], double yA[], double zA[], 
										   double p[], double &xx, double &yy, double &zz)
{
	xx=p[0]*xA[0]+p[1]*yA[0]+p[2]*zA[0];
	yy=p[0]*xA[1]+p[1]*yA[1]+p[2]*zA[1];
	zz=p[0]*xA[2]+p[1]*yA[2]+p[2]*zA[2];
}

void CCW_Geometry::RotatePointAlongX(double px, double py, double pz, double angle, 
				double &px1, double &py1, double &pz1)
{
	double a=DEGREE_TO_ROTATE(angle);
	double ca=cos(a),sa=sin(a);
	px1=px;
	py1=py*ca-pz*sa;
	pz1=py*sa+pz*ca;
}

void CCW_Geometry::RotatePointAlongY(double px, double py, double pz, double angle, 
				double &px1, double &py1, double &pz1)
{
	double a=DEGREE_TO_ROTATE(angle);
	double ca=cos(a),sa=sin(a);
	px1=pz*sa+px*ca;
	py1=py;
	pz1=pz*ca-px*sa;
}

void CCW_Geometry::RotatePointAlongZ(double px, double py, double pz, double angle, 
				double &px1, double &py1, double &pz1)
{
	double a=DEGREE_TO_ROTATE(angle);
	double ca=cos(a),sa=sin(a);
	px1=px*ca-py*sa;
	py1=px*sa+py*ca;
	pz1=pz;
}

void CCW_Geometry::RotatePointAlongVector(double px, double py, double pz, 
				double x1, double y1, double z1, double x2, double y2, double z2,
				double angle, double &px1, double &py1, double &pz1)
{
	double rx,ry,rz,rrrr;	double costheta,sintheta;

	angle=DEGREE_TO_ROTATE(angle);
	costheta=cos(angle);	sintheta=sin(angle);
	px1=0.0;	py1=0.0;	pz1=0.0;	px=px-x1;	py=py-y1;	pz=pz-z1;
	rx=x2-x1;	ry=y2-y1;	rz=z2-z1;	rrrr=sqrt(rx*rx+ry*ry+rz*rz);
	rx=rx/rrrr;	ry=ry/rrrr;	rz=rz/rrrr;

	px1 += (costheta + (1 - costheta) * rx * rx) * px;
	px1 += ((1 - costheta) * rx * ry - rz * sintheta) * py;
	px1 += ((1 - costheta) * rx * rz + ry * sintheta) * pz;

	py1 += ((1 - costheta) * rx * ry + rz * sintheta) * px;
	py1 += (costheta + (1 - costheta) * ry * ry) * py;
	py1 += ((1 - costheta) * ry * rz - rx * sintheta) * pz;

	pz1 += ((1 - costheta) * rx * rz - ry * sintheta) * px;
	pz1 += ((1 - costheta) * ry * rz + rx * sintheta) * py;
	pz1 += (costheta + (1 - costheta) * rz * rz) * pz;

	px1 += x1;	py1 += y1;	pz1 += z1;
}

void CCW_Geometry::QuickSort(int pArr[], int d, int h, bool bAscending)
{
	int i,j;
	int str;

	i = h;
	j = d;

	str = pArr[((int) ((d+h) / 2))];

	do {
		if (bAscending) {
			while (pArr[j] < str) j++;
			while (pArr[i] > str) i--;
		} else {
			while (pArr[j] > str) j++;
			while (pArr[i] < str) i--;
		}
		if ( i >= j ) {
			if ( i != j ) {
				int zal;

				zal = pArr[i];
				pArr[i] = pArr[j];
				pArr[j] = zal;
			}
			i--;
			j++;
		}
	} while (j <= i);

	if (d < i) QuickSort(pArr,d,i,bAscending);
	if (j < h) QuickSort(pArr,j,h,bAscending);
}
