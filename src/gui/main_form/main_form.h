#pragma once
#include "util/window_ex.h"
#include "base/memory/singleton.h"
#include "module/file_manager/file_instance.h"

#include "gui/main_form/log_file_list/log_file_item.h"

using namespace ui;

class CaptureFileInfo;

typedef std::map<std::wstring, std::shared_ptr<CaptureFileInfo>> CaptureFileList;

class MainForm : public nim_comp::WindowEx
{
public:
    MainForm();
    ~MainForm();

    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;
    virtual std::wstring GetWindowClassName() const override;
    virtual std::wstring GetWindowId() const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    virtual void InitWindow() override;

    static const LPCTSTR kClassName;
    static const LPCTSTR kSkinFolder;
    static const LPCTSTR kSkinFile;

	static const LPCSTR kFindString;
	static const LPCSTR kReplaceString;

public:
    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

public:
	void RemoveTrackFile(const std::wstring& file);

private:
	void OnFileSelected(BOOL result, std::wstring file_path);
	void OnLogFileChanged(const std::string& log_file, const std::string& data);

private:
    bool Notify(EventArgs* msg);
    bool OnClicked(EventArgs* msg);
	bool OnSelChanged(EventArgs* msg);
	void OnWndSizeMax(bool max);

private:
	ListBox*		list_logs_;
	RichEdit*		rich_edit_;
	Button*			btn_hide_loglist_;
	Button*			btn_show_loglist_;
	HBox*			box_container_;
	Box*			box_side_bar_;

	CaptureFileList	capture_file_list_;
};

class CaptureFileInfo
{
public:
	std::shared_ptr<FileInstance>	file_instance_;	// 文件状态管理
	RichEdit*						rich_edit_;
};
