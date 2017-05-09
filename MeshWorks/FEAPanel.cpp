
#include "stdafx.h"
#include "MeshWorks.h"
#include "FEAPanel.h"
#include "Mainfrm.h"
#include "FEM.h"
#include <iostream>

//added for ensuring OnAppFEA is not run unless 
#include "MeshWorksDoc.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFEADlg::CFEADlg() : CDialog(CFEADlg::IDD)
//, m_sMeshType(_T(""))
//, m_buttonT3Val(FALSE)
//, m_buttonQ4Val(FALSE)
//, m_T3(false)
//, m_Q4(false)
{
	std::cout << "CONSTRUCTOR: Dyn Alloc Space for class FEM" << std::endl;
	pFEM = new FEM();
}

CFEADlg::~CFEADlg()
{
	std::cout << "DESTRUCTOR: deleting Dyn Alloc Space for class FEM" << std::endl;
	//pFEM->Destroy(); Maybe unnecessary?
	delete pFEM;
}

void CFEADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control(pDX, IDCANCEL, m_buttonMeshType);
	DDX_Control(pDX, IDC_ANALYZE, m_buttonAnalyze);
}

BOOL CFEADlg::OnInitDialog() {
	CDialog::OnInitDialog();
	CWnd *butAnalyze = GetDlgItem(IDC_ANALYZE);
	if (butAnalyze) {
		butAnalyze->EnableWindow(FALSE);
	}
	return TRUE;
}


BEGIN_MESSAGE_MAP(CFEADlg, CDialog)
	ON_BN_CLICKED(IDC_SELECTBCS, &CFEADlg::OnBnClickedSelectbcs)
	ON_BN_CLICKED(IDC_SETPROPS, &CFEADlg::OnBnClickedSetprops)
	ON_BN_CLICKED(IDC_ANALYZE, &CFEADlg::OnBnClickedAnalyze)
	ON_BN_CLICKED(IDC_BUTTON4, &CFEADlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_RADIO1, &CFEADlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CFEADlg::OnBnClickedRadio2)
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
	//pFEM->setBCs(); //WONT WORK YET, because Create() is called in MainFunction()
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
	pFEM->MainFunction();

	//CString str;
	//str.Format("As string: %g", pFEM->E);
	//MessageBox(str);
}

void CFEADlg::OnBnClickedButton4()
{
	pFEM->colorFaces();
}





void CFEADlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	pFEM->meshType = 0; 

	CWnd *butAnalyze = GetDlgItem(IDC_ANALYZE);
	if (butAnalyze) {
		butAnalyze->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}


void CFEADlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	pFEM->meshType = 1;

	CWnd *butAnalyze = GetDlgItem(IDC_ANALYZE);
	if (butAnalyze) {
		butAnalyze->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}
