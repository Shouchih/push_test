
#pragma once
#include "afxwin.h"
#include "hasp_api.h"
#include <Windows.h>
#include "WmiInfo.h"
#include "bgt_motionfile.h"

using namespace std;

#define ONETHOUSANDMILLISECONDS  .0000115740740740
/*
A variant time is stored as an 8-byte real value (double), representing a date between January 1, 1753 
and December 31, 2078,inclusive. The value 2.0 represents January 1, 1900; 3.0 represents January 2, 1900, and so on. 
Adding 1 to the value increments the date by a day. The fractional part of the value represents the time of day. 
Therefore, 2.5 represents noon on January 1, 1900; 3.25 represents 6:00 A.M. on January 2, 1900, and so on. 
so 0.5 represents 12 hours ie 12*60*60 seconds, hence 1 second = .0000115740740740
*/

struct HaspKeyTime  {
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
};

struct HaspKeyData  {
	char magic[12];                   //  @Brogent00 
    char UUID[80];
	struct HaspKeyTime  expireDate;
	BOOL checkFlag;
	char LIB_APC_KEY[64];
	int videoSum;
	char AES_KEY[32];
	unsigned char reserve[744];
};

static unsigned char vendor_code[] =
		"9C1OhcK1qLWa+cLELGh9o1oxWhVQHlqGgWMcZbQcTXRy4Gr5j2k5K1NYeIUwSOCKWOZr+CltU5KQdrzT"
		"8sTZtR2RAIpuE0EgZ+AFnGZ3wFmNNxmJGJzBJcOmXRU2EYHT3a9fpijcQ2Q+ljBC3QUhMxXhE2jecYKC"
		"+LDNrfkk3tmKipSW0zF31Dzv6QbqKFuwOKQe+ReGe0VKqBak3YsLSXAqhZ4DXLmhurAVEZuYNS5+w94M"
		"4ySO9Cx+rX0D/b+5q8Bo/elXP8uameegKDfIu7LfAOeM8HehcDe27u3KiZxm20ysY87M5+I3Fqhl4VHZ"
		"fDA2ctRisqVxocD5eBdOIV8uFIvoKEPaNqOrvjbPf33JK5Uh6Iw4QOBa07yrBh+NN0Dc58zxJ9kXByXo"
		"bk1lnr7S/qYL2MuGOVGXcY5WUIdk+jcijgazL60hw4TtbdwYiiTmPdKwk0YiKXpczfSPNRh5Gi86mS+j"
		"3clFqC/kABJ1KuAjDSdR0RMZn70Zbb6N/XIkCmNlcqURzGQgQ3GTruoY4APxM70eCgGgsLx/ZibJTcYS"
		"ZKR5rUO0gKPNRTdkcIPsQu82v3VVs+zOHwlvBVwBWI1WqwWd4CJ7dxuNVgThTZyIv/1HkJtamV9VzO0c"
		"QuyY9gV0klgDvIHo8eq7rd8G0/ACyCDy+zZ3A2bxQYcCh31n6y4kvQlpZF7SNh3VE4caEcT27TBPM9iu"
		"Bo0HWO5CH5/gv9rUbV/la80H972+d1a2vSxnZA7L40nbJ+Sg517acq/Sn2RLbW0ZofUFj+UlNfLjtHDL"
		"8dMymY/qL6OezrESFxeYoEb74GLuSh6KdKkqAv2n9ehVG5jFCoZGP4jUDZfN6ZOZH/aL5+zVFAlbMFd1"
		"PQZaCG5IOGZSj7jmVSjQsYw+NJWLOH0RuU8asZPELmRDvsTiaCn6ivLpBA7QVbKSUmaD0QoZpD1sOsDe"
		"4GQKelK02xepXmY0lQGL6g==";

static hasp_handle_t m_hasp_handle;
static hasp_status_t m_hasp_status;
static hasp_time_t m_hasp_time;

//  main function to call 
BOOL hasp_key_check(char *msg);
BOOL get_hasp_key_real_time(char *msg,SYSTEMTIME & systemTime);
BOOL hasp_key_write_check_flag(char *msg,BOOL checkFlag);
BOOL hasp_key_read_check_flag(char *msg,BOOL &checkFlag);
BOOL hasp_key_write_video_sum(char *msg,int videoSum);
BOOL hasp_key_read_video_sum(char *msg,int &videoSum);

// sub function 
BOOL hasp_key_login(char *msg);
BOOL hasp_key_logout(char *msg);
BOOL hasp_key_real_time(char *msg,struct HaspKeyTime &haspTime);

BOOL hasp_key_write_data(char *msg,char *uuid,char * lib_apc_key,struct HaspKeyTime expireDate,BOOL checkFlag,int videoSum,char *aes_key);
BOOL hasp_key_read_data(char *msg,char *uuid,char * lib_apc_key,SYSTEMTIME & expireDate,BOOL &checkFlag, int & videoSum,char *aes_key);


BOOL hasp_key_encrypt_data(char *msg,void *buffer,hasp_size_t len);
BOOL hasp_key_decrypt_data(char *msg,void *buffer,hasp_size_t len);

SYSTEMTIME transToSystemTime(HaspKeyTime time);
void  stringCopy(char *strTarget, char *strSource,int lenTarget);

BOOL SystemTimeToVariantTimeWithMilliseconds (/*input*/ SYSTEMTIME st, /*output*/double *dVariantTime);
BOOL VariantTimeToSystemTimeWithMilliseconds (/*input*/ double dVariantTime, /*output*/SYSTEMTIME *st);

//------------------------------------------------------------------------------------------------------
BOOL getSysUUID(char *msg,char uuid[37]);
BOOL checkUUID(char *msg, unsigned char *buffer);

BOOL checkMotionExpireDate(char *msg, unsigned char *buffer);

BOOL checkDate(char *dateString, SYSTEMTIME timeNow);
