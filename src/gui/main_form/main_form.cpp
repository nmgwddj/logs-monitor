#include "stdafx.h"
#include "resource.h"
#include "main_form.h"

#include "shared/modal_wnd/file_dialog_ex.h"
#include "shared/ui/msgbox.h"

#include "gui/main_form/log_file_list/log_file_item.h"

const LPCTSTR MainForm::kClassName = _T("main_form");
const LPCTSTR MainForm::kSkinFolder = _T("main_form");
const LPCTSTR MainForm::kSkinFile = _T("main_form.xml");

const LPCSTR MainForm::kFindString = "\r\r";
const LPCSTR MainForm::kReplaceString = "\r";

MainForm::MainForm()
{
}


MainForm::~MainForm()
{
}

std::wstring MainForm::GetSkinFolder()
{
	return kSkinFolder;
}

std::wstring MainForm::GetSkinFile()
{
	return kSkinFile;
}

std::wstring MainForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MainForm::GetWindowId() const
{
	return kClassName;
}

LRESULT MainForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
		{
			OnWndSizeMax(false);
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			OnWndSizeMax(true);
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MainForm::InitWindow()
{
	// 设置窗口信息
	SetIcon(IDI_LOGS_MONITOR);
	SetTaskbarTitle(L"Logs Monitor");

	m_pRoot->AttachBubbledEvent(kEventAll, nbase::Bind(&MainForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(kEventClick, nbase::Bind(&MainForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(kEventSelect, nbase::Bind(&MainForm::OnSelChanged, this, std::placeholders::_1));

	box_side_bar_		= dynamic_cast<Box*>(FindControl(L"box_side_bar"));
	list_logs_			= dynamic_cast<ListBox*>(FindControl(L"list_logs"));
	rich_edit_			= dynamic_cast<RichEdit*>(FindControl(L"log_edit"));
	box_container_		= dynamic_cast<HBox*>(FindControl(L"box_container"));

	btn_hide_loglist_	= dynamic_cast<Button*>(FindControl(L"btn_hide_loglist"));
	btn_show_loglist_	= dynamic_cast<Button*>(FindControl(L"btn_show_loglist"));
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void MainForm::RemoveTrackFile(const std::wstring& file)
{
	capture_file_list_.erase(file);
}

void MainForm::OnFileSelected(BOOL result, std::wstring file_path)
{
	if (result)
	{
		// 检查文件是否已经存在
		auto capture_file_iter = capture_file_list_.find(file_path);
		if (capture_file_iter != capture_file_list_.end())
		{
			ShowMsgBox(GetHWND(), nullptr, L"这个文件已经在监控列表中了", false, L"提示", false, L"确定", false, L"", false);
			return;
		}

		// 给新添加的文件创建相关成员
		std::shared_ptr<CaptureFileInfo> capture_file_info(new CaptureFileInfo);
		capture_file_info->file_instance_.reset(new FileInstance(file_path.c_str()));
		capture_file_info->rich_edit_ = new RichEdit;
		ui::GlobalManager::FillBoxWithCache(capture_file_info->rich_edit_, L"main_form/rich_edit_template.xml");

		// 将原来列表中所有监控的 richedit 都设置为隐藏
		for (auto capture_file_info : capture_file_list_)
		{
			capture_file_info.second->rich_edit_->SetVisible(false);
		}

		// 将新添加进来的 richedit 添加到布局中并显示
		box_container_->Add(capture_file_info->rich_edit_);

		// 新建一个列表项
		LogFileItem* item = new LogFileItem;
		item->InitControl(file_path, capture_file_info, list_logs_);

		// 开始捕获文件变更
		capture_file_info->file_instance_->StartCapture(nbase::Bind(&MainForm::OnLogFileChanged, this,
			std::placeholders::_1, std::placeholders::_2));

		capture_file_list_[file_path] = capture_file_info;
	}
}

void MainForm::OnLogFileChanged(const std::string& log_file, const std::string& data)
{
	auto capture_file_info = capture_file_list_.find(nbase::UTF8ToUTF16(log_file));
	if (capture_file_info != capture_file_list_.end())
	{
		auto rich_edit = capture_file_info->second->rich_edit_;
		// if (!rich_edit->IsVisible()) return;
		std::string replace_data = data;
		nbase::StringReplaceAll(kFindString, kReplaceString, replace_data);
		rich_edit->AppendText(nbase::UTF8ToUTF16(replace_data));
		rich_edit->EndDown();
	}
}

bool MainForm::Notify(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	return true;
}

bool MainForm::OnClicked(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"btn_new_file")
	{
		std::wstring file_type = L"日志文件 (*.*)";
		LPCTSTR filter = L"*.*";
		std::map<LPCTSTR, LPCTSTR> filters;
		filters[file_type.c_str()] = filter;

		CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&MainForm::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2);

		CFileDialogEx* file_dlg = new CFileDialogEx();
		file_dlg->SetFilter(filters);
		file_dlg->SetMultiSel(TRUE);
		file_dlg->SetParentWnd(this->GetHWND());
		file_dlg->AyncShowOpenFileDlg(cb);
	}
	else if (name == L"btn_refresh_all")
	{
		for (auto capture_file_info : capture_file_list_)
		{
			capture_file_info.second->file_instance_->ClearFile();
			capture_file_info.second->rich_edit_->SetText(L"");
		}
	}
	else if (name == L"btn_remove_all")
	{
		for (auto capture_file_info : capture_file_list_)
		{
			// 从列表界面中删除
			capture_file_info.second->file_instance_->StopCapture();
			capture_file_info.second->rich_edit_->SetText(L"");
			capture_file_info.second->rich_edit_->SetVisible(false);
		}
		capture_file_list_.clear();
		list_logs_->RemoveAll();
	}
	else if (name == L"btn_hide_loglist")
	{
		box_side_bar_->SetVisible(false);
		btn_hide_loglist_->SetVisible(false);
		btn_show_loglist_->SetVisible(true);
	}
	else if (name == L"btn_show_loglist")
	{
		box_side_bar_->SetVisible(true);
		btn_hide_loglist_->SetVisible(true);
		btn_show_loglist_->SetVisible(false);
	}
	return true;
}

bool MainForm::OnSelChanged(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"log_file_item")
	{
		std::wstring item_data = msg->pSender->GetDataID();
		for (auto capture_file_info : capture_file_list_)
		{
			if (capture_file_info.first == item_data)
			{
				capture_file_info.second->rich_edit_->SetVisible(true);
				capture_file_info.second->rich_edit_->EndDown();
			}
			else
			{
				capture_file_info.second->rich_edit_->SetVisible(false);
			}
		}
	}
	return true;
}

void MainForm::OnWndSizeMax(bool max)
{
	if (!m_pRoot)
	{
		return;
	}

	FindControl(L"btn_wnd_max")->SetVisible(!max);
	FindControl(L"btn_wnd_restore")->SetVisible(max);
}
