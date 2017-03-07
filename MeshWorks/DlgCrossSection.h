#pragma once
#include "afxwin.h"

#include "MeshWorks.h"

class CMeshWorksView;
class SelectBoardTool;

class CDlgCrossSection :
	public CDialog
{
	DECLARE_DYNAMIC(CDlgCrossSection);
public:
	CDlgCrossSection(CMeshWorksView *_pView, SelectBoardTool* _sbt);
	~CDlgCrossSection(void);

	enum { IDD = IDD_CROSSSECTIONVOLUME };

	void UpdateValues(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z);

private:
	CMeshWorksView* pView;
	SelectBoardTool* sbt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual void PostNcDestroy();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CButton m_top;
	CButton m_front;
	CButton m_right;
	CButton m_bottom;
	CButton m_back;
	CButton m_left;
	CEdit e_top;
	CEdit e_front;
	CEdit e_right;
	CEdit e_bottom;
	CEdit e_back;
	CEdit e_left;

private:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedChecktop();
	
	afx_msg void OnBnClickedCheckfront();
	afx_msg void OnBnClickedCheckright();
	afx_msg void OnBnClickedCheckbottom();
	afx_msg void OnBnClickedCheckback();
	afx_msg void OnBnClickedCheckleft();
};


