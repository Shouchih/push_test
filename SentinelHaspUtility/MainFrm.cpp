// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "SentinelHaspUtilityView.h"
#include "MainFrm.h"
#include "DlgWriteKeyInfo.h"
#include "DlgWriteVideoSum.h"
#include "DlgEncyptMotionFile.h"
#include "DlgReadEncryptMotionFile.h"
#include "DlgSaveConfigParam.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_BTN_LOG_IN, &CMainFrame::OnBtnLogIn)
	ON_COMMAND(ID_BTN_LOG_OUT, &CMainFrame::OnBtnLogOut)
	ON_COMMAND(ID_BTN_GET_KEY_TIME, &CMainFrame::OnBtnGetKeyTime)
	ON_COMMAND(ID_BTN_WRITE_BUFFER, &CMainFrame::OnBtnWriteBuffer)
	ON_COMMAND(ID_BTN_READ_BUFFER, &CMainFrame::OnBtnReadBuffer)
	ON_COMMAND(ID_BTN_CHECK_HASP_KEY_ALL, &CMainFrame::OnBtnCheckHaspKeyAll)

	ON_COMMAND(ID_BTN_WRITE_TRUE_FLAG, &CMainFrame::OnBtnWriteTrueFlag)
	ON_COMMAND(ID_BTN_WRITE_FALSE_FLAG, &CMainFrame::OnBtnWriteFalseFlag)
	ON_COMMAND(ID_BTN_READ_CHECK_FLAG, &CMainFrame::OnBtnReadCheckFlag)
	ON_COMMAND(ID_BTN_WRITE_VIDEO_SUM, &CMainFrame::OnBtnWriteVideoSum)
	ON_COMMAND(ID_BTN_READ_VIDEO_SUM, &CMainFrame::OnBtnReadVideoSum)
	ON_COMMAND(ID_BTN_ENCRYPT_MOTION_FILE, &CMainFrame::OnBtnEncryptMotionFile)
	ON_COMMAND(ID_BTN_CHECK_ENCRYPT_MOTION_FILE, &CMainFrame::OnBtnCheckEncryptMotionFile)
	ON_COMMAND(ID_BTN_READ_CONFIG, &CMainFrame::OnBtnReadConfig)
	ON_COMMAND(ID_BTN_SAVE_CONFIG, &CMainFrame::OnBtnSaveConfig)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	:m_pSentinelHaspUtilityDoc(NULL)
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);

	m_pSentinelHaspUtilityView=NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU ;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnBtnLogIn()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->haspLogIn();
}


void CMainFrame::OnBtnLogOut()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->haspLogOut();
}


void CMainFrame::OnBtnGetKeyTime()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->haspKeyRealTime();
}


void CMainFrame::OnBtnWriteBuffer()
{
	// TODO: Add your command handler code here
	CDlgWriteKeyInfo dlgWriteKeyInfo;

	if(dlgWriteKeyInfo.DoModal()==IDOK){
		
	}
}


void CMainFrame::OnBtnReadBuffer()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->haspReadKeyData();
}


void CMainFrame::OnBtnCheckHaspKeyAll()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->checkHaspKey();
}




void CMainFrame::OnBtnWriteTrueFlag()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->writeHaspKeyCheckFlag(true);
}


void CMainFrame::OnBtnWriteFalseFlag()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->writeHaspKeyCheckFlag(false);
}


void CMainFrame::OnBtnReadCheckFlag()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->readHaspKeyCheckFlag();
}


void CMainFrame::OnBtnWriteVideoSum()
{
	// TODO: Add your command handler code here
	CDlgWriteVideoSum dlgWriteVideoSum;

	if(dlgWriteVideoSum.DoModal()==IDOK){
		
	}
}


void CMainFrame::OnBtnReadVideoSum()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityView){
		m_pSentinelHaspUtilityView=(CSentinelHaspUtilityView*)GetActiveView();
	}
	m_pSentinelHaspUtilityView->readHaspKeyVideoSum();
}


void CMainFrame::OnBtnEncryptMotionFile()
{
	// TODO: Add your command handler code here
	CDlgEncyptMotionFile dlgEncyptMotionFile;

	if(dlgEncyptMotionFile.DoModal()==IDOK){
		
	}
}


void CMainFrame::OnBtnCheckEncryptMotionFile()
{
	// TODO: Add your command handler code here
	CDlgReadEncryptMotionFile dlgReadEncryptMotionFile;

	if(dlgReadEncryptMotionFile.DoModal()==IDOK){
		
	}
}


void CMainFrame::OnBtnReadConfig()
{
	// TODO: Add your command handler code here
	if(!m_pSentinelHaspUtilityDoc){
		m_pSentinelHaspUtilityDoc=(CSentinelHaspUtilityDoc*)GetActiveDocument();
	}
	m_pSentinelHaspUtilityDoc->loadConfigParam();
}


void CMainFrame::OnBtnSaveConfig()
{
	// TODO: Add your command handler code here	

	CDlgSaveConfigParam dlgSaveConfigParam;

	if(dlgSaveConfigParam.DoModal()==IDOK){
		
	}

}
