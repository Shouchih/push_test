// DlgWriteKeyInfo.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgWriteKeyInfo.h"
#include "afxdialogex.h"
#include "DlgSaveConfigParam.h"

// CDlgWriteKeyInfo dialog

IMPLEMENT_DYNAMIC(CDlgWriteKeyInfo, CDialogEx)

CDlgWriteKeyInfo::CDlgWriteKeyInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWriteKeyInfo::IDD, pParent)
	, m_UUID(_T(""))
	, m_lib_apc_key(_T(""))
	, m_ExpireDate(COleDateTime::GetCurrentTime())
	, m_CheckOK(FALSE)
	, m_VideoSum(0)
	, m_AESKey(_T(""))
{

}

CDlgWriteKeyInfo::~CDlgWriteKeyInfo()
{
}

void CDlgWriteKeyInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GUID, m_UUID);
	DDX_Text(pDX, IDC_EDIT_LIB_APC_KEY, m_lib_apc_key);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_ExpireDate);
	DDX_Radio(pDX, IDC_RADIO_CHECK_FALSE, m_CheckOK);
	DDX_Text(pDX, IDC_EDIT_VIDEO_SUM, m_VideoSum);
	DDX_Text(pDX, IDC_EDIT_AES_KEY, m_AESKey);
}


BEGIN_MESSAGE_MAP(CDlgWriteKeyInfo, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgWriteKeyInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SET_TIME_NOW, &CDlgWriteKeyInfo::OnBnClickedBtnSetTimeNow)
	ON_BN_CLICKED(IDC_BTN_RE_NEW_UUID, &CDlgWriteKeyInfo::OnBnClickedBtnReNewUuid)
END_MESSAGE_MAP()


// CDlgWriteKeyInfo message handlers


BOOL CDlgWriteKeyInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	m_pSentinelHaspUtilityDoc=(CSentinelHaspUtilityDoc*)pFrame->GetActiveDocument();

	UpdateData(TRUE);

	m_UUID=m_pSentinelHaspUtilityDoc->m_ConfigParam.UUID;

	m_ExpireDate.SetDate(
		m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate.year,
		m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate.month,
		m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate.day);

	m_CheckOK=m_pSentinelHaspUtilityDoc->m_ConfigParam.checkFlag;
	m_lib_apc_key=m_pSentinelHaspUtilityDoc->m_ConfigParam.LIB_APC_KEY;
	m_AESKey=m_pSentinelHaspUtilityDoc->m_ConfigParam.AES_KEY;
	m_VideoSum=m_pSentinelHaspUtilityDoc->m_ConfigParam.videoSum;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgWriteKeyInfo::OnBnClickedOk()
{
	CWaitCursor wait;
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	HaspKeyTime expireDate;

	transferOleDateToHaspKeyTime (m_ExpireDate ,expireDate);

	expireDate.hour=23;
	expireDate.minute=59;
	expireDate.second=59;

	char msg[32];

	if(!hasp_key_write_data(
		msg,m_UUID.GetBuffer(),
		m_lib_apc_key.GetBuffer(),
		expireDate,
		m_CheckOK,
		m_VideoSum,
		m_AESKey.GetBufferSetLength(16))){

		CString temp;
		temp.Format("Hasp Key write data failure : %s",msg);
		MessageBox(temp);

	}else{
		MessageBox("Write data to key success .");
	}

	UpdateData(FALSE);

	updateConfigParam();

	wait.Restore();

	CDialogEx::OnOK();
}

void CDlgWriteKeyInfo::updateConfigParam(){

	UpdateData(TRUE);

	HaspKeyTime expireDate;
	transferOleDateToHaspKeyTime (m_ExpireDate ,expireDate);

	expireDate.hour=23;
	expireDate.minute=59;
	expireDate.second=59;
    
	stringCopy((char *)m_pSentinelHaspUtilityDoc->m_ConfigParam.UUID,
	                      m_UUID.GetBuffer(),
						  sizeof(m_pSentinelHaspUtilityDoc->m_ConfigParam.UUID));

	m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate=expireDate;

	stringCopy((char *)m_pSentinelHaspUtilityDoc->m_ConfigParam.LIB_APC_KEY,
	                      m_lib_apc_key.GetBuffer(),
						  sizeof(m_pSentinelHaspUtilityDoc->m_ConfigParam.LIB_APC_KEY));

	stringCopy((char *)m_pSentinelHaspUtilityDoc->m_ConfigParam.AES_KEY,
	                      m_AESKey.GetBuffer(),
						  sizeof(m_pSentinelHaspUtilityDoc->m_ConfigParam.AES_KEY));

	m_pSentinelHaspUtilityDoc->m_ConfigParam.checkFlag=m_CheckOK;

	m_pSentinelHaspUtilityDoc->m_ConfigParam.videoSum=m_VideoSum;

	
	CDlgSaveConfigParam dlgSaveConfigParam;

	if(dlgSaveConfigParam.DoModal()==IDOK){
		
	}

	UpdateData(FALSE);

}

void CDlgWriteKeyInfo::transferOleDateToHaspKeyTime (COleDateTime oleDateTime ,HaspKeyTime & haspkeyTime){

	haspkeyTime.year=oleDateTime.GetYear();
	haspkeyTime.month=oleDateTime.GetMonth();
	haspkeyTime.day=oleDateTime.GetDay();
	haspkeyTime.hour=oleDateTime.GetHour();
	haspkeyTime.minute=oleDateTime.GetMinute();
	haspkeyTime.second=oleDateTime.GetSecond();
}

void CDlgWriteKeyInfo::OnBnClickedBtnSetTimeNow()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_ExpireDate=COleDateTime::GetCurrentTime();

	UpdateData(FALSE);
}


void CDlgWriteKeyInfo::OnBnClickedBtnReNewUuid()
{
	// TODO: Add your control notification handler code here
	char msg[32];
	char uuid[37];

	if(getSysUUID(msg,uuid)==TRUE){
		
		UpdateData(TRUE);

		m_UUID=uuid;

		UpdateData(FALSE);

		MessageBox("Re new UUID SUCCESS.");

	}else{

		CString temp;
		temp.Format("Fail to get UUID from PC main board , error =%s",msg);

		MessageBox(temp);
	}

}
