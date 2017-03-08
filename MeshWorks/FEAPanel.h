#pragma once

#include "afxwin.h"
#include "FEM.h"
// CFEADlg dialog

class CFEADlg : public CDialog
{
public:
	CFEADlg();
	~CFEADlg();

	FEM * pFEM; 

	// Dialog Data
	enum { IDD = IDD_FEA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

														// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	// Memory allocations and management functions
	//void createFEM();
	//void deleteFEM();

	// Message handlers
	afx_msg void OnBnClickedSelectbcs();
	afx_msg void OnBnClickedSetprops();

	afx_msg void OnBnClickedAnalyze();
};

