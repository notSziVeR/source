#pragma once

namespace emoticon
{

typedef struct emoticon_info_s
{
	std::string short_sym;
	std::string image_path;
	std::string name;
} emoticon_info_t;

typedef struct emoticon_category_s
{
	std::vector<emoticon_info_t>  emoticons;
	std::string image_path;
	std::string name;
} emoticon_category_t;

struct emoticon_gap_info_t
{
	int pos;
	int len;
};

size_t get_tag_end_pos(const std::string& in_);
bool load_emoticons_config_file(std::string file);
const std::vector< emoticon_category_t>& get_emoticons_categories();
int replace_emoticons_in_place(std::string& phrase, size_t index = 0);
int replace_emoticons_in_place(std::wstring& phrase, size_t index = 0);
const emoticon_info_t* get_emoticon_info_from_name(const std::string& name);
}