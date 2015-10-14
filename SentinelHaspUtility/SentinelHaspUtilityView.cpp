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

// SentinelHaspUtilityView.cpp : implementation of the CSentinelHaspUtilityView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SentinelHaspUtility.h"
#endif

#include "SentinelHaspUtilityDoc.h"
#include "SentinelHaspUtilityView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SentinelHaspApiLib\haspUtil.h"



// CSentinelHaspUtilityView

IMPLEMENT_DYNCREATE(CSentinelHaspUtilityView, CFormView)

BEGIN_MESSAGE_MAP(CSentinelHaspUtilityView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CSentinelHaspUtilityView construction/destruction

CSentinelHaspUtilityView::CSentinelHaspUtilityView()
	: CFormView(CSentinelHaspUtilityView::IDD)
	, m_StatusInfo(_T(""))
	, m_EditContent(_T(""))
{
	// TODO: add construction code here

}

CSentinelHaspUtilityView::~CSentinelHaspUtilityView()
{
}

void CSentinelHaspUtilityView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HASP_STATUS, m_StatusInfo);
	DDX_Text(pDX, IDC_EDIT_CONTENT, m_EditContent);
}

BOOL CSentinelHaspUtilityView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CSentinelHaspUtilityView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	checkHaspKey();
}

void CSentinelHaspUtilityView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSentinelHaspUtilityView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSentinelHaspUtilityView diagnostics

#ifdef _DEBUG
void CSentinelHaspUtilityView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSentinelHaspUtilityView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSentinelHaspUtilityDoc* CSentinelHaspUtilityView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSentinelHaspUtilityDoc)));
	return (CSentinelHaspUtilityDoc*)m_pDocument;
}
#endif //_DEBUG


// CSentinelHaspUtilityView message handlers


void CSentinelHaspUtilityView::haspLogIn(void)
{
	
	CString temp;
	char msg[32];

	if(hasp_key_login(msg)){
		temp.Format(" Hasp Key LogIn : %s",msg);
	}else{
		temp.Format(" Hasp Key LogIn Fail :  %s",msg);
	}

	showMsgToStatusInfo(temp);

}

void CSentinelHaspUtilityView::haspLogOut(void)
{
	
	CString temp;
	char msg[32];

	if(hasp_key_logout(msg)){
		temp.Format(" Hasp Key Log Out : %s",msg);
	}else{
		temp.Format(" Hasp Key Log Out Fail : %s",msg);
	}

	showMsgToStatusInfo(temp);

}

void CSentinelHaspUtilityView::haspKeyRealTime(void){

	CString temp;
	char msg[32];

	SYSTEMTIME systemTime;

	if(get_hasp_key_real_time(msg,systemTime)){

		temp.Format(" Hasp Key Real Time : %d/%02d/%02d/%02d:%02d:%02d ,day of week : %d",
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wDayOfWeek
		);

	}else{
		temp=" Read Hasp Key Real Time Fail : ";
		temp+=msg;
	}

	showMsgToStatusInfo(temp);
	
}

void CSentinelHaspUtilityView::haspReadKeyData(void){

	CString temp;
	CString data;
	char msg[32];
	char uuid[80];
	char lib_apc_key[32];
	SYSTEMTIME expireDate;
	BOOL checkFlag;
	int videoSum;
	char aes_key[17];

	if(hasp_key_read_data(msg,uuid,lib_apc_key,expireDate, checkFlag,videoSum,aes_key)){

		temp.Format(" Read Hasp Key Data : %s",msg);

		UpdateData(TRUE);

		m_EditContent="";

		data.Format(" UUID : %s \r\n",uuid);
		m_EditContent+=data;

		data.Format(" LIB APC KEY : %s \r\n",lib_apc_key);
		m_EditContent+=data;

		data.Format(" Expire Date : %d,%d,%d \r\n",
			expireDate.wYear,expireDate.wMonth,expireDate.wDay);
		m_EditContent+=data;


		if(checkFlag){
			data=" Check OK \r\n";
		}else{
			data=" Check False \r\n";
		}
		m_EditContent+=data;

		data.Format(" Video Sum : %d\r\n",videoSum);
		m_EditContent+=data;

		data.Format(" AES KEY : %s\r\n",aes_key);
		m_EditContent+=data;

		UpdateData(FALSE);

	}else{
		temp.Format(" Read Hasp Key Data Fail : %s",msg);
	}

	showMsgToStatusInfo(temp);
}


void CSentinelHaspUtilityView::showMsgToStatusInfo(CString msg){

	CString temp;

	CTime tt = CTime::GetCurrentTime();
	temp += tt.Format("[%Y-%B-%d %A, %H:%M:%S]  ") ;

	UpdateData(TRUE);

	temp+=msg;
	temp+="\r\n";
	m_StatusInfo=temp+m_StatusInfo;

	UpdateData(FALSE);

}

void CSentinelHaspUtilityView::checkHaspKey(void){

	char msg[32];
	CString temp;
	if(!hasp_key_check(msg)){	
		temp.Format(" Check Hasp Key Error :  %s",msg);
		MessageBox(temp);
	}else{
		temp.Format(" Check Hasp Key  :  %s",msg);
	}
	showMsgToStatusInfo(temp);

}

void CSentinelHaspUtilityView::writeHaspKeyCheckFlag(BOOL checkFlag){

	char msg[32];
	CString temp;

	hasp_key_write_check_flag(msg,checkFlag);

	temp.Format("Write Check Flag to USB , result = %s",msg);
	showMsgToStatusInfo(temp);

}

void CSentinelHaspUtilityView::readHaspKeyCheckFlag(){

	char msg[32];
	CString temp;
	BOOL checkFlag;

	if(hasp_key_read_check_flag(msg,checkFlag)){

		if(checkFlag){
			showMsgToStatusInfo("Check Flag  in USB = TRUE");
		}else{
			showMsgToStatusInfo("Check Flag  in USB = FALSE");
		}

	}else{

		temp.Format("Fail to read check flag in USB , reason = %s",msg);
		showMsgToStatusInfo(temp);

	}

}

void CSentinelHaspUtilityView::writeHaspKeyVideoSum(int videoSum){

	char msg[32];
	CString temp;

	hasp_key_write_video_sum(msg,videoSum);

	temp.Format("Write Video Sum to USB , result = %s",msg);
	showMsgToStatusInfo(temp);

}
void CSentinelHaspUtilityView::readHaspKeyVideoSum(){

	char msg[32];
	CString temp;
	int videoSum=-1;

	if(hasp_key_read_video_sum(msg,videoSum)){

		temp.Format("Read Video Sum from USB = %d",videoSum);
		showMsgToStatusInfo(temp);

	}else{

		temp.Format("Fail to read Video Sum in USB , reason = %s",msg);
		showMsgToStatusInfo(temp);
	}
}