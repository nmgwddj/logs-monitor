#pragma once
#include "gui/main_form/log_file_session_box/log_file_session_box.h"

using namespace ui;

class LogFileSessionBox;

class KeywordItem : public ListContainerElement
{
public:
	KeywordItem();
	~KeywordItem();

	void InitControl(const std::wstring& keyword, ListBox* keyword_list, LogFileSessionBox* parent_box);
	std::wstring& GetKeyword() { return keyword_; }
	std::wstring& GetColor() { return color_; }

	

private:
	bool OnClicked(EventArgs* msg);

	void ChooseColor();

private:
	std::wstring		keyword_;
	std::wstring		color_;
	bool				color_line_ = false;
	std::list<std::wstring> color_list_;

	ListBox*			keyword_list_ = nullptr;
	LogFileSessionBox*	parent_box_ = nullptr;
	Label*				label_keyword_ = nullptr;
	Button*				btn_colors_ = nullptr;
	Button*				btn_remove_ = nullptr;
};