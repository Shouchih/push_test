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

// SentinelHaspUtilityDoc.cpp : implementation of the CSentinelHaspUtilityDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SentinelHaspUtility.h"
#endif

#include "SentinelHaspUtilityDoc.h"
#include "xmlFileUtil.h"
#include <propkey.h>
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSentinelHaspUtilityDoc

IMPLEMENT_DYNCREATE(CSentinelHaspUtilityDoc, CDocument)

BEGIN_MESSAGE_MAP(CSentinelHaspUtilityDoc, CDocument)
END_MESSAGE_MAP()


// CSentinelHaspUtilityDoc construction/destruction

CSentinelHaspUtilityDoc::CSentinelHaspUtilityDoc()
{
	// TODO: add one-time construction code here
	GetCurrentDirectory(256,m_CurrentApDirectory);
	m_ConfigFileName="";

}

CSentinelHaspUtilityDoc::~CSentinelHaspUtilityDoc()
{
}

BOOL CSentinelHaspUtilityDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	loadConfigParam();
	// (SDI documents will reuse this document)

	return TRUE;
}




// CSentinelHaspUtilityDoc serialization

void CSentinelHaspUtilityDoc::Serialize(CArchive& ar)
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

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CSentinelHaspUtilityDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CSentinelHaspUtilityDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSentinelHaspUtilityDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSentinelHaspUtilityDoc diagnostics

#ifdef _DEBUG
void CSentinelHaspUtilityDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSentinelHaspUtilityDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


void CSentinelHaspUtilityDoc::loadConfigParam(){

	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter=LPCTSTR(_T("XML Files(*.xml)\0*.xml\0"));
	//定義打開對話框的標題
	fileDlg.m_ofn.lpstrTitle=LPCTSTR(_T("Browse a Config xml file"));

	fileDlg.m_ofn.lpstrInitialDir = _T(m_CurrentApDirectory);

	bool result=false;

	if (fileDlg.DoModal()==IDOK)
	{
		m_ConfigFileName=fileDlg.GetFileName();
		CString absFilePath=fileDlg.GetPathName();

		result=readXMLConfigParam((LPCTSTR)absFilePath,m_ConfigParam);

		//   將附檔名移除
		int index=m_ConfigFileName.Find(".xml");
		if(index>=0){
			m_ConfigFileName=m_ConfigFileName.Mid(0,index);
		}
	}

	fileDlg.DestroyWindow();

	//  initialize   m_ConfigParam
	if(result==false){

		//------------- UUID -----------------------------
		char msg[32];
		char uuid[37];

		if(getSysUUID(msg,uuid)==TRUE){
			stringCopy((char *)m_ConfigParam.UUID,uuid,sizeof(m_ConfigParam.UUID));
		}else{
		    m_ConfigParam.UUID[0]='\0';
		}
		//------------- keyExpireDate -----------------------------
		COleDateTime currentTime =COleDateTime::GetCurrentTime();

		m_ConfigParam.keyExpireDate.year=currentTime.GetYear()+1;
		m_ConfigParam.keyExpireDate.month=currentTime.GetMonth();
		m_ConfigParam.keyExpireDate.day=currentTime.GetDay();
		m_ConfigParam.keyExpireDate.hour=0;
		m_ConfigParam.keyExpireDate.minute=0;
		m_ConfigParam.keyExpireDate.second=0;

		// --------------- Others -------------------------------------
		m_ConfigParam.name[0]='\0';
		m_ConfigParam.checkFlag=FALSE;
		m_ConfigParam.LIB_APC_KEY[0]='0';
		m_ConfigParam.videoSum=0;
		m_ConfigParam.AES_KEY[0]='0';

	}

	SetTitle(m_ConfigParam.name);

}


void CSentinelHaspUtilityDoc::saveConfigParam(CString configName,CString fileName){

	COleDateTime currentTime =COleDateTime::GetCurrentTime();

	if(configName.GetLength()==0){

		configName=m_ConfigParam.name;

		if(configName.GetLength()==0){			
			configName.Format("Config save at %.4d-%.2d-%.2d",
				currentTime.GetYear(),
				currentTime.GetMonth(),
				currentTime.GetDay());
		}
	}
	stringCopy((char *)m_ConfigParam.name,configName.GetBuffer(),sizeof(m_ConfigParam.name));


	if(fileName.GetLength()==0){
		fileName=m_ConfigFileName;

		if(fileName.GetLength()==0){			
			fileName.Format("haspConfig-%.4d-%.2d-%.2d",
				currentTime.GetYear(),
				currentTime.GetMonth(),
				currentTime.GetDay());
		}
	}

	CString absFilePath;
	absFilePath.Format("%s\\%s.xml",m_CurrentApDirectory,fileName);

	saveXMLConfigParam( (LPCTSTR)absFilePath,m_ConfigParam);

	SetTitle(m_ConfigParam.name);

}


// CSentinelHaspUtilityDoc commands
