#pragma once
#include "gui/main_form/main_form.h"

using namespace ui;

class CaptureFileInfo;

class LogFileItem : public ListContainerElement
{
public:
	LogFileItem();
	~LogFileItem();

	void InitControl(const std::wstring& file_path, std::shared_ptr<CaptureFileInfo> capture_file_info, ListBox* log_file_list);
	void ShowRichEdit(bool is_show = true);

private:
	bool OnClearFile(EventArgs* msg);
	bool OnRemoveItem(EventArgs* msg);
	bool OnOpenFile(EventArgs* msg);
	bool OnOpenFolder(EventArgs* msg);

private:
	std::shared_ptr<CaptureFileInfo> capture_file_info_;
	ListBox*						 file_list_box_;
	std::wstring					 file_path_;
};