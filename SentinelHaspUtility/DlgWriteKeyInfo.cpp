// DlgWriteKeyInfo.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgWriteKeyInfo.h"
#include "afxdialogex.h"


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
END_MESSAGE_MAP()


// CDlgWriteKeyInfo message handlers


BOOL CDlgWriteKeyInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(TRUE);
	char msg[32];
	char uuid[37];

	if(getSysUUID(msg,uuid)==TRUE){
		m_UUID=uuid;
	}else{
		m_UUID=msg;
	}

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

	wait.Restore();

	CDialogEx::OnOK();
}

void CDlgWriteKeyInfo::transferOleDateToHaspKeyTime (COleDateTime oleDateTime ,HaspKeyTime & haspkeyTime){

	haspkeyTime.year=oleDateTime.GetYear();
	haspkeyTime.month=oleDateTime.GetMonth();
	haspkeyTime.day=oleDateTime.GetDay();
	haspkeyTime.hour=oleDateTime.GetHour();
	haspkeyTime.minute=oleDateTime.GetMinute();
	haspkeyTime.second=oleDateTime.GetSecond();
}