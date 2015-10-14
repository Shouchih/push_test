// DlgReadEncryptMotionFile.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgReadEncryptMotionFile.h"
#include "afxdialogex.h"
#include "SentinelHaspApiLib\haspUtil.h"

// CDlgReadEncryptMotionFile dialog

IMPLEMENT_DYNAMIC(CDlgReadEncryptMotionFile, CDialogEx)

CDlgReadEncryptMotionFile::CDlgReadEncryptMotionFile(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgReadEncryptMotionFile::IDD, pParent)
	, m_EncryptMotionFilePath(_T(""))
	, m_SysUUID(_T(""))
{

}

CDlgReadEncryptMotionFile::~CDlgReadEncryptMotionFile()
{
}

void CDlgReadEncryptMotionFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ENCRYPT_MOTION_FILE_PATH, m_EncryptMotionFilePath);
	DDX_Text(pDX, IDC_EDIT_SYS_UUID, m_SysUUID);
	DDX_Control(pDX, IDC_LIST_MOTION_HEADER, m_ListCtrlHeaderInfo);
	DDX_Control(pDX, IDC_LIST_MOTION_DATA, m_CylinderDataListBox);
	DDX_Control(pDX, IDC_LIST_EFFECT_DATA, m_EffectDataListBox);
}


BEGIN_MESSAGE_MAP(CDlgReadEncryptMotionFile, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_READ_ENCRYPT_MOTION_FILE, &CDlgReadEncryptMotionFile::OnBnClickedBtnReadEncryptMotionFile)
END_MESSAGE_MAP()


// CDlgReadEncryptMotionFile message handlers


BOOL CDlgReadEncryptMotionFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	char msg[32];
	char uuid[37];

	UpdateData(TRUE);

	if(getSysUUID(msg,uuid)==TRUE){
		m_SysUUID=uuid;
	}else{
		m_SysUUID=msg;
	}

	m_CylinderDataListBox.SetExtendedStyle( m_CylinderDataListBox.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_TRACKSELECT );

	m_EffectDataListBox.SetExtendedStyle( m_EffectDataListBox.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_TRACKSELECT );

	m_ListCtrlHeaderInfo.SetExtendedStyle( m_ListCtrlHeaderInfo.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_TRACKSELECT );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgReadEncryptMotionFile::OnBnClickedBtnReadEncryptMotionFile()
{
	// TODO: Add your control notification handler code here
	if(browseMotionFile()==false){
		return;
	}
	CWaitCursor wait;

	m_FileVersion=0;

	if(openDecryptMotionFile(m_EncryptMotionFilePath)==FALSE){
		wait.Restore();
		return;
	}

	if(m_FileVersion==2){
		if(verifyMotionHeader(m_Buffer,m_BufferSize)==FALSE){
			wait.Restore();
			return;
		}
		memcpy(&m_MotionFileHeader,m_Buffer, sizeof(bgt_motion_file_header));
		m_EffectNumber=4;
		m_EffectDataList=(vector<int> *)calloc(4,sizeof(vector<int>));

		if(fetchMotionData(m_Buffer,m_BufferSize,m_CylinderDataList,m_EffectDataList)==FALSE){
			wait.Restore();
			return;
		}
		showMotionHeaderInfo();
	}

	if(m_FileVersion==3){
		if(verifyMotionCueHeader(m_Buffer,m_BufferSize)==FALSE){
			wait.Restore();
			return;
		}
		memcpy(&m_CueFileHeader,m_Buffer, sizeof(bgt_cue_file_header));
		m_EffectNumber=m_CueFileHeader.effect_channel_num;
		m_EffectDataList=(vector<int> *)calloc(m_EffectNumber,sizeof(vector<int>));

		if(fetchCueFileData(m_Buffer,m_BufferSize,m_CylinderDataList,m_EffectDataList)==FALSE){
			wait.Restore();
			return;
		}
		showCueHeaderInfo();
	}
	
	showMotionData(m_CylinderDataList);
	showEffectData(m_EffectDataList,m_EffectNumber);

	 wait.Restore();
}

//--------  讀取檔案內容 , 解碼 --------------------
BOOL CDlgReadEncryptMotionFile::openDecryptMotionFile(CString fileName){

	CFile fb;
	TRY{

		CFileException fileException;
		BOOL result=FALSE;

		result=fb.Open(fileName,CFile::modeRead | CFile::typeBinary,&fileException);
		if (!result) {
			CString msg;
			TCHAR szError[1024];
			fileException.GetErrorMessage(szError, 1024); 
			msg.Format("Error opening file : %s",szError);
			MessageBox (msg);
			fb.Close();
			return FALSE;
		}

		m_BufferSize=(int)fb.GetLength();

		if(m_BufferSize<sizeof(bgt_motion_file_header)){
			MessageBox ("Motion file too small .");
			fb.Close();
			return FALSE;
		}

		m_Buffer = (unsigned char*)malloc(m_BufferSize); 
		fb.Read  (m_Buffer , m_BufferSize);	
		fb.Close();

		char msg[32];
		//-------------------------  解碼  --------------------------------
		if(hasp_key_decrypt_data(msg,m_Buffer,m_BufferSize)==FALSE){
			CString temp;
			temp.Format("Decrypt data error : %s",msg);
			MessageBox (temp);
			fb.Close();
			return FALSE;
		}

		//  ------------  取得檔案版本 ------------------------------------------------
		memcpy(&m_MotionFileHeader,m_Buffer, sizeof(bgt_motion_file_header));
		m_FileVersion=m_MotionFileHeader.file_ver;

		//  ------------  檢查 UUID ------------------------------------------------
		if(checkUUID(msg,m_Buffer)==FALSE){
			CString temp;
			temp.Format("UUID verfy error : %s",msg);
			MessageBox (temp);
		}	
		//   -----------  檢查 Expire Date ------------------------------------------------
		if(checkMotionExpireDate(msg,m_Buffer)==FALSE){
			CString temp;
			temp.Format("Expire Date error : %s",msg);
			MessageBox (temp);
		}	

	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At CDlgShowOtherMotionFile::verifyMotionFileHeader ,exception = \"%s\"."),szCause);
		MessageBox(sMessage);
		fb.Close();
		return FALSE;
		//e->ReportError();
	}
	END_CATCH

	return TRUE;
}

int CDlgReadEncryptMotionFile::verifyMotionHeader(unsigned char *buffer,int bufferSize)
{
	//  ----------- 檢查 Header SHA1 驗證碼  -----------------------------
	int checkResult=0;
	checkResult= bgt_motion_file_check((bgt_motion_file_header *) buffer,bufferSize);

	if(checkResult!=0){

		if(checkResult==1){
			MessageBox("Checksum error when excute bgt_motion_file_check()");
		}

		if(checkResult==2){
			MessageBox("file format error when excute bgt_motion_file_check()");
		}
		return FALSE;
	} else{
		return TRUE;
	}

	return FALSE;

}

int CDlgReadEncryptMotionFile::verifyMotionCueHeader(unsigned char *buffer,int bufferSize)
{
	
	//  ----------- 檢查 Header SHA1 驗證碼  -----------------------------
	int checkResult=0;
	checkResult= bgt_motion_file_check((bgt_motion_file_header *) buffer,bufferSize);

	if(checkResult!=0){

		if(checkResult==1){
			MessageBox("Checksum error when excute bgt_motion_file_check()");
		}

		if(checkResult==2){
			MessageBox("file format error when excute bgt_motion_file_check()");
		}
		return FALSE;
	} else{
		return TRUE;
	}

	return FALSE;

}


bool CDlgReadEncryptMotionFile::browseMotionFile()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	bool flag=false;
	
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter=LPCTSTR(_T("Motion Files(*.motion)\0*.motion\0"));
	//定義打開對話框的標題
	fileDlg.m_ofn.lpstrTitle=LPCTSTR(_T("Browse a motion file"));

	if (fileDlg.DoModal()==IDOK)
	{
		m_EncryptMotionFilePath=fileDlg.GetPathName();
	
		if(m_EncryptMotionFilePath.IsEmpty()==false){
			flag=true;
		}	
	}


	fileDlg.DestroyWindow();

	UpdateData(FALSE);

	return flag;

}

BOOL CDlgReadEncryptMotionFile::fetchMotionData(unsigned char *buffer,int bufferSize,vector<struct signal> &motionDataList,vector<int> *effectDataList)
{
			
	TRY{
		motionDataList.clear();
		for(int i=0;i<m_EffectNumber;i++){
			effectDataList[i].clear();
		}

	   int counter=sizeof(bgt_motion_file_header);
	   int frameNumber=m_MotionFileHeader.motion_num;

	   for(int i=0;i<frameNumber;i++){

		   if(counter<(int)(bufferSize-sizeof(short) *6-sizeof(byte)*4)){

			   signal motionData;
			   memcpy(motionData.voltage,buffer+counter,sizeof(short) *6);
			   counter+=sizeof(short) *6;
			   motionDataList.push_back(motionData);

			   for(int i=0;i<m_EffectNumber;i++){
				   byte effect;
				   memcpy(&effect,buffer+counter,sizeof(byte));
				   effectDataList[i].push_back(effect);
				   counter++;
			   }
		   }
	   }
		
		return TRUE;
	}CATCH(CException,e){
			CString sMessage=_T("");
			TCHAR   szCause[255];
			e->GetErrorMessage(  szCause,255,NULL);
			sMessage.Format(_T("At fetchMotionData() ,exception = \"%s\"."),szCause);
			MessageBox(sMessage);
		
			return FALSE;
	}
	END_CATCH
}

BOOL CDlgReadEncryptMotionFile::fetchCueFileData(unsigned char *buffer,int bufferSize,vector<struct signal> &motionDataList,vector<int> *effectDataList)
{
			
	TRY{
		motionDataList.clear();
		for(int i=0;i<m_EffectNumber;i++){
			effectDataList[i].clear();
		}

	   int counter=sizeof(bgt_cue_file_header);
	   int frameNumber=m_CueFileHeader.frame_num;

	   int motionChannel=0;
	   if(m_CueFileHeader.motion_exist==1){
		   motionChannel=6;
	   }

	   for(int i=0;i<frameNumber;i++){

		   if(counter<(int)(bufferSize-sizeof(short) *motionChannel-sizeof(byte)*m_EffectNumber)){

			   if(m_CueFileHeader.motion_exist==1){

				   signal motionData;
				   memcpy(motionData.voltage,buffer+counter,sizeof(short) *motionChannel);
				   counter+=sizeof(short) *motionChannel;
				   motionDataList.push_back(motionData);
			   }

			   for(int i=0;i<m_EffectNumber;i++){
				   byte effect;
				   memcpy(&effect,buffer+counter,sizeof(byte));
				   effectDataList[i].push_back(effect);
				   counter++;
			   }
		   }
	   }
		
		return TRUE;
	}CATCH(CException,e){
			CString sMessage=_T("");
			TCHAR   szCause[255];
			e->GetErrorMessage(  szCause,255,NULL);
			sMessage.Format(_T("At fetchCueFileData() ,exception = \"%s\"."),szCause);
			MessageBox(sMessage);
		
			return FALSE;
	}
	END_CATCH
}

int CDlgReadEncryptMotionFile::getColumnCount(const CListCtrl *pListCtrl)
{
	CHeaderCtrl* pHeader;
	
	if (pListCtrl == NULL) return 0;
	if ((pHeader = const_cast<CListCtrl*>(pListCtrl)->GetHeaderCtrl()) ==
		NULL) return 0;
	return pHeader->GetItemCount();
}

void CDlgReadEncryptMotionFile::showMotionHeaderInfo(void)
{
	UpdateData(TRUE);
	m_ListCtrlHeaderInfo.DeleteAllItems();

	int columnCount = getColumnCount(&m_ListCtrlHeaderInfo);

	if(columnCount ==0){
		CRect rect;

		m_ListCtrlHeaderInfo.GetWindowRect(&rect);

		m_ListCtrlHeaderInfo.InsertColumn( 0, _T("Name"), LVCFMT_CENTER,120);

		m_ListCtrlHeaderInfo.InsertColumn( 1, _T("Value"), LVCFMT_LEFT,rect.Width()-120);
	}
  
   CString temp;
   CString effectName;
   int indexEffect;
   int nIndex=0;

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Update Date");
	temp.Format("%.16s",m_MotionFileHeader.updateDate);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Expire Date");
	temp.Format("%.10s",m_MotionFileHeader.expireDate);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"GUID");
	temp.Format("%.36s",m_MotionFileHeader.guid);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Project ID");
	temp.Format("%.32s",m_MotionFileHeader.project_id);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"magic");
	temp.Format("%.10s",m_MotionFileHeader.magic);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"SHA1");
	temp.Format("%.20s",m_MotionFileHeader.sha1);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"File version");
	temp.Format("%d",m_MotionFileHeader.file_ver);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Device type");
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,m_DeviceName[m_MotionFileHeader.type]);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Motion name");
	temp.Format("%.32s",m_MotionFileHeader.motion_name);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Motion version");
	temp.Format("%d",m_MotionFileHeader.motion_ver);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Motion number");
	temp.Format("%d",m_MotionFileHeader.motion_num);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"mps (FPS)");
	temp.Format("%d",m_MotionFileHeader.mps);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Video Name");
	temp.Format("%s",m_MotionFileHeader.video_name);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	// for Special Effect Data 
	for(int i=0;i<4;i++){

		temp.Format("Effect #%d",i);
		nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,temp);
		indexEffect=m_MotionFileHeader.effect[i];

		temp.Format("%d",indexEffect);
		m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	}

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"SYS UUID");
	temp.Format("%s",m_MotionFileHeader.sysUUID);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Reserved");
	temp.Format("%s",m_MotionFileHeader.reserved);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	UpdateData(FALSE);
}


void CDlgReadEncryptMotionFile::showCueHeaderInfo(void)
{
	UpdateData(TRUE);
	m_ListCtrlHeaderInfo.DeleteAllItems();

	int columnCount = getColumnCount(&m_ListCtrlHeaderInfo);

	if(columnCount ==0){
		CRect rect;

		m_ListCtrlHeaderInfo.GetWindowRect(&rect);

		m_ListCtrlHeaderInfo.InsertColumn( 0, _T("Name"), LVCFMT_CENTER,120);

		m_ListCtrlHeaderInfo.InsertColumn( 1, _T("Value"), LVCFMT_LEFT,rect.Width()-120);
	}
  
   CString temp;
   int nIndex=0;

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Update Date");
	temp.Format("%.16s",m_CueFileHeader.updateDate);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Expire Date");
	temp.Format("%.10s",m_CueFileHeader.expireDate);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);
	
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"GUID");
	temp.Format("%.36s",m_CueFileHeader.guid);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Project ID");
	temp.Format("%.32s",m_CueFileHeader.project_id);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"magic");
	temp.Format("%.10s",m_CueFileHeader.magic);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"SHA1");
	temp.Format("%.20s",m_CueFileHeader.sha1);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Header version");
	temp.Format("%d",m_CueFileHeader.header_ver);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"File Desc.");
	temp.Format("%.32s",m_CueFileHeader.file_desc);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Frame Number");
	temp.Format("%d",m_CueFileHeader.frame_num);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"mps (FPS)");
	temp.Format("%d",m_CueFileHeader.fps);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Video Name");
	temp.Format("%s",m_CueFileHeader.video_name);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Motion Exist");
	temp.Format("%d",m_CueFileHeader.motion_exist);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	// for Special Effect Data 
	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Effect Number");
	temp.Format("%d",m_CueFileHeader.effect_channel_num);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"SYS UUID");
	temp.Format("%s",m_CueFileHeader.sysUUID);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Reserved");
	temp.Format("%s",m_CueFileHeader.reserved);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	UpdateData(FALSE);
}


void CDlgReadEncryptMotionFile::showMotionData(vector<struct signal> &motionDataList)
{
	UpdateData(TRUE);

	CString columns[]={"     X","Cyld 1","Cyld 2","Cyld 3","Cyld 4","Cyld 5","Cyld 6"};

	m_CylinderDataListBox.DeleteAllItems();

	int columnCount = getColumnCount(&m_CylinderDataListBox);

	if(columnCount ==0){

		CRect rect;
		m_CylinderDataListBox.GetWindowRect(&rect);

		int width=(rect.Width()-15)/7;

		for(int i=0;i<7;i++){
			m_CylinderDataListBox.InsertColumn( i, _T(columns[i]), LVCFMT_CENTER,width);
		}

	}
	int count=motionDataList.size();

	for(int i=0;i<count;i++){
		CString temp;
		signal data=motionDataList.at(i);

		 temp.Format("%6d",i+1);
	    int nIndex = m_CylinderDataListBox.InsertItem( 0xFFFF,temp);

		for(int j=0;j<6;j++){
			  temp.Format("%d", data.voltage[j]);
			  m_CylinderDataListBox.SetItemText( nIndex, 1+j, temp);
		}
	}

	UpdateData(FALSE);
}

void CDlgReadEncryptMotionFile::showEffectData(vector<int> *effectDataList,int effectTypeNumber){

	UpdateData(TRUE);

	m_EffectDataListBox.DeleteAllItems();
	
	int columnCount = getColumnCount(&m_EffectDataListBox);

	if(columnCount!=0){
		for(int i=0;i<columnCount;i++){
			m_EffectDataListBox.DeleteColumn(0);
		}
	}

	CRect rect;
	m_EffectDataListBox.GetWindowRect(&rect);

	int width=(rect.Width()-15)/(effectTypeNumber+1);

	m_EffectDataListBox.InsertColumn( 0, _T("   No"), LVCFMT_CENTER,width);

	CString temp;
	for(int i=0;i<effectTypeNumber;i++){
		temp.Format("#%d",i+1);
		m_EffectDataListBox.InsertColumn( i+1, temp, LVCFMT_CENTER,width);
	}

	int count=effectDataList[0].size();

	for(int i=0;i<count;i++){
	
		 temp.Format("%6d",i+1);
	    int nIndex = m_EffectDataListBox.InsertItem( 0xFFFF,temp);

		for(int j=0;j<effectTypeNumber;j++){
			if(i<(int)effectDataList[j].size()){

				temp.Format("%d", effectDataList[j].at(i));
				m_EffectDataListBox.SetItemText( nIndex, 1+j, temp);

			}
		}

	}

	UpdateData(FALSE);

}