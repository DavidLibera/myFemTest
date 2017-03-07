#include "StdAfx.h"
#include "DlgCrossSection.h"

#include "MainFrm.h"
#include "MeshWorksDoc.h"
#include "MeshWorksView.h"
#include "..\InteractiveTools\SelectBoardTool.h"
#include "..\InteractiveTools\InterBoard.h"
#include "..\GLKernel\GLKernelView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDlgCrossSection, CDialog)

BEGIN_MESSAGE_MAP(CDlgCrossSection, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCrossSection::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgCrossSection::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECKTOP, &CDlgCrossSection::OnBnClickedChecktop)
	ON_BN_CLICKED(IDC_CHECKFRONT, &CDlgCrossSection::OnBnClickedCheckfront)
	ON_BN_CLICKED(IDC_CHECKright, &CDlgCrossSection::OnBnClickedCheckright)
	ON_BN_CLICKED(IDC_CHECKBOTTOM, &CDlgCrossSection::OnBnClickedCheckbottom)
	ON_BN_CLICKED(IDC_CHECKBACK, &CDlgCrossSection::OnBnClickedCheckback)
	ON_BN_CLICKED(IDC_CHECKLEFT, &CDlgCrossSection::OnBnClickedCheckleft)
END_MESSAGE_MAP()

CDlgCrossSection::CDlgCrossSection(CMeshWorksView *_pView, SelectBoardTool* _sbt)
	:CDialog(CDlgCrossSection::IDD)
{
	pView = _pView;
	sbt = _sbt;
}

CDlgCrossSection::~CDlgCrossSection(void)
{
}

void CDlgCrossSection::PostNcDestroy() 
{
	delete this;
}
void CDlgCrossSection::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString tmp;
	e_top.GetWindowText(tmp);
	double max_y = atof(tmp);
	e_bottom.GetWindowText(tmp);
	double min_y = atof(tmp);
	e_front.GetWindowText(tmp);
	double max_z = atof(tmp);
	e_back.GetWindowText(tmp);
	double min_z = atof(tmp);
	e_right.GetWindowText(tmp);
	double max_x = atof(tmp);
	e_left.GetWindowText(tmp);
	double min_x = atof(tmp);

	if (max_x>sbt->maxX){max_x=sbt->maxX; tmp.Format("%f", max_x); e_right.SetWindowText(tmp);}
	if (min_x<sbt->minX){min_x=sbt->minX; tmp.Format("%f", min_x); e_left.SetWindowText(tmp);}
	if (max_z>sbt->maxZ){max_z=sbt->maxZ; tmp.Format("%f", max_z); e_front.SetWindowText(tmp);}
	if (min_z<sbt->minZ){min_z=sbt->minZ; tmp.Format("%f", min_z); e_back.SetWindowText(tmp);}
	if (max_y>sbt->maxY){max_y=sbt->maxY; tmp.Format("%f", max_y); e_top.SetWindowText(tmp);}
	if (min_y<sbt->minY){min_y=sbt->minY; tmp.Format("%f", min_y); e_bottom.SetWindowText(tmp);}

	if (max_x<min_x){max_x=min_x; tmp.Format("%f", max_x); e_right.SetWindowText(tmp);}	
	if (max_z<min_z){max_z=min_z; tmp.Format("%f", max_z); e_front.SetWindowText(tmp);}
	if (max_y<min_z){max_y=min_y; tmp.Format("%f", max_y); e_top.SetWindowText(tmp);}

	sbt->max_x = max_x;
	sbt->max_y = max_y;
	sbt->max_z = max_z;
	sbt->min_x = min_x;
	sbt->min_y = min_y;
	sbt->min_z = min_z;

	sbt->UpdateBoards();
	sbt->UpdateMesh();

	//OnOK();
}

void CDlgCrossSection::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	pView->OnViewOrbit();
	//pView->PostMessage(WM_COMMAND, ID_VIEW_ORBIT);
	//OnCancel();
}


void CDlgCrossSection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECKTOP, m_top);
	DDX_Control(pDX, IDC_CHECKFRONT, m_front);
	DDX_Control(pDX, IDC_CHECKright, m_right);
	DDX_Control(pDX, IDC_CHECKBOTTOM, m_bottom);
	DDX_Control(pDX, IDC_CHECKBACK, m_back);
	DDX_Control(pDX, IDC_CHECKLEFT, m_left);
	DDX_Control(pDX, IDC_EDIT2, e_top);
	DDX_Control(pDX, IDC_EDIT4, e_front);
	DDX_Control(pDX, IDC_EDIT3, e_right);
	DDX_Control(pDX, IDC_EDIT5, e_bottom);
	DDX_Control(pDX, IDC_EDIT6, e_back);
	DDX_Control(pDX, IDC_EDIT7, e_left);

	m_top.SetCheck(1);
	m_bottom.SetCheck(1);
	m_front.SetCheck(1);
	m_back.SetCheck(1);
	m_left.SetCheck(1);
	m_right.SetCheck(1);

	UpdateValues(sbt->min_x, sbt->max_x, sbt->min_y, sbt->max_y, sbt->min_z, sbt->max_z);
}

void CDlgCrossSection::UpdateValues(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z){
	CString tmp;
	tmp.Format("%f", sbt->max_z); e_front.SetWindowText(tmp);
	tmp.Format("%f", sbt->max_x); e_right.SetWindowText(tmp);
	tmp.Format("%f", sbt->min_z); e_back.SetWindowText(tmp);
	tmp.Format("%f", sbt->min_x); e_left.SetWindowText(tmp);
	tmp.Format("%f", sbt->max_y); e_top.SetWindowText(tmp);
	tmp.Format("%f", sbt->min_y); e_bottom.SetWindowText(tmp);
}

void CDlgCrossSection::OnBnClickedChecktop()
{
	// TODO: Add your control notification handler code here
	int k = 4;
	if (m_top.GetCheck()==BST_CHECKED){
		sbt->board[k]->bShow=true;
	} else {
		sbt->board[k]->bShow=false;
	}
	pView->GetGLKernelView()->refresh();
}
void CDlgCrossSection::OnBnClickedCheckfront()
{
	// TODO: Add your control notification handler code here
	int k=0;
	if (m_front.GetCheck()==BST_CHECKED){
		sbt->board[k]->bShow=true;
	} else {
		sbt->board[k]->bShow=false;
	}
	pView->GetGLKernelView()->refresh();
}

void CDlgCrossSection::OnBnClickedCheckright()
{
	// TODO: Add your control notification handler code here
	int k=1;
	if (m_right.GetCheck()==BST_CHECKED){
		sbt->board[k]->bShow=true;
	} else {
		sbt->board[k]->bShow=false;
	}
	pView->GetGLKernelView()->refresh();
}

void CDlgCrossSection::OnBnClickedCheckbottom()
{
	// TODO: Add your control notification handler code here
	int k=5;
	if (m_bottom.GetCheck()==BST_CHECKED){
		sbt->board[k]->bShow=true;
	} else {
		sbt->board[k]->bShow=false;
	}
	pView->GetGLKernelView()->refresh();
}

void CDlgCrossSection::OnBnClickedCheckback()
{
	// TODO: Add your control notification handler code here
	int k=2;
	if (m_back.GetCheck()==BST_CHECKED){
		sbt->board[k]->bShow=true;
	} else {
		sbt->board[k]->bShow=false;
	}
	pView->GetGLKernelView()->refresh();
}

void CDlgCrossSection::OnBnClickedCheckleft()
{
	// TODO: Add your control notification handler code here
	int k=3;
	if (m_left.GetCheck()==BST_CHECKED){
		sbt->board[k]->bShow=true;
	} else {
		sbt->board[k]->bShow=false;
	}
	pView->GetGLKernelView()->refresh();
}
