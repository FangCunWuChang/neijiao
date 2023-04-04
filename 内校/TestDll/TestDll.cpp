
// TestDll.cpp : 定义应用程序的类行为。
//

#include  <crtdbg.h> //leak check

#include "stdafx.h"
#include "TestDll.h"
#include "TestDllDlg.h"
#include "DlgCam4.h"
#include "DlgBK.h"
#include "DlgMes.h"

#define UNIQUE_APP "MES"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDllApp

BEGIN_MESSAGE_MAP(CTestDllApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestDllApp 构造

CTestDllApp::CTestDllApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

	// leak check
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(long(818));
}


// 唯一的一个 CTestDllApp 对象

CTestDllApp theApp;


// CTestDllApp 初始化
#include <afxsock.h>
bool CTestDllApp::LinkDB(void)
{
	CString str;
	CSingleton* pSng = CSingleton::GetInstance();
	if (!AfxSocketInit())
		return false;
	char  HostName[128];
	CString StrName;
	if (gethostname(HostName, 128) == 0)
	{
		struct hostent * pHost;
		pHost = gethostbyname(HostName);
		StrName += HostName;
		pSng->_strHostName.Format("%s", HostName);
	}
	CString sql;
	sql.Format("Provider=SQLOLEDB.1;Persist Security Info = False;User ID = sa;Password = 123;Initial Catalog = ST;Data Source =");
	sql += StrName;
	sql = "Driver={SQLite3 ODBC Driver};Database=D:\\WUHAN.db;StepAPI=0;SyncPragma=;NoTXN=0;Timeout=;ShortNames=0;LongNames=0;NoCreat=0;NoWCHAR=1;FKSupport=0;JournalMode=;OEMCP=0;LoadExt=;BigInt=0;JDConv=0;PWD=";
	if (!pSng->_DB.Open(sql))
	{
		MessageBox(NULL, _T("没有连接到服务器数据库"), _T("系统提示"), MB_ICONWARNING);
		return false;
	}
	return true;
}

BOOL CTestDllApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	CSingleton* pSng = CSingleton::GetInstance();

	HANDLE hOne = ::CreateMutexA(NULL, FALSE, UNIQUE_APP);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox("已经启动了一个程序！");
		CloseHandle(hOne);
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}
		CSingleton::DeleteInstance();
		return FALSE;
	}

	if (LinkDB() == false)
	{
		CloseHandle(hOne);
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}
		CSingleton::DeleteInstance();
		return FALSE;
	}
	LogFile("数据库连接成功！");

	//CDlgBK dlg;
	CDlgMes dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	if (auto pSng = CSingleton::GetInstance()) {
		if (pSng->_DB.IsOpen()) {
			pSng->_DB.Close();
		}
	}

	CloseHandle(hOne);

	CSingleton::DeleteInstance();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

