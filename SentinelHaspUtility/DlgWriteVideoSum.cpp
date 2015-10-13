// DlgWriteVideoSum.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgWriteVideoSum.h"
#include "afxdialogex.h"


// CDlgWriteVideoSum dialog

IMPLEMENT_DYNAMIC(CDlgWriteVideoSum, CDialogEx)

CDlgWriteVideoSum::CDlgWriteVideoSum(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWriteVideoSum::IDD, pParent)
	, m_VideoSum(0)
{

}

CDlgWriteVideoSum::~CDlgWriteVideoSum()
{
}

void CDlgWriteVideoSum::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VIDEO_SUM_AT_DIALOG, m_VideoSum);
}


BEGIN_MESSAGE_MAP(CDlgWriteVideoSum, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgWriteVideoSum::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgWriteVideoSum message handlers


void CDlgWriteVideoSum::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	char msg[32];

	if(!hasp_key_write_video_sum(msg,m_VideoSum)){

		CString temp;
		temp.Format("Hasp Key write video sum failure : %s",msg);
		MessageBox(temp);

	}else{
		MessageBox("Write video sum to key success .");
	}

	UpdateData(FALSE);

	CDialogEx::OnOK();
}
