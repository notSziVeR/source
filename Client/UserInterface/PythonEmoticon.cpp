#include "StdAfx.h"
#include "../EterLib/TextFileLoader.h"
#include "../EterPack/EterPackManager.h"
#include <string>

#include "PythonEmoticon.h"

struct emoticon_tag_info_t
{
	size_t start;
	size_t end;
};

void make_split_token(const std::string& line, std::vector<std::string>& token, char ch, bool remove_empty)
{
	const auto _count = std::count(line.begin(), line.end(), ch);

	if (_count == 0)
	{
		token = {line};
		return;
	}

	token.clear();
	token.reserve(_count + 1);

	std::vector<size_t> positions;
	positions.reserve(_count);

	size_t index = 0;
	for (const auto& _ch : line)
	{
		if (_ch == ch)
		{
			positions.push_back(index);
		}
		index++;
	}

	int old = 0;
	for (int i = 0; i < positions.size(); i++)
	{
		if (old < line.length())
		{
			const auto pos = positions[i];
			token.push_back(line.substr(old, pos - old));
			old = pos + 1;
		}
	}

	if (old < line.length())
	{
		token.push_back(line.substr(old));
	}

	if (remove_empty)
		token.erase(std::remove_if(token.begin(), token.end(), [](const std::string & r) ->bool {return r.empty(); }), token.end());
}

#define __as_emoticon_flag(path) std::string("|E") + path + "|e"
#define __as_emoticon_flagw(path) std::wstring(L"|E") + path + L"|e"

int replace_in_place_emoticon_flag(std::string& phrase, const std::string& short_sym, const std::string& imagepath, size_t index, std::vector<emoticon_tag_info_t>& out_info)
{
	std::string flag;
	size_t pos = 0;
	int gap = 0;

	while ((pos = phrase.find(short_sym.c_str(), index)) != std::string::npos)
	{
		bool skip = false;
		for (const auto& tag : out_info)
		{
			if (pos >= tag.start && pos < tag.end)
			{
				index = tag.end;
				skip = true;
				break;
			}
		}

		if (skip)
		{
			continue;
		}

		if (flag.empty())
		{
			flag = __as_emoticon_flag(imagepath);
		}

		emoticon_tag_info_t new_tag = {pos, pos + flag.length()};
		out_info.emplace_back(new_tag);

		for (auto& tag : out_info)
		{
			if (tag.start > pos)
			{
				tag.start += flag.length() - short_sym.length();
				tag.end += flag.length() - short_sym.length();
			}
		}

		phrase.replace(pos, short_sym.length(), flag.c_str());
		gap += short_sym.length() - 1;
	}

	return gap;
}

int replace_in_place_emoticon_flag(std::wstring& phrase, const std::string& _short_sym, const std::string& _imagepath, size_t index, std::vector<emoticon_tag_info_t>& out_info)
{
	std::wstring flag = L"", short_sym = { _short_sym.begin(), _short_sym.end() }, imagepath = {_imagepath.begin(), _imagepath.end() };
	size_t pos = 0;
	int gap = 0;

	while ((pos = phrase.find(short_sym.c_str(), index)) != std::string::npos)
	{
		bool skip = false;
		for (const auto& tag : out_info)
		{
			if (pos >= tag.start && pos < tag.end)
			{
				index = tag.end;
				skip = true;
				break;
			}
		}

		if (skip)
		{
			continue;
		}

		if (flag.empty())
		{
			flag = __as_emoticon_flagw(imagepath);
		}

		emoticon_tag_info_t new_tag = { pos, pos + flag.length() };
		out_info.emplace_back(new_tag);

		phrase.replace(pos, short_sym.length(), flag.c_str());
		gap += short_sym.length() - 1;
	}

	return gap;
}

namespace emoticon
{
std::vector<emoticon_category_t> g_emoticon_categories;
boost::unordered_map<std::string, emoticon_info_t> g_emoticon_by_name;

size_t get_tag_end_pos(const std::string& in_)
{
	auto pos = in_.find("|H");
	auto end = 0;
	while (pos != std::string::npos)
	{
		pos += strlen("|H");
		end = in_.find("|h", pos);
		if (end == std::string::npos)
		{
			return pos != in_.length() - 1 ? pos + 1 : pos;
		}
		pos = in_.find("|H", end);
	}

	return end;
}

bool load_emoticons_config_file(std::string file)
{
	CMappedFile mapped_file;
	LPCVOID dataptr;

	if (!CEterPackManager::Instance().Get(mapped_file, file.c_str(), &dataptr))
	{
		return false;
	}

	CMemoryTextFileLoader textloader;
	textloader.Bind(mapped_file.Size(), dataptr);

	if (textloader.GetLineCount() == 0)
	{
		return false;
	}

	emoticon_category_t* categoryptr = nullptr;
	emoticon_info_t info = {};

	std::vector<std::string> token;
	for (DWORD i = 0; i < textloader.GetLineCount(); i++)
	{
		const auto& line = textloader.GetLineString(i);
		if (line.empty() == true || line.at(0) == '#' || line.find_first_not_of(" \n\r\t") == std::string::npos)
		{
			continue;
		}

		make_split_token(line, token, '\t', true);

		if (token.size() == 2)
		{
			g_emoticon_categories.push_back(emoticon_category_t());
			categoryptr = &g_emoticon_categories.back();
			categoryptr->name = token[0];
			categoryptr->image_path = token[1];
		}

		else if (token.size() == 3)
		{
			if (categoryptr == nullptr)
			{
				continue;
			}

			info.name = token[0];
			info.short_sym = token[1];
			info.image_path = token[2];

			categoryptr->emoticons.push_back(info);
			g_emoticon_by_name.emplace(info.name, info);
		}
	}

	return true;
}

const std::vector<emoticon_category_t>& get_emoticons_categories()
{
	return g_emoticon_categories;
}

int replace_emoticons_in_place(std::string & phrase, size_t index)
{
	// Emoji rework
	std::size_t hyperPosStart = phrase.find("|H");
	std::size_t hyperPosEnd = 0;
	std::string subChatStr;

	while (hyperPosStart != std::string::npos)
	{
		subChatStr = phrase.substr(hyperPosEnd, hyperPosStart - hyperPosEnd);
		replace_emoticons_in_place(subChatStr, 0);
		phrase.replace(hyperPosEnd, hyperPosStart - hyperPosEnd, subChatStr);

		hyperPosEnd = phrase.find("|h", hyperPosStart);
		if (hyperPosEnd != std::string::npos)
		{
			hyperPosEnd += strlen("|h");
		}

		hyperPosStart = phrase.find("|H", hyperPosEnd);
	}

	index = get_tag_end_pos(phrase);

	int i = 0;
	std::vector< emoticon_tag_info_t > emoticon_info_vec;

	for (const auto& category : g_emoticon_categories)
	{
		for (const auto& emoticon : category.emoticons)
		{
			i += replace_in_place_emoticon_flag(phrase, emoticon.short_sym, emoticon.image_path, index, emoticon_info_vec);
		}
	}

	return i;
}

int replace_emoticons_in_place(std::wstring& phrase, size_t index)
{
	// Emoji rework
	std::size_t hyperPosStart = phrase.find(L"|H");
	std::size_t hyperPosEnd = 0;
	std::wstring subChatStr = L"";

	while (hyperPosStart != std::string::npos)
	{
		subChatStr = phrase.substr(hyperPosEnd, hyperPosStart - hyperPosEnd);
		replace_emoticons_in_place(subChatStr, 0);
		phrase.replace(hyperPosEnd, hyperPosStart - hyperPosEnd, subChatStr);

		hyperPosEnd = phrase.find(L"|h", hyperPosStart);
		if (hyperPosEnd != std::string::npos)
		{
			hyperPosEnd += strlen("|h");
		}

		hyperPosStart = phrase.find(L"|H", hyperPosEnd);
	}

	std::string tmpPhrase((const char*)&phrase[0], sizeof(wchar_t) / sizeof(char)*phrase.size());
	index = get_tag_end_pos(tmpPhrase);

	int i = 0;
	std::vector< emoticon_tag_info_t > emoticon_info_vec;

	for (const auto& category : g_emoticon_categories)
	{
		for (const auto& emoticon : category.emoticons)
		{
			i += replace_in_place_emoticon_flag(phrase, emoticon.short_sym, emoticon.image_path, index, emoticon_info_vec);
		}
	}

	return i;
}

const emoticon_info_t* get_emoticon_info_from_name(const std::string & name)
{
	const auto iter = g_emoticon_by_name.find(name);
	if (iter != g_emoticon_by_name.cend())
	{
		return &(iter->second);
	}
	return nullptr;
}
}