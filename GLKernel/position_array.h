// position_array.h: interface for the position class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CW_POSITIONARRAY
#define CW_POSITIONARRAY

#include <afx.h>
#include <afxtempl.h>

class position : public CObject
{
public:
	position(double xi, double yi, double zi) {xx=xi;yy=yi;zz=zi;};
	~position() {};

	double x() {return xx;}
	double y() {return yy;}
	double z() {return zz;}
	void set_x(double xi) {xx=xi;};
	void set_y(double yi) {yy=yi;};
	void set_z(double zi) {zz=zi;};

private:
	double xx,yy,zz;
};

class position_array : public CObject  
{
public:
	position_array() {Empty();};
	virtual ~position_array() {Empty();};

	////////////////////////////////////////////////////////////
	//	Add position into the array
	void Add(double xi, double yi, double zi)
	{
		x.Add(xi);	y.Add(yi);	z.Add(zi);
	};

	////////////////////////////////////////////////////////////
	//	Clear all positions in the array
	void Empty()
	{
		x.RemoveAll();	y.RemoveAll();	z.RemoveAll();
	}; 

	////////////////////////////////////////////////////////////
	//	Get the size of the array
	UINT GetSize() {return (UINT)x.GetSize();};

	////////////////////////////////////////////////////////////
	//	Get the element (xi,yi,zi) at index - nIndex (begin from 0)
	void ElementAt(UINT nIndex, double &xi, double &yi, double &zi)
	{
		xi=x[nIndex];	yi=y[nIndex];	zi=z[nIndex];
	};

	////////////////////////////////////////////////////////////
	//	Remove the element (xi,yi,zi) at index - nIndex (begin from 0)
	void RemoveAt(UINT nIndex)	//	Begin from 0
	{
		x.RemoveAt(nIndex);		y.RemoveAt(nIndex);		z.RemoveAt(nIndex);
	}

	////////////////////////////////////////////////////////////
	//	Insert the element (xi,yi,zi) at index - nIndex (begin from 0)
	void InsertAt(UINT nIndex, double xi, double yi, double zi)	//	Begin from 0
	{
		x.InsertAt(nIndex, xi);	y.InsertAt(nIndex, yi);	z.InsertAt(nIndex, zi);
	}

private:
	CArray<double,double> x;
	CArray<double,double> y;
	CArray<double,double> z;
};

#endif
