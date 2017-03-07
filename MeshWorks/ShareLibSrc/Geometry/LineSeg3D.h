// LineSeg3D.h: interface for the LineSeg3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINESEG3D_H__136E14D3_CEC5_422E_ACA2_21CD997B1767__INCLUDED_)
#define AFX_LINESEG3D_H__136E14D3_CEC5_422E_ACA2_21CD997B1767__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point3D.h"

class LineSeg3D  
{
public:
	__inline LineSeg3D()
	{
	}
    __inline LineSeg3D(const Point3D &p0, const Point3D &p1){
      assign(p0, p1);
      }
    __inline LineSeg3D(const LineSeg3D &l){
       assign(l);
      }

    //  assignment
    __inline void assign(const Point3D &p0, const Point3D &p1){
      this->p0 = p0;
      this->p1 = p1;
      }

    __inline void assign(const LineSeg3D &seg){
      p0 = seg.p0;
      p1 = seg.p1;
      }
	void operator = (LineSeg3D seg)
	{
		p0 = seg.p0;
		p1 = seg.p1;		
	}

	virtual ~LineSeg3D();
public:
	Point3D p0,p1;
};

#endif // !defined(AFX_LINESEG3D_H__136E14D3_CEC5_422E_ACA2_21CD997B1767__INCLUDED_)
