#pragma once
#include <thread>
#include "base/callback/callback.h"

typedef std::function<void(const std::wstring&, const std::string&, bool)> FileChangedCallback;
typedef std::shared_ptr<std::thread> SharedPtrThread;

class FileInstance : public nbase::SupportWeakCallback
{
public:
	FileInstance(const std::wstring& file_path);
	~FileInstance();

	/**
	 * 开启捕获线程
	 */
	void StartCapture(FileChangedCallback cb);

	/**
	 * 尚未完成，停止捕获的线程
	 */
	bool StopCapture();

	/**
	 * 将文件内容归零
	 */
	bool ClearFile();

private:
	void CaptureFileThread();
	DWORD GetFileSizeBytes();

private:
	HANDLE				find_handle_ = nullptr;		// 文件监控句柄
	std::wstring		file_;						// 文件完整路径
	SharedPtrThread		file_capture_thread_;		// 捕获文件变更的线程
	FileChangedCallback file_changed_callback_;		// 外部传递进来的回调函数

	DWORD				last_file_size_ = 0;		// 记录最后一次读取文件改变的大小（相对于文件的位置）
	DWORD				curr_file_size_ = 0;		// 记录当前文件变更后读取的文件位置，与最后一次对比得出是否需要读取新数据

	bool				file_first_load_ = true;	// 决定是否是第一次打开文件来 load 整个文件数据
	bool				stop_capture_ = false;
};