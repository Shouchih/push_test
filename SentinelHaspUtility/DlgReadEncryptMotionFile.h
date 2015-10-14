#pragma once
#include "afxcmn.h"

#include "polarssl\sha1.h"
#include "SentinelHaspApiLib\bgt_motionfile.h"
#include <vector>

using namespace std;
// CDlgReadEncryptMotionFile dialog

struct signal
{
  short voltage[6];
};

const CString m_DeviceName[]={"4D","I-Ride","Side","3-Cyld","4-Cyld","EDA-4D"};

class CDlgReadEncryptMotionFile : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgReadEncryptMotionFile)

public:
	CDlgReadEncryptMotionFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReadEncryptMotionFile();

// Dialog Data
	enum { IDD = IDD_DLG_READ_ENCRYPT_MOTION_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_EncryptMotionFilePath;
	CString m_SysUUID;
	CListCtrl m_ListCtrlHeaderInfo;
	CListCtrl m_CylinderDataListBox;
	virtual BOOL OnInitDialog();
	CListCtrl m_EffectDataListBox;
	afx_msg void OnBnClickedBtnReadEncryptMotionFile();

	bool browseMotionFile();
	struct bgt_motion_file_header m_MotionFileHeader;
	struct bgt_cue_file_header m_CueFileHeader;
	
	BOOL openDecryptMotionFile(CString fileName);
	int m_FileVersion;
	unsigned char *m_Buffer ;
	int m_BufferSize;
	BOOL verifyMotionHeader(unsigned char *buffer,int bufferSize);
	BOOL verifyMotionCueHeader(unsigned char *buffer,int bufferSize);

	//----------------------------------------------------------------------------------------
	vector<struct signal> m_CylinderDataList;
	vector<int> *m_EffectDataList;
	int m_EffectNumber;
	BOOL fetchMotionData(unsigned char *buffer,int bufferSize,vector<struct signal> &motionDataList,vector<int> *effectDataList);
	BOOL fetchCueFileData(unsigned char *buffer,int bufferSize,vector<struct signal> &motionDataList,vector<int> *effectDataList);

	//-------------------------------------------------------------------------------------
	int getColumnCount(const CListCtrl *pListCtrl);
	void showMotionHeaderInfo(void);
	void showCueHeaderInfo(void);

	void showMotionData(vector<struct signal> &motionDataList);
	void showEffectData(vector<int> *effectDataList,int effectTypeNumber);

};
