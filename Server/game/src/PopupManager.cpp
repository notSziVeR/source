#include "stdafx.h"
#ifdef __POPUP_SYSTEM_ENABLE__
#include "char.h"
#include "item.h"
#include "PopupManager.hpp"

#include <boost/algorithm/string.hpp>

namespace
{
	inline std::string ReplaceWhiteMarks(const std::string& sStr)
	{
		std::string sStr_(sStr);
		boost::replace_all(sStr_, " ", "|");
		return sStr_;
	}
}

CPopupManager::CPopupManager()
{
	using namespace NSPopupMessage;
	um_popup_classes[ECPopupTypes::SWITCHBOT_END] = std::make_unique<PopupMessageSwitchBotEnd>();
	um_popup_classes[ECPopupTypes::ALCHEMY_STONE_END] = std::make_unique<PopupMessageAlchemyStoneEnd>();
	um_popup_classes[ECPopupTypes::LIMITYPE_END] = std::make_unique<PopupMessageLimittypeEnd>();
	um_popup_classes[ECPopupTypes::BIOLOGIST_COLLECTION] = std::make_unique<PopupMessageBiologColl>();
	um_popup_classes[ECPopupTypes::DUNGEON_RERUN] = std::make_unique<PopupMessageDungeonRerun>();
}

void PopupMessageBase::SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs)
{
	std::string sFullCommand(((std::string)NSPopupMessage::sPopupAPI + ReplaceWhiteMarks(GetCommandHeaderString(ch)) + " " + ReplaceWhiteMarks(sTxt) + " " + GetIconImage()));
	ch->ChatPacket(CHAT_TYPE_COMMAND, sFullCommand.c_str());
}

void PopupMessageSwitchBotEnd::SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs)
{
	auto pItem = std::get<LPITEM>(vBaseArgs);
	std::string sFullCommand(((std::string)NSPopupMessage::sPopupAPI + ReplaceWhiteMarks(GetCommandHeaderString(ch)) + " " + ReplaceWhiteMarks(sTxt) + " " + std::to_string(pItem->GetVnum())));
	ch->ChatPacket(CHAT_TYPE_COMMAND, sFullCommand.c_str());
}

void PopupMessageSwitchBotEnd::ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg)
{
	// Check variant
	if (!CheckVariant<LPITEM>(vBaseArgs))
		return;

	LPITEM pItem = std::get<LPITEM>(vBaseArgs);
	if (!pItem) return;

	// Command sample: PopupMessageAdd switchbot item_name item_slot
	SendNotificationText(ch, fmt::format(GetCommandString(ch), pItem->GetName(), pItem->GetCell()), pItem);
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageSwitchBotEnd::GetCommandHeaderString(LPCHARACTER ch)
#else
const char* PopupMessageSwitchBotEnd::GetCommandHeaderString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_SWITCH_BOT_END_HEADER", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_SWITCH_BOT_END_HEADER");
#endif
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageSwitchBotEnd::GetCommandString(LPCHARACTER ch)
#else
const char* PopupMessageSwitchBotEnd::GetCommandString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_SWITCH_BOT_END", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_SWITCH_BOT_END");
#endif
}

void PopupMessageAlchemyStoneEnd::SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs)
{
	auto pItem = std::get<LPITEM>(vBaseArgs);
	std::string sFullCommand(((std::string)NSPopupMessage::sPopupAPI + ReplaceWhiteMarks(GetCommandHeaderString(ch)) + " " + ReplaceWhiteMarks(sTxt) + " " + std::to_string(pItem->GetVnum())));
	ch->ChatPacket(CHAT_TYPE_COMMAND, sFullCommand.c_str());
}

void PopupMessageAlchemyStoneEnd::ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg)
{
	// Check variant
	if (!CheckVariant<LPITEM>(vBaseArgs))
		return;

	LPITEM pItem = std::get<LPITEM>(vBaseArgs);
	if (!pItem) return;

	// Command sample: PopupMessageAdd alchemy item_name item_slot
	SendNotificationText(ch, fmt::format(GetCommandString(ch), pItem->GetName(), pItem->GetCell()), pItem);
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageAlchemyStoneEnd::GetCommandHeaderString(LPCHARACTER ch)
#else
const char* PopupMessageAlchemyStoneEnd::GetCommandHeaderString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_ALCHEMY_STONE_END_HEADER", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_ALCHEMY_STONE_END_HEADER");
#endif
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageAlchemyStoneEnd::GetCommandString(LPCHARACTER ch)
#else
const char* PopupMessageAlchemyStoneEnd::GetCommandString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_ALCHEMY_STONE_END", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_ALCHEMY_STONE_END");
#endif
}

void PopupMessageLimittypeEnd::SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs)
{
	auto pItem = std::get<LPITEM>(vBaseArgs);
	std::string sFullCommand(((std::string)NSPopupMessage::sPopupAPI + ReplaceWhiteMarks(GetCommandHeaderString(ch)) + " " + ReplaceWhiteMarks(sTxt) + " " + std::to_string(pItem->GetVnum())));
	ch->ChatPacket(CHAT_TYPE_COMMAND, sFullCommand.c_str());
}

void PopupMessageLimittypeEnd::ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg)
{
	// Check variant
	if (!CheckVariant<LPITEM>(vBaseArgs))
		return;

	LPITEM pItem = std::get<LPITEM>(vBaseArgs);
	if (!pItem) return;

	// Command sample: PopupMessageAdd limittype item_name item_slot
	SendNotificationText(ch, fmt::format(GetCommandString(ch), pItem->GetName(), pItem->GetCell()), pItem);
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageLimittypeEnd::GetCommandHeaderString(LPCHARACTER ch)
#else
const char* PopupMessageLimittypeEnd::GetCommandHeaderString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_LIMITTYPE_HEADER", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_LIMITTYPE_HEADER");
#endif
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageLimittypeEnd::GetCommandString(LPCHARACTER ch)
#else
const char* PopupMessageLimittypeEnd::GetCommandString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_LIMITTYPE_END", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_LIMITTYPE_END");
#endif
}

void PopupMessageBiologColl::ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg)
{
	// Check variant
	if (!CheckVariant<int>(vBaseArgs))
		return;

	// Command sample: PopupMessageAdd biologist_collection
	SendNotificationText(ch, fmt::format(GetCommandString(ch)));
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageBiologColl::GetCommandHeaderString(LPCHARACTER ch)
#else
const char* PopupMessageBiologColl::GetCommandHeaderString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_BIOLOGIST_COLL_HEADER", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_BIOLOGIST_COLL_HEADER");
#endif
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageBiologColl::GetCommandString(LPCHARACTER ch)
#else
const char* PopupMessageBiologColl::GetCommandString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_BIOLOGIST_COLL", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_BIOLOGIST_COLL");
#endif
}

void PopupMessageDungeonRerun::ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg)
{
	// Check variant
	if (!CheckVariant<int>(vBaseArgs))
		return;

	// Command sample: PopupMessageAdd dungeon_rerun
	SendNotificationText(ch, fmt::format(GetCommandString(ch)));
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageDungeonRerun::GetCommandHeaderString(LPCHARACTER ch)
#else
const char* PopupMessageDungeonRerun::GetCommandHeaderString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_DUNGEON_RERUN_HEADER", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_DUNGEON_RERUN_HEADER");
#endif
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* PopupMessageDungeonRerun::GetCommandString(LPCHARACTER ch)
#else
const char* PopupMessageDungeonRerun::GetCommandString(LPCHARACTER ch)
#endif
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	return LC::TranslateText("POPUP_SYSTEM_DUNGEON_RERUN", LC::TRANSLATION_TYPE_GAME, ch->GetLocale());
#else
	return LC_TEXT("POPUP_SYSTEM_DUNGEON_RERUN");
#endif
}
#endif
