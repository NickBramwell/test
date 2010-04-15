// JavaService.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "JavaService_i.h"

#include <stdio.h>

class CJavaServiceModule : public CAtlServiceModuleT< CJavaServiceModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_JavaServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_JAVASERVICE, "{39AE592D-1665-43CB-BB7B-D8CFE80529CB}")
	DWORD dwProcessId;
	
	CJavaServiceModule()
	{
		InitializeSecurity();
	}
	
	HRESULT InitializeSecurity() throw()
	{
		CSecurityDescriptor sd;
		sd.InitializeFromThreadToken();
		HRESULT hr = CoInitializeSecurity(sd, -1, NULL, NULL,
			RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);
		
		
		return hr;
	}

	HRESULT Run(int nShowCmd = SW_HIDE)
	{
		HRESULT hr=S_OK;
		
		// Start worker thread here 		
		std::wstring appexe;
		std::wstring params;
		hr = getParamsFromXMLConfigFile(STARTUP,&appexe,&params);						
		if (hr==S_OK) 
		{
			LogEvent(_T("Started"));						
			hr = ExecuteProcess(appexe, params,10);
			if (hr==S_OK) 
			{
				hr = __super::Run(nShowCmd);
			}
		}
		return hr;

	}


	HRESULT getParamsFromXMLConfigFile(short mode, std::wstring * appexe, std::wstring * params)
	{
		::CoInitialize(NULL);
		MSXML::IXMLDOMDocumentPtr m_plDomDocument;
		MSXML::IXMLDOMElementPtr m_pDocRoot;

		HRESULT hr = m_plDomDocument.CreateInstance(MSXML::CLSID_DOMDocument);
		if (hr==S_OK)
		{		
			_bstr_t bstrFileName = _T("JavaService.xml");
			variant_t vResult;
			vResult = m_plDomDocument->load(bstrFileName);
			if (((bool)vResult) == TRUE) // success!
			{
				if (mode==STARTUP)
				{
					// now that the document is loaded, we need to initialize the root pointer
					m_pDocRoot = m_plDomDocument->documentElement;
					MSXML::IXMLDOMNodePtr jdkRoot = m_pDocRoot->selectSingleNode("//startup/jdk/javahome");
					MSXML::IXMLDOMNodePtr jvmmode = m_pDocRoot->selectSingleNode("//startup/jdk/jvmmode");
					MSXML::IXMLDOMNodePtr jvmclasspath = m_pDocRoot->selectSingleNode("//startup/jdk/classpath");
					MSXML::IXMLDOMNodePtr endorsedpath = m_pDocRoot->selectSingleNode("//startup/jdk/endorsedpath");				
					MSXML::IXMLDOMNodePtr syshost = m_pDocRoot->selectSingleNode("//startup/jdk/syshost");
					MSXML::IXMLDOMNodePtr minheap = m_pDocRoot->selectSingleNode("//startup/jdk/initmemory");
					MSXML::IXMLDOMNodePtr maxheap = m_pDocRoot->selectSingleNode("//startup/jdk/maxmemory");
					MSXML::IXMLDOMNodePtr newsize = m_pDocRoot->selectSingleNode("//startup/jdk/NewSize");
					MSXML::IXMLDOMNodePtr maxnewsize = m_pDocRoot->selectSingleNode("//startup/jdk/MaxNewSize");
					MSXML::IXMLDOMNodePtr survivorratio = m_pDocRoot->selectSingleNode("//startup/jdk/survivorratio");				
					MSXML::IXMLDOMNodePtr permsize = m_pDocRoot->selectSingleNode("//startup/jdk/permsize");
					MSXML::IXMLDOMNodePtr gcInterval = m_pDocRoot->selectSingleNode("//startup/jdk/gcInterval");
					MSXML::IXMLDOMNodePtr threadstacksize = m_pDocRoot->selectSingleNode("//startup/jdk/threadstacksize");
					MSXML::IXMLDOMNodePtr progname = m_pDocRoot->selectSingleNode("//startup/app/progname");				
					MSXML::IXMLDOMNodePtr appclass = m_pDocRoot->selectSingleNode("//startup/app/class");
					MSXML::IXMLDOMNodePtr classrunparams = m_pDocRoot->selectSingleNode("//startup/app/classrunparams");
					MSXML::IXMLDOMNodePtr extrajvmparams = m_pDocRoot->selectSingleNode("//startup/app/extrajvmparams");


					BSTR bjdkRoot;
					hr = jdkRoot->get_text(&bjdkRoot); 
					if (hr==S_OK) 
					{
						*appexe = _bstr_t(bjdkRoot) + _T("\\java.exe");

						BSTR bjvmmode;				
						BSTR bjvmclasspath;
						BSTR bendorsedpath;
						BSTR bsyshost;
						BSTR bminheap;
						BSTR bmaxheap;
						BSTR bnewsize;
						BSTR bmaxnewsize;
						BSTR bsurvivorratio;
						BSTR bpermsize;
						BSTR bgcInterval;
						BSTR bthreadstacksize;
						BSTR bprogname;
						BSTR bappclass;
						BSTR bclassrunparams;
						BSTR bextrajvmparams;

						hr = jvmmode->get_text(&bjvmmode) & 
							jvmclasspath->get_text(&bjvmclasspath) & 
							endorsedpath->get_text(&bendorsedpath) &
							syshost->get_text(&bsyshost) & 
							minheap->get_text(&bminheap) & 
							maxheap->get_text(&bmaxheap) & 
							newsize->get_text(&bnewsize) & 
							maxnewsize->get_text(&bmaxnewsize) & 
							survivorratio->get_text(&bsurvivorratio) &
							permsize->get_text(&bpermsize) & 
							gcInterval->get_text(&bgcInterval) &
							threadstacksize->get_text(&bthreadstacksize) &
							progname->get_text(&bprogname) & 
							appclass->get_text(&bappclass) & 
							classrunparams->get_text(&bclassrunparams) &
							extrajvmparams->get_text(&bextrajvmparams);  
						if (hr==S_OK)
						{		
							*params = _T("-") + _bstr_t(bjvmmode) + _T(" -Dprogram.name=") + _bstr_t(bprogname) 
								+ _T(" -Dsys.host=") + _bstr_t(bsyshost)
								+ _T(" -Xms") + _bstr_t(bminheap) + _T(" -Xmx") + _bstr_t(bmaxheap) 
								+ _T(" -XX:NewSize=") + _bstr_t(bnewsize) + _T(" -XX:MaxNewSize=") + _bstr_t(bmaxnewsize) 
								+ _T(" -XX:SurvivorRatio=") + _bstr_t(bsurvivorratio) 
								+ _T(" -XX:MaxPermSize=") + _bstr_t(bpermsize) 
								+ _T(" -Xss") + _bstr_t(bthreadstacksize)
								+ _T(" -Dsun.rmi.dgc.server.gcInterval=") + _bstr_t(bgcInterval) 
								+ _T(" -Djava.endorsed.dirs=") + _bstr_t(bendorsedpath) 
								+ _T(" ") + _bstr_t(bextrajvmparams) 
								+ _T(" -classpath ") + _bstr_t(bjvmclasspath) 
								+ _T(" ") + _bstr_t(bappclass)
								+ _T(" ") + _bstr_t(bclassrunparams);
						} else {
							LogEvent(_T("Failed to load application params from configuration file"));
						}
					} else {
						LogEvent(_T("Failed to load JDK setting from configuration file"));
					}
				} else {
					// Mode must be STOPPING. Now that the document is loaded, we need to initialize the root pointer
					m_pDocRoot = m_plDomDocument->documentElement;
					MSXML::IXMLDOMNodePtr jdkRoot = m_pDocRoot->selectSingleNode("//shutdown/jdk/javahome");
					
					MSXML::IXMLDOMNodePtr jvmclasspath = m_pDocRoot->selectSingleNode("//shutdown/jdk/classpath");
					MSXML::IXMLDOMNodePtr progname = m_pDocRoot->selectSingleNode("//shutdown/app/progname");				
					MSXML::IXMLDOMNodePtr appclass = m_pDocRoot->selectSingleNode("//shutdown/app/class");					
					MSXML::IXMLDOMNodePtr extrajvmparams = m_pDocRoot->selectSingleNode("//shutdown/app/extrajvmparams");

					BSTR bjdkRoot;
					hr = jdkRoot->get_text(&bjdkRoot); 
					if (hr==S_OK) 
					{
						*appexe = _bstr_t(bjdkRoot) + _T("\\java.exe");

						
						BSTR bjvmclasspath;						
						BSTR bprogname;
						BSTR bappclass;						
						BSTR bextrajvmparams;

						hr = jvmclasspath->get_text(&bjvmclasspath) & 							
							progname->get_text(&bprogname) & 							
							extrajvmparams->get_text(&bextrajvmparams) &
							appclass->get_text(&bappclass);  
						if (hr==S_OK)
						{		
							*params = _T(" -Dprogram.name=") + _bstr_t(bprogname) 								
								+ _T(" ") + _bstr_t(bextrajvmparams) 
								+ _T(" -classpath ") + _bstr_t(bjvmclasspath) 								
								+ _T(" ") + _bstr_t(bappclass);
								
						} else {
							LogEvent(_T("Failed to load application params from configuration file"));
						}
					} else {
						LogEvent(_T("Failed to load JDK setting from configuration file"));
					}
				}
			}
			else
			{
				LogEvent(_T("Failed to load configuration file"));
				return E_FAIL;
			}
		} else {
			LogEvent(_T("Failed to MSXML library."));
		}

		return hr;
	}



	size_t ExecuteProcess(std::wstring FullPathToExe, std::wstring Parameters,
                                 size_t SecondsToWait)
	{
		 size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
		 DWORD dwExitCode = 0;
		 std::wstring sTempStr = L"";
		 
		 /* Add a space to the beginning of the Parameters */
		 if (Parameters.size() != 0)
		 { 
			  if (Parameters[0] != L' ')
			  {
				   Parameters.insert(0,L" ");
			  }
		 }

		 /* The first parameter needs to be the exe itself */
		 sTempStr = FullPathToExe;
		 iPos = sTempStr.find_last_of(L"\\");
		 sTempStr.erase(0, iPos +1);
		 Parameters = sTempStr.append(Parameters);

		 /*
			  CreateProcessW can modify Parameters thus we
			  allocate needed memory
		 */
		 wchar_t * pwszParam = new wchar_t[Parameters.size() + 1];
		 if (pwszParam == 0)
		 {
			  return 1;
		 } 
		 const wchar_t* pchrTemp = Parameters.c_str();
		 wcscpy_s(pwszParam, Parameters.size() + 1, pchrTemp);

		 /* CreateProcess API initialization */
		 STARTUPINFOW siStartupInfo;
		 PROCESS_INFORMATION piProcessInfo;
		 memset(&siStartupInfo, 0, sizeof(siStartupInfo));
		 memset(&piProcessInfo, 0, sizeof(piProcessInfo));
		 siStartupInfo.cb = sizeof(siStartupInfo);

		 int result = CreateProcess(const_cast<LPCWSTR>(FullPathToExe.c_str()),
									  pwszParam, 0, 0, false,
									  CREATE_DEFAULT_ERROR_MODE, 0, 0,
									  &siStartupInfo, &piProcessInfo);
		 if (result !=0)
		 {
			  /* Watch the process. */
			  dwExitCode = WaitForSingleObject(piProcessInfo.hProcess,(SecondsToWait * 1000));
			  dwProcessId = piProcessInfo.dwProcessId;
		 }
		 else
		 {
			  /* CreateProcess failed */
			  iReturnVal = GetLastError();
		 }

		 /* Free memory */
		 delete[]pwszParam;
		 pwszParam = 0;

		 /* Release handles */
		 CloseHandle(piProcessInfo.hProcess);
		 CloseHandle(piProcessInfo.hThread);

		 return iReturnVal;
	}	

	

	void OnStop()
	{
		
		//Kill the JAVA command thread.
		if (dwProcessId)
		{
			//attempt a graceful shutdown. Kill the process if that fails
			std::wstring appexe;
			std::wstring params;
			HRESULT hr = getParamsFromXMLConfigFile(SHUTDOWN,&appexe,&params);						
			if (hr == S_OK) 
			{				
				hr = ExecuteProcess(appexe, params,20);			
				if (hr != S_OK)
				{
					//Failed to shutdown within the specified timeout. Kill process
					BOOL ret = false;
					HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, 0, dwProcessId );
					if (hProcess != NULL)
					{
						//LogEvent(_T("Terminating java process"));
						
						
						UINT exitCode = 0;
						ret = TerminateProcess( hProcess, exitCode );
						if (!ret)
						{
							
							LogEvent(_T("Failed to terminate java process"));
						}
						else
						{					
							//LogEvent(_T("Terminated java process"));
						}
					}
				}
			}
			
			
		}
		// Stop worker-thread
		__super::OnStop();

		LogEvent(_T("JavaService Stopped"));
	}
};

CJavaServiceModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

