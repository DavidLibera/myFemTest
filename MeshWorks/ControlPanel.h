#pragma once
#include "afxwin.h"
#include <stdio.h>

// CControlPanel form view

class CControlPanel : public CFormView
{
	DECLARE_DYNCREATE(CControlPanel)

protected:
	CControlPanel();           // protected constructor used by dynamic creation
	virtual ~CControlPanel();

public:
	enum { IDD = IDD_CONTROLPANEL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//CEdit m_LogWindow;
	void SetLogWindowText(CString str);
	virtual void OnInitialUpdate();

	CEdit m_ptSize;
	afx_msg void OnBnClickedChange();
	CEdit m_LogWindow;
};
