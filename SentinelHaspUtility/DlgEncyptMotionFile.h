#pragma once


#include "polarssl\sha1.h"
#include "SentinelHaspApiLib\bgt_motionfile.h"
#include "atlcomtime.h"

// CDlgEncyptMotionFile dialog

class CDlgEncyptMotionFile : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEncyptMotionFile)

public:
	CDlgEncyptMotionFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEncyptMotionFile();

// Dialog Data
	enum { IDD = IDD_DLG_ENCRYPT_MOTION_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_SysUUID;
	CString m_MotionFilePath;
	CString m_EncryptMotionFilePath;
	CString m_FolderPath;
	CString m_MotionFileName;
	afx_msg void OnBnClickedBtnBrowseMotionFile();
	afx_msg void OnBnClickedBtnStartEncryptMotionFile();

	bool chcekAllFilePath();
	int encryptMotionFileHeader(CString sourceFileName,CString encryptFileName,char *msg);
	int encryptCueFileHeader(CString sourceFileName,CString encryptFileName,char *msg);
	struct bgt_motion_file_header m_MotionFileHeader;
	struct bgt_cue_file_header m_CueFileHeader;
	int m_FileVersion;

	bool saveMotionBufferToFile( char  *filePath,unsigned char *buffer,int bufferSize,char *msg);
	COleDateTime m_ExpireDate;
	afx_msg void OnBnClickedBtnSetTimeNow();
};
