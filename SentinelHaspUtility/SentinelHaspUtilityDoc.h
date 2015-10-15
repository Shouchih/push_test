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

// SentinelHaspUtilityDoc.h : interface of the CSentinelHaspUtilityDoc class
//
#pragma once

#include <vector>
#include "SentinelHaspApiLib\haspUtil.h"
using namespace std;

struct ConfigParam {
	char name[80];                                           //   name of param , relate project and device
    char UUID[80];
	struct HaspKeyTime  keyExpireDate;
	BOOL checkFlag;
	char LIB_APC_KEY[32];
	int videoSum;
	char AES_KEY[16];
};


class CSentinelHaspUtilityDoc : public CDocument
{
protected: // create from serialization only
	CSentinelHaspUtilityDoc();
	DECLARE_DYNCREATE(CSentinelHaspUtilityDoc)

// Attributes
public:
	char m_CurrentApDirectory[256];
	CString m_ConfigFileName;
	struct ConfigParam m_ConfigParam;

// Operations
public:

	void loadConfigParam();
	void saveConfigParam(CString configName,CString fileName);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CSentinelHaspUtilityDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
	

};
