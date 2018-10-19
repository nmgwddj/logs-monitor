#pragma once
#include "module/file_manager/file_instance.h"

using namespace ui;

class KeywordInfo;

class LogFileSessionBox : public VBox
{
public:
	LogFileSessionBox();
	~LogFileSessionBox();

	void InitControl(const std::wstring& file_path, ListBox* log_file_list);
	void Clear();

	void StartCapture();
	void StopCapture();

	bool OnClicked(EventArgs* msg);
	void OnFileChangeCallback(const std::wstring& log_file, const std::string& data, bool append = true);

	static const LPCTSTR kFindString;
	static const LPCTSTR kReplaceString;

private:
	RichEdit*	keyword_rich_edit_ = nullptr;
	RichEdit*	rich_edit_ = nullptr;
	Button*		btn_add_keyword_ = nullptr;

	// data
	std::shared_ptr<FileInstance>	file_instance_;			// 文件状态管理
	std::vector<KeywordInfo>		keyword_filter_list_;
};

class KeywordInfo
{
public:
	void SetTextColor(const std::wstring& color) { color_text_ = color; }
	std::wstring& GetTextColor() { return color_text_; }

	void SetFullLine(bool full_line) { full_line_ = full_line; }
	bool GetFullLine() { return full_line_; }

	void SetKeyword(const std::wstring& keyword) { keyword_ = keyword; }
	std::wstring& GetKeyword(){ return keyword_; }

private:
	std::wstring		keyword_;				// 关键字
	std::wstring		color_text_;			// 关键字着色颜色，必须在 global 中已经定义好
	bool				full_line_ = false;		// 是否高亮一整行
};