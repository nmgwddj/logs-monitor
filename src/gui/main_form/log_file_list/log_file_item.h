#pragma once
#include "gui/main_form/main_form.h"
#include "gui/main_form/log_file_session_box/log_file_session_box.h"

using namespace ui;

class CaptureFileInfo;

class LogFileItem : public ListContainerElement
{
public:
	LogFileItem();
	~LogFileItem();

	void InitControl(const std::wstring& file_path, LogFileSessionBox* log_file_session_box, ListBox* log_file_list);
	void ShowRichEdit(bool is_show = true);

private:
	bool OnClearFile(EventArgs* msg);
	bool OnRemoveItem(EventArgs* msg);
	bool OnOpenFile(EventArgs* msg);
	bool OnOpenFolder(EventArgs* msg);

private:
	LogFileSessionBox*				log_file_session_box_;
	ListBox*						file_list_box_;
	std::wstring					file_path_;
};