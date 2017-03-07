// MeshWorksView.h : interface of the CMeshWorksView class
//


#pragma once

class CGLKernelView;
class CDlgCrossSection;


class CMeshWorksView : public CView
{
protected: // create from serialization only
	CMeshWorksView();
	DECLARE_DYNCREATE(CMeshWorksView)

// Attributes
public:
	CMeshWorksDoc* GetDocument() const;
	CGLKernelView* GetGLKernelView() {return m_GLKernelView;};

private:
	CGLKernelView* m_GLKernelView;

	CDlgCrossSection *CrossSectionControl;
public:
	void UpdateCrossSectionDialog(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z);

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMeshWorksView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewIsometric();
	afx_msg void OnViewBackisometric();
	afx_msg void OnViewFront();
	afx_msg void OnViewBack();
	afx_msg void OnViewTop();
	afx_msg void OnViewBottom();
	afx_msg void OnViewLeft();
	afx_msg void OnViewRight();
	afx_msg void OnViewOrbit();
	afx_msg void OnViewZoomwindow();
	afx_msg void OnViewDynamiczoom();
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnViewZoomall();
	afx_msg void OnViewGlobalaxis();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewPolycount();
	afx_msg void OnViewShading();
	afx_msg void OnViewProfile();
	afx_msg void OnViewMesh();
	afx_msg void OnViewNode();
	afx_msg void OnUpdateViewShading(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewProfile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewMesh(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewNode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewGlobalaxis(CCmdUI *pCmdUI);
	afx_msg void OnViewSeed();
	afx_msg void OnViewNormal();
	afx_msg void OnUpdateViewNormal(CCmdUI *pCmdUI);
	afx_msg void OnViewLight();
	afx_msg void OnUpdateViewLight(CCmdUI *pCmdUI);
	afx_msg void OnViewCrossSectionVolume();
	afx_msg void OnUpdateViewCrossSectionVolume(CCmdUI *pCmdUI);
	afx_msg void OnShadingFacenormal();
	afx_msg void OnShadingNodenormal();
	afx_msg void OnUpdateShadingFacenormal(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShadingNodenormal(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in MeshWorksView.cpp
inline CMeshWorksDoc* CMeshWorksView::GetDocument() const
   { return reinterpret_cast<CMeshWorksDoc*>(m_pDocument); }
#endif

