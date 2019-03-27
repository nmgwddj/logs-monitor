#include "stdafx.h"
#include "keyword_item.h"

KeywordItem::KeywordItem()
{

}

KeywordItem::~KeywordItem()
{

}

void KeywordItem::InitControl(const std::wstring& keyword, ListBox* keyword_list, LogFileSessionBox* parent_box)
{
	GlobalManager::FillBoxWithCache(this, L"main_form/keywords/keyword_item.xml");
	
	this->AttachBubbledEvent(kEventClick, nbase::Bind(&KeywordItem::OnClicked, this, std::placeholders::_1));

	keyword_list_ = keyword_list;
	parent_box_ = parent_box;
	keyword_ = keyword;

	color_list_.emplace_back(L"keyword_blue");
	color_list_.emplace_back(L"keyword_green");
	color_list_.emplace_back(L"keyword_yellow");
	color_list_.emplace_back(L"keyword_red");
	color_list_.emplace_back(L"keyword_orange");
	color_list_.emplace_back(L"keyword_purple");

	ChooseColor();

	label_keyword_ = dynamic_cast<Label*>(FindSubControl(L"label_keyword"));
	btn_colors_ = dynamic_cast<Button*>(FindSubControl(L"btn_colors"));
	btn_remove_ = dynamic_cast<Button*>(FindSubControl(L"btn_remove"));

	label_keyword_->SetText(keyword_);
	keyword_list->Add(this);
}

bool KeywordItem::OnClicked(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();

	if (name == L"btn_colors")
	{
		ChooseColor();
	}
	else if (name == L"btn_remove")
	{
		keyword_list_->Remove(this);
		parent_box_->RemoveKeyword(this);
	}

	return true;
}

void KeywordItem::ChooseColor()
{
	color_ = color_list_.front();

	color_list_.pop_front();
	SetBkColor(color_);

	color_list_.emplace_back(color_);
}
