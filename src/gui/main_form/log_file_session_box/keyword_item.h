#pragma once

using namespace ui;

class KeywordItem : public ListContainerElement
{
public:
	KeywordItem();
	~KeywordItem();

	void InitControl(const std::wstring& keyword, ListBox* keyword_list);
	std::wstring& GetKeyword() { return keyword_; }

	bool OnClicked(EventArgs* msg);

private:
	std::wstring	keyword_;
	std::wstring	color_;
	bool			color_line_ = false;

	Label*			label_keyword_;
	Button*			btn_colors_;
	Button*			btn_remove_;
};