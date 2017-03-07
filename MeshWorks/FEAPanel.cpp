
#include "stdafx.h"
#include "MeshWorks.h"
#include "FEAPanel.h"
#include "Mainfrm.h"
#include "FEM.h"
#include <iostream>

CFEADlg::CFEADlg() : CDialog(CFEADlg::IDD)
{
	std::cout << "CONSTRUCTOR: Dyn Alloc Space for class FEM" << std::endl;
	pFEM = new FEM();
}

CFEADlg::~CFEADlg()
{
	std::cout << "DESTRUCTOR: deleting Dyn Alloc Space for class FEM" << std::endl;
	delete pFEM;
}

void CFEADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFEADlg, CDialog)
	ON_BN_CLICKED(IDC_SELECTBCS, &CFEADlg::OnBnClickedSelectbcs)
	ON_BN_CLICKED(IDC_SETPROPS, &CFEADlg::OnBnClickedSetprops)
END_MESSAGE_MAP()

// App command to run the dialog
void CMeshWorksApp::OnAppFEA()
{
	CFEADlg FEADlg;
	FEADlg.DoModal();
}


// CMeshWorksApp message handlers
void CFEADlg::OnBnClickedSelectbcs()
{
	/*
	//AfxGetApp()->BeginWaitCursor();
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	//CMainFrame's function to print information
	pWnd->OnClickSetBCs();

	//AfxGetApp()->EndWaitCursor();
	*/
	std::cout << "Printing Coordinates with class FEM function" << std::endl;
	pFEM->printNodeCoord();
}

void CFEADlg::OnBnClickedSetprops()
{
	/*
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();
	CMainFrame *pWnd = (CMainFrame *)(AfxGetMainWnd());

	// Calling Function from CMainFrame because it has access to everything? 
	pWnd->OnClickSetProps();

	AfxGetApp()->EndWaitCursor();
	*/
	std::cout << "CFEADlg::OnBnClickedSetprops" << std::endl;
}

