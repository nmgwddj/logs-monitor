#include "stdafx.h"
#include "file_instance.h"

FileInstance::FileInstance(const std::wstring& file_path)
	: file_(file_path)
{

}

FileInstance::~FileInstance()
{

}

void FileInstance::StartCapture(FileChangedCallback cb)
{
	file_changed_callback_ = cb;
	file_capture_thread_.reset(new std::thread(&FileInstance::CaptureFileThread, this));
	file_capture_thread_->detach();
}

bool FileInstance::StopCapture()
{
	if (find_handle_)
	{
		FindCloseChangeNotification(find_handle_);
		find_handle_ = nullptr;
	}
	stop_capture_ = true;
	return true;
}

bool FileInstance::ClearFile()
{
	bool	result = false;
	HANDLE  file_handle = NULL;

	file_handle = CreateFile(file_.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle)
	{
		result = true;
	}

	CloseHandle(file_handle);
	file_handle = NULL;

	return result;
}

void FileInstance::CaptureFileThread()
{
	std::wstring file_path = file_.substr(0, file_.rfind(L"\\"));
	find_handle_ = FindFirstChangeNotification(file_.substr(0, file_.rfind(L"\\")).c_str(), FALSE, FILE_NOTIFY_CHANGE_SIZE);
	if (!find_handle_)
	{
		QLOG_ERR(L"Failed to watch directory: {0}") << file_path.c_str();
		return;
	}

	while (true)
	{
		if (!file_first_load_ && !stop_capture_)
		{
			// 等待文件大小变更的事件
			DWORD wait = WaitForSingleObject(find_handle_, INFINITE);
			if (wait == WAIT_OBJECT_0)
			{
				if (!FindNextChangeNotification(find_handle_))
				{
					break;
				}
			}
			else
			{
				// 返回不是 WAIT_OBJECT_0 或者句柄已经无效
				break;
			}
		}

		if (stop_capture_)
		{
			break;
		}

		DWORD trunk_file_size = 0;
		curr_file_size_ = GetFileSizeBytes();

		// 当前文件为空，标记是否第一次读取文件为否后，等待文件变更
		if (curr_file_size_ == 0)
		{
			file_first_load_ = false;
			continue;
		}

		// 提高效率，因为监控的是目录，所以目录下所有文件修改都会被监控
		// 如果当前监控文件大小没变，则跳出。
		if (curr_file_size_ == last_file_size_)
		{
			continue;
		}

		// 如果打开文件失败可能文件被删除或者被其他程序占用
		// 这里直接 continue 等待下一次文件变更消息出现就有可能重新创建新的日志文件了
		HANDLE file_handle = CreateFile(file_.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == file_handle)
		{
			continue;
		}

		// 文件增大则截取增大的内容，文件如果减小了则重新截取文件
		if (curr_file_size_ > last_file_size_)
		{
			trunk_file_size = curr_file_size_ - last_file_size_;
			SetFilePointer(file_handle, last_file_size_, NULL, FILE_BEGIN);
		}
		else
		{
			trunk_file_size = curr_file_size_;
			SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
		}

		// 待完成
		// 当要截取的数据过大时，则只截取自己设定值的数据大小

		// 根据要截取的数据大小分配内存
		std::shared_ptr<TCHAR> buffer(new TCHAR[trunk_file_size + 1]);
		ZeroMemory(buffer.get(), trunk_file_size + 1);

		// 开始读取指定大小的数据
		DWORD  read_bytes = 0;
		if (ReadFile(file_handle, buffer.get(), trunk_file_size, &read_bytes, NULL))
		{
			QLOG_APP(L"File {0} changed, current file size = {1}, last file size = {2}") 
				<< file_.c_str() << curr_file_size_ << last_file_size_;

			bool append = curr_file_size_ >= last_file_size_;

			// 回调到上层 UI
			Post2UI(ToWeakCallback([this, buffer, append]() {
				file_changed_callback_(file_, (PCHAR)buffer.get(), append);
			}));

			// 记录最后读取文件的位置
			last_file_size_ = curr_file_size_;
		}

		// 读取一次文件后将是否第一次读取的状态置为否
		if (file_first_load_)
		{
			file_first_load_ = false;
		}

		if (file_handle)
		{
			CloseHandle(file_handle);
			file_handle = NULL;
		}
	}
}

DWORD FileInstance::GetFileSizeBytes()
{
	HANDLE  file_handle = NULL;
	DWORD   file_size = 0;

	file_handle = CreateFile(file_.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle != INVALID_HANDLE_VALUE)
	{
		file_size = GetFileSize(file_handle, NULL);
		CloseHandle(file_handle);
		file_handle = NULL;
	}

	return file_size;
}

