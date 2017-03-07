// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MeshWorks.h"

#include "MainFrm.h"
#include "MeshWorksDoc.h"
#include "MeshWorksView.h"
#include "ControlPanel.h"



#include "..\GLKernel\GLKernelView.h"

#include "..\MeshEntities\QBody.h"


#include "..\MeshEntities\CH_ATTRIB_EDGENODE.h"
#include "..\MeshEntities\CH_ATTRIB_FACENODE.h"


#include "..\GLKLib\GLKHeap.h"
#include "..\GLKLib\GLKMatrixLib.h"



#include "..\InteractiveTools\SelectNodeTool.h"
#include "..\InteractiveTools\SelectFaceTool.h"
#include "..\InteractiveTools\RegionSelectionTool.h"


#include "..\ANN\ANN.h"

#define CROSS(dest,v1,v2) \
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




#pragma warning (disable:4996)


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()

	ON_COMMAND(ID_IMPORT_OFFFILES, &CMainFrame::OnImportOfffiles)
	ON_COMMAND(ID_IMPORT_OBJFILES, &CMainFrame::OnImportObjfiles)
	ON_COMMAND(ID_IMPORT_ASCFILES, &CMainFrame::OnImportAscfiles)
	ON_COMMAND(ID_IMPORT_BINARYFILES, &CMainFrame::OnImportBinaryfiles)
	ON_COMMAND(ID_EXPORT_NODES, &CMainFrame::OnExportNodes)
	ON_COMMAND(ID_EXPORT_PWNFILES, &CMainFrame::OnExportPwnfiles)

	ON_COMMAND(ID_IMPORT_BSFFILES, &CMainFrame::OnImportBsffiles)
	ON_COMMAND(ID_EDIT_FIND_NORMALS, &CMainFrame::OnEditFindNormals)

	ON_COMMAND(ID_IMPORT_PWNFILES, &CMainFrame::OnImportPwnfiles)

	ON_COMMAND(ID_EXPORT_OBJ_FILES, &CMainFrame::OnExportObjFiles)

	ON_COMMAND(ID_EDIT_SHIFTTOORIGIN, &CMainFrame::OnEditShifttoorigin)



	ON_COMMAND(ID_EXPORT_PLYFILES, &CMainFrame::OnExportPlyfiles)


	ON_WM_DROPFILES()
	ON_COMMAND(ID_EDIT_ADJUSTORIENTATION, &CMainFrame::OnEditAdjustorientation)
	ON_COMMAND(ID_EDIT_SCALETOUNITHEIGHT, &CMainFrame::OnEditScaletounitheight)
	ON_COMMAND(ID_CHECKING_GENUSNUMBER, &CMainFrame::OnCheckingGenusNumber)
	ON_COMMAND(ID_CHECKING_HOLESNUMBER, &CMainFrame::OnCheckingHolesNumber)
	ON_COMMAND(ID_SELECT_NODE, &CMainFrame::OnSelectNode)
	ON_COMMAND(ID_SELECT_FACENODE, &CMainFrame::OnSelectFace)
	ON_COMMAND(ID_SELECTION_REGIONSELECT, &CMainFrame::OnSelectionRegionSelect)
	ON_COMMAND(ID_IMPORT_TETFILES, &CMainFrame::OnImportTetfiles)
	ON_COMMAND(ID_EXPORT_TETFILES, &CMainFrame::OnExportTetfiles)
	ON_COMMAND(ID_SELECTION_TETRAHEDRON, &CMainFrame::OnSelectionTetrahedron)
	ON_COMMAND(ID_SELECTION_FACENODE, &CMainFrame::OnSelectionFacenode)
	ON_COMMAND(ID_EDIT_FLIPALLNORMALS, &CMainFrame::OnEditFlipallnormals)
	ON_COMMAND(ID_IMPORT_ELEFILES, &CMainFrame::OnImportElefiles)
	ON_COMMAND(ID_IMPORT_MESHFILES, &CMainFrame::OnImportMeshfiles)
	ON_COMMAND(ID_CHECKING_DEGENERATETETRA, &CMainFrame::OnCheckingDegeneratetetra)
	ON_COMMAND(ID_EXPORT_ELEFILES, &CMainFrame::OnExportElefiles)
	ON_COMMAND(ID_MESHOPERATION_DELETESELECTED, &CMainFrame::OnMeshoperationDeleteselected)
	ON_COMMAND(ID_MODIFICATION_SMOOTHBOUNDARYSURFACE, &CMainFrame::OnModificationSmoothboundarysurface)
	ON_COMMAND(ID_IMPORT_HEXFILES, &CMainFrame::OnImportHexfiles)
	ON_COMMAND(ID_EXPORT_HEXFILES, &CMainFrame::OnExportHexfiles)
	ON_COMMAND(ID_MESHOPERATION_FEA, &CMeshWorksApp::OnAppFEA)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_NUM,
};


// CMainFrame construction/destruction

#include <time.h>
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here

	AllocConsole();
	freopen ("CONOUT$", "w", stdout );

	printf("sizeof: ");
	printf("QMeshNode %d | ", sizeof(QMeshNode));
	printf("QMeshEdge %d | ", sizeof(QMeshEdge));
	printf("QMeshFace %d | ", sizeof(QMeshFace));
	printf("QMeshTetra %d | ", sizeof(QMeshTetra));
	printf("\n");
}

CMainFrame::~CMainFrame()
{

}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	InitToolBar();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	////////////////////////////////////////////////////////////////////////////
	//
	//	The following lines given the X, Y, Z coordinate on the status bar
	//
	m_wndStatusBar.SetPaneInfo(1,m_wndStatusBar.GetItemID(0),SBPS_NORMAL,70);
  	m_wndStatusBar.SetPaneInfo(2,m_wndStatusBar.GetItemID(1),SBPS_NORMAL,70);
	m_wndStatusBar.SetPaneInfo(3,m_wndStatusBar.GetItemID(2),SBPS_NORMAL,70);
	m_wndStatusBar.SetPaneInfo(4,m_wndStatusBar.GetItemID(3),SBPS_NORMAL,80);
    RECT MyRect;
    m_wndStatusBar.GetItemRect(3, &MyRect);  
	m_wndStatusBar.SetPaneText(1, "0.00");
	m_wndStatusBar.SetPaneText(2, "0.00");
	m_wndStatusBar.SetPaneText(3, "0.00");
	m_wndStatusBar.SetPaneText(4, "Verts: 0");
	///////////////////////////////////////////////////////////////////////////////

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	this->DragAcceptFiles();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | /*WS_MAXIMIZE | */WS_SYSMENU;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers
CMeshWorksView* CMainFrame::GetMainView()
{
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)GetActiveDocument();
	CMeshWorksView *pView;
	POSITION Pos = pDoc->GetFirstViewPosition();	
	pDoc->GetNextView(Pos);
	pView = (CMeshWorksView *)(pDoc->GetNextView(Pos));

	return pView;
}

CControlPanel* CMainFrame::GetControlPanel()
{
	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)GetActiveDocument();
	CControlPanel *pView;
	POSITION Pos = pDoc->GetFirstViewPosition();	
	//pDoc->GetNextView(Pos);
	pView = (CControlPanel *)(pDoc->GetNextView(Pos));

	return pView;
	return NULL;
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class


	VERIFY(m_wndSplitter.CreateStatic(this, 1, 2));

	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CControlPanel), CSize(0,0), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMeshWorksView), CSize(0,0), pContext);
	m_wndSplitter.SetColumnInfo(0, 250, 0);

	return TRUE;
}



//-----------------------------------------------------------------------------------------------------------------
//FILE IO
void CMainFrame::OnImportOfffiles()
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".OFF"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("OFF files(*.off)|*.off||"), NULL );

	cFile.m_ofn.lpstrTitle="OFF Files Import";
	if (cFile.DoModal()==IDOK) {
		//CMainFrame *pWnd=(CMainFrame *)(AfxGetMainWnd());
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
		CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
		pview3d->clear_tools();

		AfxGetApp()->BeginWaitCursor();

		QBody *mesh = new QBody;
		mesh->SetIndexNo(1);
		bool flag = pDoc->m_meshList.GetCount() >= 1;
		if (flag) {
			pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
			pDoc->m_meshList.RemoveAll();
		}

		pDoc->m_meshList.AddTail(mesh);

		bool rc = mesh->GetMeshPatch()->inputOFFFile(LPCTSTR(cFile.GetPathName()));
		if (rc) 
		{
			int n = 1;
			for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;n++) 
			{
				QBody *patch=(QBody *)((pDoc->m_meshList).GetNext(Pos));
				pview3d->AddDisplayObj(patch, false);
				patch->SetIndexNo(n);

				if (n==2) patch->SetAttribFlag(6);

				CString str;
				str.Format("Verts: %d", patch->GetTrglNodeNumber());
				m_wndStatusBar.SetPaneText(4, str); 
			}
			pDoc->UpdateAllViews(NULL);
			GetMainView()->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		}

		AfxGetApp()->EndWaitCursor();

		CControlPanel* panel = GetControlPanel();
		panel->SetLogWindowText("OFF File imported.\n");
	}	
}



void CMainFrame::OnImportObjfiles()
{
	

	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".OBJ"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("OBJ files(*.obj)|*.obj||"), NULL );

	cFile.m_ofn.lpstrTitle="OBJ Files Import";
	if (cFile.DoModal()==IDOK) {


		CString filename = cFile.GetPathName();
		ImportObjfiles(filename);
		
	}	
}


void CMainFrame::OnImportPwnfiles()
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".PWN"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("PWN files(*.pwn)|*.pwn||"), NULL );

	cFile.m_ofn.lpstrTitle="PWN Files Import";
	if (cFile.DoModal()==IDOK) {
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
		CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
		pview3d->clear_tools();

		AfxGetApp()->BeginWaitCursor();

		QBody *mesh = new QBody;
		mesh->SetIndexNo(1);
		//remove existing
		bool flag = pDoc->m_meshList.GetCount() >= 1;
		if (flag) {
			pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
			pDoc->m_meshList.RemoveAll();
		}

		pDoc->m_meshList.AddTail(mesh);

		bool rc = mesh->GetMeshPatch()->inputPWNFile(LPCTSTR(cFile.GetPathName()));
		if (rc) 
		{
			int n = 1;
			for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;n++) 
			{
				QBody *patch=(QBody *)((pDoc->m_meshList).GetNext(Pos));
				pview3d->AddDisplayObj(patch, false);
				patch->SetIndexNo(n);

				if (n==2) patch->SetAttribFlag(6);

				CString str;
				str.Format("Verts: %d", patch->GetTrglNodeNumber());
				m_wndStatusBar.SetPaneText(4, str); 
			}
			pDoc->UpdateAllViews(NULL);
			GetMainView()->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		}

		AfxGetApp()->EndWaitCursor();

		CControlPanel* panel = GetControlPanel();
		panel->SetLogWindowText("PWN File imported.\n");
	}
}

void CMainFrame::OnImportAscfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".ASC"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("ASC files(*.asc)|*.asc||"), NULL );

	cFile.m_ofn.lpstrTitle="ASC Files Import";
	if (cFile.DoModal()==IDOK) {
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
		CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
		pview3d->clear_tools();

		AfxGetApp()->BeginWaitCursor();

		QBody *mesh = new QBody;
		mesh->SetIndexNo(1);
		//remove existing
		bool flag = pDoc->m_meshList.GetCount() >= 1;
		if (flag) {
			pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
			pDoc->m_meshList.RemoveAll();
		}

		pDoc->m_meshList.AddTail(mesh);

		bool rc = mesh->GetMeshPatch()->inputASCFile(LPCTSTR(cFile.GetPathName()));
		if (rc) 
		{
			int n = 1;
			for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;n++) 
			{
				QBody *patch=(QBody *)((pDoc->m_meshList).GetNext(Pos));
				pview3d->AddDisplayObj(patch, false);
				patch->SetIndexNo(n);

				if (n==2) patch->SetAttribFlag(6);

				CString str;
				str.Format("Verts: %d", patch->GetTrglNodeNumber());
				m_wndStatusBar.SetPaneText(4, str); 
			}
			pDoc->UpdateAllViews(NULL);
			pview3d->refresh();
			GetMainView()->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
			OnEditShifttoorigin();
		}

		AfxGetApp()->EndWaitCursor();

		CControlPanel* panel = GetControlPanel();
		panel->SetLogWindowText("ASC File imported.\n");
	}

	
}

void CMainFrame::OnImportBsffiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".BSF"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("BSF files(*.bsf)|*.bsf||"), NULL );

	cFile.m_ofn.lpstrTitle="BSF Files Import";
	if (cFile.DoModal()==IDOK) {
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
		CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
		pview3d->clear_tools();

		AfxGetApp()->BeginWaitCursor();

		QBody *mesh = new QBody;
		mesh->SetIndexNo(1);
		//remove existing
		bool flag = pDoc->m_meshList.GetCount() >= 1;
		if (flag) {
			pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
			pDoc->m_meshList.RemoveAll();
		}

		pDoc->m_meshList.AddTail(mesh);

		bool rc = mesh->GetMeshPatch()->inputBSFFile(LPCTSTR(cFile.GetPathName()));
		if (rc) 
		{
			int n = 1;
			for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;n++) 
			{
				QBody *patch=(QBody *)((pDoc->m_meshList).GetNext(Pos));
				pview3d->AddDisplayObj(patch, false);
				patch->SetIndexNo(n);

				if (n==2) patch->SetAttribFlag(6);

				CString str;
				str.Format("Verts: %d", patch->GetTrglNodeNumber());
				m_wndStatusBar.SetPaneText(4, str); 
			}
			pDoc->UpdateAllViews(NULL);
			GetMainView()->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		}

		AfxGetApp()->EndWaitCursor();

		CControlPanel* panel = GetControlPanel();
		panel->SetLogWindowText("BSF File imported.\n");
	}
}

void CMainFrame::OnImportBinaryfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".TMP"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Binary files(*.tmp)|*.tmp||"), NULL );

	cFile.m_ofn.lpstrTitle="Binary Files Import";
	if (cFile.DoModal()==IDOK) {
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
		CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
		pview3d->clear_tools();

		AfxGetApp()->BeginWaitCursor();

		QBody *mesh = new QBody;
		mesh->SetIndexNo(1);
		bool flag = pDoc->m_meshList.GetCount() >= 1;
		if (flag) {
			pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
			pDoc->m_meshList.RemoveAll();
		}

		pDoc->m_meshList.AddTail(mesh);

		bool rc = mesh->GetMeshPatch()->inputBinaryFile(LPCTSTR(cFile.GetPathName()));
		if (rc) 
		{
			int n = 1;
			for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;n++) 
			{
				QBody *patch=(QBody *)((pDoc->m_meshList).GetNext(Pos));
				pview3d->AddDisplayObj(patch, false);
				patch->SetIndexNo(n);

				if (n==2) patch->SetAttribFlag(6);

				CString str;
				str.Format("Verts: %d", patch->GetTrglNodeNumber());
				m_wndStatusBar.SetPaneText(4, str); 
			}
			pDoc->UpdateAllViews(NULL);
			GetMainView()->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		}

		AfxGetApp()->EndWaitCursor();

		CControlPanel* panel = GetControlPanel();
		panel->SetLogWindowText("Binary File imported.\n");
	}
}

void CMainFrame::OnExportNodes()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".txt"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("TXT files(*.txt)|*.txt||"), NULL );
	cFile.m_ofn.lpstrTitle="TXT Files Export";

	if (cFile.DoModal()==IDOK) {
		//CMainFrame *pWnd=(CMainFrame *)(AfxGetMainWnd());
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

		AfxGetApp()->BeginWaitCursor();
		//write obj
		//const char * filename = LPCTSTR(cFile.GetPathName());
		FILE *fp;	
		double xx,yy,zz;
	
		if (!(fp=fopen(LPCTSTR(cFile.GetPathName()), "w"))) 
			return;

		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				node->GetCoord3D(xx,yy,zz);
				fprintf(fp, "%f %f %f\n", (float)xx, (float)yy, (float)zz);
			}
		}
		fclose(fp);
		AfxGetApp()->EndWaitCursor();
	}
}


void CMainFrame::OnExportPwnfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".pwn"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("PWN files(*.pwn)|*.pwn||"), NULL );
	cFile.m_ofn.lpstrTitle="PWN Files Export";

	if (cFile.DoModal()==IDOK) {
		//CMainFrame *pWnd=(CMainFrame *)(AfxGetMainWnd());
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

		AfxGetApp()->BeginWaitCursor();
		
		FILE *fp;	
		double xx,yy,zz, nx, ny, nz;
	
		if (!(fp=fopen(LPCTSTR(cFile.GetPathName()), "w"))) 
			return;

		
		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			int i = 1;
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
			fprintf(fp, "%d\n", mesh->GetTrglNodeNumber());
			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;++i)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				if (node->inner) continue;
				node->GetCoord3D(xx,yy,zz);
				fprintf(fp, "%f %f %f\n", (float)xx, (float)yy, (float)zz);
				//if (i == 20) break;
			}
			i = 1;
			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;++i)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				if (node->inner) continue;
				node->GetNormal(nx,ny,nz);
				fprintf(fp, "%f %f %f\n", (float)nx, (float)ny, (float)nz);
				//if (i == 20) break; 
			}
		}
		fclose(fp);
		AfxGetApp()->EndWaitCursor();
	}
}

void CMainFrame::OnExportObjFiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".obj"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("OBJ files(*.obj)|*.obj||"), NULL );
	cFile.m_ofn.lpstrTitle="OBJ Files Export";

	if (cFile.DoModal()==IDOK) 
	{
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

		AfxGetApp()->BeginWaitCursor();

		FILE *fp;	
		double xx,yy,zz;

		if (!(fp=fopen(LPCTSTR(cFile.GetPathName()), "w"))) 
			return;

		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			int i = 1;
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
			Pos=NULL;
			
			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				if (node->inner) continue;
				node->GetCoord3D(xx,yy,zz);
				node->SetIndexNo(i); i++;
				fprintf(fp,"v %.6f %.6f %.6f\n",(float)xx,(float)yy,(float)zz);
			}

			for(GLKPOSITION Pos2=(mesh->GetTrglFaceList()).GetHeadPosition();Pos2!=NULL;)
			{
				QMeshFace *face=(QMeshFace *)((mesh->GetTrglFaceList()).GetNext(Pos2));
				if (face->inner) continue;
				const int num = face->GetEdgeNum();
				fprintf(fp,"f ");
				for (int k = 0; k < num; k++) 
				{
					const int index = face->GetNodeRecordPtr(k+1)->GetIndexNo();
					fprintf(fp,"%d ",index);
				}
				fprintf(fp,"\n");
			}
		}
		fclose(fp);
		printf("%s exported\n", cFile.GetFileName());
		AfxGetApp()->EndWaitCursor();
	}
}

void CMainFrame::OnExportPlyfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".ply"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("PLY files(*.ply)|*.ply||"), NULL );
	cFile.m_ofn.lpstrTitle="PLY Files Export";

	if (cFile.DoModal()==IDOK) 
	{
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

		AfxGetApp()->BeginWaitCursor();

		FILE *fp;	
		double xx,yy,zz;

		if (!(fp=fopen(LPCTSTR(cFile.GetPathName()), "w"))) 
			return;

		int faceNum = 0, nodeNum = 0;

		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));

			faceNum = mesh->GetTrglFaceNumber();
			nodeNum = mesh->GetTrglNodeNumber();
		}

		fprintf(fp,"ply\n");
		fprintf(fp,"format ascii 1.0\n");
		fprintf(fp,"element vertex %d\n",nodeNum);
		fprintf(fp,"property float x\n");
		fprintf(fp,"property float y\n");
		fprintf(fp,"property float z\n");
		fprintf(fp,"element face %d\n",faceNum);
		fprintf(fp,"property list uchar int vertex_indices\n");
		fprintf(fp,"end_header\n");

		/*for(Pos=m_polyMeshBody->GetMeshList().GetHeadPosition();Pos!=NULL;) {
			QMeshPatch *mesh=(QMeshPatch *)(m_polyMeshBody->GetMeshList().GetNext(Pos));
			for(PosNode=mesh->GetNodeList().GetHeadPosition();PosNode!=NULL;n++) {
				QMeshNode *node=(QMeshNode *)(mesh->GetNodeList().GetNext(PosNode));
				node->GetCoord3D(xx,yy,zz);
				fprintf(fp,"%f %f %f \n",(float)xx,(float)yy,(float)zz);
			}
		}

		for(Pos=m_polyMeshBody->GetMeshList().GetHeadPosition();Pos!=NULL;) {
			QMeshPatch *mesh=(QMeshPatch *)(m_polyMeshBody->GetMeshList().GetNext(Pos));
			for(PosFace=mesh->GetFaceList().GetHeadPosition();PosFace!=NULL;n++) {
				QMeshFace *face=(QMeshFace *)(mesh->GetFaceList().GetNext(PosFace));
				int edgeNum=face->GetEdgeNum();
				fprintf(fp,"%d ",edgeNum);
				for(int i=0;i<edgeNum;i++) {
					fprintf(fp,"%d ",face->GetNodeRecordPtr(i)->GetIndexNo());
				}
				fprintf(fp,"\n");
			}
		}*/

		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			int i = 0;
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));

			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;++i)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				if (node->inner) continue;
				node->GetCoord3D(xx,yy,zz);
				node->SetIndexNo(i);
				fprintf(fp,"%f %f %f\n",(float)xx,(float)yy,(float)zz);
			}

			for(GLKPOSITION Pos2=(mesh->GetTrglFaceList()).GetHeadPosition();Pos2!=NULL;)
			{
				QMeshFace *face=(QMeshFace *)((mesh->GetTrglFaceList()).GetNext(Pos2));
				if (face->inner) continue;
				const int num = face->GetEdgeNum();
				fprintf(fp,"%d ", num);
				for (int k = 0; k < num; k++) 
				{
					const int index = face->GetNodeRecordPtr(k+1)->GetIndexNo();
					fprintf(fp,"%d ",index);
				}
				fprintf(fp,"\n");
			}
		}
		fclose(fp);
		AfxGetApp()->EndWaitCursor();
	}
}
//-----------------------------------------------------------------------------------------------------------------

void CMainFrame::InitToolBar(void)
{
	HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
										MAKEINTRESOURCE(IDB_VIEW_TOOLBAR),
										IMAGE_BITMAP,
										0,0,
										LR_CREATEDIBSECTION /*| LR_LOADMAP3DCOLORS*/ );

	CBitmap bm;
	bm.Attach(hbm); 

    //active
    m_imageToolBar.Create( 24, 24, TRUE | ILC_COLOR24, 4, 0 );
    //bm.LoadBitmap( IDB_BITMAP1 );
    m_imageToolBar.Add( &bm,(CBitmap*)NULL );
    //bm.Detach();
    m_wndToolBar.GetToolBarCtrl().SetImageList( &m_imageToolBar );

    //not active
    m_imageToolBarDisable.Create( 24, 24,TRUE | ILC_COLOR24, 4, 0 );
    //bm.LoadBitmap( IDB_BITMAP1 );
    m_imageToolBarDisable.Add( &bm,(CBitmap*)NULL );
    //bm.Detach();
    m_wndToolBar.GetToolBarCtrl().SetDisabledImageList( &m_imageToolBarDisable );

    //mouse on
    m_imageToolBarHot.Create( 24, 24, TRUE | ILC_COLOR24, 4, 0 );
    //bm.LoadBitmap( IDB_BITMAP1 );
    m_imageToolBarHot.Add( &bm,(CBitmap*)NULL );
    //bm.Detach();
    m_wndToolBar.GetToolBarCtrl().SetHotImageList( &m_imageToolBarHot );
}

void CMainFrame::changePtSize(int model, const float& ptSize)
{
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

	for(POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;) {
		QBody *patch=(QBody *)((pDoc->m_meshList).GetNext(Pos));
		if (patch->m_ptSize != ptSize)
		{
			patch->m_ptSize = ptSize;
			GetMainView()->GetGLKernelView()->m_nodeDrawn = false;
			GetMainView()->GetGLKernelView()->refresh();
		}
		
	}
}





void CMainFrame::OnEditFindNormals()
{
	// TODO: Add your command handler code here
	long startTime = clock();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CGLKernelView *pview3d=GetMainView()->GetGLKernelView();

	int totalNodeNum;
	QBody *mesh;
	for(POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;) {
			mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
			totalNodeNum = mesh->GetTrglNodeNumber();
	}

	ANNpointArray dataPts = annAllocPts(totalNodeNum, 3); // allocate data points
	QMeshNode **nodeArray = new QMeshNode*[totalNodeNum]; // nodeArray
	
	//store nodes to dataPts
	int i = 0;
	for(GLKPOSITION PosNode=mesh->GetTrglNodeList().GetHeadPosition();PosNode!=NULL;++i) {
		QMeshNode *node=(QMeshNode *)(mesh->GetTrglNodeList().GetNext(PosNode));
		
		double xx, yy, zz;
		node->GetCoord3D(xx, yy, zz);
		dataPts[i][0] = xx;
		dataPts[i][1] = yy;
		dataPts[i][2] = zz;
		nodeArray[i] = node;
	}

	ANNkd_tree *KdTree = new ANNkd_tree( // build search structure
						dataPts, // the data points
						totalNodeNum, // number of points
						3); // dimension of space

	int neighNum = 21;  //number of neighborhoods to be found
	ANNpoint queryPt = annAllocPt(3); // allocate query point
	ANNidxArray nnIdx = new ANNidx[neighNum]; // allocate near neigh indices
	ANNdistArray dists = new ANNdist[neighNum]; // allocate near neighbor dists

	//data for Jacobi method
	double **A = new double*[3];
	double **v = new double*[3];
	for (int k = 0; k < 3; ++k)
	{
		A[k] = new double[3];
		v[k] = new double[3];
	}

	//do for each point
	for (int i = 0; i < totalNodeNum; ++i)
	{
		queryPt[0] = dataPts[i][0];
		queryPt[1] = dataPts[i][1];
		queryPt[2] = dataPts[i][2];

		KdTree->annkSearch( // search
								queryPt, // query point
								neighNum, // number of near neighbors
								nnIdx, // nearest neighbors (returned)
								dists, // distance (returned)
								0); // error bound

		//compute the centroid
		double cx = 0.0f;
		double cy = 0.0f;
		double cz = 0.0f;
		for (int j = 0; j < neighNum; ++j)
		{
			cx += dataPts[nnIdx[j]][0];
			cy += dataPts[nnIdx[j]][1];
			cz += dataPts[nnIdx[j]][2];
		}
		cx /= neighNum;
		cy /= neighNum;
		cz /= neighNum;

		//data for Jacobi method
		
		for (int k = 0; k < 3; ++k)
			A[k][0] = A[k][1] = A[k][2] = 0.0f;
		
		//CV matrix
		for (int j = 0; j < neighNum; ++j)
		{   
			const double vx = dataPts[nnIdx[j]][0] - cx;
			const double vy = dataPts[nnIdx[j]][1] - cy;
			const double vz = dataPts[nnIdx[j]][2] - cz;

			A[0][0] += vx*vx;
			A[0][1] += vx*vy;
			A[0][2] += vx*vz;

			A[1][1] += vy*vy;
			A[1][2] += vy*vz;

			A[2][2] += vz*vz;
		}
		A[1][0] = A[0][1];
		A[2][0] = A[0][2];
		A[2][1] = A[1][2];

		double w[3];

		GLKMatrixLib::JacobianEigensystemSolver(A, 3, v, w, 0.000001, 100);

		int mini;

		if (fabs(w[0]) < fabs(w[1])) 
			mini = 0;
		else mini = 1;

		if (fabs(w[mini]) > fabs(w[2])) 
			mini = 2;
	    
		nodeArray[i]->SetNormal(v[0][mini], v[1][mini], v[2][mini]);
	}	
	
	//--------------------------------------------------------------------------
	//	free the memory
	/*heap->ClearList();
	delete heap;*/
	for (int i = 0; i < 3; ++i) 
	{
		delete[] A[i];
		delete[] v[i];
    }
	delete [] A;
    delete [] v;
	delete [] nodeArray;
	delete [] nnIdx;
	delete [] dists;
	/*delete [] nnIdx2;
	delete [] dists2;
	delete [] nnIdx3;
	delete [] dists3;*/
	delete KdTree;
	annClose();

	long stopTime = clock();

	pDoc->UpdateAllViews(NULL);
	GetMainView()->PostMessage(WM_COMMAND, ID_VIEW_ZOOMALL);
	pview3d->RedrawAll();
	pview3d->refresh();
	
	CControlPanel* panel = GetControlPanel();
	double elapsedTime = (double)(stopTime - startTime) / CLOCKS_PER_SEC;
	CString str;
	str.Format("%.3fs used for estimating all normals.\n", elapsedTime);
	panel->SetLogWindowText(str);
}

void CMainFrame::OnEditShifttoorigin()
{
	// TODO: Add your command handler code here
	long startTime = clock();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CGLKernelView *pview3d=GetMainView()->GetGLKernelView();

	int totalNodeNum;
	QBody *mesh;
	for(POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;) {
		mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
		totalNodeNum = mesh->GetTrglNodeNumber();
	}

	//---------------------------------------------------------------------------------
	//compute the centroid of all the points
	double centroid[3] = {0.0, 0.0, 0.0};
	for (GLKPOSITION PosNode=mesh->GetTrglNodeList().GetHeadPosition();PosNode!=NULL;) 
	{
		QMeshNode *node=(QMeshNode *)(mesh->GetTrglNodeList().GetNext(PosNode));

		double xx, yy, zz;
		node->GetCoord3D(xx, yy, zz);
		
		centroid[0] += xx;
		centroid[1] += yy;
		centroid[2] += zz;
	}

	centroid[0] /= (double)totalNodeNum;
	centroid[1] /= (double)totalNodeNum;
	centroid[2] /= (double)totalNodeNum;

	//---------------------------------------------------------------------------------
	//shift all the points to the origin
	for (GLKPOSITION PosNode=mesh->GetTrglNodeList().GetHeadPosition();PosNode!=NULL;) 
	{
		QMeshNode *node=(QMeshNode *)(mesh->GetTrglNodeList().GetNext(PosNode));

		double xx, yy, zz;
		node->GetCoord3D(xx, yy, zz);
		node->SetCoord3D(xx - centroid[0], yy - centroid[1], zz - centroid[2]);
	}

	long stopTime = clock();

	pDoc->UpdateAllViews(NULL);
	GetMainView()->PostMessage(WM_COMMAND, ID_VIEW_ZOOMALL);
	pview3d->RedrawAll();
	pview3d->refresh();

	CControlPanel* panel = GetControlPanel();
	double elapsedTime = (double)(stopTime - startTime) / CLOCKS_PER_SEC;
	CString str;
	str.Format("%.3fs used for shifting to origin.\n", elapsedTime);
	panel->SetLogWindowText(str);

}

void CMainFrame::ImportObjfiles(CString filename)
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CMeshWorksView *pView=GetMainView();	

	CGLKernelView *pview3d=pView->GetGLKernelView();

	
	QBody *mesh = new QBody((void*)pView);
	mesh->SetIndexNo(1);

	bool flag = pDoc->m_meshList.GetCount() >= 1;
	if (flag) {
		pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
		pDoc->m_meshList.RemoveAll();
	}

	pDoc->m_meshList.AddTail(mesh);
	bool rc = mesh->GetMeshPatch()->inputOBJFile(filename);
	if (rc) 
	{
		QBody *patch=(QBody *)(pDoc->m_meshList).GetTail();
		pview3d->AddDisplayObj(patch,FALSE);
        
		pDoc->UpdateAllViews(NULL);

		pView->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		pView->PostMessage(WM_COMMAND,ID_VIEW_FRONT);

		CString str;
		str.Format("Verts: %d", patch->GetTrglNodeNumber());
		m_wndStatusBar.SetPaneText(4, str); 
	}

}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{ 
	/*TakeScreenshot("C:\\Users\\thkwok\\Desktop\\tmp.bmp");*/
	// TODO: Add your message handler code here and/or call default

	int nFileCount = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,100);
	AfxGetApp()->BeginWaitCursor();

	for(int k=0;k<nFileCount;k++)   
	{   
		TCHAR szFile[MAX_PATH];
		int i = DragQueryFile(hDropInfo, k, szFile, MAX_PATH);

		CString filename=szFile;
		filename.TrimRight();
		int index = filename.ReverseFind('.');
		index = filename.GetLength()-index-1;
		CString tmp =filename.Right(index);
		if (!tmp.CompareNoCase("obj"))
			ImportObjfiles(filename);
		if (!tmp.CompareNoCase("tet"))
			ImportTetfiles(filename);
		if (!tmp.CompareNoCase("ele"))
			ImportELEfiles(filename);
		if (!tmp.CompareNoCase("mesh"))
			ImportMESHfiles(filename);
		if (!tmp.CompareNoCase("hex"))
			ImportHexfiles(filename);
	}

	DragFinish(hDropInfo);

	AfxGetApp()->EndWaitCursor();


//	CFrameWnd::OnDropFiles(hDropInfo);
}
void CMainFrame::OnEditAdjustorientation()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
	// TODO: Add your command handler code here
	float rx,ry;
	BeginWaitCursor();
	pview3d->GetRotation(rx,ry);
	QBody *patch=(QBody *)(pDoc->m_meshList.GetHead());
	patch->GetMeshPatch()->AdjustOrientation(rx,ry);
	pview3d->RedrawAll();
	pview3d->refresh();
	EndWaitCursor();
	pview3d->SetViewDirection(VD_FRONTVIEW);
}

void CMainFrame::OnEditScaletounitheight()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CGLKernelView *pview3d=GetMainView()->GetGLKernelView();
	QBody *body=(QBody *)(pDoc->m_meshList.GetHead());
	QMeshPatch *patch = body->GetMeshPatch();

	patch->ScaleToUnitHeight();

	pview3d->RedrawAll();
	pview3d->refresh();
}

void CMainFrame::OnCheckingGenusNumber()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * mesh = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		int euler = mesh->GetTrglNodeList().GetCount()-mesh->GetTrglEdgeList().GetCount()+mesh->GetTrglFaceList().GetCount();

		int boundary=Hole_Identification(mesh->GetMeshPatch());
		euler += boundary;

		int genus = (2-(euler))/2;

		
		pDoc->UpdateAllViews(NULL);
		cView->RedrawAll();
		cView->refresh();

		CControlPanel* panel = GetControlPanel();
		CString tmp;
		tmp.Format("genus # %d\n", genus);
		panel->SetLogWindowText(tmp);
	}
}

int CMainFrame::Hole_Identification(QMeshPatch *patch){
	int num_hole=0;
	int edge_num = patch->GetTrglEdgeList().GetCount();
	bool *passed = new bool[edge_num];
	for (int i=0; i<edge_num; i++){
		passed[i]=false;
	}

	for (GLKPOSITION Pos = patch->GetTrglEdgeList().GetHeadPosition(); Pos!=NULL;){
		QMeshEdge* edge = (QMeshEdge*)patch->GetTrglEdgeList().GetNext(Pos);
		if (!edge->IsBoundaryEdge() || passed[edge->GetIndexNo()-1]) continue;
		passed[edge->GetIndexNo()-1]=true;

		QMeshNode *process_node=NULL, *fixed_node=NULL, *prev_node = NULL;
		QMeshFace *face = edge->GetLeftFace();
		if (face){
			prev_node=edge->GetEndPoint(); process_node=edge->GetStartPoint();	
		}else{
			prev_node=edge->GetStartPoint(); process_node=edge->GetEndPoint();
		}
		fixed_node=prev_node;
		int c=0;
		while (process_node!=fixed_node){
			c++; 
			if (c>patch->GetTrglEdgeNumber()) {
				printf("(Hole_Identification)ERROR: looped");
				AfxMessageBox("(Hole_Identification)ERROR: looped");
				return -1;
			}
			for (GLKPOSITION Pos1 = process_node->GetTrglEdgeList().GetHeadPosition(); Pos1!=NULL;){
				QMeshEdge *edge = (QMeshEdge*)process_node->GetTrglEdgeList().GetNext(Pos1);
				if (!edge->IsBoundaryEdge()) continue;
				if (edge->GetStartPoint()==prev_node || edge->GetEndPoint()==prev_node) continue;
				passed[edge->GetIndexNo()-1]=true;
				QMeshNode* next_node=NULL;
				if (edge->GetStartPoint()==process_node) 
					next_node=edge->GetEndPoint(); 
				else next_node=edge->GetStartPoint();
				//if (next_node==prev_node) continue;
				prev_node=process_node; process_node=next_node;
				break;
			}
		}
		num_hole++;
	}
	delete [] passed;
	return num_hole;
}

void CMainFrame::OnCheckingHolesNumber()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * mesh = (QBody *)((pDoc->m_meshList).GetNext(Pos));
	
		CControlPanel* panel = GetControlPanel();
		CString tmp;
		tmp.Format("Holes # %d\n", Hole_Identification(mesh->GetMeshPatch()));
		panel->SetLogWindowText(tmp);
	}
}

void CMainFrame::OnSelectNode()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * body = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		QMeshPatch* patch = (QMeshPatch*)body->GetMeshPatch();
		SelectNodeTool* tool= new SelectNodeTool(cView, patch, cView->GetCurrentTool());
		//cView->clear_tools();
		cView->set_tool(tool);
	}
	cView->SetNode(true);
	cView->refresh();
}

void CMainFrame::OnSelectFace()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * body = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		QMeshPatch* patch = (QMeshPatch*)body->GetMeshPatch();
		SelectFaceTool* tool= new SelectFaceTool(cView, patch, cView->GetCurrentTool(), FACE);
		//cView->clear_tools();
		cView->set_tool(tool);
	}
}

void CMainFrame::OnSelectionRegionSelect()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * body = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		QMeshPatch* patch = (QMeshPatch*)body->GetMeshPatch();
		RegionSelectionTool* tool= new RegionSelectionTool(cView, patch);
		cView->clear_tools();
		cView->set_tool(tool);
	}
}

void CMainFrame::OnImportTetfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".TET"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("TET files(*.tet)|*.tet||"), NULL );

	cFile.m_ofn.lpstrTitle="TET Files Import";
	if (cFile.DoModal()==IDOK) {
		CString filename = cFile.GetPathName();
		ImportTetfiles(filename);
		
	}
}

void CMainFrame::OnExportTetfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".tet"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("TET files(*.tet)|*.tet||"), NULL );
	cFile.m_ofn.lpstrTitle="TET Files Export";

	if (cFile.DoModal()==IDOK) 
	{
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

		AfxGetApp()->BeginWaitCursor();

		FILE *fp;	
		double xx,yy,zz;

		if (!(fp=fopen(LPCTSTR(cFile.GetPathName()), "w"))) 
			return;

		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			int i = 1;
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
			Pos=NULL;
			QMeshPatch *patch = mesh->GetMeshPatch();
			
			//n vertices
			//n tets
			fprintf(fp, "%d vertices\n", mesh->GetTrglNodeList().GetCount());
			fprintf(fp, "%d tets\n", mesh->GetTrglTetraList().GetCount());

			
			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;++i)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				node->GetCoord3D(xx,yy,zz);
				node->SetIndexNo(i);
				fprintf(fp,"%.6f %.6f %.6f\n",(float)xx,(float)yy,(float)zz);
			}

			for(GLKPOSITION Pos2=(mesh->GetTrglTetraList()).GetHeadPosition();Pos2!=NULL;)
			{
				QMeshTetra *tetra=(QMeshTetra *)((mesh->GetTrglTetraList()).GetNext(Pos2));
				
					//for (int j=1; j<=4; j++){
					//	bool exist = false;
					//	for (int i=0; i<4; i++){
					//		if (tetra->nodeindex[i]==tetra->GetNodeRecordPtr(j)->GetIndexNo()-1)
					//			exist=true;
					//	}
					//	if (!exist) printf("Not exist: %d tetra, %d node\n", tetra->GetIndexNo(), tetra->GetNodeRecordPtr(j)->GetIndexNo());
					//}
				

				//const int num = tetra->GetEdgeNum();
				fprintf(fp,"4 ");
				for (int k = 0; k < 4; k++) 
				{
					int index = tetra->GetNodeRecordPtr(k+1)->GetIndexNo();
					//int index = tetra->nodeindex[k]+1;
					fprintf(fp,"%d ",index-1);
				}
				fprintf(fp,"\n");
			}
		}
		fclose(fp);
		printf("%s exported\n", cFile.GetFileName());
		AfxGetApp()->EndWaitCursor();
	}
}

void CMainFrame::ImportTetfiles(CString filename)
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CMeshWorksView *pView=GetMainView();	

	CGLKernelView *pview3d=pView->GetGLKernelView();


	QBody *mesh = new QBody((void*)pView);
	mesh->SetIndexNo(1);

	bool flag = pDoc->m_meshList.GetCount() >= 1;
	if (flag) {
		pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
		pDoc->m_meshList.RemoveAll();
	}

	pDoc->m_meshList.AddTail(mesh);
	bool rc = mesh->GetMeshPatch()->inputTETFile(filename);
	if (rc) 
	{
		QBody *patch=(QBody *)(pDoc->m_meshList).GetTail();
		pview3d->AddDisplayObj(patch,FALSE);
        
		pDoc->UpdateAllViews(NULL);

		pView->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		pView->PostMessage(WM_COMMAND,ID_VIEW_FRONT);

		CString str;
		str.Format("Verts: %d", patch->GetTrglNodeNumber());
		m_wndStatusBar.SetPaneText(4, str); 
	}

}

void CMainFrame::OnSelectionTetrahedron()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * body = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		QMeshPatch* patch = (QMeshPatch*)body->GetMeshPatch();
		SelectFaceTool* tool= new SelectFaceTool(cView, patch, cView->GetCurrentTool(), TETRA);
		//cView->clear_tools();
		cView->set_tool(tool);
	}
}

void CMainFrame::OnSelectionFacenode()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * body = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		QMeshPatch* patch = (QMeshPatch*)body->GetMeshPatch();
		SelectFaceTool* tool= new SelectFaceTool(cView, patch, cView->GetCurrentTool(), FACENODE);
		//cView->clear_tools();
		cView->set_tool(tool);
	}
}

void CMainFrame::OnEditFlipallnormals()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView=GetMainView()->GetGLKernelView();

	for (POSITION Pos=(pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
	{
		QBody * body = (QBody *)((pDoc->m_meshList).GetNext(Pos));

		QMeshPatch* patch = (QMeshPatch*)body->GetMeshPatch();

		for (GLKPOSITION Pos1 = patch->GetTrglFaceList().GetHeadPosition(); Pos1!=NULL;){
			QMeshFace* face = (QMeshFace*)patch->GetTrglFaceList().GetNext(Pos1);
			QMeshEdge* edge1 = face->GetEdgeRecordPtr(1);
			QMeshEdge* edge3 = face->GetEdgeRecordPtr(3);
			face->SetEdgeRecordPtr(1, edge3);
			face->SetEdgeRecordPtr(3, edge1);
			BOOL n1 = face->IsNormalDirection(1);
			BOOL n3 = face->IsNormalDirection(3);
			face->SetDirectionFlag(1, n3);
			face->SetDirectionFlag(3, n1);
			for (int i=1; i<=3; i++){
				face->SetDirectionFlag(i, !face->IsNormalDirection(i));
			}
		}

		//---------------------------------------------------------------------
		//	Step 2: compute the normal
		for(GLKPOSITION Pos=patch->GetTrglFaceList().GetHeadPosition();Pos!=NULL;) {
			QMeshFace *face=(QMeshFace*)(patch->GetTrglFaceList().GetNext(Pos));
			//if (face->inner) continue;
			face->CalPlaneEquation();
			face->CalCenterPos();
		}
		for(GLKPOSITION Pos=patch->GetTrglNodeList().GetHeadPosition();Pos!=NULL;) {
			QMeshNode *node=(QMeshNode*)(patch->GetTrglNodeList().GetNext(Pos));
			if (node->inner) continue;
			node->CalNormal();
		}
	}
	cView->RedrawAll();
	cView->refresh();
}

void CMainFrame::OnImportElefiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".ELE"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("ELE files(*.ele)|*.ele||"), NULL );

	cFile.m_ofn.lpstrTitle="ELE Files Import";
	if (cFile.DoModal()==IDOK) {
		CString filename = cFile.GetPathName();
		ImportELEfiles(filename);
		
	}
}

void CMainFrame::ImportELEfiles(CString filename)
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CMeshWorksView *pView=GetMainView();	

	CGLKernelView *pview3d=pView->GetGLKernelView();


	QBody *mesh = new QBody((void*)pView);
	mesh->SetIndexNo(1);

	bool flag = pDoc->m_meshList.GetCount() >= 1;
	if (flag) {
		pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
		pDoc->m_meshList.RemoveAll();
	}


	pDoc->m_meshList.AddTail(mesh);
	bool rc = mesh->GetMeshPatch()->inputELEFile(filename);
	if (rc) 
	{
		QBody *patch=(QBody *)(pDoc->m_meshList).GetTail();
		pview3d->AddDisplayObj(patch,FALSE);
        
		pDoc->UpdateAllViews(NULL);

		pView->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		pView->PostMessage(WM_COMMAND,ID_VIEW_FRONT);

		CString str;
		str.Format("Verts: %d", patch->GetTrglNodeNumber());
		m_wndStatusBar.SetPaneText(4, str); 
	}

}

void CMainFrame::OnImportMeshfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".ELE"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("ELE files(*.ele)|*.ele||"), NULL );

	cFile.m_ofn.lpstrTitle="ELE Files Import";
	if (cFile.DoModal()==IDOK) {
		CString filename = cFile.GetPathName();
		ImportMESHfiles(filename);
		
	}
}

void CMainFrame::ImportMESHfiles(CString filename)
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CMeshWorksView *pView=GetMainView();	

	CGLKernelView *pview3d=pView->GetGLKernelView();


	QBody *mesh = new QBody((void*)pView);
	mesh->SetIndexNo(1);

	bool flag = pDoc->m_meshList.GetCount() >= 1;
	if (flag) {
		pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
		pDoc->m_meshList.RemoveAll();
	}

	pDoc->m_meshList.AddTail(mesh);
	bool rc = mesh->GetMeshPatch()->inputMESHFile(filename);
	if (rc) 
	{
		QBody *patch=(QBody *)(pDoc->m_meshList).GetTail();
		pview3d->AddDisplayObj(patch,FALSE);
        
		pDoc->UpdateAllViews(NULL);

		pView->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		pView->PostMessage(WM_COMMAND,ID_VIEW_FRONT);

		CString str;
		str.Format("Verts: %d", patch->GetTrglNodeNumber());
		m_wndStatusBar.SetPaneText(4, str); 
	}

}

void CMainFrame::OnCheckingDegeneratetetra()
{
	// TODO: Add your command handler code here
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

	int count=0;
	for (POSITION Pos = pDoc->m_meshList.GetHeadPosition(); Pos!=NULL;){
		QBody *body = (QBody*)pDoc->m_meshList.GetNext(Pos);
		for (GLKPOSITION Pos1 = body->GetTrglTetraList().GetHeadPosition(); Pos1!=NULL;){
			QMeshTetra *tetra = (QMeshTetra*)body->GetTrglTetraList().GetNext(Pos1);
			double volume = tetra->CalVolume();
			if (volume>=0) continue;
			count++;
			if (volume<-1e-5) {
				printf("%.10f\n", volume);
				for (int i=1; i<=4; i++){
					tetra->GetFaceRecordPtr(i)->selected=true;
				}
			}
		}
	}

	CControlPanel* panel = GetControlPanel();
	CString tmp;
	tmp.Format("Degenerate Tetras # %d\n", count);
	panel->SetLogWindowText(tmp);

	GetMainView()->GetGLKernelView()->RedrawAll();
	GetMainView()->GetGLKernelView()->refresh();

}

void CMainFrame::OnExportElefiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".ele"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("ELE files(*.ele)|*.ele||"), NULL );
	cFile.m_ofn.lpstrTitle="ELE Files Export";

	if (cFile.DoModal()==IDOK) 
	{
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
		AfxGetApp()->BeginWaitCursor();
		QBody *mesh = (QBody *)((pDoc->m_meshList).GetHead());

		CString ELEFile = cFile.GetPathName();
		CString NodeFile = cFile.GetPathName();
		NodeFile.Delete(NodeFile.GetLength()-3, 3);
		NodeFile.Append("node");

		FILE *fp;
		if (!(fp=fopen(LPCTSTR(NodeFile), "w"))) 
			return;

		fprintf(fp, "%d  3  0  0\n", mesh->GetTrglNodeNumber());
		int n=0;
		for (GLKPOSITION Pos = mesh->GetTrglNodeList().GetHeadPosition(); Pos!=NULL; n++){
			QMeshNode * node = (QMeshNode *)mesh->GetTrglNodeList().GetNext(Pos);
			double xx,yy,zz;
			node->GetCoord3D(xx, yy, zz);
			node->SetIndexNo(n+1);
			fprintf(fp, "%4d    %.17f  %.17f  %.17f\n", n, xx, yy, zz);
		}
		fprintf(fp, "# Generated by VolumeMeshWork\n");
		fclose(fp);

		if (!(fp=fopen(LPCTSTR(ELEFile), "w"))) 
			return;
		fprintf(fp, "%d  4  0\n", mesh->GetTrglTetraNumber());
		n=0;
		for (GLKPOSITION Pos = mesh->GetTrglTetraList().GetHeadPosition(); Pos!=NULL; n++){
			QMeshTetra *tetra = (QMeshTetra*)mesh->GetTrglTetraList().GetNext(Pos);
			int idx[4];
			for (int i=0; i<4; i++)
				idx[i] = tetra->GetNodeRecordPtr(i+1)->GetIndexNo()-1;
			tetra->SetIndexNo(n+1);
			fprintf(fp, "%5d %7d %5d %5d %5d\n", n, idx[0], idx[1], idx[2], idx[3]);
		}
		fprintf(fp, "# Generated by VolumeMeshWork\n");
		fclose(fp);
		AfxGetApp()->EndWaitCursor();
	}
}

void CMainFrame::OnMeshoperationDeleteselected()
{
	// TODO: Add your command handler code here
	AfxGetApp()->BeginWaitCursor();
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	QBody *mesh = (QBody *)((pDoc->m_meshList).GetHead());
	QMeshPatch *patch = mesh->GetMeshPatch();

	SetMeshListPos(patch);

	int delCount=0;
	for (GLKPOSITION Pos = patch->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
		QMeshTetra *tetra = (QMeshTetra *)patch->GetTrglTetraList().GetNext(Pos);
		int count=0;
		for (int i=1; i<=4; i++){
			QMeshFace *face = tetra->GetFaceRecordPtr(i);
			if (face->selected) count++;
		}
		if (count>1){
			patch->deleteTetra(tetra);
			delCount++;
		}
		//Pos=patch->GetTrglFaceList().GetHeadPosition();
	}

	patch->RecomputeAllNormals();

	GetMainView()->GetGLKernelView()->RedrawAll();
	GetMainView()->GetGLKernelView()->refresh();

	
	int n=1;
	for (GLKPOSITION Pos = patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
		QMeshNode * node = (QMeshNode *)patch->GetTrglNodeList().GetNext(Pos);
		node->SetIndexNo(n++);
	}
	n=1;
	for (GLKPOSITION Pos = patch->GetTrglEdgeList().GetHeadPosition(); Pos!=NULL;){
		QMeshEdge * edge = (QMeshEdge *)patch->GetTrglEdgeList().GetNext(Pos);
		edge->SetIndexNo(n++);
	}
	n=1;
	for (GLKPOSITION Pos = patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
		QMeshFace * face = (QMeshFace *)patch->GetTrglFaceList().GetNext(Pos);
		face->SetIndexNo(n++);
		face->selected=false;
	}
	n=1;
	for (GLKPOSITION Pos = patch->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
		QMeshTetra * tetra = (QMeshTetra *)patch->GetTrglTetraList().GetNext(Pos);
		tetra->SetIndexNo(n++);
	}

	AfxGetApp()->EndWaitCursor();

	printf("deleted %d tetrahedra\n", delCount);
}

void CMainFrame::SetMeshListPos(QMeshPatch *patch, bool recompute){
	if (recompute || !((QMeshNode*)patch->GetTrglNodeList().GetHead())->Pos){
		for (GLKPOSITION Pos = patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
			QMeshNode * node = (QMeshNode *)patch->GetTrglNodeList().GetAt(Pos);
			node->Pos = Pos;
			Pos=Pos->next;
		}
		for (GLKPOSITION Pos = patch->GetTrglEdgeList().GetHeadPosition(); Pos!=NULL;){
			QMeshEdge * edge = (QMeshEdge *)patch->GetTrglEdgeList().GetAt(Pos);
			edge->Pos = Pos;
			Pos=Pos->next;
		}
		for (GLKPOSITION Pos = patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
			QMeshFace * face = (QMeshFace *)patch->GetTrglFaceList().GetAt(Pos);
			face->Pos = Pos;
			Pos=Pos->next;
		}
		for (GLKPOSITION Pos = patch->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
			QMeshTetra * tetra = (QMeshTetra *)patch->GetTrglTetraList().GetAt(Pos);
			tetra->Pos = Pos;
			Pos=Pos->next;
		}
	}
}

void CMainFrame::OnModificationSmoothboundarysurface()
{
	// TODO: Add your command handler code here
	AfxGetApp()->BeginWaitCursor();
	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	QBody *mesh = (QBody *)((pDoc->m_meshList).GetHead());
	QMeshPatch *patch = mesh->GetMeshPatch();

	SetMeshListPos(patch);

	int delCount=0;
	while (true){
		int tmpC=0;
		for (GLKPOSITION Pos = patch->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
			QMeshTetra * tetra = (QMeshTetra *)patch->GetTrglTetraList().GetNext(Pos);
			int count=0;
			for (int i=1; i<=4; i++)
				if (!tetra->GetFaceRecordPtr(i)->i_inner)
					count++;
			if (count>=2) {
				patch->deleteTetra(tetra);
				tmpC++;
			}
		}
		if (tmpC==0) break;
		delCount+=tmpC;
	}

	patch->RecomputeAllNormals();

	int n=1;
	for (GLKPOSITION Pos = patch->GetTrglNodeList().GetHeadPosition(); Pos!=NULL;){
		QMeshNode * node = (QMeshNode *)patch->GetTrglNodeList().GetNext(Pos);
		node->SetIndexNo(n++);
	}
	n=1;
	for (GLKPOSITION Pos = patch->GetTrglEdgeList().GetHeadPosition(); Pos!=NULL;){
		QMeshEdge * edge = (QMeshEdge *)patch->GetTrglEdgeList().GetNext(Pos);
		edge->SetIndexNo(n++);
	}
	n=1;
	for (GLKPOSITION Pos = patch->GetTrglFaceList().GetHeadPosition(); Pos!=NULL;){
		QMeshFace * face = (QMeshFace *)patch->GetTrglFaceList().GetNext(Pos);
		face->SetIndexNo(n++);
		face->selected=false;
	}
	n=1;
	for (GLKPOSITION Pos = patch->GetTrglTetraList().GetHeadPosition(); Pos!=NULL;){
		QMeshTetra * tetra = (QMeshTetra *)patch->GetTrglTetraList().GetNext(Pos);
		tetra->SetIndexNo(n++);
	}


	GetMainView()->GetGLKernelView()->RedrawAll();
	GetMainView()->GetGLKernelView()->refresh();

	AfxGetApp()->EndWaitCursor();

	printf("deleted %d tetrahedra\n", delCount);
}

void CMainFrame::OnImportHexfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(TRUE/*if save then turn to FALSE*/, _T(".HEX"), NULL,
		OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("HEX files(*.tet)|*.hex||"), NULL );

	cFile.m_ofn.lpstrTitle="HEX Files Import";
	if (cFile.DoModal()==IDOK) {
		CString filename = cFile.GetPathName();
		ImportHexfiles(filename);
		
	}
}

void CMainFrame::ImportHexfiles(CString filename)
{
	// TODO: Add your command handler code here
	GetMainView()->GetGLKernelView()->clear_tools();

	CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();
	CMeshWorksView *pView=GetMainView();	

	CGLKernelView *pview3d=pView->GetGLKernelView();


	QBody *mesh = new QBody((void*)pView);
	mesh->SetIndexNo(1);

	bool flag = pDoc->m_meshList.GetCount() >= 1;
	if (flag) {
		pview3d->DelDisplayObj((QBody *)((pDoc->m_meshList).GetHead()));
		pDoc->m_meshList.RemoveAll();
	}


	pDoc->m_meshList.AddTail(mesh);
	bool rc = mesh->GetMeshPatch()->inputHEXFile(filename);
	if (rc) 
	{
		QBody *patch=(QBody *)(pDoc->m_meshList).GetTail();
		pview3d->AddDisplayObj(patch,FALSE);
        
		pDoc->UpdateAllViews(NULL);

		pView->PostMessage(WM_COMMAND,ID_VIEW_ZOOMALL);
		pView->PostMessage(WM_COMMAND,ID_VIEW_FRONT);

		CString str;
		str.Format("Verts: %d", patch->GetTrglNodeNumber());
		m_wndStatusBar.SetPaneText(4, str); 
	}

}

void CMainFrame::OnExportHexfiles()
{
	// TODO: Add your command handler code here
	CFileDialog cFile(FALSE/*if load then turn to TRUE*/, _T(".hex"), NULL,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("HEX files(*.hex)|*.hex||"), NULL );
	cFile.m_ofn.lpstrTitle="HEX Files Export";

	if (cFile.DoModal()==IDOK) 
	{
		CMeshWorksDoc *pDoc=(CMeshWorksDoc *)GetActiveDocument();

		AfxGetApp()->BeginWaitCursor();

		FILE *fp;	
		double xx,yy,zz;

		if (!(fp=fopen(LPCTSTR(cFile.GetPathName()), "w"))) 
			return;

		for(POSITION Pos = (pDoc->m_meshList).GetHeadPosition();Pos!=NULL;)
		{
			int i = 1;
			QBody *mesh=(QBody *)((pDoc->m_meshList).GetNext(Pos));
			Pos=NULL;
			QMeshPatch *patch = mesh->GetMeshPatch();
			
			//n vertices
			//n tets
			fprintf(fp, "%d vertices\n", mesh->GetTrglNodeList().GetCount());
			fprintf(fp, "%d hexs\n", mesh->GetTrglHexaList().GetCount());

			
			for(GLKPOSITION Pos2=(mesh->GetTrglNodeList()).GetHeadPosition();Pos2!=NULL;++i)
			{
				QMeshNode *node=(QMeshNode *)((mesh->GetTrglNodeList()).GetNext(Pos2));
				node->GetCoord3D(xx,yy,zz);
				node->SetIndexNo(i);
				fprintf(fp,"v %.6f %.6f %.6f\n",(float)xx,(float)yy,(float)zz);
			}

			for(GLKPOSITION Pos2=(mesh->GetTrglHexaList()).GetHeadPosition();Pos2!=NULL;)
			{
				QMeshHexa *hexa=(QMeshHexa *)((mesh->GetTrglHexaList()).GetNext(Pos2));
				
					//for (int j=1; j<=4; j++){
					//	bool exist = false;
					//	for (int i=0; i<4; i++){
					//		if (tetra->nodeindex[i]==tetra->GetNodeRecordPtr(j)->GetIndexNo()-1)
					//			exist=true;
					//	}
					//	if (!exist) printf("Not exist: %d tetra, %d node\n", tetra->GetIndexNo(), tetra->GetNodeRecordPtr(j)->GetIndexNo());
					//}
				

				//const int num = tetra->GetEdgeNum();
				fprintf(fp,"h ");
				for (int k = 0; k < 8; k++) 
				{
					int index = hexa->GetNodeRecordPtr(k+1)->GetIndexNo();
					//int index = tetra->nodeindex[k]+1;
					fprintf(fp,"%d ",index-1);
				}
				fprintf(fp,"\n");
			}
		}
		fclose(fp);
		printf("%s exported\n", cFile.GetFileName());
		AfxGetApp()->EndWaitCursor();
	}
}


// FEM Implementation

void CMainFrame::PrintInfo() {
	/*
	std::cout << "-----------------IN ON SELECT NODE-------------------------------" << std::endl;
	std::cout << "pDoc->m_meshList.GetHead():" << pDoc->m_meshList.GetHead() << std::endl;
	std::cout << "pDoc->m_meshList.GetHeadPosition():" << pDoc->m_meshList.GetHeadPosition() << std::endl;
	std::cout << "pDoc->m_meshList.GetTail():" << pDoc->m_meshList.GetTail() << std::endl;
	std::cout << "pDoc->m_meshList.GetTailPositon():" << pDoc->m_meshList.GetTailPosition() << std::endl;
	std::cout << "pDoc->m_meshList.GetCount():" << pDoc->m_meshList.GetCount() << std::endl;
	std::cout << "-----------------------------------------------------------------" << std::endl;

	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();
	//Access all nodes in for loop
	for (GLKPOSITION Pos = body->GetTrglNodeList().GetHeadPosition(); Pos != NULL; ) {

	QMeshNode* pNode = (QMeshNode*)body->GetTrglNodeList().GetNext(Pos);

	double xx, yy, zz;
	pNode->GetCoord3D(xx, yy, zz);
	std::cout << "Coordinates:" << xx << std::endl;
	std::cout << "Coordinates:" << yy << std::endl;
	std::cout << "Coordinates:" << zz << std::endl;
	}
	*/
	std::cout << "-----------------------------------------------------------------" << std::endl;

	CMeshWorksDoc *pDoc = (CMeshWorksDoc *)(GetActiveDocument());
	CGLKernelView *cView = GetMainView()->GetGLKernelView();

	//Accessed Face : was not able to use drawShade
	QBody * body = (QBody*)(pDoc->m_meshList).GetHead();
	QMeshFace* pFace = (QMeshFace*)body->GetTrglFaceList().GetHead();

	double M[2][3][3];

	int f = 0;
	for (GLKPOSITION Pos = body->GetTrglFaceList().GetHeadPosition(); Pos != NULL; ) {
		pFace = (QMeshFace*)body->GetTrglFaceList().GetNext(Pos);
		double xx, yy, zz;
		std::cout << "FACE: " << f << std::endl;
		for (int i = 1; i <= 3; i++) {
			pFace->GetNodePos(i, xx, yy, zz);
			M[f][i][0] = xx; M[f][i][1] = yy; M[f][i][2] = zz;
			std::cout << "Node number: " << i << " " << xx << " " << yy << " " << zz << std::endl;

		}
		f = f + 1;

	}

	// Printing values out

	std::cout << "-----------------------------------------------------------------" << std::endl;
}