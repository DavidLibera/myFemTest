
#include "stdafx.h"
#include "MeshWorks.h"
#include "FEAPanel.h"
#include "Mainfrm.h"
#include "FEM.h"
#include <iostream>

//added for ensuring OnAppFEA is not run unless 
#include "MeshWorksDoc.h"
#include "stdafx.h"

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
	ON_BN_CLICKED(IDC_ANALYZE, &CFEADlg::OnBnClickedAnalyze)
	ON_BN_CLICKED(IDC_BUTTON4, &CFEADlg::OnBnClickedButton4)
END_MESSAGE_MAP()

// App command to run the dialog
void CMeshWorksApp::OnAppFEA()
{
	// TO DO:
	// Bug : when User clicks on FEA from Menu and no mesh imported it crashes 
	// Need to make sure file is loaded before DoModal because it crashes
	// Dyn memory is not deallocated !! Bad. 

	CFEADlg FEADlg;
	FEADlg.DoModal();
}


// CMeshWorksApp message handlers
void CFEADlg::OnBnClickedSelectbcs()
{
	std::cout << "Setting boundaries conditions" << std::endl;
	pFEM->setBCs(); //WONT WORK YET, because Create() is called in MainFunction()
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



void CFEADlg::OnBnClickedAnalyze()
{
	// TODO: Add your control notification handler code here
	pFEM->MainFunction();
}


void CFEADlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	pFEM->colorFaces();
}
