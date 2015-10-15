#pragma once

#include "SentinelHaspApiLib\haspUtil.h"
#include "SentinelHaspUtilityDoc.h"
// CDlgWriteVideoSum dialog

class CDlgWriteVideoSum : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWriteVideoSum)

public:
	CDlgWriteVideoSum(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWriteVideoSum();

// Dialog Data
	enum { IDD = IDD_DLG_WRITE_VIDEO_SUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_VideoSum;
	afx_msg void OnBnClickedOk();

	CSentinelHaspUtilityDoc *m_pSentinelHaspUtilityDoc;
	void updateConfigParam();
	virtual BOOL OnInitDialog();
};
