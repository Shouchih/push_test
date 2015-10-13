#include "stdafx.h"
#include "log.h"

void OutputLog( CString msg)
{
    TRY
    {
        CString path="C:\\Users\\seven\\Desktop\\log.txt";

		//path.Format("%s\\log.txt",m_CurrentApDirectory);
		
		CStdioFile outFile(path, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::typeText);
        CString msLine;
        CTime tt = CTime::GetCurrentTime();

        msLine = tt.Format("[%Y-%B-%d %A, %H:%M:%S] ") + msg;
        msLine += "\n";

        outFile.SeekToEnd();
        outFile.WriteString( msLine );
        outFile.Close();
    }
    CATCH(CException,e){
			CString sMessage=_T("");
			TCHAR   szCause[255];
			e->GetErrorMessage(  szCause,255,NULL);
			sMessage.Format(_T("At CMotionCurveEditorDoc::OutputLog ,exception = \"%s\"."),szCause);
			AfxMessageBox(sMessage);
			//e->ReportError();
	}
	END_CATCH
}


