#pragma once
#include "atlcomtime.h"
#include "SentinelHaspApiLib\haspUtil.h"

// CDlgWriteKeyInfo dialog

class CDlgWriteKeyInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWriteKeyInfo)

public:
	CDlgWriteKeyInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWriteKeyInfo();

// Dialog Data
	enum { IDD = IDD_DLG_WRITE_KEY_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedOk();
	CString m_UUID;
	CString m_lib_apc_key;
	COleDateTime m_ExpireDate;
	void transferOleDateToHaspKeyTime (COleDateTime oleDateTime ,HaspKeyTime & haspkeyTime);
	BOOL m_CheckOK;
	int m_VideoSum;
	CString m_AESKey;
	afx_msg void OnBnClickedBtnSetTimeNow();
};
