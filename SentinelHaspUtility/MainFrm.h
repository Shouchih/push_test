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

// MainFrm.h : interface of the CMainFrame class
//

#include "SentinelHaspUtilityDoc.h"

#pragma once
class CSentinelHaspUtilityView;

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CSentinelHaspUtilityDoc *m_pSentinelHaspUtilityDoc;

// Operations
public:

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

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	CSentinelHaspUtilityView* m_pSentinelHaspUtilityView;

	afx_msg void OnBtnLogIn();
	afx_msg void OnBtnLogOut();
	afx_msg void OnBtnGetKeyTime();
	afx_msg void OnBtnWriteBuffer();
	afx_msg void OnBtnReadBuffer();
	afx_msg void OnBtnCheckHaspKeyAll();

	afx_msg void OnBtnOpenMotionFile();
	afx_msg void OnBtnWriteTrueFlag();
	afx_msg void OnBtnWriteFalseFlag();
	afx_msg void OnBtnReadCheckFlag();
	afx_msg void OnBtnWriteVideoSum();
	afx_msg void OnBtnReadVideoSum();
	afx_msg void OnBtnEncryptMotionFile();
	afx_msg void OnBtnCheckEncryptMotionFile();
	afx_msg void OnBtnReadConfig();
	afx_msg void OnBtnSaveConfig();
};


