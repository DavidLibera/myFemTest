// MeshWorks.h : main header file for the MeshWorks application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMeshWorksApp:
// See MeshWorks.cpp for the implementation of this class
//

class CMeshWorksApp : public CWinApp
{
public:
	CMeshWorksApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnAppFEA();
	DECLARE_MESSAGE_MAP()
};

extern CMeshWorksApp theApp;