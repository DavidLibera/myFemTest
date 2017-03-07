#include "Stroke.h"
// OPENGL support head files
#include <afxwin.h>
#include <gl/gl.h>
//-----------------
#include <math.h>
#define     MIN(x,y) (x < y ? x : y)
#define     MAX(x,y) (x > y ? x : y)
static const double EPSILON_LARGE = 1.0E-5 ;
static const double EPSILON_SMALL = 1.0E-7 ;

CStroke::CStroke(void)
: m_bFinished(false)
{
	m_p=3;
	m_n=0;
	m_TotLen=0;
	m_fWidth=30;
	m_pStrokeVert3DVect= new vector<Point3D*>;
	m_pStrokeDisVertVect= new vector<Point3D*>;
	m_pStrokeVert2DVect= new vector<Point2D*>;
	m_pStrokeNormVect= new vector<Point3D*>;
	m_pStrokeBoxVect= new vector<BOX*>;
	m_fLineColor[0]=1.0;
	m_fLineColor[1]=1.0;
	m_fLineColor[2]=0.0;
	m_fPointColor[0]=0.0;
	m_fPointColor[1]=0.0;
	m_fPointColor[2]=1.0;
}

CStroke::~CStroke(void)
{
	delete m_pStrokeVert3DVect;
}

void CStroke::AddPoint3D(double x, double y, double z,bool dis)
{
	struct Point3D* pVert=new Point3D;
	pVert->x=x;
	pVert->y=y;
	pVert->z=z;
	m_pStrokeVert3DVect->push_back(pVert);
	if(dis)
		m_pStrokeDisVertVect->push_back(pVert);
	struct Point3D* pNorm=new Point3D;
	pNorm->x=1.0;
	pNorm->y=0.0;
	pNorm->z=0.0;
	m_pStrokeNormVect->push_back(pNorm);


}

void CStroke::Display(bool offset)
{
	glDisable(GL_LIGHTING);

	glDepthRange(0.0,0.9999);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(2.0);

    glColor3fv(m_fPointColor);
	//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POINTS);
	for(unsigned int i=0;i<m_pStrokeDisVertVect->size();i++)
	{
		Point3D tmpVert;
		tmpVert.x=m_pStrokeDisVertVect->at(i)->x;
		tmpVert.y=m_pStrokeDisVertVect->at(i)->y;
		tmpVert.z=m_pStrokeDisVertVect->at(i)->z;
		glVertex3f(tmpVert.x,tmpVert.y,tmpVert.z);
	}
	glEnd();

	glLineWidth(2.0);
	glColor3fv(m_fLineColor);
	glBegin(GL_LINE_STRIP);
	for(i=0;i<m_pStrokeDisVertVect->size();i++)
	{
		Point3D tmpVert;
		tmpVert.x=m_pStrokeDisVertVect->at(i)->x;
		tmpVert.y=m_pStrokeDisVertVect->at(i)->y;
		tmpVert.z=m_pStrokeDisVertVect->at(i)->z;
		glVertex3f(tmpVert.x,tmpVert.y,tmpVert.z);
	}
	glEnd();

	if(offset)
	{
		glColor3f(0.0,1.0,0.0);
	//	glBegin(GL_LINES);
		glBegin(GL_POINTS);
		float norm[3], off[3],raw[3];
		Point3D* tmpPt;
		for(i=0;i<m_pStrokeVert3DVect->size();i++)
		{
			tmpPt=m_pStrokeVert3DVect->at(i);		
			raw[0]=tmpPt->x;
			raw[1]=tmpPt->y;
			raw[2]=tmpPt->z;
	//		glVertex3fv(raw);
			tmpPt=m_pStrokeNormVect->at(i);		
			norm[0]=tmpPt->x;
			norm[1]=tmpPt->y;
			norm[2]=tmpPt->z;
			off[0]=raw[0]+norm[0]*0.05;
			off[1]=raw[1]+norm[1]*0.05;
			off[2]=raw[2]+norm[2]*0.05;
			glVertex3fv(off);
		}
		glEnd();
	}
	glLineWidth(1.0);
	glPointSize(1.0);

	glEnable(GL_LIGHTING);
}

int CStroke::GetStrokePointsNum(void)
{
	return m_pStrokeVert3DVect->size();
}

void CStroke::GetStrokePoint(int index, double* x, double* y, double* z)
{
	Point3D* pVert=m_pStrokeVert3DVect->at(index);
	*x=pVert->x;
	*y=pVert->y;
	*z=pVert->z;

}

void CStroke::AddPoint2D(float x, float y)
{
	Point2D* pP2D=new Point2D;
	pP2D->x=x;
	pP2D->y=y;
	m_pStrokeVert2DVect->push_back(pP2D);
}

void CStroke::BuildStrokeBox(void)
{
	int i=0;
	Point2D* pPt2D1,*pPt2D2;
	for(i=0;i<m_pStrokeVert2DVect->size()-1;i++)
	{
		float u[2];
		pPt2D1=m_pStrokeVert2DVect->at(i);
		pPt2D2=m_pStrokeVert2DVect->at(i+1);
		u[0]=pPt2D1->y-pPt2D2->y;
		u[1]=pPt2D2->x-pPt2D1->x;
		float l=sqrt(u[0]*u[0]+u[1]*u[1]);
		u[0]/=l;
		u[1]/=l;
		BOX* pBox=new BOX;

		pBox->pt[0].x=pPt2D1->x+u[0]*m_fWidth;
		pBox->pt[0].y=pPt2D1->y+u[1]*m_fWidth;

		pBox->pt[1].x=pPt2D2->x+u[0]*m_fWidth;
		pBox->pt[1].y=pPt2D2->y+u[1]*m_fWidth;

		pBox->pt[2].x=pPt2D2->x-u[0]*m_fWidth;
		pBox->pt[2].y=pPt2D2->y-u[1]*m_fWidth;

		pBox->pt[3].x=pPt2D1->x-u[0]*m_fWidth;
		pBox->pt[3].y=pPt2D1->y-u[1]*m_fWidth;

		m_pStrokeBoxVect->push_back(pBox);
	}

}
bool CStroke::IsPointInStroke(float x, float y,float* vtos)
{
	int i;
	BOX* pBox;
	float dst;
	for(i=0;i<m_pStrokeBoxVect->size();i++)
	{
		pBox=m_pStrokeBoxVect->at(i);
		if(IsPointInBox(x,y,pBox))
		{
			float a[2],b[2];
			Point2D *pt1,*pt2;
			pt1=m_pStrokeVert2DVect->at(i);
			pt2=m_pStrokeVert2DVect->at(i+1);
			a[0]=x-pt1->x;
			a[1]=y-pt1->y;
			b[0]=pt2->x-pt1->x;
			b[1]=pt2->y-pt1->y;
			float l=sqrt(b[0]*b[0]+b[1]*b[1]);
			b[0]/=l;
			b[1]/=l;
			*vtos=a[0]*b[1]-b[0]*a[1];		
			if(*vtos<0)*vtos*=-1.0;
			return true;
		}
	}
	Point2D* pPt2D;
	for(i=0;i<m_pStrokeVert2DVect->size();i++)
	{
		pPt2D=m_pStrokeVert2DVect->at(i);
		dst=sqrt((pPt2D->x-x)*(pPt2D->x-x)+(pPt2D->y-y)*(pPt2D->y-y));
		if(dst<m_fWidth){*vtos=dst;return true;}
	}
	*vtos=-1;
	return false;
}

bool CStroke::IsPointInBox(float x, float y,BOX* box)
{

	int    counter = 0;
	int    i;
	double xinters;
	Point2D  p1,p2;

	p1 = box->pt[0];
	for (i=1;i<=4;i++) {
		p2 = box->pt[i % 4];
		if (y > MIN(p1.y,p2.y)) {
			if (y <= MAX(p1.y,p2.y)) {
				if (x <= MAX(p1.x,p2.x)) {
					if (p1.y != p2.y) {
						xinters = (y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (counter % 2 == 0)
		return false;
	else
		return true;
}
float CStroke::GetWidth(void)
{
	return m_fWidth;
}

void CStroke::EraseLast(void)
{
	m_pStrokeVert2DVect->pop_back();    
	m_pStrokeVert3DVect->pop_back();    
}

void CStroke::SetFinished(bool flag)
{
	m_bFinished=flag;
}

void CStroke::BSplineFitting(void)
{
	Point3D *pPt1,*pPt2;
	float coord[3],l;
	int size=m_pStrokeVert3DVect->size();
	//	outfile<<"size="<<size<<endl;
	int num=0;
	int i;
	m_Q=new Point3D[size];
	int a=0;
	int b=a+1;
	pPt1=m_pStrokeVert3DVect->at(0);
	m_Q[num].x=pPt1->x;
	m_Q[num].y=pPt1->y;
	m_Q[num++].z=pPt1->z;
	while(b<size)
	{
		pPt1=m_pStrokeVert3DVect->at(a);
		do 
		{
			pPt2=m_pStrokeVert3DVect->at(b);
			coord[0]= pPt2->x-pPt1->x ;
			coord[1]= pPt2->y-pPt1->y ;
			coord[2]= pPt2->z-pPt1->z ;
			l=sqrt(coord[0]*coord[0]+coord[1]*coord[1]+coord[2]*coord[2]);
			b++;
		} while(b<size&&l<EPSILON_SMALL);
		m_Q[num].x=pPt2->x;
		m_Q[num].y=pPt2->y;
		m_Q[num++].z=pPt2->z;
		//		outfile<<"m_Q["<<num-1<<"]="<<m_Q[num-1].x<<","<<m_Q[num-1].y<<","<<m_Q[num-1].z<<endl;
		a = b-1;
	}
	m_n=num-1;
	//	outfile<<"num="<<num<<endl;
	double * uk=new double[m_n+1];
	for(i = 1 ; i<=m_n ; i++ )
	{
		coord[0]= m_Q[i].x-m_Q[i-1].x ;
		coord[1]= m_Q[i].y-m_Q[i-1].y ;
		coord[2]= m_Q[i].z-m_Q[i-1].z ;
		uk[i]=sqrt(coord[0]*coord[0]+coord[1]*coord[1]+coord[2]*coord[2]);
		//		outfile<<"uk["<<i<<"]="<<uk[i]<<endl;
		m_TotLen += uk[i] ;
	}
	uk[0] = 0.0  ;
	for( i = 1 ; i < m_n ; i++ )
	{
		uk[i] = uk[i-1] + uk[i]/m_TotLen ;
		//		outfile<<"uk["<<i<<"]="<<uk[i]<<endl;
	}
	uk[m_n] = 1.0  ;

//	m_Q[m_n].x=pPt2->x;
//	m_Q[m_n].y=pPt2->y;
//	m_Q[m_n].z=pPt2->z;
	m_U=new double[m_n+7];
	for( i = 0 ; i<= 3 ; i++ )
	{
		m_U[i] = 0.0 ;
	}
	for( i=4 ; i<m_n+3; i++ )
	{

		m_U[i] = uk[i-3] ;
		//		outfile<<"m_U["<<i<<"]="<<m_U[i]<<endl;
	}
	for( i=m_n+3 ; i<=m_n+6 ; i++ )
	{
		m_U[i] = 1.0 ;
	}
	m_P=new Point3D[m_n+3];
	SolveTridiagonal(m_n,m_Q,m_U,m_P) ;
	for(i=0;i<m_n+3;i++)
	{
		//	outfile<<"m_P["<<i<<"]="<<m_P[i].x<<","<<m_P[i].y<<","<<m_P[i].z<<endl;
	}

}
void CStroke::SolveTridiagonal(int n, Point3D* Q, double* U, Point3D* P)
{
	Point3D*  R = new Point3D[n+1] ;
	double* dd = new double[n+1]  ;
	P[0].x = Q[0].x ;
	P[0].y = Q[0].y ;
	P[0].z = Q[0].z ;
	P[1].x = Q[1].x ;
	P[1].y = Q[1].y ;
	P[1].z = Q[1].z ;
	P[n+1].x = Q[n-1].x ;
	P[n+1].y = Q[n-1].y ;
	P[n+1].z = Q[n-1].z ;
	P[n+2].x=Q[n].x ;
	P[n+2].y=Q[n].y ;
	P[n+2].z=Q[n].z ;

	double abc[4] ;
	//	abc[0] = U[5] , abc[1] = -(U[4]+U[5]) , abc[2] = U[4] ,abc[3] = 0.0 ;
	for(int i = 3 ; i<n  ; i++ ){
		R[i].x = Q[i-1].x ;
		R[i].y = Q[i-1].y ;
		R[i].z = Q[i-1].z ;
	}
	BasisFuns(4,U[4],3,U,abc) ;
	double den = abc[1] ;
	P[2].x=(Q[1].x-abc[0]*P[1].x)/den;
	P[2].y=(Q[1].y-abc[0]*P[1].y)/den;
	P[2].z=(Q[1].z-abc[0]*P[1].z)/den;
	for( i=3 ; i<n ; i++)
	{
		dd[i] = abc[2]/den ;
		//		outfile<<"i="<<i<<"; U[i+2]="<<U[i+2]<<endl;
		BasisFuns(i+2,U[i+2],3,U,abc) ;
		//	outfile<<"abc[0]="<<abc[0]<<"; abc[1]="<<abc[1]<<"; abc[2]="<<abc[2]<<"; abc[3]="<<abc[3]<<endl;
		den = abc[1] - abc[0]*dd[i] ;
		P[i].x = (R[i].x-abc[0]*P[i-1].x)*(1.0/den) ;
		P[i].y = (R[i].y-abc[0]*P[i-1].y)*(1.0/den) ;
		P[i].z = (R[i].z-abc[0]*P[i-1].z)*(1.0/den) ;
	}
	dd[n] = abc[2]/den ;
	BasisFuns(n+2,U[n+2],3,U,abc);
	den=abc[1]-abc[0]*dd[n];
	P[n].x=(Q[n-1].x-abc[2]*P[n+1].x-abc[0]*P[n-1].x)/den;
	P[n].y=(Q[n-1].y-abc[2]*P[n+1].y-abc[0]*P[n-1].y)/den;
	P[n].z=(Q[n-1].z-abc[2]*P[n+1].z-abc[0]*P[n-1].z)/den;
	for( i = n-1 ; i>=2 ; i-- )
	{
		P[i].x = P[i].x - P[i+1].x * dd[i+1] ;
		P[i].y = P[i].y - P[i+1].y * dd[i+1] ;
		P[i].z = P[i].z - P[i+1].z * dd[i+1] ;
	}
	delete []R ;
	delete [] dd ;


}

void CStroke::BasisFuns(int i, double t, int p, double* U, double* N)
{
	double * left = new double[p+1];
	double * right= new double[p+1];

	N[0] = 1.0 ;
	for( int j=1 ; j <= p ; j++ )
	{
		left[j]  = t - U[i+1-j] ;
		right[j] = U[i+j] - t ;
		double saved = 0.0 ;
		for(int r = 0 ; r < j ; r++ )
		{
			double temp = N[r]/(right[r+1]+left[j-r]) ;
			N[r] = saved + right[r+1]*temp ;
			saved = left[j-r]*temp ;
		}
		N[j] = saved ;
	}
	delete [] left ;
	delete [] right ;

}

void CStroke::Resample(void)
{
	vector <Point3D*> *pTmpVertVect=new vector<Point3D*>;
	Point3D *pt;
	double t=0.0,deltat;
	deltat=1.0/50.0;
	int i;
	pt=m_pStrokeVert3DVect->at(0);
	pTmpVertVect->push_back(pt);
	for(i=1;i<50;i++)
	{
		t+=deltat;
		pt=new Point3D;
		CurvePoint(t,pt);
		pTmpVertVect->push_back(pt);
	}
	pt=m_pStrokeVert3DVect->at(m_n);
	pTmpVertVect->push_back(pt);
	for(i=1;i<m_pStrokeVert3DVect->size()-1;i++)
	{
		pt=m_pStrokeVert3DVect->at(i);
		delete pt;
	}
	m_pStrokeVert3DVect->clear();
	delete m_pStrokeVert3DVect;
	m_pStrokeVert3DVect=pTmpVertVect;
}


void CStroke::CurvePoint(double t , Point3D* pPt)
{
	int span = FindSpan(t,m_n+2,m_p,m_U) ;
	//	outfile<<"span="<<span<<endl;

	double * N = new double[m_p+1];
	BasisFuns(span,t,m_p,m_U,N) ;
	//	outfile<<"t="<<t<<";N="<<N[0]<<","<<N[1]<<","<<N[2]<<","<<N[3]<<endl;

	pPt->x=pPt->y=pPt->z=0.0;
	Point3D temp;
	for(int i = 0 ; i<=m_p; i++ )
	{
		temp.x = m_P[span-m_p+i].x;
		temp.y = m_P[span-m_p+i].y;
		temp.z = m_P[span-m_p+i].z;
		pPt->x += ( temp.x *= N[i] ) ;
		pPt->y += ( temp.y *= N[i] ) ;
		pPt->z += ( temp.z *= N[i] ) ;
	}
	delete [] N ;

}

int CStroke::FindSpan(double t, int n, int p, double* U)
{
	if( fabs(t-U[n+1])<EPSILON_LARGE ) return n ;

	//Do binary search
	int low = p ;
	int high = n+1 ;
	int mid = (low+high)/2 ;
	while( t< U[mid]|| t >= U[mid+1] )
	{
		if(t<U[mid]) high = mid ;
		else low = mid ;
		mid = (low+high)/2 ;
	}
	return mid ;
}




void CStroke::ExportV(FILE* file)
{
	int i,j;
	Point3D *pt1,*pt2;
	float tmp[3],tgt1[3],tgt2[3],vtmp[3];
	float l;
	float cos_table[12],sin_table[12];
	float x,dx;

	dx = (float)(2.*M_PI/12);
	for ( i = 0, x = 0.; i < 12; i++, x += dx) {
		cos_table[i] = cos(x);
		sin_table[i] = sin(x);
	}
    for(i=0;i<m_pStrokeVert3DVect->size()-1;i++)
	{
        pt1=m_pStrokeVert3DVect->at(i);
		pt2=m_pStrokeVert3DVect->at(i+1);
		tmp[0]=pt1->x-pt2->x;
		tmp[1]=pt1->y-pt2->y;
		tmp[2]=pt1->z-pt2->z;
		l=sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
		tmp[0]/=l;
		tmp[1]/=l;
		tmp[2]/=l;
		if (tmp[0]!=0.0) {
			tgt1[0] = -tmp[1];
			tgt1[1] = tmp[0];
			tgt1[2] = 0.0;
		}
		else if (tmp[1]!=0.0) {
			tgt1[0] = 0.0;
			tgt1[1] = -tmp[2];
			tgt1[2] = tmp[1];
		}
		else if (tmp[2]!=0.0) {
			tgt1[0] =  -tmp[2];
			tgt1[1] =0.0;
			tgt1[2] = 0.0;
		}
		l=sqrt(tgt1[0]*tgt1[0]+tgt1[1]*tgt1[1]+tgt1[2]*tgt1[2]);
		tgt1[0]/=l;
		tgt1[1]/=l;
		tgt1[2]/=l;
		tgt2[0]=tmp[1]*tgt1[2]-tmp[2]*tgt1[1];
		tgt2[1]=tmp[2]*tgt1[0]-tmp[0]*tgt1[2];
		tgt2[2]=tmp[0]*tgt1[1]-tmp[1]*tgt1[0];
		l=sqrt(tgt2[0]*tgt2[0]+tgt2[1]*tgt2[1]+tgt2[2]*tgt2[2]);
		tgt2[0]/=l;
		tgt2[1]/=l;
		tgt2[2]/=l;
		for ( j = 0; j < 12; j++) {

			vtmp[0] = pt1->x + (tgt1[0]*cos_table[j] + tgt2[0]*sin_table[j])*0.01; 
			vtmp[1] = pt1->y + (tgt1[1]*cos_table[j] + tgt2[1]*sin_table[j])*0.01;
			vtmp[2] = pt1->z + (tgt1[2]*cos_table[j] + tgt2[2]*sin_table[j])*0.01;
			fprintf(file,"v %f %f %f\n",vtmp[0],vtmp[1],vtmp[2]);
		}
		for ( j = 0; j < 12; j++) {

			vtmp[0] = pt2->x + (tgt1[0]*cos_table[j] + tgt2[0]*sin_table[j])*0.01; 
			vtmp[1] = pt2->y + (tgt1[1]*cos_table[j] + tgt2[1]*sin_table[j])*0.01;
			vtmp[2] = pt2->z + (tgt1[2]*cos_table[j] + tgt2[2]*sin_table[j])*0.01;
			fprintf(file,"v %f %f %f\n",vtmp[0],vtmp[1],vtmp[2]);
		}		
	}
}

void CStroke::ExportF(FILE* file, int *offPos)
{
	int i,pos=*offPos;
	int m,n,j;
	for(i=0;i<m_pStrokeVert3DVect->size()-1;i++)
	{
        for(j=0;j<12;j++)
		{
			m=j;
			n=(j<11)?(j+1):1;
			fprintf(file,"f %d %d %d\n",pos+m,pos+m+12,pos+n);
			fprintf(file,"f %d %d %d\n",pos+n,pos+m+12,pos+n+12);
		}
		pos+=24;
	}
	*offPos=pos;

}

void CStroke::GetStrokePoint2D(int index, float* x, float* y)
{
	Point2D* pPt;
	pPt=m_pStrokeVert2DVect->at(index);
	*x=pPt->x;
	*y=pPt->y;
}

void CStroke::GetStroke2S(float* x, float* y)
{
	Point2D* pPt;
	pPt=m_pStrokeVert2DVect->at(0);
	*x=pPt->x;
	*y=pPt->y;

}

void CStroke::GetStroke2E(float* x, float* y)
{
	Point2D* pPt;
	int num=m_pStrokeVert2DVect->size();
	pPt=m_pStrokeVert2DVect->at(num-1);
	*x=pPt->x;
	*y=pPt->y;
}

void CStroke::SetStrokePoint(int index, double x, double y, double z)
{
	Point3D* pVert=m_pStrokeVert3DVect->at(index);
	pVert->x=x;
	pVert->y=y;
	pVert->z=z;
}

void CStroke::GetStrokeNorm(int index, float* x, float* y, float* z)
{
	Point3D* pNorm=m_pStrokeVert3DVect->at(index);
	*x=pNorm->x;
	*y=pNorm->y;
	*z=pNorm->z;

}

void CStroke::SetStrokeNorm(int index, float x, float y, float z)
{
	Point3D* pNorm=m_pStrokeNormVect->at(index);
	pNorm->x=x;
	pNorm->y=y;
	pNorm->z=z;

}

void CStroke::AddDisPoint(double x, double y, double z)
{
	struct Point3D* pVert=new Point3D;
	pVert->x=x;
	pVert->y=y;
	pVert->z=z;
	m_pStrokeDisVertVect->push_back(pVert);

}

void CStroke::SetLineColor(float r, float g, float b)
{
	m_fLineColor[0]=r;
	m_fLineColor[1]=g;
	m_fLineColor[2]=b;
}

void CStroke::SetPointColor(float r, float g, float b)
{
	m_fPointColor[0]=r;
	m_fPointColor[1]=g;
	m_fPointColor[2]=b;
}
