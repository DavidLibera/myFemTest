// MeshWorksDoc.h : interface of the CMeshWorksDoc class
//


#pragma once

class CMeshWorksDoc : public CDocument
{
protected: // create from serialization only
	CMeshWorksDoc();
	DECLARE_DYNCREATE(CMeshWorksDoc)

// Attributes
public:
	int m_Unit;
	float m_unitScale;
	bool m_NodeDisplay;
	bool m_SeedDisplay;
	bool m_NodeNormalDisplay;

	CObList m_meshList;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CMeshWorksDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


