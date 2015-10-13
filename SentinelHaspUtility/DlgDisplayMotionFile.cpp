// DlgDisplayMotionFile.cpp : implementation file
//

#include "stdafx.h"
#include "SentinelHaspUtility.h"
#include "DlgDisplayMotionFile.h"
#include "afxdialogex.h"
#include "log.h"

// CDlgDisplayMotionFile dialog

IMPLEMENT_DYNAMIC(CDlgDisplayMotionFile, CDialogEx)

CDlgDisplayMotionFile::CDlgDisplayMotionFile(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDisplayMotionFile::IDD, pParent)
	, m_EditAnalysis(_T(""))
	, m_AbsMotionFilePath(_T(""))
	, m_FolderPath(_T(""))
	, m_MotionFileName(_T(""))
	, m_FPS(60)
	, m_DeviceType(0)
	, m_RegistDate(COleDateTime::GetCurrentTime())
	, m_ExpireDate(COleDateTime::GetCurrentTime())
	, m_NewSpliteMotionFileName(_T(""))
	, m_EditSpecialNote(_T(""))
	, m_IsMotionFileHeaderVerified(false)
{

}

CDlgDisplayMotionFile::~CDlgDisplayMotionFile()
{
}

void CDlgDisplayMotionFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OTHER_MOTION_DATA, m_CylinderDataListBox);
	DDX_Text(pDX, IDC_EDIT_MOTION_DATA_ANALIZ, m_EditAnalysis);
	DDX_Control(pDX, IDC_LIST_MOTION_HEADER_INFO, m_ListCtrlHeaderInfo);
	DDX_Control(pDX, IDC_LIST_OTHER_EFFECT_DATA, m_EffectDataListBox);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REGIST_DATE_WITH_MOTION, m_RegistDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_EXPIRE_DATE_WITH_MOTION, m_ExpireDate);
	DDX_Text(pDX, IDC_EDIT_NEW_FILE_NAME, m_NewSpliteMotionFileName);
	DDX_Text(pDX, IDC_EDIT_SPECIAL_NOTE_WITH_MOTION, m_EditSpecialNote);
}


BEGIN_MESSAGE_MAP(CDlgDisplayMotionFile, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_READ_MOTION_FILE, &CDlgDisplayMotionFile::OnBnClickedBtnReadMotionFile)
	ON_BN_CLICKED(IDOK, &CDlgDisplayMotionFile::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDisplayMotionFile message handlers


BOOL CDlgDisplayMotionFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	m_CylinderDataListBox.SetExtendedStyle( m_CylinderDataListBox.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_TRACKSELECT );

	m_EffectDataListBox.SetExtendedStyle( m_EffectDataListBox.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_TRACKSELECT );

	m_ListCtrlHeaderInfo.SetExtendedStyle( m_ListCtrlHeaderInfo.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_TRACKSELECT );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDisplayMotionFile::OnBnClickedBtnReadMotionFile()
{
	// TODO: Add your control notification handler code here
	int result;
	result =openMotionFile();

	if(result==0){
		return;
	}
	
	CWaitCursor wait;

	result=verifyMotionFileHeader(m_AbsMotionFilePath);
	if(result==1){
		m_IsMotionFileHeaderVerified=true;
	    showHeaderInfo();
		int flag=0;
		flag=readMotionFile(m_AbsMotionFilePath,m_CylinderDataList,m_EffectDataList,m_EffectType,true);
		if(flag==1){
			showMotionData(m_CylinderDataList);
			motionAnalysis();
			showEffectData(m_EffectDataList,m_EffectType);
		}
	}else{
		m_IsMotionFileHeaderVerified=false;
		int flag=0;
		flag=readMotionFile(m_AbsMotionFilePath,m_CylinderDataList,m_EffectDataList,m_EffectType,false);
		if(flag==1){
			showMotionData(m_CylinderDataList);
			motionAnalysis();
		}
	}
	
	 wait.Restore();
}


int CDlgDisplayMotionFile::openMotionFile(void)
{
	int result=0;
	UpdateData(TRUE);
	
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter=LPCTSTR(_T("Motion Files(*.motion)\0*.motion\0"));
	//定義打開對話框的標題
	fileDlg.m_ofn.lpstrTitle=LPCTSTR(_T("Browse a motion file"));

	if (fileDlg.DoModal()==IDOK)
	{
		m_AbsMotionFilePath=fileDlg.GetPathName();
		m_FolderPath=fileDlg.GetFolderPath();
		m_MotionFileName=fileDlg.GetFileName();

		UpdateData(TRUE);
		m_NewSpliteMotionFileName.Format("splite-%s",m_MotionFileName);
		UpdateData(FALSE);
		result=1;
	}

	fileDlg.DestroyWindow();

	UpdateData(FALSE);
	return result;
}

int CDlgDisplayMotionFile::readMotionFile(CString fileName,vector<struct signal> &motionDataList,vector<int> effectDataList[MAX_CURVE],int effectType[4],bool isHaveHeader)
{
	signal motionData;

	CFile fb;

	TRY{
		motionDataList.clear();

		CFileException fileException;
		BOOL result=FALSE;
		TCHAR szError[1024];
		CString msg;
		byte effect;

		result=fb.Open(fileName,CFile::modeRead | CFile::typeBinary,&fileException);
		if (! result) {
			fileException.GetErrorMessage(szError, 1024); 
			msg.Format("Error opening file : %s",szError);
			AfxMessageBox (msg);
			return 0;
		}

		if(isHaveHeader){
			fb.Read (&m_MotionFileHeader , sizeof(bgt_motion_file_header));
		}

		while (fb.Read (motionData.voltage , sizeof(short) *6 )!=0)
		{
			motionDataList.push_back(motionData);

			// read 4 byte effect data
			if(m_MotionFileHeader.file_ver==2){
				for(int i=0;i<4;i++){
					if(fb.Read (&effect , sizeof(byte) )!=0){

						if(effectType[i]>=WATER && effectType[i]<=LEG_TICKLE){
							effectDataList[ effectType[i]].push_back((int)effect);
						}
					}
				}
			}
		}

		fb.Close() ;

		return 1;
	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At CDlgDisplayMotionFile::readMotionFile ,exception = \"%s\"."),szCause);
		AfxMessageBox(sMessage);
		fb.Close();
		//e->ReportError();
		return 0;
	}
	END_CATCH
}

int CDlgDisplayMotionFile::verifyMotionFileHeader(CString fileName)
{
	CFile fb;
	TRY{

		CFileException fileException;
		BOOL result=FALSE;
		TCHAR szError[1024];
		CString msg;
		result=fb.Open(fileName,CFile::modeRead | CFile::typeBinary,&fileException);
		if (!result) {
			fileException.GetErrorMessage(szError, 1024); 
			msg.Format("Error opening file : %s",szError);
			AfxMessageBox (msg);
			return 0;
		}

		short motionValue;
		fb.Read (&m_MotionFileHeader , sizeof(bgt_motion_file_header) );

		CString magic(m_MotionFileHeader.magic);
		magic.SetAt(10,'\0');
		if(magic.Compare("@Brogent00")!=0){
			fb.Close();
			return 0;
		}

		int size= m_MotionFileHeader.motion_num;

		unsigned char *buffer;
		buffer = (unsigned char*)malloc(sizeof(bgt_motion_file_header) +sizeof(short)*size*8); // 8 short = 6 short + 4 byte
		memcpy(buffer, &m_MotionFileHeader, sizeof(bgt_motion_file_header));


		int counter=sizeof(bgt_motion_file_header);
		while (  fb.Read  (&motionValue , sizeof(short) )!=0){
			memcpy(&buffer[counter], &motionValue, sizeof(short));
			counter+=2;
		}
		fb.Close();

		int checkResult=0;
		result= bgt_motion_file_check((bgt_motion_file_header *) buffer);
		delete [] buffer;
		if(checkResult!=0){

			if(checkResult==1){
				AfxMessageBox("Checksum error when excute bgt_motion_file_check()");
			}

			if(checkResult==2){
				AfxMessageBox("file format error when excute bgt_motion_file_check()");
			}
			return 0;
		} else{
			return 1;
		}

	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At CDlgDisplayMotionFile::verifyMotionFileHeader ,exception = \"%s\"."),szCause);
		AfxMessageBox(sMessage);
		fb.Close();
		
		//e->ReportError();
	}
	END_CATCH

	return 0;

}

void CDlgDisplayMotionFile::showMotionData(vector<struct signal> &motionDataList)
{
	UpdateData(TRUE);

	CString columns[]={"     X","Cylind 1","Cylind 2","Cylind 3","Cylind 4","Cylind 5","Cylind 6"};

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

void CDlgDisplayMotionFile::showEffectData(vector<int> effectDataList[MAX_CURVE],int effectType[4]){

	UpdateData(TRUE);
	CString columns[5];

	m_EffectDataListBox.DeleteAllItems();

	int columnCount = getColumnCount(&m_EffectDataListBox);


	if(columnCount ==0){
		columns[0]="     X";

		for(int i=0;i<4;i++){
			if(effectType[i]>=WATER && effectType[i] <=LEG_TICKLE){
				columns[i+1]=EffectName[effectType[i]];
			}else{
				columns[i+1]="None";
			}
		}

		CRect rect;
		m_EffectDataListBox.GetWindowRect(&rect);

		int width=(rect.Width()-15)/5;

		for(int i=0;i<5;i++){
			m_EffectDataListBox.InsertColumn( i, _T(columns[i]), LVCFMT_CENTER,width);
		}

	}
	int count=effectDataList[effectType[0]].size();

	for(int i=0;i<count;i++){
		CString temp;

		 temp.Format("%6d",i+1);
	    int nIndex = m_EffectDataListBox.InsertItem( 0xFFFF,temp);

		for(int j=0;j<4;j++){
			if(i<(int)effectDataList[effectType[j]].size()){

				temp.Format("%d", effectDataList[effectType[j]].at(i));
				m_EffectDataListBox.SetItemText( nIndex, 1+j, temp);

			}
		}

	}

	UpdateData(FALSE);

}

void CDlgDisplayMotionFile::showHeaderInfo(void)
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

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"File name");
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,m_MotionFileName);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Update Date");
	temp.Format("%.16s",m_MotionFileHeader.updateDate);
	
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
	m_DeviceType=m_MotionFileHeader.type;
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,DeviceName[m_DeviceType]);

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
	m_FPS=(float)m_MotionFileHeader.mps/100.0f;
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Video Name");
	temp.Format("%s",m_MotionFileHeader.video_name);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	// for Special Effect Data 
	for(int i=0;i<4;i++){

		temp.Format("Effect #%d",i);
		nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,temp);
		indexEffect=m_MotionFileHeader.effect[i];

		if(indexEffect>=WATER && indexEffect <=LEG_TICKLE){
			effectName=EffectName[indexEffect];
			m_EffectType[i]=indexEffect;
		}else{
			effectName="None";
			m_EffectType[i]=0;
		}
		m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,effectName);
	}

	nIndex =m_ListCtrlHeaderInfo.InsertItem( 0xFFFF,"Reserved");
	temp.Format("%s",m_MotionFileHeader.reserved);
	m_ListCtrlHeaderInfo.SetItemText( nIndex, 1,temp);

	UpdateData(FALSE);
}

void CDlgDisplayMotionFile::motionAnalysis(void)
{
	UpdateData(TRUE);

	m_EditAnalysis="";

	UpdateData(FALSE);
	
	calStepping();
	analysisAcceleration();
	
}

void CDlgDisplayMotionFile::calStepping(void)
{
	UpdateData(TRUE);
	
	int count=m_CylinderDataList.size();

	float totalTime=count/m_FPS;
	
	float max[6]={-99999,-99999,-99999,-99999,-99999,-99999};
	float min[6]={99999,99999,99999,99999,99999,99999};
	float sum[6]={0,0,0,0,0,0};
	float avg[6]={0,0,0,0,0,0};

	for(int i=0;i<count-1;i++){
		signal data1=m_CylinderDataList.at(i);
		signal data2=m_CylinderDataList.at(i+1);
		for(int j=0;j<6;j++){
			float dif=(float)abs(data2.voltage[j]-data1.voltage[j]);

			dif=dif*400/4096;               //   transfer to Cylinder length

			sum[j]+=dif;

			if(dif>=max[j]){
				max[j]=dif;
			}

			if(dif<=min[j]){
				min[j]=dif;
			}
		}
	}

	for(int i=0;i<6;i++){
		avg[i]=sum[i]/(count-1);
	}

	CString temp;

	temp="Stroke Stepping Analysis : (Unit : mm)\r\n";
	m_EditAnalysis.Append(temp);
	temp.Format("Total time(sec) : %.1f\r\n",totalTime);
	m_EditAnalysis.Append(temp);
	temp.Format("Device Type : %s\r\n",DeviceName[m_DeviceType]);
	m_EditAnalysis.Append(temp);

	temp="------------------------------------------------------------------\r\n";
	m_EditAnalysis.Append(temp);

	float totalVelocity=0;

	for(int i=0;i<6;i++){
		temp.Format("Cylinder #%d , min =%.1f , max=%.1f , avg=%.1f , total move stroke =%.1f ,avg volocity = %.1f (mm/sec)\r\n",i+1,min[i],max[i],avg[i],sum[i],sum[i]/totalTime);
		m_EditAnalysis.Append(temp);
		totalVelocity+=sum[i]/totalTime;
	}

	temp.Format("Avg volocity of six cylinder = %.1f (mm/sec)\r\n",(totalVelocity/6));
	m_EditAnalysis.Append(temp);

	UpdateData(FALSE);
}


BOOL CDlgDisplayMotionFile::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	// for CFileDialog getFolderPath
	cs.style |= (OFN_EXPLORER| WS_CHILD|WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	return CDialogEx::PreCreateWindow(cs);
}

int CDlgDisplayMotionFile::getColumnCount(const CListCtrl *pListCtrl)
{
	CHeaderCtrl* pHeader;

	if (pListCtrl == NULL) return 0;
	if ((pHeader = const_cast<CListCtrl*>(pListCtrl)->GetHeaderCtrl()) ==
		NULL) return 0;
	return pHeader->GetItemCount();
}

double CDlgDisplayMotionFile::calVelocity(int cylinderNumber,int index)
{
	if(m_CylinderDataList.size()<=5){
		return 0;
	}
	if(index<=5) {
		return 0;
	}

	double x2=index;
	double y2;

	if(x2>=0 && x2<m_CylinderDataList.size()){
		y2=m_CylinderDataList.at((int)x2).voltage[cylinderNumber];
	}else{
		y2=0;
	}

	double x1=index-2;
	double y1;
	if(x1>=0 && x1<m_CylinderDataList.size()){
		y1=m_CylinderDataList.at((int)x1).voltage[cylinderNumber];
	} else{
		y1=0;
	}

	double sec=(x2-x1)/ m_FPS;
	double meter=((y2-y1)*40/4096)/100;

	if(sec==0){
		return 0;
	}

	double v=floor(meter*1000/sec);

	return v/1000;
}

double CDlgDisplayMotionFile::calAcceleration(int cylinderNumber,int index)
{
	if(m_CylinderDataList.size()<=35){
		return 0;
	}
	if(index<=5) {
		return 0;
	}

	double x2=index;
	double v2=calVelocity(cylinderNumber,(int)x2);

	double x1=x2-2;
	double v1=calVelocity(cylinderNumber,(int)x1);

	double sec=(x2-x1)/ m_FPS;

	if(sec==0){
		return 0;
	}

	double a= floor((v2-v1)*1000/sec );

	return a/1000;
}

void CDlgDisplayMotionFile::analysisAcceleration(){

	UpdateData(TRUE);
	
	int count=m_CylinderDataList.size();
	
	float max[6]={-99999,-99999,-99999,-99999,-99999,-99999};
	float min[6]={99999,99999,99999,99999,99999,99999};
	float sum[6]={0,0,0,0,0,0};
	float avg[6]={0,0,0,0,0,0};

	float acceleration=0;

	for(int i=0;i<count-1;i++){
	
		for(int j=0;j<6;j++){
	
			acceleration=(float)calAcceleration(j,i);

			sum[j]+=abs(acceleration);

			if(acceleration>=max[j]){
				max[j]=acceleration;
			}

			if(acceleration<=min[j]){
				min[j]=acceleration;
			}
		}
	}

	for(int i=0;i<6;i++){
		avg[i]=sum[i]/(count-1);
	}

	CString temp;

	temp="------------------------------------------------------------------\r\n";
	m_EditAnalysis.Append(temp);
	temp="Acceleration Analysis : (Unit : m/sec2 )\r\n";
	m_EditAnalysis.Append(temp);
	temp="------------------------------------------------------------------\r\n";
	m_EditAnalysis.Append(temp);

	float totalAcceleration=0;

	for(int i=0;i<6;i++){
		temp.Format("Cylinder #%d , min =%.3f , max=%.3f , avg=%.3f , total  =%.3f \r\n",i+1,min[i],max[i],avg[i],sum[i]);
		m_EditAnalysis.Append(temp);
		totalAcceleration+=avg[i];
	}

	temp.Format("Avg Acceleration of six cylinder = %.3f (m/sec2) , ==  %.3f(G) \r\n",(totalAcceleration/6),(totalAcceleration/6)/9.8);
	m_EditAnalysis.Append(temp);

	UpdateData(FALSE);
}

// to fetch and move first 100 set motion data to to buffer
void CDlgDisplayMotionFile::builtUpMotionDataForKey(void){

	m_MotionData=(unsigned char *)calloc(16*FRAMES_SAVE_IN_KEY,sizeof(unsigned char));

	int counter=0;

	for(int i=0; i<FRAMES_SAVE_IN_KEY; i++){
		for(int j=0;j<6;j++){
			memcpy(&m_MotionData[counter], &m_CylinderDataList[i].voltage[j], sizeof(short));
			counter+=2;
		}

		if(m_IsMotionFileHeaderVerified && m_MotionFileHeader.file_ver==2){
			for(int j=0;j<4;j++){
				byte temp=0;
				if(m_EffectType[j]==0){
					temp=0;
				}else{
					temp=m_EffectDataList[m_EffectType[j]].at(i);
				}
				memcpy(&m_MotionData[counter], &temp, sizeof(byte));
				counter+=1;
			}
		}
	}

	//  erase first 120 set data
	m_CylinderDataList.erase (m_CylinderDataList.begin(),m_CylinderDataList.begin()+FRAMES_SAVE_IN_KEY);

	for(int i=0;i<4;i++){
		int typeIndex=m_EffectType[i];
		if(typeIndex!=0 && m_EffectDataList[typeIndex].size()>FRAMES_SAVE_IN_KEY){
			m_EffectDataList[typeIndex].erase (m_EffectDataList[typeIndex].begin(),m_EffectDataList[typeIndex].begin()+FRAMES_SAVE_IN_KEY);
		}
	}

}

void CDlgDisplayMotionFile::writeMotionDataToKey(void){

	UpdateData(TRUE);

	HaspKeyTime registDate,expireDate;

	transferOleDateToHaspKeyTime (m_RegistDate ,registDate);
	transferOleDateToHaspKeyTime (m_ExpireDate ,expireDate);

	registDate.hour=0;
	registDate.minute=0;
	registDate.second=0;

	expireDate.hour=23;
	expireDate.minute=59;
	expireDate.second=59;

	char msg[32];

	int byteOfEachFrame=0;
	if(m_IsMotionFileHeaderVerified && m_MotionFileHeader.file_ver==2){
		byteOfEachFrame=16;
	}else{
		byteOfEachFrame=12;
	}

	if(!hasp_key_write_data(
		msg,
		registDate,
		expireDate,
		m_EditSpecialNote.GetBuffer(),
		FRAMES_SAVE_IN_KEY,
		byteOfEachFrame,
		m_MotionData)){

		CString temp;
		temp.Format("Hasp Key write data failure : %s",msg);
		MessageBox(temp);

	}else{
		//MessageBox("Write data to key success .");
	}

	UpdateData(FALSE);

}

void CDlgDisplayMotionFile::transferOleDateToHaspKeyTime (COleDateTime oleDateTime ,HaspKeyTime & haspkeyTime){

	haspkeyTime.year=oleDateTime.GetYear();
	haspkeyTime.month=oleDateTime.GetMonth();
	haspkeyTime.day=oleDateTime.GetDay();
	haspkeyTime.hour=oleDateTime.GetHour();
	haspkeyTime.minute=oleDateTime.GetMinute();
	haspkeyTime.second=oleDateTime.GetSecond();
}

void CDlgDisplayMotionFile::saveMotionToNewFile(void)
{
	UpdateData(TRUE); 
	
	unsigned char *buffer;
	int bufferSize;
	int listSize=m_CylinderDataList.size() ;
	bufferSize=sizeof(bgt_motion_file_header) +sizeof(short)*listSize*8;     // 8 short = 6 short + 4 byte
	buffer = (unsigned char*)calloc(bufferSize,sizeof(byte));     

	if(m_IsMotionFileHeaderVerified){

		int initHeaderOK=0;
		initHeaderOK=initMotionFileHeader(
			m_CylinderDataList,
			m_EffectDataList,
			m_EffectType,
			buffer,
			bufferSize);

		if(initHeaderOK==1){

			saveMotionDataBufferToFile(
				m_NewSpliteMotionFileName.GetBuffer(),
				buffer,
			    bufferSize);

		}

	}else{

		saveMotionDataListToFile(
			m_CylinderDataList,
			m_NewSpliteMotionFileName.GetBuffer(),FALSE);
	}

	UpdateData(FALSE); 

	delete[] buffer;

}

int CDlgDisplayMotionFile::initMotionFileHeader(vector<struct signal> motionDataList, vector<int> effectDataList[MAX_CURVE], int effectType[4],unsigned char *buffer ,int &bufferSize)
	{

		// dump data to buffer
		int listSize=motionDataList.size() ;
		m_MotionFileHeader.motion_num=listSize;

		//bufferSize=sizeof(bgt_motion_file_header) +sizeof(short)*listSize*8;     // 8 short = 6 short + 4 byte
		//buffer = (unsigned char*)calloc(bufferSize,sizeof(byte));     
		memcpy(buffer, &m_MotionFileHeader, sizeof(bgt_motion_file_header));

		int counter=sizeof( bgt_motion_file_header);

		for(int i=0; i<listSize; i++){
			for(int j=0;j<6;j++){
				memcpy(&buffer[counter], &motionDataList[i].voltage[j], sizeof(short));
				counter+=2;
			}

			if(m_MotionFileHeader.file_ver==2){
				for(int j=0;j<4;j++){
					byte temp=0;
					if(effectType[j]==0){
						temp=0;
					}else{
						temp=effectDataList[effectType[j]].at(i);
					}
					memcpy(&buffer[counter], &temp, sizeof(byte));
					counter+=1;
				}
			}
			
		}

		int result= bgt_motion_file_hash((struct bgt_motion_file_header *) buffer);

		if(result!=0){

			if(result==1){
				AfxMessageBox("Checksum error when excute bgt_motion_file_hash()");
			}

			if(result==2){
				AfxMessageBox("file format error when excute bgt_motion_file_hash()");
			}

			return 0;

		} else{
		    memcpy(&m_MotionFileHeader.sha1, &buffer[10], sizeof(char)*20);

			return 1;
		}

		return 0;
	}

	void CDlgDisplayMotionFile::saveMotionDataBufferToFile( char  *motionFileName,unsigned char *buffer,int bufferSize){

		CFile fb;
		TRY{

			CString  path;

			path.Format("%s\\%s",m_FolderPath,m_NewSpliteMotionFileName);

			CFileException fileException;
			BOOL result=FALSE;
			TCHAR szError[1024];
			CString msg;

			result=fb.Open(path,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary,&fileException);
			if (!result) {
                fileException.GetErrorMessage(szError, 1024); 
				msg.Format("Error opening file : %s",szError);
				AfxMessageBox (msg);
				return;
			}

			fb.Write (buffer,bufferSize );

			fb.Close();

			AfxMessageBox ("Save complete.");

		}CATCH(CException,e){
			CString sMessage=_T("");
			TCHAR   szCause[255];
			e->GetErrorMessage(  szCause,255,NULL);
			sMessage.Format(_T("At CDlgDisplayMotionFile::saveMotionDataListToFile ,exception = \"%s\"."),szCause);
			AfxMessageBox(sMessage);
			fb.Close();
			//e->ReportError();
		}
		END_CATCH

	}

	void CDlgDisplayMotionFile::saveMotionDataListToFile(vector<struct signal> motionDataList, char  *motionFileName,bool isWriteHeader)
	{
		CFile fb;
		TRY{

			short ys[6]={0};

			CString  path;

			path.Format("%s\\%s",m_FolderPath,m_NewSpliteMotionFileName);

			CFileException fileException;
			BOOL result=FALSE;
			TCHAR szError[1024];
			CString msg;

			result=fb.Open(path,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary,&fileException);
			if (!result) {
                fileException.GetErrorMessage(szError, 1024); 
				msg.Format("Error opening file : %s",szError);
				AfxMessageBox (msg);
				return;
			}

			if(isWriteHeader){
				fb.Write (&m_MotionFileHeader , sizeof(bgt_motion_file_header) );
			}

			int listSize=motionDataList.size();
			// last one will be dump data
			for(int i=0;i<listSize;i++){

				for(int j=0;j<6;j++){
					ys[j] = motionDataList[i].voltage[j];
				}

				fb.Write(&ys,sizeof(short)*6);
			}
			fb.Close();
			AfxMessageBox ("Save complete.");

		}CATCH(CException,e){
			CString sMessage=_T("");
			TCHAR   szCause[255];
			e->GetErrorMessage(  szCause,255,NULL);
			sMessage.Format(_T("At CDlgDisplayMotionFile::saveMotionDataListToFile ,exception = \"%s\"."),szCause);
			AfxMessageBox(sMessage);
			fb.Close();
			//e->ReportError();
		}
		END_CATCH
	}

	void CDlgDisplayMotionFile::OnBnClickedOk()
	{
		// TODO: Add your control notification handler code here
		CWaitCursor wait;

		builtUpMotionDataForKey();
		writeMotionDataToKey();
		saveMotionToNewFile();

		wait.Restore();

		CDialogEx::OnOK();
	}
