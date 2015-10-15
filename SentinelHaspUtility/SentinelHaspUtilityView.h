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

// SentinelHaspUtilityView.h : interface of the CSentinelHaspUtilityView class
//

#pragma once

#include "resource.h"
#include "SentinelHaspUtilityDoc.h"

class CSentinelHaspUtilityView : public CFormView
{
protected: // create from serialization only
	CSentinelHaspUtilityView();
	DECLARE_DYNCREATE(CSentinelHaspUtilityView)

public:
	enum{ IDD = IDD_SENTINELHASPUTILITY_FORM };

// Attributes
public:
	CSentinelHaspUtilityDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CSentinelHaspUtilityView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	CSentinelHaspUtilityDoc *m_pSentinelHaspUtilityDoc;
	CString m_StatusInfo;
	void haspLogIn(void);
	void haspLogOut(void);
	void haspKeyRealTime(void);
	void haspReadKeyData(void);

	void checkHaspKey(void);

	void writeHaspKeyCheckFlag(BOOL checkFlag);
	void readHaspKeyCheckFlag();

	void writeHaspKeyVideoSum(int videoSum);
	void readHaspKeyVideoSum();

	void showMsgToStatusInfo(CString msg);
	CString m_EditContent;
	
};

#ifndef _DEBUG  // debug version in SentinelHaspUtilityView.cpp
inline CSentinelHaspUtilityDoc* CSentinelHaspUtilityView::GetDocument() const
   { return reinterpret_cast<CSentinelHaspUtilityDoc*>(m_pDocument); }
#endif

