#include "stdAfx.h"
#include "xmlFileUtil.h"
#include "SentinelHaspUtilityDoc.h"
#include "SentinelHaspApiLib\haspUtil.h"
#include "log.h"

using namespace std;  
using namespace tinyxml2; 

//------------------------------------------------------------------------------------------------------------------------------------------
void saveXMLConfigParam(const char * filePath,struct ConfigParam param){

		TRY{

		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();   
		tinyxml2::XMLDeclaration* decl = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");    
		pDoc->LinkEndChild( decl );  

		tinyxml2::XMLElement * root = pDoc->NewElement( "BasicConfigData" );  
		pDoc->LinkEndChild( root );  

		CString strValue;

		//-------------------------------------------------------------------------------------------------
		//  char name[80]; 
		tinyxml2::XMLElement * element = pDoc->NewElement( "Name" );  
		strValue.Format("%s",param.name);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element );  

		//-------------------------------------------------------------------------------------------------
		// char UUID[80]; 
		element = pDoc->NewElement( "UUID" );  
		strValue.Format("%s",param.UUID);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element );  

		//-------------------------------------------------------------------------------------------------
		// struct HaspKeyTime  keyExpireDate;
		element =  pDoc->NewElement( "ExpireDate_YEAR" );  
		strValue.Format("%d",param.keyExpireDate.year);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element );  

		element =  pDoc->NewElement( "ExpireDate_MONTH" );  
		strValue.Format("%d",param.keyExpireDate.month);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element );  

		element =  pDoc->NewElement( "ExpireDate_DAY" );  
		strValue.Format("%d",param.keyExpireDate.day);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element ); 

		//-------------------------------------------------------------------------------------------------
		// BOOL checkFlag;
		element =  pDoc->NewElement( "CheckFlag" );  
		if(param.checkFlag==TRUE){
			strValue="1";
		}else{
			strValue="0";
		}
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element ); 

		//--------------------------------------------------------------------------------------------------
		//  char LIB_APC_KEY[32];
		element = pDoc->NewElement( "LIB_APC_KEY" );  
		strValue.Format("%s",param.LIB_APC_KEY);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element );  

		//--------------------------------------------------------------------------------------------------
		// int videoSum;
		element =  pDoc->NewElement( "VideoSum" );  
		strValue.Format("%d",param.videoSum);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element ); 

		//--------------------------------------------------------------------------------------------------
		// char AES_KEY[16];
		element = pDoc->NewElement( "AES_KEY" );  
		strValue.Format("%s",param.AES_KEY);
		element->LinkEndChild( pDoc->NewText( (LPCTSTR)strValue ));  
		root->LinkEndChild( element );  

		pDoc->SaveFile( filePath ); 

		AfxMessageBox("Save config conplete.");

		/*if (pDoc) {  
		delete pDoc;  
		}  */

	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At saveXMLConfigParam() ,exception = \"%s\"."),szCause);
		AfxMessageBox(sMessage);
		//e->ReportError();
	}
	END_CATCH

}


bool readXMLConfigParam(const char * filePath,struct ConfigParam &param){

	bool flag=false;

	// Load XML file
	TRY{

		CString strValue;

		tinyxml2::XMLDocument doc;

		if(doc.LoadFile( filePath)==0){

			tinyxml2::XMLElement * root=doc.FirstChildElement( "BasicConfigData" );

			if( root){
				// -------------------------------------------------------------------------------------------------
				// char name[80]; ;
				tinyxml2::XMLElement* element = root->FirstChildElement( "Name" );
				if(element ){	
					if(element->GetText()){
						stringCopy((char *)param.name,(char *)element->GetText(),sizeof(param.name));
					}
				}

				// -------------------------------------------------------------------------------------------------
				//  char UUID[80];
				element = root->FirstChildElement( "UUID" );
				if(element ){	
					if(element->GetText()){
						stringCopy((char *)param.UUID,(char *)element->GetText(),sizeof(param.UUID));
					}
				}

				// -------------------------------------------------------------------------------------------------
				// struct HaspKeyTime  keyExpireDate;
				param.keyExpireDate.year=0;
				param.keyExpireDate.month=0;
				param.keyExpireDate.day=0;
				param.keyExpireDate.hour=0;
				param.keyExpireDate.minute=0;
				param.keyExpireDate.second=0;

				element = root->FirstChildElement( "ExpireDate_YEAR" );
				if(element ){
					if(element->GetText()){
						param.keyExpireDate.year=(short)atoi(element->GetText());
					}
				}
				element = root->FirstChildElement( "ExpireDate_MONTH" );
				if(element ){
					if(element->GetText()){
						param.keyExpireDate.month=(short)atoi(element->GetText());
					}
				}
				element = root->FirstChildElement( "ExpireDate_DAY" );
				if(element ){
					if(element->GetText()){
						param.keyExpireDate.day=(short)atoi(element->GetText());
					}
				}

				// -------------------------------------------------------------------------------------------------
				// BOOL checkFlag;
				param.checkFlag=FALSE;
				element = root->FirstChildElement( "CheckFlag" );
				if(element ){
					if(element->GetText()){
						int temp=0;
						temp=(short)atoi(element->GetText());
						if(temp>0){
							param.checkFlag=TRUE;
						}
					}
				}

				// -------------------------------------------------------------------------------------------------
				// char LIB_APC_KEY[32];
				element = root->FirstChildElement( "LIB_APC_KEY" );
				if(element ){	
					if(element->GetText()){
						stringCopy((char *)param.LIB_APC_KEY,(char *)element->GetText(),sizeof(param.LIB_APC_KEY));
					}
				}

				// -------------------------------------------------------------------------------------------------
				// int videoSum;
				param.videoSum=0;
				element = root->FirstChildElement( "VideoSum" );
				if(element ){
					if(element->GetText()){
						param.videoSum=(short)atoi(element->GetText());
					}
				}

				// -------------------------------------------------------------------------------------------------
				// char AES_KEY[16];
				element = root->FirstChildElement( "AES_KEY" );
				if(element ){	
					if(element->GetText()){
						stringCopy((char *)param.AES_KEY,(char *)element->GetText(),sizeof(param.AES_KEY));
					}
				}

				flag=true;
				return flag;
			}
		}

	}CATCH(CException,e){
		CString sMessage=_T("");
		TCHAR   szCause[255];
		e->GetErrorMessage(  szCause,255,NULL);
		sMessage.Format(_T("At readXMLConfigParam() ,exception = \"%s\"."),szCause);
		AfxMessageBox(sMessage);

		return false;

		//e->ReportError();
	}
	END_CATCH

		return flag;

}
