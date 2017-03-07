// ControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "MeshWorks.h"
#include "ControlPanel.h"
#include "Mainfrm.h"

// CControlPanel

IMPLEMENT_DYNCREATE(CControlPanel, CFormView)

CControlPanel::CControlPanel()
	: CFormView(CControlPanel::IDD)
{

}

CControlPanel::~CControlPanel()
{
}

void CControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT1, m_LogWindow);

	DDX_Control(pDX, IDC_PT_SIZE, m_ptSize);
	DDX_Control(pDX, IDC_LOGWINDOW, m_LogWindow);
}

BEGIN_MESSAGE_MAP(CControlPanel, CFormView)

	ON_BN_CLICKED(IDC_CHANGE, &CControlPanel::OnBnClickedChange)
END_MESSAGE_MAP()


// CControlPanel diagnostics

#ifdef _DEBUG
void CControlPanel::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlPanel::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlPanel message handlers

void CControlPanel::SetLogWindowText(CString str)
{
	CString   strEdit;  
    m_LogWindow.GetWindowText(strEdit);  
    m_LogWindow.SetFocus();  
    m_LogWindow.SetSel(strEdit.GetLength(), strEdit.GetLength());//move pointer to the end
	m_LogWindow.ReplaceSel(str);
}

//remove the scroll bar
void CControlPanel::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	GetParentFrame()->RecalcLayout();  
    ResizeParentToFit();   
	SetScrollSizes(MM_TEXT, CSize(20,750));  

	//set the initial values to be displayed
	m_ptSize.SetWindowText("3");
}

void CControlPanel::OnBnClickedChange()
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();
	CMainFrame *pWnd=(CMainFrame *)(AfxGetMainWnd());
	CString str;
	m_ptSize.GetWindowText(str);
	if (str != "") 
	{
		SetLogWindowText("Point size is set to " + str + ".\n");
		pWnd->changePtSize(0, atof(str));
	}
	AfxGetApp()->EndWaitCursor();
}
