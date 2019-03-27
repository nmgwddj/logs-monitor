#include "stdafx.h"
#include "log_file_session_box.h"

const LPCTSTR LogFileSessionBox::kFindString = L"\r\r";
const LPCTSTR LogFileSessionBox::kReplaceString = L"\r";

LogFileSessionBox::LogFileSessionBox()
{

}

LogFileSessionBox::~LogFileSessionBox()
{

}

void LogFileSessionBox::InitControl(const std::wstring& file_path, ListBox* log_file_list)
{
	GlobalManager::FillBoxWithCache(this, L"main_form/log_file_session_box.xml");

	this->AttachBubbledEvent(kEventClick, nbase::Bind(&LogFileSessionBox::OnClicked, this, std::placeholders::_1));
	this->AttachBubbledEvent(kEventReturn, nbase::Bind(&LogFileSessionBox::OnReturnKeyDown, this, std::placeholders::_1));

	keyword_list_	= dynamic_cast<ListBox*>(FindSubControl(L"keyword_list"));
	keyword_input_	= dynamic_cast<RichEdit*>(FindSubControl(L"keyword_input"));
	keyword_add_	= dynamic_cast<Button*>(FindSubControl(L"keyword_add"));
	log_content_	= dynamic_cast<RichEdit*>(FindSubControl(L"log_content"));
	start_capture_	= dynamic_cast<Button*>(FindSubControl(L"start_capture"));
	stop_capture_	= dynamic_cast<Button*>(FindSubControl(L"stop_capture"));
	global_message_	= dynamic_cast<HBox*>(FindSubControl(L"global_message"));

	log_instance_.reset(new FileInstance(file_path.c_str()));
}

void LogFileSessionBox::Clear()
{
	log_content_->SetText(L"");
	log_instance_->ClearFile();
}

void LogFileSessionBox::StartCapture()
{
	log_instance_->StartCapture(nbase::Bind(&LogFileSessionBox::OnFileChangedCallback, this, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void LogFileSessionBox::StopCapture()
{
	log_instance_->StopCapture();
}

void LogFileSessionBox::RemoveKeyword(KeywordItem* keyword_item)
{
	keyword_filter_list_.remove(keyword_item);
	if (keyword_filter_list_.size() == 0)
	{
		global_message_->SetVisible(true);
	}
}

bool LogFileSessionBox::OnClicked(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"keyword_add")
	{
		AddKeyword();
	}
	else if (name == L"start_capture")
	{
		StartCapture();
		stop_capture_->SetVisible(true);
		start_capture_->SetVisible(false);
	}
	else if (name == L"stop_capture")
	{
		StopCapture();
		start_capture_->SetVisible(true);
		stop_capture_->SetVisible(false);
	}

	return true;
}

bool LogFileSessionBox::OnReturnKeyDown(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"keyword_input")
	{
		AddKeyword();
	}

	return 0;
}

void LogFileSessionBox::OnFileChangedCallback(const std::wstring& log_file, const std::string& data, bool append /*= true*/)
{
	if (!append)
	{
		log_content_->SetText(L"");
	}

	// Convert MBCS to UTF8
	std::wstring utf8_str;
	ui::StringHelper::MBCSToUnicode(data, utf8_str, CP_UTF8);

	// Replace \r\r to \r
	nbase::StringReplaceAll(kFindString, kReplaceString, utf8_str);

	// 先记录所有关键字的位置
	std::map<int, KeywordItem*> keywords_pos;
	for (auto keyword : keyword_filter_list_)
	{
		std::size_t begin = 0;
		while (begin < utf8_str.length())
		{
			std::size_t pos = utf8_str.find(keyword->GetKeyword(), begin);
			if (std::string::npos != pos)
			{
				// 发现关键字
				std::wstring normal_text = utf8_str.substr(begin, pos - begin);
				std::wstring color_text = utf8_str.substr(pos, keyword->GetKeyword().length());

				// 记录关键字位置和要截取的关键字长度，插入到 map 中
				keywords_pos[pos] = keyword;
				begin = pos + keyword->GetKeyword().length();
			}
			else
			{
				break;
			}
		}
	}

	// 遍历关键字位置列表，对需要着色的文字拆分进行插入
	size_t last_keyword_pos = 0;
	for (auto keyword_pos : keywords_pos)
	{
		auto begin = keyword_pos.first;
		auto length = keyword_pos.second->GetKeyword().length();
		auto color = keyword_pos.second->GetColor();

		std::wstring normal_text = utf8_str.substr(last_keyword_pos, begin - last_keyword_pos);
		std::wstring color_text = utf8_str.substr(begin, length);

		log_content_->AppendText(normal_text);
		log_content_->AddColorText(color_text, color);

		last_keyword_pos = begin + length;
	}

	// 将遍历关键字后剩余的文字追加到界面中
	if (last_keyword_pos < utf8_str.length())
	{
		log_content_->AppendText(utf8_str.substr(last_keyword_pos, utf8_str.length()));
	}

	log_content_->EndDown();
}

bool LogFileSessionBox::AddKeyword()
{
	std::wstring keyword = keyword_input_->GetText();
	if (keyword.size() == 0)
	{
		return false;
	}

	KeywordItem* item = new KeywordItem;
	item->InitControl(keyword, keyword_list_, this);
	keyword_filter_list_.push_back(item);
	keyword_input_->SetText(L"");

	global_message_->SetVisible(false);

	return true;
}
