// MainFrm.h : interface of the CMainFrame class
//

#pragma once



class GLKObList;
class QMeshPatch;
class QMeshNode;
class CMeshWorksView;
class CControlPanel;


class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)



// Attributes
public:
	

// Operations
public:
	CMeshWorksView* GetMainView();
	CControlPanel* GetControlPanel();
	void changePtSize(int model, const float& ptSize);

	void ImportObjfiles(CString filename);
	void ImportTetfiles(CString filename);
	void ImportELEfiles(CString filename);
	void ImportMESHfiles(CString filename);

	void ImportHexfiles(CString filename);

	int Hole_Identification(QMeshPatch *patch);


// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CSplitterWnd	m_wndSplitter; 
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	//CSplitterWnd	m_wndSplitter2;

	

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	CImageList    m_imageToolBar;
	CImageList    m_imageToolBarDisable;
	CImageList    m_imageToolBarHot;
	void InitToolBar(void);

	void SetMeshListPos(QMeshPatch *patch, bool recompute=false);

	afx_msg void OnImportOfffiles();
	afx_msg void OnImportObjfiles();
	afx_msg void OnImportAscfiles();
	afx_msg void OnImportBinaryfiles();
	afx_msg void OnExportNodes();
	afx_msg void OnExportPwnfiles();

	afx_msg void OnImportBsffiles();
	afx_msg void OnEditFindNormals();

	afx_msg void OnImportPwnfiles();
	afx_msg void OnExportObjFiles();
	afx_msg void OnEditShifttoorigin();


	afx_msg void OnExportPlyfiles();



	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditAdjustorientation();
	afx_msg void OnEditScaletounitheight();
	afx_msg void OnCheckingGenusNumber();
	afx_msg void OnCheckingHolesNumber();
	afx_msg void OnSelectNode();
	afx_msg void OnSelectFace();
	afx_msg void OnSelectionRegionSelect();
	afx_msg void OnImportTetfiles();
	afx_msg void OnExportTetfiles();
	afx_msg void OnSelectionTetrahedron();
	afx_msg void OnSelectionFacenode();
	afx_msg void OnEditFlipallnormals();
	afx_msg void OnImportElefiles();
	afx_msg void OnImportMeshfiles();
	afx_msg void OnCheckingDegeneratetetra();
	afx_msg void OnExportElefiles();
	afx_msg void OnMeshoperationDeleteselected();
	afx_msg void OnModificationSmoothboundarysurface();
	afx_msg void OnImportHexfiles();
	afx_msg void OnExportHexfiles();
};


