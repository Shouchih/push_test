// DlgEncyptMotionFile.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgEncyptMotionFile.h"
#include "afxdialogex.h"
#include "SentinelHaspApiLib\haspUtil.h"
#include "DlgSaveConfigParam.h"

// CDlgEncyptMotionFile dialog

IMPLEMENT_DYNAMIC(CDlgEncyptMotionFile, CDialogEx)

CDlgEncyptMotionFile::CDlgEncyptMotionFile(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEncyptMotionFile::IDD, pParent)
	, m_SysUUID(_T(""))
	, m_MotionFilePath(_T(""))
	, m_EncryptMotionFilePath(_T(""))
	, m_FolderPath(_T(""))
	, m_MotionFileName(_T(""))
	, m_ExpireDate(COleDateTime::GetCurrentTime())
{

}

CDlgEncyptMotionFile::~CDlgEncyptMotionFile()
{
}

void CDlgEncyptMotionFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SYS_UUID, m_SysUUID);
	DDX_Text(pDX, IDC_EDIT1, m_MotionFilePath);
	DDX_Text(pDX, IDC_EDIT2, m_EncryptMotionFilePath);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_EXPIRE_DATE, m_ExpireDate);
}


BEGIN_MESSAGE_MAP(CDlgEncyptMotionFile, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROWSE_MOTION_FILE, &CDlgEncyptMotionFile::OnBnClickedBtnBrowseMotionFile)
	ON_BN_CLICKED(IDC_BTN_START_ENCRYPT_MOTION_FILE, &CDlgEncyptMotionFile::OnBnClickedBtnStartEncryptMotionFile)
	ON_BN_CLICKED(IDC_BTN_SET_TIME_NOW, &CDlgEncyptMotionFile::OnBnClickedBtnSetTimeNow)
	ON_BN_CLICKED(IDC_BTN_RE_NEW_UUID, &CDlgEncyptMotionFile::OnBnClickedBtnReNewUuid)
END_MESSAGE_MAP()


// CDlgEncyptMotionFile message handlers


BOOL CDlgEncyptMotionFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	m_pSentinelHaspUtilityDoc=(CSentinelHaspUtilityDoc*)pFrame->GetActiveDocument();

	m_SysUUID=m_pSentinelHaspUtilityDoc->m_ConfigParam.UUID;

	m_ExpireDate.SetDate(
		m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate.year,
		m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate.month,
		m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate.day);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgEncyptMotionFile::OnBnClickedBtnBrowseMotionFile()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter=LPCTSTR(_T("Motion Files(*.motion)\0*.motion\0"));
	//定義打開對話框的標題
	fileDlg.m_ofn.lpstrTitle=LPCTSTR(_T("Browse a motion file"));

	if (fileDlg.DoModal()==IDOK)
	{
		m_MotionFilePath=fileDlg.GetPathName();
		m_FolderPath=fileDlg.GetFolderPath();
		m_MotionFileName=fileDlg.GetFileName();

		CString temp;
		int index=m_MotionFileName.Find(".motion");
		temp=m_MotionFileName.GetBufferSetLength(index);

		m_EncryptMotionFilePath.Format("%s\\%s_encrypt.motion",m_FolderPath,temp);
	}


	fileDlg.DestroyWindow();

	UpdateData(FALSE);

}


bool CDlgEncyptMotionFile::chcekAllFilePath(){

	UpdateData(TRUE);

	if(m_MotionFilePath.IsEmpty()){
		MessageBox("Motion File Path not set !");
		UpdateData(FALSE);
		return false;
	}

	if(m_EncryptMotionFilePath.IsEmpty()){
		MessageBox("Encrypt Motion File Path not set !");
		UpdateData(FALSE);
		return false;
	}

	if(m_EncryptMotionFilePath.Find(".motion")==-1){
		MessageBox("Encrypt Motion File Name must end with .motion !");
		UpdateData(FALSE);
		return false;
	}

	if(m_MotionFilePath.Compare(m_EncryptMotionFilePath)==0){
		MessageBox("Encrypt Motion File Name  must use defferent Name !");
		UpdateData(FALSE);
		return false;
	}
		
	UpdateData(TRUE);

	return true;
}

void CDlgEncyptMotionFile::OnBnClickedBtnStartEncryptMotionFile()
{
	// TODO: Add your control notification handler code here
	if(chcekAllFilePath()==false){
		return;
	}

	CWaitCursor wait;

	m_FileVersion=0;
	int result;
	char msg[256];
	result=encryptMotionFileHeader(m_MotionFilePath,m_EncryptMotionFilePath,msg);

	if(result==0 && m_FileVersion==3){
		result=encryptCueFileHeader(m_MotionFilePath,m_EncryptMotionFilePath,msg);
	}

	wait.Restore();
	MessageBox(msg);

	updateConfigParam();
}

int CDlgEncyptMotionFile::encryptMotionFileHeader(CString sourceFileName,CString encryptFileName,char *msg)
{
	CFile fb;
	TRY{

		CFileException fileException;
		BOOL result=FALSE;
		TCHAR szError[1024];
		
		result=fb.Open(sourceFileName,CFile::modeRead | CFile::typeBinary,&fileException);
		if (!result) {
			CString temp;
			fileException.GetErrorMessage(szError, 1024); 
			temp.Format("Error opening file : %s\0",szError);
			stringCopy(msg,temp.GetBuffer(),256);
			return 0;
		}

		short motionValue;
		fb.Read (&m_MotionFileHeader , sizeof(bgt_motion_file_header) );

		CString magic;
		magic.Format("%.10s",m_MotionFileHeader.magic);
		if(magic.Compare("@Brogent00")!=0){
			fb.Close();
			stringCopy(msg,"Error magic word of Motion Header.\0 ",256);
			return 0;
		}

		m_FileVersion=m_MotionFileHeader.file_ver;

		if(m_FileVersion>2){
			fb.Close();
			return 0;
		}

		int size= m_MotionFileHeader.motion_num;
	
		unsigned char *buffer;
		int bufferSize=sizeof(bgt_motion_file_header) +sizeof(short)*size*8; // 8 short = 6 short + 4 byte
		buffer = (unsigned char*)malloc(bufferSize); 
		memcpy(buffer, &m_MotionFileHeader, sizeof(bgt_motion_file_header));

		int counter=sizeof(bgt_motion_file_header);
		while (  fb.Read  (&motionValue , sizeof(short) )!=0){
			memcpy(&buffer[counter], &motionValue, sizeof(short));
			counter+=2;
		}
		fb.Close();

		// ---------  檢查原本的 SHA1 驗證碼  -----------------------------------------
		int sha1_result=0;
		sha1_result= bgt_motion_file_check((struct bgt_motion_file_header *) buffer,counter);
	
		if(sha1_result!=0){

			if(sha1_result==1){
				stringCopy(msg,"Checksum error when excute bgt_motion_file_check().\0 ",256);
			}

			if(sha1_result==2){
				stringCopy(msg,"File format error when excute bgt_motion_file_check().\0 ",256);
			}
			delete [] buffer;
			return 0;
		} 

		// --------- Write Sys UUID -------------------------------------------------
		memcpy(m_MotionFileHeader.sysUUID,m_SysUUID.GetBuffer(),36);
		m_MotionFileHeader.sysUUID[36]='\0';

		// ---------- Write Expire Date ----------------------------------------------------
		CString strDateTime;
		strDateTime.Format("%4d-%.2d-%.2d",m_ExpireDate.GetYear(),m_ExpireDate.GetMonth(),m_ExpireDate.GetDay());
		stringCopy( m_MotionFileHeader.expireDate,strDateTime.GetBuffer(),sizeof(m_MotionFileHeader.expireDate));
		m_MotionFileHeader.expireDate[10]='\0';
		//-----------------------------------------------------------------------------------------------------------------------------
		memcpy(buffer, &m_MotionFileHeader, sizeof(bgt_motion_file_header));
	

		// ---------  因為 Header 的內容已經變更 , 需重新產生 SHA1驗證碼  -----------------------------------------
	    sha1_result=0;
		sha1_result= bgt_motion_file_hash((struct bgt_motion_file_header *) buffer,counter);
	
		if(sha1_result!=0){

			if(sha1_result==1){
				stringCopy(msg,"Checksum error when excute bgt_motion_file_hash().\0 ",256);
			}

			if(sha1_result==2){
				stringCopy(msg,"File format error when excute bgt_motion_file_hash().\0 ",256);
			}
			delete [] buffer;
			return 0;
		} 

		//  --  更新新的 SHA1 驗證碼-----------------------
		memcpy(&m_MotionFileHeader.sha1, &buffer[10], sizeof(char)*20);

		//  -------- encrypt  data by hasp key  ---------------------------------------------------
		if(!hasp_key_encrypt_data(msg,buffer,bufferSize)){
			delete [] buffer;
			return 0;
		}

		// ---------  save Buffer to File  -------------------------------------------------------------------
		if(saveMotionBufferToFile( encryptFileName.GetBuffer(),buffer,bufferSize,msg)){
			delete [] buffer;
			return 1;
		}else{
			delete [] buffer;
			return 0;
		}


	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At encryptMotionFileHeader ,exception = \"%s\".\0"),szCause);
		fb.Close();
		stringCopy(msg,sMessage.GetBuffer(),256);
		//e->ReportError();
		return 0;
	}
	END_CATCH

	

	return 1;

}



int CDlgEncyptMotionFile::encryptCueFileHeader(CString sourceFileName,CString encryptFileName,char *msg)
{
	CFile fb;
	TRY{

		CFileException fileException;
		BOOL result=FALSE;
		TCHAR szError[1024];
		
		result=fb.Open(sourceFileName,CFile::modeRead | CFile::typeBinary,&fileException);
		if (!result) {
			fileException.GetErrorMessage(szError, 1024); 
			CString temp;
			temp.Format("Error opening file : %s\0",szError);
			stringCopy(msg,temp.GetBuffer(),256);
			return 0;
		}
		
		fb.Read (&m_CueFileHeader , sizeof(bgt_cue_file_header) );

		CString magic;
		magic.Format("%.10s",m_CueFileHeader.magic);
		if(magic.Compare("@Brogent00")!=0){
			fb.Close();
			stringCopy(msg,"Error magic word of Motion Header. \0",256);
			return 0;
		}

		m_FileVersion=m_CueFileHeader.header_ver;

		if(m_FileVersion!=3){
			fb.Close();
			stringCopy(msg,"Error BGT Cue File Version .\0",256);
			return 0;
		}	

		int size= m_CueFileHeader.frame_num;
		int effectTypeNumber=m_CueFileHeader.effect_channel_num;

		unsigned char *buffer;
		int bufferSize=sizeof(bgt_cue_file_header) +size*(12+effectTypeNumber);
		buffer = (unsigned char*)malloc(bufferSize); 
		memcpy(buffer, &m_CueFileHeader, sizeof(bgt_cue_file_header));

		int counter=sizeof(bgt_cue_file_header);
		byte byteValue;
		while (  fb.Read  (&byteValue ,1 )!=0){
			memcpy(&buffer[counter], &byteValue, 1);
			counter+=1;
		}
		fb.Close();

		// ---------  檢查原本的 SHA1 驗證碼  -----------------------------------------
		int sha1_result=0;
		sha1_result= bgt_cue_file_check((bgt_cue_file_header *) buffer,12+effectTypeNumber);
	
		if(sha1_result!=0){

			if(sha1_result==1){		
				stringCopy(msg,"Checksum error when excute bgt_cue_file_check().\0",256);
			}

			if(sha1_result==2){
			   stringCopy(msg,"File format error when excute bgt_cue_file_check() .\0",256);
			}
			delete [] buffer;
			return 0;
		} 

		// --------- Write Sys UUID -------------------------------------------------
		memcpy(m_CueFileHeader.sysUUID,m_SysUUID.GetBuffer(),36);
		m_CueFileHeader.sysUUID[36]='\0';

		// ---------- Write Expire Date ----------------------------------------------------
		CString strDateTime;
		strDateTime.Format("%4d-%.2d-%.2d",m_ExpireDate.GetYear(),m_ExpireDate.GetMonth(),m_ExpireDate.GetDay());
		stringCopy( m_CueFileHeader.expireDate,strDateTime.GetBuffer(),sizeof(m_CueFileHeader.expireDate));
		m_MotionFileHeader.expireDate[10]='\0';
		//-----------------------------------------------------------------------------------------------------------------------------

		memcpy(buffer, &m_CueFileHeader, sizeof(bgt_cue_file_header));

	    // ---------  因為 Header 的內容已經變更 , 需重新產生 SHA1驗證碼  -----------------------------------------
		sha1_result=0;
		sha1_result= bgt_cue_file_hash((bgt_cue_file_header *) buffer,12+effectTypeNumber);
	
		if(sha1_result!=0){

			if(sha1_result==1){		
				stringCopy(msg,"Checksum error when excute bgt_cue_file_hash().\0",256);
			}

			if(sha1_result==2){
			   stringCopy(msg,"File format error when excute bgt_cue_file_hash() .\0",256);
			}
			delete [] buffer;
			return 0;
		} 


		//  -------- encrypt  data by hasp key  ---------------------------------------------------
		if(!hasp_key_encrypt_data(msg,buffer,bufferSize)){
			delete [] buffer;
			return 0;
		}

		// ---------  save Buffer to File  -------------------------------------------------------------------
		if(saveMotionBufferToFile( encryptFileName.GetBuffer(),buffer,bufferSize,msg)){
			delete [] buffer;
			return 1;
		}else{
			delete [] buffer;
			return 0;
		}
		

	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At encryptCueFileHeader ,exception = \"%s\"."),szCause);
		fb.Close();
		stringCopy(msg,sMessage.GetBuffer(),256);
		//e->ReportError();
	}
	END_CATCH

	return 0;

}

void CDlgEncyptMotionFile::updateConfigParam(){

	UpdateData(TRUE);

	HaspKeyTime expireDate;
	transferOleDateToHaspKeyTime (m_ExpireDate ,expireDate);

	expireDate.hour=23;
	expireDate.minute=59;
	expireDate.second=59;
    
	stringCopy((char *)m_pSentinelHaspUtilityDoc->m_ConfigParam.UUID,
	                      m_SysUUID.GetBuffer(),
						  sizeof(m_pSentinelHaspUtilityDoc->m_ConfigParam.UUID));

	m_pSentinelHaspUtilityDoc->m_ConfigParam.keyExpireDate=expireDate;

	
	CDlgSaveConfigParam dlgSaveConfigParam;

	if(dlgSaveConfigParam.DoModal()==IDOK){
		
	}

	UpdateData(FALSE);

}

void CDlgEncyptMotionFile::transferOleDateToHaspKeyTime (COleDateTime oleDateTime ,HaspKeyTime & haspkeyTime){

	haspkeyTime.year=oleDateTime.GetYear();
	haspkeyTime.month=oleDateTime.GetMonth();
	haspkeyTime.day=oleDateTime.GetDay();
	haspkeyTime.hour=oleDateTime.GetHour();
	haspkeyTime.minute=oleDateTime.GetMinute();
	haspkeyTime.second=oleDateTime.GetSecond();
}

bool CDlgEncyptMotionFile::saveMotionBufferToFile( char  *filePath,unsigned char *buffer,int bufferSize,char *msg){

		CFile fb; 
		TRY{			

			CFileException fileException;
			BOOL result=FALSE;
			TCHAR szError[1024];	

			result=fb.Open(filePath,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary,&fileException);
			if (!result) {
				fileException.GetErrorMessage(szError, 1024); 
				CString temp;
				temp.Format("Error opening file : %s",szError);
				stringCopy(msg,temp.GetBuffer(),256);
				return false;
			}

			fb.Write (buffer,bufferSize );

			fb.Close();

			stringCopy(msg,"Save complete.\0",256);
			return true;

		}CATCH(CException,e){
			CString sMessage=_T("");
			TCHAR   szCause[255];
			e->GetErrorMessage(  szCause,255,NULL);
			sMessage.Format(_T("At saveMotionBufferToFile ,exception = \"%s\".\0"),szCause);
			fb.Close();
			//e->ReportError();
			stringCopy(msg,sMessage.GetBuffer(),256);

			return false;
		}
		END_CATCH

	}


void CDlgEncyptMotionFile::OnBnClickedBtnSetTimeNow()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_ExpireDate=COleDateTime::GetCurrentTime();

	UpdateData(FALSE);
}


void CDlgEncyptMotionFile::OnBnClickedBtnReNewUuid()
{
	// TODO: Add your control notification handler code here
	char msg[32];
	char uuid[37];

	if(getSysUUID(msg,uuid)==TRUE){
		
		UpdateData(TRUE);

		m_SysUUID=uuid;

		UpdateData(FALSE);

		MessageBox("Re new UUID SUCCESS.");

	}else{

		CString temp;
		temp.Format("Fail to get UUID from PC main board , error =%s",msg);

		MessageBox(temp);
	}
}
