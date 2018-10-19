#pragma once
#include "util/window_ex.h"
#include "base/memory/singleton.h"
#include "module/file_manager/file_instance.h"

#include "gui/main_form/log_file_list/log_file_item.h"
#include "gui/main_form/log_file_session_box/log_file_session_box.h"

using namespace ui;

class CaptureFileInfo;

typedef std::map<std::wstring, LogFileSessionBox*> CaptureFileList;

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
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

    virtual void InitWindow() override;

public:
	static const LPCTSTR kClassName;
	static const LPCTSTR kSkinFolder;
	static const LPCTSTR kSkinFile;

public:
	void RemoveTrackFile(const std::wstring& file);

private:
	void OnFileSelected(BOOL result, std::wstring file_path);

private:
    bool Notify(EventArgs* msg);
    bool OnClicked(EventArgs* msg);
	bool OnSelChanged(EventArgs* msg);
	void OnWndSizeMax(bool max);

private:
	ListBox*		list_logs_;
	Button*			btn_hide_loglist_;
	Button*			btn_show_loglist_;
	Box*			box_container_;
	Box*			box_side_bar_;

	CaptureFileList	capture_file_list_;
};
