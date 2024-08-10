#include "stdafx.h"
#ifdef __ENABLE_AUTO_NOTICE__
#include <fstream>
#include <rapidxml/rapidxml_utils.hpp>
#include <boost/algorithm/string.hpp>

#include "utils.h"
#include "desc.h"
#include "char.h"
#include "desc_manager.h"
#include "CAutoNotice.hpp"

namespace CAutoNoticeManager
{
	struct Message {
		std::string message;
		uint32_t countdown;
		time_t cycleTime;
	};

	std::vector<Message> vMessages{};

	bool Initialize()
	{
		if (!LoadConfiguration("locale/germany/notice_manager.xml"))
			return false;

		return true;
	}

	void Reload()
	{
		//..
	}

	void Clear()
	{
		vMessages.clear();
	}

	bool LoadConfiguration(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			return false;
		}

		try
		{
			rapidxml::file<> xmlFile(file);
			rapidxml::xml_document<> xmlDoc;
			try
			{
				xmlDoc.parse<rapidxml::parse_trim_whitespace>(xmlFile.data());
			}
			catch (const std::runtime_error& e)
			{
				return false;
			}
			catch (const rapidxml::parse_error& e)
			{
				return false;
			}

			auto parent = xmlDoc.first_node("notice_manager_proto", 0, false);
			if (!parent)
			{
				return false;
			}

			for (auto category = parent->first_node(); category; category = category->next_sibling())
			{
				auto name_attr = category->first_attribute("key", 0, false);
				if (!name_attr)
				{
					continue;
				}

				auto time_attr = category->first_attribute("time", 0, false);
				if (!time_attr)
				{
					continue;
				}

				Message msg = {
					.message = name_attr->value(),
					.countdown = std::stoi(time_attr->value()),
					.cycleTime = get_global_time() + std::stoi(time_attr->value()) * 60
				};
				vMessages.push_back(msg);
			}
		}
		catch (const std::exception& e)
		{
			return false;
		}

		return true;
	}

	void Process()
	{
		for (auto& msg : vMessages) {
			if (get_global_time() > msg.cycleTime) {
				msg.cycleTime = get_global_time() + msg.countdown * 60;

				std::vector<std::string> vec;
				boost::split(vec, msg.message, boost::is_any_of(";"));

				for (const auto& it : vec) BroadcastMessage(it);
			}
		}
	}

	void BroadcastMessage(const std::string& text)
	{
		std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [&](LPDESC d)
			{
				if (d->GetCharacter())
				{
					d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, text.c_str());
				}
			});
	}
}
#endif
