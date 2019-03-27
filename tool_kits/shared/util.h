#pragma once
#include <string>
#include <map>

class QPath
{
public:
	static std::wstring GetAppPath(); //获取exe所在目录，最后有"\\"
	static std::wstring GetLocalAppDataDir(const std::wstring& app_data_dir);
};

class QCommand
{
public:
	static void ParseCommand(const std::wstring &cmd);
	static bool AppStartWidthCommand(const std::wstring &app, const std::wstring &cmd);
	static bool RestartApp(const std::wstring &cmd);
	static std::wstring Get(const std::wstring &key);
	static void Set(const std::wstring &key, const std::wstring &value);
	static void Erase(const std::wstring &key);
private:
	static std::map<std::wstring,std::wstring> key_value_;
};