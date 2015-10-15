#pragma once


// CDlgSaveConfigParam dialog
#include "SentinelHaspUtilityDoc.h"


class CDlgSaveConfigParam : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSaveConfigParam)

public:
	CDlgSaveConfigParam(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSaveConfigParam();

// Dialog Data
	enum { IDD = IDD_DLG_SAVE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSentinelHaspUtilityDoc *m_pSentinelHaspUtilityDoc;
	CString m_ConfigName;
	CString m_ConfigFileName;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
