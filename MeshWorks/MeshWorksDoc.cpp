// MeshWorksDoc.cpp : implementation of the CMeshWorksDoc class
//

#include "stdafx.h"
#include "MeshWorks.h"

#include "MeshWorksDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMeshWorksDoc

IMPLEMENT_DYNCREATE(CMeshWorksDoc, CDocument)

BEGIN_MESSAGE_MAP(CMeshWorksDoc, CDocument)
END_MESSAGE_MAP()


// CMeshWorksDoc construction/destruction

CMeshWorksDoc::CMeshWorksDoc()
{
	// TODO: add one-time construction code here
	FILE *fp;
	if (fp = fopen("unit.ini","r"))
	{
		fscanf(fp,"%d",&m_Unit);
		fclose(fp);
	}
	else
		m_Unit=0;

	switch(m_Unit)
	{
		case 0:	m_unitScale = 1.0f;break;
		case 1:	m_unitScale = 0.1f;break;
		case 2:	m_unitScale = 30.479976f;break;
		case 3:	m_unitScale = 2.539998f;break;
	}

	m_meshList.RemoveAll();	
	m_NodeDisplay = false;
	m_SeedDisplay = false;
	m_NodeNormalDisplay = false;
}

CMeshWorksDoc::~CMeshWorksDoc()
{
}

BOOL CMeshWorksDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CMeshWorksDoc serialization
void CMeshWorksDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CMeshWorksDoc diagnostics

#ifdef _DEBUG
void CMeshWorksDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMeshWorksDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMeshWorksDoc commands
