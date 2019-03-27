#include "stdafx.h"
#include "main.h"
#include "app_dump.h"
#include "base/util/at_exit.h"
#include "util/windows_manager.h"
#include "gui/main_form/main_form.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	nbase::AtExitManager at_manager;

	CComModule _Module;
	_Module.Init(NULL, hInstance);
	_wsetlocale(LC_ALL, L"chs");

#ifdef _DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	wprintf_s(_T("Command: %s\n\n"), lpCmdLine);
#endif

	// 创建日志目录并定义日志文件名
	std::wstring log_dir = QPath::GetLocalAppDataDir(APP_DATA_DIR);
	if (!nbase::FilePathIsExist(log_dir, true))
	{
		nbase::CreateDirectory(log_dir);
	}
	QLogImpl::GetInstance()->SetLogLevel(LV_APP);
#ifdef _DEBUG
	QLogImpl::GetInstance()->SetLogFile(log_dir + _T("logs_monitor_d.log"));
#else
	QLogImpl::GetInstance()->SetLogFile(log_dir + _T("logs_monitor.log"));
#endif

	srand((unsigned int)time(NULL));
	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	QCommand::ParseCommand(lpCmdLine);

	HRESULT hr = ::OleInitialize(NULL);
	if (FAILED(hr))
	{
		return 0;
	}

	// 创建主线程并执行主线程循环
	MainThread thread;
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	_Module.Term();
	::OleUninitialize();

#ifdef _DEBUG
	fclose(fp);
	FreeConsole();
#endif

	return 0;
}

void MainThread::Init()
{
	std::wstring app_crash = QCommand::Get(kCmdAppCrash);

	PreMessageLoop();
	nbase::ThreadManager::RegisterThread(kThreadUI);

	if (!app_crash.empty())
	{
		QLOG_APP(_T("App was crashed: {0}")) << app_crash;
		return;
	}

	std::wstring theme_dir = QPath::GetAppPath();
	ui::GlobalManager::Startup(theme_dir + L"themes\\default", ui::CreateControlCallback(), false, L"");

	nim_comp::WindowsManager::SingletonShow<MainForm>(MainForm::kClassName);
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
	PostMessageLoop();
}

void MainThread::PreMessageLoop()
{
	misc_thread_.reset(new MiscThread(kThreadGlobalMisc, "Global Misc Thread"));
	misc_thread_->Start();

	db_thread_.reset(new MiscThread(kThreadDatabase, "Database Thread"));
	db_thread_->Start();
}

void MainThread::PostMessageLoop()
{
	misc_thread_->Stop();
	misc_thread_.reset(nullptr);

	db_thread_->Stop();
	db_thread_.reset(nullptr);
}
