

#include "stdAfx.h"
#include "haspUtil.h"
#include "..\log.h"


BOOL hasp_key_login(char *msg){

	const hasp_feature_t feature = HASP_DEFAULT_FID;  

	m_hasp_handle = HASP_INVALID_HANDLE_VALUE;

	m_hasp_status = hasp_login(feature, vendor_code, &m_hasp_handle);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);

			break;
		case HASP_HASP_NOT_FOUND:
			stringCopy(msg,"HASP_KEY_NOT_FOUND",32);
			break;
		case HASP_OLD_DRIVER:
			stringCopy(msg,"HASP_OLD_DRIVER",32);
			break;
		case HASP_NO_DRIVER:
			stringCopy(msg,"HASP_NO_DRIVER",32);
			break;
		case HASP_INV_VCODE:
			stringCopy(msg,"HASP_INV_VCODE",32);
			break;
		case HASP_FEATURE_TYPE_NOT_IMPL:
			stringCopy(msg,"HASP_FEATURE_TYPE_NOT_IMPL",32);
			break;
		case HASP_TMOF:
			stringCopy(msg,"HASP_TMOF",32);
			break;
		case HASP_TS_DETECTED:
			stringCopy(msg,"HASP_TS_DETECTED",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}

		return false;

	}else{
		//msg="SUCCESS";
		stringCopy(msg,"SUCCESS",32);
		return true;
	}
}

BOOL hasp_key_logout(char *msg){

	m_hasp_status = hasp_logout(m_hasp_handle);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{
		stringCopy(msg,"SUCCESS",32);
		return true;
	}

}

BOOL hasp_key_real_time(char *msg,struct HaspKeyTime &haspTime){

	m_hasp_status = hasp_get_rtc(m_hasp_handle, &m_hasp_time);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_NO_TIME:
			stringCopy(msg,"HASP_NO_TIME",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{

		m_hasp_status = hasp_hasptime_to_datetime(m_hasp_time, 
			&haspTime.day, 
			&haspTime.month, 
			&haspTime.year, 
			&haspTime.hour,
			&haspTime.minute, 
			&haspTime.second);

		if(m_hasp_status == HASP_INV_TIME)
		{
			/* handle error */
			stringCopy(msg,"HASP_INV_TIME",32);
			return false;
		}else{
			stringCopy(msg,"SUCCESS",32);

			TIME_ZONE_INFORMATION timeZoneInfo;
			GetTimeZoneInformation( &timeZoneInfo );

			haspTime.hour-=(int)(timeZoneInfo.Bias/60);

			return true;
		}

	}

}


BOOL hasp_key_write_data(char *msg,char *uuid,char * lib_apc_key,struct HaspKeyTime expireDate,BOOL checkFlag, int videoSum,char *aes_key){

	hasp_size_t offset = 0;
	hasp_size_t len = sizeof( struct HaspKeyData);

	struct HaspKeyData *haspKeyData;
	haspKeyData = new HaspKeyData();

    stringCopy(haspKeyData->magic,"@Brogent00",10);
	
    stringCopy(haspKeyData->UUID,uuid,80);
	stringCopy(haspKeyData->LIB_APC_KEY,lib_apc_key,32);
	haspKeyData->expireDate=expireDate;
	haspKeyData->checkFlag=checkFlag;
	haspKeyData->videoSum=videoSum;
	stringCopy(haspKeyData->AES_KEY,aes_key,16);

	//  Just date data , no motion 
	if(!hasp_key_encrypt_data(msg,haspKeyData,len)){
		return false;
	}

	m_hasp_status = hasp_write(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_INV_FILEID:
			stringCopy(msg,"HASP_INV_FILEID",32);
			break;
		case HASP_MEM_RANGE:
			stringCopy(msg,"HASP_MEM_RANGE",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{
		stringCopy(msg,"SUCCESS",32);
		return true;
	}

}


BOOL hasp_key_write_check_flag(char *msg,BOOL checkFlag){

//	if(m_hasp_handle==NULL){
		if(!hasp_key_login(msg)){
			return false;
		}
//	}

	hasp_size_t offset = 0;
	hasp_size_t len = sizeof( struct HaspKeyData);

	struct HaspKeyData *haspKeyData;
	haspKeyData = new HaspKeyData();

	m_hasp_status = hasp_read(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_INV_FILEID:
			stringCopy(msg,"HASP_INV_FILEID",32);
			break;
		case HASP_MEM_RANGE:
			stringCopy(msg,"HASP_MEM_RANGE",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{

		if(!hasp_key_decrypt_data(msg,haspKeyData, len)){
			return false;
		}

		haspKeyData->checkFlag=checkFlag;

		if(!hasp_key_encrypt_data(msg,haspKeyData,len)){
			return false;
		}

		m_hasp_status = hasp_write(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

		/* check if operation was successful */
		if (m_hasp_status != HASP_STATUS_OK)
		{
			switch (m_hasp_status)
			{
			case HASP_FEATURE_NOT_FOUND:
				stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
				break;
			case HASP_INV_HND:
				stringCopy(msg,"HASP_INV_HND",32);
				break;
			case HASP_INV_FILEID:
				stringCopy(msg,"HASP_INV_FILEID",32);
				break;
			case HASP_MEM_RANGE:
				stringCopy(msg,"HASP_MEM_RANGE",32);
				break;
			default:
				stringCopy(msg,"HASP_STATUS_NOT_READY",32);
				break;
			}
			return false;
		}else{
			stringCopy(msg,"SUCCESS",32);
			return true;
		}

	}
}


BOOL hasp_key_read_check_flag(char *msg,BOOL &checkFlag){

	//if(m_hasp_handle==NULL){
	if(!hasp_key_login(msg)){
		return false;
	}
	//}

	hasp_size_t offset = 0;
	hasp_size_t len = sizeof( struct HaspKeyData);

	struct HaspKeyData *haspKeyData;
	haspKeyData = new HaspKeyData();

	m_hasp_status = hasp_read(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_INV_FILEID:
			stringCopy(msg,"HASP_INV_FILEID",32);
			break;
		case HASP_MEM_RANGE:
			stringCopy(msg,"HASP_MEM_RANGE",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{

		if(!hasp_key_decrypt_data(msg,haspKeyData, len)){
			return false;
		}

		checkFlag=haspKeyData->checkFlag;

		stringCopy(msg,"SUCCESS",32);
		return true;

	}
}

BOOL hasp_key_read_data(char *msg,char *uuid,char * lib_apc_key,SYSTEMTIME & expireDate,BOOL &checkFlag , int & videoSum,char *aes_key){

	hasp_size_t offset = 0;
	hasp_size_t len = sizeof( struct HaspKeyData);

	struct HaspKeyData *haspKeyData;
	haspKeyData = new HaspKeyData();

	m_hasp_status = hasp_read(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_INV_FILEID:
			stringCopy(msg,"HASP_INV_FILEID",32);
			break;
		case HASP_MEM_RANGE:
			stringCopy(msg,"HASP_MEM_RANGE",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{

		if(!hasp_key_decrypt_data(msg,haspKeyData, len)){
			return false;
		}
		
		char magic[11];
		stringCopy(magic, haspKeyData->magic,sizeof( magic)-1);

		if(strcmp(magic,"@Brogent00")!=0){
			stringCopy(msg,"HASP_MAGIC_ERROR",32);
			return false;
		}

		stringCopy(uuid, haspKeyData->UUID,80);
		stringCopy(lib_apc_key,haspKeyData->LIB_APC_KEY,32);
		expireDate=transToSystemTime(haspKeyData->expireDate);
		checkFlag=haspKeyData->checkFlag;
		videoSum=haspKeyData->videoSum;
		stringCopy(aes_key,haspKeyData->AES_KEY,16);
	
		stringCopy(msg,"SUCCESS",32);
		return true;
	}

}


BOOL hasp_key_encrypt_data(char *msg,void *buffer,hasp_size_t len){

	m_hasp_status = hasp_encrypt(m_hasp_handle,buffer, len);

	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_TOO_SHORT:
			stringCopy(msg,"HASP_TOO_SHORT",32);
			break;
		case HASP_ENC_NOT_SUPP:
			stringCopy(msg,"HASP_ENC_NOT_SUPP",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{
		stringCopy(msg,"SUCCESS",32);
		return true;
	}
}

BOOL hasp_key_decrypt_data(char *msg,void *buffer,hasp_size_t len){

	m_hasp_status = hasp_decrypt(m_hasp_handle, buffer, len);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_TOO_SHORT:
			stringCopy(msg,"HASP_TOO_SHORT",32);
			break; 
		case HASP_ENC_NOT_SUPP:
			stringCopy(msg,"HASP_ENC_NOT_SUPP",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{
		stringCopy(msg,"SUCCESS",32);
		return true;
	}

}


BOOL hasp_key_check(char *msg){
	
	if(!hasp_key_login(msg)){
		return false;
	}
	

	HaspKeyTime haspTime;
	if(!hasp_key_real_time(msg,haspTime)){
		return false;
	}
	
	char uuid[80];
	char lib_apc_key[32];
	SYSTEMTIME expireDate;
	BOOL checkFlag=false;
	int videoSum;
	char aes_key[16];

	if(!hasp_key_read_data(msg,uuid,lib_apc_key,expireDate, checkFlag,videoSum,aes_key)){
		return false;
	}
	
	stringCopy(msg,"SUCCESS",32);
	return true;

}

BOOL get_hasp_key_real_time(char *msg,SYSTEMTIME & systemTime){

	HaspKeyTime haspTime;
	if(!hasp_key_real_time(msg,haspTime)){
		return false;
	}

	systemTime=transToSystemTime(haspTime);

	return  true;

}

SYSTEMTIME transToSystemTime(HaspKeyTime time){

	SYSTEMTIME systemTime ;

	systemTime.wYear=time.year;
	systemTime.wMonth=time.month;
	systemTime.wDay=time.day;
	systemTime.wHour=time.hour;
	systemTime.wMinute=time.minute;
	systemTime.wSecond=time.second;
	systemTime.wMilliseconds=0;

	double dVariantTime;
	
	SystemTimeToVariantTimeWithMilliseconds (systemTime, &dVariantTime);

	VariantTimeToSystemTimeWithMilliseconds (dVariantTime, &systemTime);


	return systemTime;
}

void  stringCopy(char *strTarget, char *strSource,int lenTarget){

	int lenSource=strlen(strSource);
	int len=0;

	if( lenSource>lenTarget){
		len=lenTarget;
	}else{
		len=lenSource;
	}
	memcpy(strTarget,strSource,len);
	strTarget[len]='\0';
}

BOOL SystemTimeToVariantTimeWithMilliseconds (/*input*/ SYSTEMTIME st, /*output*/double *dVariantTime)
{
	BOOL retVal = TRUE;

	WORD wMilliSeconds = st.wMilliseconds;  // save the milli second information
	st.wMilliseconds = 0;  // pass 0 milliseconds to the function and get the converted value without milliseconds
	double dWithoutms;
	retVal = SystemTimeToVariantTime(&st, &dWithoutms) ;

	// manually convert the millisecond information into variant fraction and add it to system converted value
	double OneMilliSecond =  ONETHOUSANDMILLISECONDS/1000 ;
	*dVariantTime = dWithoutms +  (OneMilliSecond * wMilliSeconds);

	return retVal;
}

BOOL VariantTimeToSystemTimeWithMilliseconds (/*input*/ double dVariantTime, /*output*/SYSTEMTIME *st)
{
	BOOL retVal = TRUE;

	double halfsecond = ONETHOUSANDMILLISECONDS / 2.0;  // see definition of ONETHOUSANDMILLISECONDS in the header file

	retVal = VariantTimeToSystemTime(dVariantTime - halfsecond, st);   // this takes care of rounding problem with VariantTimetoSystemTime function
	if (retVal == FALSE)
	{
		return retVal;
	}


	double fraction = dVariantTime  - (int) dVariantTime;  // extracts the fraction part
	
	double hours; 
	hours = fraction = (fraction - (int)fraction) * 24;

	double minutes;
	minutes = (hours - (int)hours) * 60;

	double seconds;
	seconds = (minutes - (int)minutes) * 60;

	double milliseconds;
	milliseconds = (seconds - (int)seconds) * 1000;
	
	milliseconds = milliseconds + 0.5;	// rounding off millisecond to the nearest millisecond				
	if (milliseconds < 1.0 || milliseconds > 999.0)    //Fractional calculations may yield in results like
		milliseconds = 0;		   // 0.00001 or 999.9999 which should actually be zero (slightly above or below limits are actually zero)

	if (milliseconds)	
		st->wMilliseconds = (WORD) milliseconds;
	else // if there is 0 milliseconds, then we dont have the problem !!
		retVal = VariantTimeToSystemTime(dVariantTime, st);   

	return retVal;
}


BOOL hasp_key_write_video_sum(char *msg,int videoSum){

	if(!m_hasp_handle){
		if(!hasp_key_login(msg)){
			return false;
		}
	}

	hasp_size_t offset = 0;
	hasp_size_t len = sizeof( struct HaspKeyData);

	struct HaspKeyData *haspKeyData;
	haspKeyData = new HaspKeyData();

	m_hasp_status = hasp_read(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_INV_FILEID:
			stringCopy(msg,"HASP_INV_FILEID",32);
			break;
		case HASP_MEM_RANGE:
			stringCopy(msg,"HASP_MEM_RANGE",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{

		if(!hasp_key_decrypt_data(msg,haspKeyData, len)){
			return false;
		}

		haspKeyData->videoSum=videoSum;

		if(!hasp_key_encrypt_data(msg,haspKeyData,len)){
			return false;
		}

		m_hasp_status = hasp_write(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

		/* check if operation was successful */
		if (m_hasp_status != HASP_STATUS_OK)
		{
			switch (m_hasp_status)
			{
			case HASP_FEATURE_NOT_FOUND:
				stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
				break;
			case HASP_INV_HND:
				stringCopy(msg,"HASP_INV_HND",32);
				break;
			case HASP_INV_FILEID:
				stringCopy(msg,"HASP_INV_FILEID",32);
				break;
			case HASP_MEM_RANGE:
				stringCopy(msg,"HASP_MEM_RANGE",32);
				break;
			default:
				stringCopy(msg,"HASP_STATUS_NOT_READY",32);
				break;
			}
			return false;
		}else{
			stringCopy(msg,"SUCCESS",32);
			return true;
		}

	}

}


BOOL hasp_key_read_video_sum(char *msg,int &videoSum){

//	if(m_hasp_handle==NULL){
		if(!hasp_key_login(msg)){
			return false;
		}
//	}

	hasp_size_t offset = 0;
	hasp_size_t len = sizeof( struct HaspKeyData);

	struct HaspKeyData *haspKeyData;
	haspKeyData = new HaspKeyData();

	m_hasp_status = hasp_read(m_hasp_handle, HASP_FILEID_RW, offset, len, haspKeyData);

	/* check if operation was successful */
	if (m_hasp_status != HASP_STATUS_OK)
	{
		switch (m_hasp_status)
		{
		case HASP_FEATURE_NOT_FOUND:
			stringCopy(msg,"HASP_FEATURE_NOT_FOUND",32);
			break;
		case HASP_INV_HND:
			stringCopy(msg,"HASP_INV_HND",32);
			break;
		case HASP_INV_FILEID:
			stringCopy(msg,"HASP_INV_FILEID",32);
			break;
		case HASP_MEM_RANGE:
			stringCopy(msg,"HASP_MEM_RANGE",32);
			break;
		default:
			stringCopy(msg,"HASP_STATUS_NOT_READY",32);
			break;
		}
		return false;
	}else{

		if(!hasp_key_decrypt_data(msg,haspKeyData, len)){
			return false;
		}

		videoSum=haspKeyData->videoSum;

		stringCopy(msg,"SUCCESS",32);
		return true;

	}
}

char m_sysUUID[37];
bool m_IsSysUUIDSet=false;

BOOL getSysUUID(char *msg ,char uuid[37]){

	wmi_sys_info wmiEnc;

	WmiInfo wmiConn;

	if( m_IsSysUUIDSet==false){

		if(wmiConn.getWmiInfoEnc(wmiEnc)!=1){

			stringCopy(uuid,(char *)wmiEnc.mb_uuid,36);
			uuid[36]='\0';

			stringCopy(m_sysUUID,(char *)wmiEnc.mb_uuid,36);
			m_sysUUID[36]='\0';
			m_IsSysUUIDSet=true;

			stringCopy(msg,"SUCCESS",32);

			return TRUE;

		}else{

			stringCopy(msg,"Fail to get UUID",32);
			return FALSE;
		}	

	}else{

		stringCopy(uuid,m_sysUUID,36);
		uuid[36]='\0';

		stringCopy(msg,"SUCCESS",32);
		return TRUE;
	}

}



BOOL checkUUID(char *msg, unsigned char *buffer){

	//----   get  UUID  of mother board ---------------------------
	char uuid[37];
	if(getSysUUID(msg,uuid)==FALSE){
		return FALSE;
	}

	// ---  get UUID of Motion header  -----------------------			
	struct bgt_motion_file_header motionFileHeader;
	memcpy(&motionFileHeader,buffer, sizeof(bgt_motion_file_header));

	//  ---- for  motion file header ver 2 -----
	if(motionFileHeader.file_ver==2){
		if(memcmp(uuid,motionFileHeader.sysUUID,36)==0){
			stringCopy(msg,"SUCCESS.",32);
			return TRUE;
		}else{
			stringCopy(msg,"UUID Error.",32);
			return FALSE;
		}

		//  ---- for  cue file header ver 3 -----
	}else if(motionFileHeader.file_ver==3){
		struct bgt_cue_file_header cueFileHeader;
		memcpy(&cueFileHeader,buffer, sizeof(bgt_cue_file_header));

		if(memcmp(uuid,cueFileHeader.sysUUID,36)==0){
			stringCopy(msg,"SUCCESS.",32);
			return TRUE;
		}else{
			stringCopy(msg,"UUID Error.",32);
			return FALSE;
		}
	}

	stringCopy(msg,"File Ver unkown.",32);
	return FALSE;

}