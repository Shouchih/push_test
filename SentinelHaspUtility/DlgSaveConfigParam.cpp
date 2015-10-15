// DlgSaveConfigParam.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgSaveConfigParam.h"
#include "afxdialogex.h"


// CDlgSaveConfigParam dialog

IMPLEMENT_DYNAMIC(CDlgSaveConfigParam, CDialogEx)

CDlgSaveConfigParam::CDlgSaveConfigParam(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSaveConfigParam::IDD, pParent)
	, m_ConfigName(_T(""))
	, m_ConfigFileName(_T(""))
{
	
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	m_pSentinelHaspUtilityDoc=(CSentinelHaspUtilityDoc*)pFrame->GetActiveDocument();

}

CDlgSaveConfigParam::~CDlgSaveConfigParam()
{
}

void CDlgSaveConfigParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ConfigName);
	DDX_Text(pDX, IDC_EDIT2, m_ConfigFileName);
}


BEGIN_MESSAGE_MAP(CDlgSaveConfigParam, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSaveConfigParam::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSaveConfigParam message handlers


BOOL CDlgSaveConfigParam::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(TRUE);

	m_ConfigName=m_pSentinelHaspUtilityDoc->m_ConfigParam.name;
	m_ConfigFileName=m_pSentinelHaspUtilityDoc->m_ConfigFileName;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSaveConfigParam::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_pSentinelHaspUtilityDoc->saveConfigParam(m_ConfigName,m_ConfigFileName);

	UpdateData(FALSE);

	CDialogEx::OnOK();
}
