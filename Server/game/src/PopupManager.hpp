#pragma once
#ifdef __POPUP_SYSTEM_ENABLE__
#include <variant>

namespace NSPopupMessage
{
	enum class ECPopupTypes : int
	{
		SWITCHBOT_END,
		ALCHEMY_STONE_END,
		LIMITYPE_END,
		BIOLOGIST_COLLECTION,
		DUNGEON_RERUN,
	};

	template <typename T, typename U>
	struct is_present;

	template <typename T, typename... U>
	struct is_present<T, std::variant<U...>> : std::bool_constant<(std::is_same_v<T, U> || ...)>
	{};

	constexpr std::string_view sPopupAPI("PopupMessageAppend ");
}

class PopupMessageBase
{
public:
	using VNotArg = std::variant<LPITEM, LPCHARACTER, long long, int, std::string>;
	PopupMessageBase() {};
	~PopupMessageBase() {}

public:
	virtual void ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg = "") = 0;

protected:
	template <typename T>
	bool CheckVariant(const VNotArg& rVar)
	{
		// Check if variant holds any alternative
		try
		{
			std::get<T>(rVar);
		}
		catch (const std::bad_variant_access& ex)
		{
			sys_err("Variant doesn't hold any value!");
			return false;
		}
		// Check if variant contains appropriate argument
		if (!std::holds_alternative<T>(rVar))
		{
			sys_err("Variant holds unsupported alterantive: %s", typeid(std::get<0>(rVar)).name());
			return false;
		}

		return true;
	}

	virtual void SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs = {});
	virtual const char* GetIconImage() { return "EMPTY"; }
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	virtual const char* GetCommandHeaderString(LPCHARACTER ch) { return ""; }
	virtual const char* GetCommandString(LPCHARACTER ch) { return ""; }
#else
	virtual const char* GetCommandHeaderString() { return ""; }
	virtual const char* GetCommandString() { return ""; }
#endif
};

class PopupMessageSwitchBotEnd : public PopupMessageBase
{
public:
	PopupMessageSwitchBotEnd() {};
	virtual ~PopupMessageSwitchBotEnd() {}

public:
	void ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg = "") override;

protected:
	virtual void SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs = {});
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	virtual const char* GetCommandHeaderString(LPCHARACTER ch) override;
	virtual const char* GetCommandString(LPCHARACTER ch) override;
#else
	virtual const char* GetCommandHeaderString() override;
	virtual const char* GetCommandString();
#endif
};

class PopupMessageAlchemyStoneEnd : public PopupMessageBase
{
public:
	PopupMessageAlchemyStoneEnd() {};
	virtual ~PopupMessageAlchemyStoneEnd() {}

public:
	void ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg = "") override;

protected:
	virtual void SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs = {});
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	virtual const char* GetCommandHeaderString(LPCHARACTER ch) override;
	virtual const char* GetCommandString(LPCHARACTER ch) override;
#else
	virtual const char* GetCommandHeaderString() override;
	virtual const char* GetCommandString();
#endif
};

class PopupMessageLimittypeEnd : public PopupMessageBase
{
public:
	PopupMessageLimittypeEnd() {};
	virtual ~PopupMessageLimittypeEnd() {}

public:
	void ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg = "") override;

protected:
	virtual void SendNotificationText(LPCHARACTER ch, const std::string& sTxt, const VNotArg& vBaseArgs = {});
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	virtual const char* GetCommandHeaderString(LPCHARACTER ch) override;
	virtual const char* GetCommandString(LPCHARACTER ch) override;
#else
	virtual const char* GetCommandHeaderString() override;
	virtual const char* GetCommandString();
#endif
};

class PopupMessageBiologColl : public PopupMessageBase
{
public:
	PopupMessageBiologColl() {};
	virtual ~PopupMessageBiologColl() {}

public:
	void ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg = "") override;

protected:
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	virtual const char* GetCommandHeaderString(LPCHARACTER ch) override;
	virtual const char* GetCommandString(LPCHARACTER ch) override;
#else
	virtual const char* GetCommandHeaderString() override;
	virtual const char* GetCommandString();
#endif
};

class PopupMessageDungeonRerun : public PopupMessageBase
{
public:
	PopupMessageDungeonRerun() {};
	virtual ~PopupMessageDungeonRerun() {}

public:
	void ProcessNotificationEvent(LPCHARACTER ch, const VNotArg& vBaseArgs, const std::string& sAddArg = "") override;

protected:
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	virtual const char* GetCommandHeaderString(LPCHARACTER ch) override;
	virtual const char* GetCommandString(LPCHARACTER ch) override;
#else
	virtual const char* GetCommandHeaderString() override;
	virtual const char* GetCommandString();
#endif
};

class CPopupManager : public singleton<CPopupManager>
{
public:
	CPopupManager();
	virtual ~CPopupManager() {};

public:
	template <typename T>
	void ProcessPopup(LPCHARACTER ch, const NSPopupMessage::ECPopupTypes& eType, const T& sArg, const std::string& sAddArg = "");

private:
	std::unordered_map<NSPopupMessage::ECPopupTypes, std::unique_ptr<PopupMessageBase>> um_popup_classes;
};

template <typename T>
void CPopupManager::ProcessPopup(LPCHARACTER ch, const NSPopupMessage::ECPopupTypes& eType, const T& sArg, const std::string& sAddArg)
{
	// Check if type exists
	auto fIt = um_popup_classes.find(eType);
	if (fIt == um_popup_classes.end())
	{
		sys_err("Class num %u {ECPopupTypes} does not exist in popup dictionary!", static_cast<int>(eType));
		return;
	}

	// Check if T argument exist in variant
	if (!NSPopupMessage::is_present<T, PopupMessageBase::VNotArg>::value)
	{
		sys_err("Argument of type: %s is not support by popup variant type!", typeid(T).name());
		return;
	}

	fIt->second->ProcessNotificationEvent(ch, sArg, sAddArg);
}
#endif
