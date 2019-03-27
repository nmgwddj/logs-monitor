#pragma once
#include "module/file_manager/file_instance.h"
#include "gui/main_form/log_file_session_box/keyword_item.h"

using namespace ui;

class KeywordItem;

class LogFileSessionBox : public VBox
{
public:
	LogFileSessionBox();
	~LogFileSessionBox();

	void InitControl(const std::wstring& file_path, ListBox* log_file_list);
	void Clear();

	void StartCapture();
	void StopCapture();
	void RemoveKeyword(KeywordItem* keyword_item);

	bool OnClicked(EventArgs* msg);
	bool OnReturnKeyDown(EventArgs* msg);
	void OnFileChangedCallback(const std::wstring& log_file, const std::string& data, bool append = true);

	static const LPCTSTR kFindString;
	static const LPCTSTR kReplaceString;

private:
	bool AddKeyword();

private:
	RichEdit*	keyword_input_ = nullptr;
	RichEdit*	log_content_ = nullptr;
	ListBox*	keyword_list_ = nullptr;
	Button*		keyword_add_ = nullptr;
	Button*		start_capture_ = nullptr;
	Button*		stop_capture_ = nullptr;
	HBox*		global_message_ = nullptr;

	// data
	std::shared_ptr<FileInstance>	log_instance_;			// 文件状态管理
	std::list<KeywordItem*>			keyword_filter_list_;
};
