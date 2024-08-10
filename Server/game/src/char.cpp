#include "stdafx.h"

#include "../../common/teen_packet.h"
#include "../../common/VnumHelper.h"

#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "castle.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "HackShield.h"
#include "skill_power.h"
#include "XTrapManager.h"
#include "buff_on_attributes.h"

#include "OXEvent.h"

#ifdef __PET_SYSTEM__
	#include "PetSystem.h"
#endif

#ifdef __ENABLE_SWITCHBOT__
	#include "switchbot.h"
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	#include "GayaSystemManager.hpp"
#endif

#include "DragonSoul.h"
#include "../../common/CommonDefines.h"

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	#include "NotificatonSender.hpp"
#endif

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	#include "ZuoEvent.hpp"
#endif

#ifdef __SHIP_DUNGEON_ENABLE__
	#include "ShipDefendDungeonManager.hpp"
#endif

#ifdef __TRANSMUTATION_SYSTEM__
	#include "Transmutation.h"
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	#include "find_letters_event.h"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __ITEM_TOGGLE_SYSTEM__
	#include "ItemUtils.h"
#endif

#ifdef __ITEM_SHOP_ENABLE__
	#include "ItemShopManager.hpp"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	#include "LegendaryStonesHandler.hpp"
#endif

#ifdef __ENABLE_NEW_LOGS_CHAT__
	#include "ChatLogsHelper.hpp"
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	#include "TeleportManagerSystem.hpp"
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	#include "SashSystemHelper.hpp"
#endif

#ifdef __ENABLE_PASSIVE_SKILLS_HELPER__
	#include "PSkillsManager.hpp"
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	#include "AmuletSystemHelper.hpp"
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	#include "ShamanSystem.hpp"
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	#include "BeginnerHelper.hpp"
#endif


extern const BYTE g_aBuffOnAttrPoints;
extern bool RaceToJob(unsigned race, unsigned *ret_job);

extern bool IS_SUMMONABLE_ZONE(int map_index); // char_item.cpp
bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
	case 301:
	case 302:
	case 303:
	case 304:
		if (ch->GetLevel() < 90)
		{
			return false;
		}
		break;
#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	case CZuoEventManager::GetEventMapIndex():
#endif
		return false;
	}

	return true;
}

#ifdef NEW_ICEDAMAGE_SYSTEM
const DWORD CHARACTER::GetNoDamageRaceFlag()
{
	return m_dwNDRFlag;
}

void CHARACTER::SetNoDamageRaceFlag(DWORD dwRaceFlag)
{
	if (dwRaceFlag >= MAIN_RACE_MAX_NUM) { return; }
	if (IS_SET(m_dwNDRFlag, 1 << dwRaceFlag)) { return; }
	SET_BIT(m_dwNDRFlag, 1 << dwRaceFlag);
}

void CHARACTER::UnsetNoDamageRaceFlag(DWORD dwRaceFlag)
{
	if (dwRaceFlag >= MAIN_RACE_MAX_NUM) { return; }
	if (!IS_SET(m_dwNDRFlag, 1 << dwRaceFlag)) { return; }
	REMOVE_BIT(m_dwNDRFlag, 1 << dwRaceFlag);
}

void CHARACTER::ResetNoDamageRaceFlag()
{
	m_dwNDRFlag = 0;
}

const std::set<DWORD> & CHARACTER::GetNoDamageAffectFlag()
{
	return m_setNDAFlag;
}

void CHARACTER::SetNoDamageAffectFlag(DWORD dwAffectFlag)
{
	m_setNDAFlag.insert(dwAffectFlag);
}

void CHARACTER::UnsetNoDamageAffectFlag(DWORD dwAffectFlag)
{
	m_setNDAFlag.erase(dwAffectFlag);
}

void CHARACTER::ResetNoDamageAffectFlag()
{
	m_setNDAFlag.clear();
}
#endif

// <Factor> DynamicCharacterPtr member function definitions

LPCHARACTER DynamicCharacterPtr::Get() const
{
	LPCHARACTER p = NULL;
	if (is_pc)
	{
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	}
	else
	{
		p = CHARACTER_MANAGER::instance().Find(id);
	}
	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character)
{
	if (character == NULL)
	{
		Reset();
		return *this;
	}
	if (character->IsPC())
	{
		is_pc = true;
		id = character->GetPlayerID();
	}
	else
	{
		is_pc = false;
		id = character->GetVID();
	}
	return *this;
}

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
CHARACTER::CHARACTER() : m_toggleMount(nullptr, [](CHARACTER * ch) -> void
{
	ch->SetRider(nullptr);
	M2_DESTROY_CHARACTER(ch);
})
#else
CHARACTER::CHARACTER()
#endif
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	CEntity::Initialize(ENTITY_CHARACTER);
	m_skipUpdatePacket = false;

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	m_locale = LC_DEFAULT;
#endif

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;

	m_fSyncTime = get_float_time() - 3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;

	m_iMoveCount = 0;

	m_pkRegen = NULL;
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData		= NULL;
	m_pkMobInst		= NULL;

	m_pkShop		= NULL;
	m_pkChrShopOwner	= NULL;
	m_pkMyShop		= NULL;
	m_pkExchange	= NULL;
	m_pkParty		= NULL;
	m_pkPartyRequestEvent = NULL;

	m_pGuild = NULL;

	m_pkChrTarget = NULL;

	m_pkMuyeongEvent = NULL;

	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;

#ifdef __ENABLE_WOLFMAN_CHARACTER__
	m_pkBleedingEvent = NULL;
#endif

	m_pkFireEvent = NULL;
	m_pkCheckSpeedHackEvent	= NULL;
	m_speed_hack_count	= 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

	memset(&m_points, 0, sizeof(m_points));
	memset(&m_pointsInstant, 0, sizeof(m_pointsInstant));
	memset(&m_quickslot, 0, sizeof(m_quickslot));

	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;

	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time() - 180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;

#ifdef __ENABLE_WOLFMAN_CHARACTER__
	m_bHasBled = false;
#endif

	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_pQuestItem = NULL;

	m_szMobileAuth[0] = '\0';

	m_dwUnderGuildWarInfoMessageTime = get_dword_time() - 60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;

	ResetStopTime();

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	m_isinPCBang = false;

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	//PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	//END_PREVENT_TRADE_WINDOW

	//PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	//END_PREVENT_REFINE_HACK

	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	//PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE
	//
	m_iSafeboxLoadTime = 0;

	m_iMyShopTime = 0;

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;

	m_dwMountTime = 0;

	m_dwLastGoldDropTime = 0;

#ifdef __ENABLE_NEWSTUFF__
	m_dwLastItemDropTime = 0;
	m_dwLastBoxUseTime = 0;
	m_dwLastBuySellTime = 0;
#endif

	m_HackShieldCheckEvent = NULL;
	m_HackShieldCheckMode = false;

	m_bIsLoadedAffect = false;
	cannot_dead = false;

#ifdef __PET_SYSTEM__
	m_petSystem = nullptr;
	m_petActor = nullptr;
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	m_shamanSystem = nullptr;
	m_shamanActor = nullptr;
#endif

#ifdef NEW_ICEDAMAGE_SYSTEM
	m_dwNDRFlag = 0;
	m_setNDAFlag.clear();
#endif

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

#ifdef ENABLE_ANTI_CMD_FLOOD
	m_dwCmdAntiFloodCount = 0;
	m_dwCmdAntiFloodPulse = 0;
#endif

	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;

#ifdef __NEW_AGGREGATE_MONSTERS__
	m_iCapeEffectPulse = 0;
#endif

#ifdef __ADMIN_MANAGER__
	m_dwChatBanCount = 0;
#endif

#ifdef __HIT_LIMITER_ENABLE__
	dw_hit_count = 0;
	dw_hit_next_update = 0;
#endif

#ifdef __ABUSE_CONTROLLER_ENABLE__
	//Abuse controller
	m_bIsSuspectWaitDmg = false;
	m_abuse = nullptr;
	m_pkCheckWallHackEvent = NULL;
#endif

#ifdef __HIT_TRACKER_ENABLE__
	ttTrackingEventDelay = 0;
	iHitCount = 0;
	bTrackingTrigger = false;
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	dwNotificationCollector = 0;
	ttNotificationSentTime = 0;
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	m_pkBiologManager = nullptr;
#endif

#ifdef __TRANSMUTATION_SYSTEM__
	transmutation_items = nullptr;
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	m_bIsCubeOpen = false;
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	m_letterSlots = NULL;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	m_sRefineElementSrcCell = -1;
	m_sRefineElementDstCell = -1;
	m_cRefineElementType = -1;
#endif

#ifdef __ENABLE_LUCKY_BOX__
	dwLuckyBoxVnum = 0;
	bLuckyBoxRewardIndex = 0;
	bLuckyBoxOpenCount = 0;
	dwLuckyBoxPrice = 0;
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	m_bIsShowTeamler = false;
#endif

#ifdef __INVENTORY_BUFFERING__
	bInvBuff = false;
#endif

#ifdef __DUNGEON_RETURN_ENABLE__
	bWaitingForDungeonReponse = false;
	bCanReturnToDungeon = false;
#endif

	tt_next_pickup_time = 0;

#ifdef __ENABLE_CHANGE_CHANNEL__
	this->m_pkSwitchChannelEvent = NULL;
	this->iNextChannelSwitchTime = time(0) + 10;
#endif

#ifdef __ENABLE_PARTY_ATLAS__
	m_dwLastPartyPositionSent = 0;
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	keepingOfflineShop_ = 0;
	isOpeningOfflineShop_ = false;
	offlineShopOpeningItem_ = nullptr;
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	m_dwMountToggleItemStartID = 0;
#endif

#ifdef __ACTION_RESTRICTIONS__
	m_pkRestrictions = nullptr;
#endif

	m_bIsDisconnecting = false;

#ifdef __ITEM_SHOP_ENABLE__
	m_iItemShopBuyTime = 0;
	i_VolatileCoinValue = 0;
	bItemShopEditor = false;
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	pTombolaReward = nullptr;
#endif

#ifdef __TEAM_DAMAGE_FLAG_CHECK__
	m_bTeamSendDamageFlag = false;
#endif
}

void CHARACTER::Create(const char * c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128 + 1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
	{
		m_stName = c_pszName;
#ifdef __ABUSE_CONTROLLER_ENABLE__
		//Start the controller
		m_abuse = spAbuseController(new AbuseController(this));
#endif
	}
}

void CHARACTER::Destroy()
{
#ifdef __ADMIN_MANAGER__
	if (IsNPC())
	{
		CAdminManager::instance().OnMonsterDestroy(this);
	}
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	if (m_bIsShowTeamler)
	{
		SetIsShowTeamler(false);
	}
#endif

	CloseMyShop();
#ifdef __ENABLE_OFFLINE_SHOP__
	RemoveFromViewingOfflineShops();
#endif

	if (m_pkRegen)
	{
		if (m_pkDungeon)
		{
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_))
			{
				--m_pkRegen->count;
			}
		}
		else
		{
			// Is this really safe?
			--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}

	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		delete m_petSystem;
		m_petSystem = nullptr;
	}

	if (m_petActor)
	{
		m_petActor->OnDestroy();
		m_petActor = nullptr;
	}
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	if (m_shamanSystem)
	{
		delete m_shamanSystem;
		m_shamanSystem = nullptr;
	}

	if (m_shamanActor)
	{
		m_shamanActor->OnDestroy();
		m_shamanActor = nullptr;
	}
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	if (m_pkBiologManager)
	{
		m_pkBiologManager->OnDestroy();
		delete m_pkBiologManager;
		m_pkBiologManager = nullptr;
	}
#endif

#ifdef __ACTION_RESTRICTIONS__
	if (m_pkRestrictions)
	{
		M2_DELETE(m_pkRestrictions);
		m_pkRestrictions = nullptr;
		sys_log(0, "CHARACTER::Destroy - Deleting m_pkRestrictions");
	}
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	LegendaryStonesHandler::UnregisterExchange(this);
	LegendaryStonesHandler::UnregisterRefine(this);
#endif

	HorseSummon(false);

	if (GetRider())
	{
		GetRider()->ClearHorseInfo();
	}

	if ( IsPC() )
	{
		if (isHackShieldEnable)
		{
			CHackShieldManager::instance().DeleteClientHandle(GetPlayerID());
		}
	}

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	if (m_pkExchange)
	{
		m_pkExchange->Cancel();
	}

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
			{
				party->Quit(GetVID());
			}
		}

		SetParty(NULL);
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();  it != m_map_buff_on_attrs.end(); it++)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

#ifdef __TRANSMUTATION_SYSTEM__
	if (transmutation_items)
	{
		delete[] transmutation_items;
	}
#endif

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	event_cancel(&m_pkBleedingEvent);
#endif
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	//DELAYED_WARP
	event_cancel(&m_pkWarpEvent);
	event_cancel(&m_pkCheckSpeedHackEvent);
	//END_DELAYED_WARP

#ifdef __ABUSE_CONTROLLER_ENABLE__
	event_cancel(&m_pkCheckWallHackEvent);
#endif

	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
	// END_OF_MINING

	StopHackShieldCheckCycle();

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	event_cancel(&m_pkDestroyWhenIdleEvent);

#ifdef __ENABLE_CHANGE_CHANNEL__
	this->QuitSwitchChannelEvent();
#endif

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	if (m_letterSlots)
	{
		M2_DELETE_ARRAY(m_letterSlots);
		m_letterSlots = NULL;
	}
#endif

	CEntity::Destroy();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
	}

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

const char * CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
}

void CHARACTER::OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount)
{
	if (!CanHandleItem()) // @fixme149
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
		return;
	}

#ifndef ENABLE_OPEN_SHOP_WITH_ARMOR
	if (GetPart(PART_MAIN) > 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only open the shop if you take off your armour."));
		return;
	}
#endif

	if (GetMyShop())
	{
		CloseMyShop();
		return;
	}

	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (pPC->IsRunning())
	{
		return;
	}

	if (bItemCount == 0)
	{
		return;
	}

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t totalMoney = GetGold();
	for (auto i = 0; i < bItemCount; ++i)
	{
		totalMoney += (pTable + i)->price;
	}

	if (GOLD_MAX <= totalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't open a Warehouse because you carry more than 2 billion Gold."));
		return;
	}
#else
	int64_t nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<int64_t>((pTable + n)->price);
	}

	nTotalMoney += static_cast<int64_t>(GetGold());

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade because you are carrying more than 2 billion Yang."));
		return;
	}
#endif

	char szSign[SHOP_SIGN_MAX_LEN + 1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
	{
		return;
	}

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't give your shop an invalid name."));
		return;
	}

	// MYSHOP_PRICE_LIST
	std::map<DWORD, DWORD> itemkind;
	// END_OF_MYSHOP_PRICE_LIST

	std::set<TItemPos> cont;
	for (BYTE i = 0; i < bItemCount; ++i)
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable * item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't sell Item-Shop items in a private shop."));
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell items you have equipped."));
				return;
			}

			if (true == pkItem->isLocked())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell locked items."));
				return;
			}

			// MYSHOP_PRICE_LIST
			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
			// END_OF_MYSHOP_PRICE_LIST
		}

		cont.insert((pTable + i)->pos);
	}

	// MYSHOP_PRICE_LIST

	if (CountSpecifyItem(71049))
	{

		// @fixme403 BEGIN
		TItemPriceListTable header;
		memset(&header, 0, sizeof(TItemPriceListTable));

		header.dwOwnerID = GetPlayerID();
		header.byCount = itemkind.size();

		size_t idx = 0;
		for (itertype(itemkind) it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			header.aPriceInfo[idx].dwVnum = it->first;
			header.aPriceInfo[idx].dwPrice = it->second;
			idx++;
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, GetDesc()->GetHandle(), &header, sizeof(TItemPriceListTable));
		// @fixme403 END
	}
	// END_OF_MYSHOP_PRICE_LIST
	else if (CountSpecifyItem(50200))
	{
		RemoveSpecifyItem(50200, 1);
	}
	else
	{
		return;
	}

	if (m_pkExchange)
	{
		m_pkExchange->Cancel();
	}

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));

	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, bItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon( false, true );
	}
	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}

	SetPolymorph(30000, true);

}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;

		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		p.szSign[0] = '\0';

		PacketAround(&p, sizeof(p));
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		SetPolymorph(m_points.job, true);
		// SetPolymorph(0, true);
#else
		SetPolymorph(GetJob(), true);
#endif
	}
}

void EncodeMovePacket(TPacketGCMove & pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc   = bFunc;
	pack.bArg    = bArg;
	pack.dwVID   = dwVID;
	pack.dwTime  = dwTime ? dwTime : get_dword_time();
	pack.bRot    = bRot;
	pack.lX		= x;
	pack.lY		= y;
	pack.dwDuration	= dwDuration;
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
	{
		return;
	}

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
		{
			EncodeInsertPacket(entity);
		}

		if ( entity->IsType(ENTITY_CHARACTER) )
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if ( lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster() )
			{
				if (!entity->IsObserverMode())
				{
					entity->EncodeInsertPacket(this);
				}
			}
		}
		else
		{
			if ( !entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}

// #define ENABLE_SHOWNPCLEVEL
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{
	LPDESC d;

	if (!(d = entity->GetDesc()))
	{
		return;
	}

	LPCHARACTER ch = (LPCHARACTER) entity;

#ifdef __OX_EVENT_SYSTEM_ENABLE__
	if (GetMapIndex() == OXEVENT_MAP_INDEX && ch->IsPC() && IsPC() && this != ch && ch->GetGMLevel() != GM_IMPLEMENTOR && GetGMLevel() != GM_IMPLEMENTOR)
	{
		if (COXEventManager::instance().IsAttender(ch->GetPlayerID()) || COXEventManager::instance().IsAttender(GetPlayerID()))
		{
			return;
		}
	}
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	if (IsNPC() && GetRaceNum() == COfflineShop::CLOSED_RACE)
	{
		auto shopPtr = COfflineShop::Get(GetKeepingOfflineShop());
		if (shopPtr)
		{
			auto shop = shopPtr->get();

			if (shop->IsClosed() && shop->GetOwnerPid() != ch->GetPlayerID())
			{
				return;
			}
		}
	}
#endif

	ch->SendGuildName(GetGuild());

#ifdef __ENABLE_SOUL_SYSTEM__
	TAffectFlag sendAffectFlag = m_afAffectFlag;
	if (sendAffectFlag.IsSet(AFF_SOUL_RED) && sendAffectFlag.IsSet(AFF_SOUL_BLUE))
	{
		sendAffectFlag.Reset(AFF_SOUL_RED);
		sendAffectFlag.Reset(AFF_SOUL_BLUE);
		sendAffectFlag.Set(AFF_SOUL_MIX);
	}
#endif

	TPacketGCCharacterAdd pack;

	pack.header		= HEADER_GC_CHARACTER_ADD;
	pack.dwVID		= m_vid;
	pack.bType		= GetCharType();
	pack.angle		= GetRotation();
	pack.x			= GetX();
	pack.y			= GetY();
	pack.z			= GetZ();
	pack.wRaceNum	= GetRaceNum();
	pack.dwLevel	= GetLevel();

	if (IsPet())
	{
		pack.bMovingSpeed	= 150;
	}
	else
	{
		pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
#ifdef __ENABLE_SOUL_SYSTEM__
	pack.dwAffectFlag[0] = sendAffectFlag.bits[0];
	pack.dwAffectFlag[1] = sendAffectFlag.bits[1];
#else
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
#endif

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

	if (IsPC() == true || m_bCharType == CHAR_TYPE_NPC
#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
		|| m_bCharType == CHAR_TYPE_MOUNT
#endif
		)
	{
		TPacketGCCharacterAdditionalInfo addPacket = {};

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

#ifdef __ENABLE_HIDE_COSTUMES__
		addPacket.awPart[CHR_EQUIPPART_ARMOR] = HasCostumeFlag(COSTUME_FLAG_BODY) ? (GetWear(WEAR_BODY) ? (GetWear(WEAR_BODY)->GetTransmutate() ? GetWear(WEAR_BODY)->GetTransmutate() : GetWear(WEAR_BODY)->GetVnum()) : 0) : GetPart(PART_MAIN);
#else
		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
#endif
#ifdef __ENABLE_HIDE_COSTUMES__
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = HasCostumeFlag(COSTUME_FLAG_WEAPON) ? (GetWear(WEAR_WEAPON) ? (GetWear(WEAR_WEAPON)->GetTransmutate() ? GetWear(WEAR_WEAPON)->GetTransmutate() : GetWear(WEAR_WEAPON)->GetVnum()) : 0) : GetPart(PART_WEAPON);
#else
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
#endif
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
#ifdef __ENABLE_HIDE_COSTUMES__
		addPacket.awPart[CHR_EQUIPPART_HAIR] = HasCostumeFlag(COSTUME_FLAG_HEAD) ? 0 : GetPart(PART_HAIR);
#else
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#endif

#ifdef __ENABLE_SASH_SYSTEM__
#ifdef __ENABLE_HIDE_COSTUMES__
		addPacket.awPart[CHR_EQUIPPART_SASH] = HasCostumeFlag(COSTUME_FLAG_SASH) ? 0 : GetPart(PART_SASH);
#else
		addPacket.awPart[CHR_EQUIPPART_SASH] = GetPart(PART_SASH);
#endif
#endif

		addPacket.bPKMode = m_bPKMode;
		addPacket.dwMountVnum = GetMountVnum();
		addPacket.bEmpire = m_bEmpire;
		addPacket.dwLevel = pack.dwLevel;

#ifdef __ENABLE_REFINE_ELEMENT__
		addPacket.bRefineElementType = GetRefineElementType();
#endif

#ifdef __ENABLE_QUIVER_SYSTEM__
		addPacket.dwArrow = (IsPC() && GetWear(WEAR_ARROW) != NULL) ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		addPacket.locale = (IsPC() ? GetLocale() : -1);
#endif

		strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

		if (GetGuild() != nullptr)
			addPacket.dwGuildID = GetGuild()->GetID();

		addPacket.sAlignment = m_iAlignment / 10;

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE) (GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		LPCHARACTER ch = (LPCHARACTER) entity;
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

	if (GetMyShop())
	{
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));

		d->Packet(&p, sizeof(TPacketGCShopSign));
	}

#ifdef __ENABLE_OFFLINE_SHOP__
	if (IsNPC())
	{
		auto shopPtr = COfflineShop::Get(GetKeepingOfflineShop());
		if (shopPtr)
		{
			shopPtr->get()->SendSpawnPacket(ch);
		}
	}
#endif

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
				GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
	{
		return;
	}

	LPDESC d;

	if (!(d = entity->GetDesc()))
	{
		return;
	}

	TPacketGCCharacterDelete pack;

	pack.header	= HEADER_GC_CHARACTER_DEL;
	pack.id	= m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD) m_vid, ((LPCHARACTER) entity)->GetName());
	}
}

void CHARACTER::UpdatePacket()
{
	if (IsSkipUpdatePacket())
	{
		return;
	}

	if (!GetSectree())
	{
		return;
	}

#ifdef __ENABLE_SOUL_SYSTEM__
	TAffectFlag sendAffectFlag = m_afAffectFlag;
	if (sendAffectFlag.IsSet(AFF_SOUL_RED) && sendAffectFlag.IsSet(AFF_SOUL_BLUE))
	{
		sendAffectFlag.Reset(AFF_SOUL_RED);
		sendAffectFlag.Reset(AFF_SOUL_BLUE);
		sendAffectFlag.Set(AFF_SOUL_MIX);
	}
#endif

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

#ifdef __ENABLE_HIDE_COSTUMES__
	pack.awPart[CHR_EQUIPPART_ARMOR] = HasCostumeFlag(COSTUME_FLAG_BODY) ? (GetWear(WEAR_BODY) ? (GetWear(WEAR_BODY)->GetTransmutate() ? GetWear(WEAR_BODY)->GetTransmutate() : GetWear(WEAR_BODY)->GetVnum()) : 0) : GetPart(PART_MAIN);
#else
	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
#endif
#ifdef __ENABLE_HIDE_COSTUMES__
	pack.awPart[CHR_EQUIPPART_WEAPON] = HasCostumeFlag(COSTUME_FLAG_WEAPON) ? (GetWear(WEAR_WEAPON) ? (GetWear(WEAR_WEAPON)->GetTransmutate() ? GetWear(WEAR_WEAPON)->GetTransmutate() : GetWear(WEAR_WEAPON)->GetVnum()) : 0) : GetPart(PART_WEAPON);
#else
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
#endif
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
#ifdef __ENABLE_HIDE_COSTUMES__
	pack.awPart[CHR_EQUIPPART_HAIR] = HasCostumeFlag(COSTUME_FLAG_HEAD) ? 0 : GetPart(PART_HAIR);
#else
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#endif

#ifdef __ENABLE_SASH_SYSTEM__
#ifdef __ENABLE_HIDE_COSTUMES__
	pack.awPart[CHR_EQUIPPART_SASH] = HasCostumeFlag(COSTUME_FLAG_SASH) ? 0 : GetPart(PART_SASH);
#else
	pack.awPart[CHR_EQUIPPART_SASH] = GetPart(PART_SASH);
#endif
#endif

	pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag	= m_bAddChrState;
#ifdef __ENABLE_SOUL_SYSTEM__
	pack.dwAffectFlag[0] = sendAffectFlag.bits[0];
	pack.dwAffectFlag[1] = sendAffectFlag.bits[1];
#else
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
#endif
	pack.dwGuildID	= 0;
	pack.sAlignment	= m_iAlignment / 10;
	pack.bPKMode	= m_bPKMode;

#ifdef __ENABLE_REFINE_ELEMENT__
	pack.bRefineElementType = GetRefineElementType();
#endif

#ifdef __ENABLE_QUIVER_SYSTEM__
	pack.dwArrow = GetWear(WEAR_ARROW) != NULL ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif

	if (GetGuild())
	{
		pack.dwGuildID = GetGuild()->GetID();
	}

#ifdef __ENABLE_ANTI_POLY_ON_MOUNT__
	// Poly Fix
	if (GetMountVnum() > 0 && IsPolymorphed())
	{
		RemoveAffect(AFFECT_POLYMORPH);
		SetPolymorph(0);
	}
#endif

	pack.dwMountVnum	= GetMountVnum();

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char * c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
		{
			continue;
		}

		LPCHARACTER tch = (LPCHARACTER) it->first;

		if (bFindPCOnly && tch->IsNPC())
		{
			continue;
		}

		if (!strcasecmp(tch->GetName(), c_pszName))
		{
			return (tch);
		}
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
	}

	if (!IsStone())
	{
		switch (pos)
		{
		case POS_FIGHTING:
			if (!IsState(m_stateBattle))
			{
				MonsterLog("[BATTLE] 싸우는 상태");
			}

			GotoState(m_stateBattle);
			break;

		default:
			if (!IsState(m_stateIdle))
			{
				MonsterLog("[IDLE] 쉬는 상태");
			}

			GotoState(m_stateIdle);
			break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
	{
		CHARACTER_MANAGER::instance().DelayedSave(this);
	}
}

void CHARACTER::CreatePlayerProto(TPlayerTable & tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id			= m_dwPlayerID;
	tab.voice		= GetPoint(POINT_VOICE);
	tab.level		= GetLevel();
	tab.level_step	= GetPoint(POINT_LEVEL_STEP);
	tab.exp			= GetExp();
	tab.gold		= GetGold();
	tab.job			= m_points.job;
	tab.part_base	= m_pointsInstant.bBasePart;
	tab.skill_group	= m_points.skill_group;
#ifdef __ADMIN_MANAGER__
	tab.chat_ban_count = m_dwChatBanCount;
#endif

	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
				{
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				}
				else
				{
					UpdateAlignment(60 * (dwPlayedTime / 60000));
				}
			}
			else
			{
				UpdateAlignment(5 * (dwPlayedTime / 60000));
			}
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex	= GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex	= tab.lMapIndex;
		tab.lExitX		= tab.x;
		tab.lExitY		= tab.y;
	}
	else
	{
		tab.lExitMapIndex	= m_lExitMapIndex;
		tab.lExitX		= m_posExit.x;
		tab.lExitY		= m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		tab.quickslot[i] = m_quickslot[i];
	}

	if (m_stMobile.length() && !*m_szMobileAuth)
	{
		strlcpy(tab.szMobile, m_stMobile.c_str(), sizeof(tab.szMobile));
	}

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));

	// REMOVE_REAL_SKILL_LEVLES
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	tab.horse = GetHorseData();

#ifdef __GAYA_SHOP_SYSTEM__
	tab.gaya_coins = m_points.gaya_coins;
#endif
#ifdef __ENABLE_HIDE_COSTUMES__
	tab.dwCostumeFlag = GetCostumeFlag();
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	tab.m_BiologActualMission = GetBiologMissions();
	tab.m_BiologCollectedItems = GetBiologCollectedItems();
	tab.m_BiologCooldownReminder = GetBiologCooldownReminder();
	tab.m_BiologCooldown = GetBiologCooldown();
#endif
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	tab.m_MarbleActiveMission = GetMarbleActiveMission();
	tab.m_MarbleActiveExtandedCount = GetMarbleActiveExtandedCount();
	tab.m_MarbleKilledMonsters = GetMarbleKilledMonsters();
#endif
#ifdef __ENABLE_FIND_LETTERS_EVENT__
	thecore_memcpy(tab.letterSlots, m_letterSlots, sizeof(TPlayerLetterSlot)* FIND_LETTERS_SLOTS_NUM);
#endif

}


void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
	{
		return;
	}

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC * pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
	{
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	}
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
	{
		pMarriage->Save();
	}
}

void CHARACTER::FlushDelayedSaveItem()
{
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
		{
			ITEM_MANAGER::instance().FlushDelayedSave(item);
		}
}

void CHARACTER::Disconnect(const char * c_pszReason)
{
	assert(GetDesc() != NULL);

	m_bIsDisconnecting = true;

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset" );

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	marriage::CManager::instance().Logout(this);

#ifdef __DUNGEON_RETURN_ENABLE__
	if (GetDungeon())
	{
		sys_err("Trying to save coords on dungeon, after dissconect");
		GetDungeon()->SaveCoords(this);
	}
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	if (GetGMLevel() == GM_IMPLEMENTOR)
	{
		NotificatonSender::EraseCacheCollector(GetPlayerID());
	}
#endif

	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
	LogManager::instance().CharLog(this, 0, "LOGOUT", "");

#ifdef ENABLE_PCBANG_FEATURE // @warme006
	{
		long playTime = GetRealPoint(POINT_PLAYTIME) - m_dwLoginPlayTime;
		LogManager::instance().LoginLog(false, GetDesc()->GetAccountTable().id, GetPlayerID(), GetLevel(), GetJob(), playTime);

		if (0)
		{
			CPCBangManager::instance().Log(GetDesc()->GetHostName(), GetPlayerID(), playTime);
		}
	}
#endif

	if (m_pWarMap)
	{
		SetWarMap(NULL);
	}

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
	{
		GetGuild()->LogoutMember(this);
	}

	quest::CQuestManager::instance().LogoutPC(this);

	if (GetParty())
	{
		GetParty()->Unlink(this);
	}

	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}


	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

	m_bSkipSave = true;

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	CZuoEventManager::instance().UnregisterPlayer(this);
#endif

#ifdef __SHIP_DUNGEON_ENABLE__
	CShipDefendDungeonManager::instance().EraseAttender(this);
#endif

#ifdef __ITEM_SHOP_ENABLE__
	CItemShopManager::instance().RemoveViewer(this);
#endif
	
#ifdef __SASH_ABSORPTION_ENABLE__
	SashSystemHelper::ReleasePlayer(GetPlayerID());
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	AmuletSystemHelper::ReleasePlayer(GetPlayerID());
#endif

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

#ifdef __ENABLE_OFFLINE_SHOP__
	COfflineShop::Logout(GetPlayerID());
#endif

	if (g_TeenDesc)
	{
		int		offset = 0;
		char	buf[245] = {0};

		buf[0] = HEADER_GT_LOGOUT;
		offset += 1;

		memset(buf + offset, 0x00, 2);
		offset += 2;

		TAccountTable	&acc_table = GetDesc()->GetAccountTable();
		memcpy(buf + offset, &acc_table.id, 4);
		offset += 4;

		g_TeenDesc->Packet(buf, offset);
	}

#ifdef __ADMIN_MANAGER__
	CAdminManager::instance().OnLogoutPlayer(this);
#endif

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	CXTrapManager::instance().DestroyClientSession(this);

	M2_DESTROY_CHARACTER(this);
}

bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */)
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

#ifdef __ADMIN_MANAGER__
	bool bAdminManagerIsOldMap = false;
	bool bAdminManagerNewMap = false;
	if (GetMapIndex() != lMapIndex)
	{
		bAdminManagerNewMap = true;
		if (GetMapIndex() && GetSectree() != NULL)
		{
			bAdminManagerIsOldMap = true;
		}
	}
#endif

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
	{
		bChangeTree = true;
	}

	if (bChangeTree)
	{
		if (GetSectree())
		{
			GetSectree()->RemoveEntity(this);
		}

		ViewCleanup();
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
		{
			StartAffectEvent();
		}
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "      in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

#ifdef __ABUSE_CONTROLLER_ENABLE__
	if (IsPC())
	{
		GetAbuseController()->ResetMovementSpeedhackChecker();
	}
#endif

	SetValidComboInterval(0);

#ifdef __ADMIN_MANAGER__
	if (bAdminManagerNewMap && IsNPC())
	{
		if (bAdminManagerIsOldMap)
		{
			CAdminManager::instance().OnMonsterDestroy(this);
		}
		CAdminManager::instance().OnMonsterCreate(this);
	}
	else if (!bAdminManagerNewMap && IsPC())
	{
		CAdminManager::instance().OnPlayerMove(this);
	}
#endif

	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string	name;
	float		vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap 	gs_bgmInfoMap;
static bool		gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = {"", 0.0f};
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	TPacketGCMainCharacter pack;
	pack.header = HEADER_GC_MAIN_CHARACTER;
	pack.dwVID = m_vid;
	pack.wRaceNum = GetRaceNum();
	pack.lx = GetX();
	pack.ly = GetY();
	pack.lz = GetZ();
	pack.empire = GetDesc()->GetEmpire();
	pack.skill_group = GetSkillGroup();
	strlcpy(pack.szName, GetName(), sizeof(pack.szName));
	pack.fBGMVol = bgmInfo.vol;
	strlcpy(pack.szBGMName, bgmInfo.name.c_str(), sizeof(pack.szBGMName));
	GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));

	if (m_stMobile.length())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
}

void CHARACTER::PointsPacket()
{
	if (IsSkipUpdatePacket())
	{
		return;
	}

	if (!GetDesc())
	{
		return;
	}

	TPacketGCPoints pack;

	pack.header	= HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL]		= GetLevel();
	pack.points[POINT_EXP]			= GetExp();
	pack.points[POINT_NEXT_EXP]		= GetNextExp();
	pack.points[POINT_HP]			= GetHP();
	pack.points[POINT_MAX_HP]		= GetMaxHP();
	pack.points[POINT_SP]			= GetSP();
	pack.points[POINT_MAX_SP]		= GetMaxSP();
	pack.points[POINT_GOLD]			= GetGold();
	pack.points[POINT_STAMINA]		= GetStamina();
	pack.points[POINT_MAX_STAMINA]	= GetMaxStamina();

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
	{
		pack.points[i] = GetPoint(i);
	}

#ifdef __GAYA_SHOP_SYSTEM__
	pack.points[POINT_GAYA] = GetGayaPoints();
#endif

	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
	case MAIN_RACE_WARRIOR_M:
		m_points.job = MAIN_RACE_WARRIOR_W;
		break;

	case MAIN_RACE_WARRIOR_W:
		m_points.job = MAIN_RACE_WARRIOR_M;
		break;

	case MAIN_RACE_ASSASSIN_M:
		m_points.job = MAIN_RACE_ASSASSIN_W;
		break;

	case MAIN_RACE_ASSASSIN_W:
		m_points.job = MAIN_RACE_ASSASSIN_M;
		break;

	case MAIN_RACE_SURA_M:
		m_points.job = MAIN_RACE_SURA_W;
		break;

	case MAIN_RACE_SURA_W:
		m_points.job = MAIN_RACE_SURA_M;
		break;

	case MAIN_RACE_SHAMAN_M:
		m_points.job = MAIN_RACE_SHAMAN_W;
		break;

	case MAIN_RACE_SHAMAN_W:
		m_points.job = MAIN_RACE_SHAMAN_M;
		break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case MAIN_RACE_WOLFMAN_M:
		m_points.job = MAIN_RACE_WOLFMAN_M;
		break;
#endif
	default:
		sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
		return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
	return true;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
	{
		return m_dwPolymorphRace;
	}

	if (m_pkMobData)
	{
		return m_pkMobData->m_table.dwVnum;
	}

	return m_points.job;
}

#ifdef __ADMIN_MANAGER__
uint16_t CHARACTER::GetRealRaceNum() const
{
	if (m_pkMobData)
	{
		return (uint16_t)m_pkMobData->m_table.dwVnum;
	}

	return (uint16_t)m_points.job;
}
#endif

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

#ifdef __ADMIN_MANAGER__
	//m_dwChatBanCount = t->chat_ban_count;
#endif

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
	{
		return job;
	}

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE level)
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
		{
			SetPKMode(PK_MODE_PROTECT);
		}
		else if (GetGMLevel() != GM_PLAYER)
		{
			SetPKMode(PK_MODE_PROTECT);
		}
		else if (m_bPKMode == PK_MODE_PROTECT)
		{
			SetPKMode(PK_MODE_PEACE);
		}

		// Check equipment level restrictions
		for (auto i = 0; i < WEAR_MAX; ++i)
		{
			const auto& item = GetWear(i);
			if (!item)
			{
				continue;
			}

			if (item->GetLevelLimit() > level)
			{
				// Unequip item if level restriction check fails
				UnequipItem(item);
			}
		}
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

#define ENABLE_GM_FLAG_IF_TEST_SERVER
#define ENABLE_GM_FLAG_FOR_LOW_WIZARD
void CHARACTER::SetPlayerProto(const TPlayerTable * t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
	{
		sys_err("cannot get desc or hostname");
	}
	else
	{
		SetGMLevel();
	}

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group;

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#ifdef __ENABLE_SASH_SYSTEM__
	SetPart(PART_SASH, t->parts[PART_SASH]);
#endif
	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	// REMOVE_REAL_SKILL_LEVLES
	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
	}

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	if (m_letterSlots)
	{
		M2_DELETE_ARRAY(m_letterSlots);
	}

	m_letterSlots = M2_NEW TPlayerLetterSlot[FIND_LETTERS_SLOTS_NUM];
	thecore_memcpy(m_letterSlots, t->letterSlots, sizeof(TPlayerLetterSlot) * FIND_LETTERS_SLOTS_NUM);
#endif

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

#ifdef __GAYA_SHOP_SYSTEM__
	m_points.gaya_coins = t->gaya_coins;
	SetPoint(POINT_GAYA, t->gaya_coins);
#endif

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);

	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);

	ComputePoints();

	SetHP(t->hp);
	SetSP(t->sp);
	SetStamina(t->stamina);

#ifndef ENABLE_GM_FLAG_IF_TEST_SERVER
	if (!test_server)
#endif
	{
#ifdef ENABLE_GM_FLAG_FOR_LOW_WIZARD
		if (GetGMLevel() > GM_PLAYER)
#else
		if (GetGMLevel() > GM_LOW_WIZARD)
#endif
		{
			m_afAffectFlag.Set(AFF_YMIR);
			m_bPKMode = PK_MODE_PROTECT;
		}
	}

	if (GetLevel() < PK_PROTECT_LEVEL)
	{
		m_bPKMode = PK_MODE_PROTECT;
	}

	m_stMobile = t->szMobile;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
	{
		UpdateHorseDataByLogoff(t->logoff_interval);
	}

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}

#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		delete m_petSystem;
	}

	m_petSystem = new CPetSystem(*this);
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	if (m_shamanSystem)
		delete m_shamanSystem;

	m_shamanSystem = new CShamanSystem(*this);
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
	SetCostumeFlag(t->dwCostumeFlag);
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	if (m_pkBiologManager)
	{
		sys_err("Biolog manager already exist for owner %u %s", GetPlayerID(), GetName());
		delete m_pkBiologManager;
	}

	SetBiologMissions(t->m_BiologActualMission);
	SetBiologCollectedItems(t->m_BiologCollectedItems);
	SetBiologCooldownReminder(t->m_BiologCooldownReminder);
	SetBiologCooldown(t->m_BiologCooldown);

	m_pkBiologManager = M2_NEW CBiologSystem(this);
#endif
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	SetMarbleActiveMission(t->m_MarbleActiveMission);
	SetMarbleActiveExtandedCount(t->m_MarbleActiveExtandedCount);
	SetMarbleKilledMonsters(t->m_MarbleKilledMonsters);
#endif
#ifdef __ACTION_RESTRICTIONS__
	if (m_pkRestrictions)
	{
		M2_DELETE(m_pkRestrictions);
	}

	m_pkRestrictions = M2_NEW CActionRestricts(this);
#endif
}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "kill_ore_load_even> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)   // <Factor>
	{
		return 0;
	}

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob * pkMob)
{
	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
	}

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable * t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

	// XXX X-mas santa hardcoding
	if (GetRaceNum() == xmas::MOB_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		SetPoint(POINT_DEF_GRADE_BONUS, 6);

		//m_dwPlayStartTime = get_dword_time() + 10 * 60 * 1000;
		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
		{
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		}
	}

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (m_bCharType == CHAR_TYPE_HORSE ||
			GetRaceNum() == 20101 ||
			GetRaceNum() == 20102 ||
			GetRaceNum() == 20103 ||
			GetRaceNum() == 20104 ||
			GetRaceNum() == 20105 ||
			GetRaceNum() == 20106 ||
			GetRaceNum() == 20107 ||
			GetRaceNum() == 20108 ||
			GetRaceNum() == 20109
	   )
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre (GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable & CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
	{
		fDamMultiply = fDamMultiply * 2.0f;
	}

	return fDamMultiply;
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
	{
		return MOB_RANK_KNIGHT;
	}

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
	{
		return MOBSIZE_MEDIUM;
	}

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{
	switch (GetMobBattleType())
	{
	case BATTLE_TYPE_RANGE:
	case BATTLE_TYPE_MAGIC:
		return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);
	default:
		return m_pkMobData->m_table.wAttackRange;
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
	{
		return BATTLE_TYPE_MELEE;
	}

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	/*
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	*/
	if (IsPC())
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		//
		// ATK = 2lev + 2str
		//
		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
		case JOB_WARRIOR:
		case JOB_SURA:
			iStatAtk = (2 * GetPoint(POINT_ST));
			break;

		case JOB_ASSASSIN:
			iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
			break;

		case JOB_SHAMAN:
			iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
			break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		case JOB_WOLFMAN:
			iStatAtk = (2 * GetPoint(POINT_ST));
			break;
#endif
		default:
			sys_err("invalid job %d", GetJob());
			iStatAtk = (2 * GetPoint(POINT_ST));
			break;
		}

		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
		{
			iStatAtk = (2 * GetPoint(POINT_ST));
		}

		iAtk += iStatAtk;

		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		//
		// ATK Setting
		//
		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT);
		int iDef = GetLevel() + (int) (GetPoint(POINT_HT) / 1.25); // For Other
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
			if ((pkItem = GetWear(i)) && pkItem->GetType() == ITEM_ARMOR)
			{
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD || pkItem->GetSubType() == ARMOR_PENDANT)
#else
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD)
#endif
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}

#ifdef __SASH_ABSORPTION_ENABLE__
				if (GetWear(WEAR_COSTUME_SASH) && SashSystemHelper::HasAbsorption(GetWear(WEAR_COSTUME_SASH)))
				{
					auto a_pProto = ITEM_MANAGER::instance().GetTable_NEW(SashSystemHelper::GetAbsorptionBase(GetWear(WEAR_COSTUME_SASH)));
					if (a_pProto && a_pProto->bType == ITEM_ARMOR)
						iArmor += std::max((int)1, static_cast<int>((static_cast<float>(a_pProto->alValues[1]) + static_cast<int>(static_cast<float>(a_pProto->alValues[5] * 2))) * SashSystemHelper::GetAbsorptionValue(GetWear(WEAR_COSTUME_SASH)) / 100.0f));
				}
#endif
			}

		if ( true == IsHorseRiding() )
		{
			if (iArmor < GetHorseArmor())
			{
				iArmor = GetHorseArmor();
			}

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		// INTERNATIONAL_VERSION
		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		// END_OF_INTERNATIONAL_VERSION

		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	// Skip update packets being sent to client
	this->SetSkipUpdatePacket(true);

	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);

#ifdef __GAYA_SHOP_SYSTEM__
	long lGayaPoints = GetPoint(POINT_GAYA);
#endif

	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

#ifdef __GAYA_SHOP_SYSTEM__
	SetPoint(POINT_GAYA, lGayaPoints);
#endif

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));

#ifdef __ENABLE_SASH_SYSTEM__
	SetPart(PART_SASH, GetOriginalPart(PART_SASH));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

	// PC_BANG_ITEM_ADD
	SetPoint(POINT_PC_BANG_EXP_BONUS, 0);
	SetPoint(POINT_PC_BANG_DROP_BONUS, 0);
	// END_PC_BANG_ITEM_ADD

	int iMaxHP, iMaxSP;
	int iMaxStamina;

	if (IsPC())
	{
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

				iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
			}
		}

		SetPoint(POINT_MOV_SPEED,	100);
		SetPoint(POINT_ATT_SPEED,	100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED,	100);
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
		if (GetMountVnum())
		{
			if (GetHorseST() > GetPoint(POINT_ST))
			{
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));
			}

			if (GetHorseDX() > GetPoint(POINT_DX))
			{
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));
			}

			if (GetHorseHT() > GetPoint(POINT_HT))
			{
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));
			}

			if (GetHorseIQ() > GetPoint(POINT_IQ))
			{
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));
			}
		}

	}

#ifdef __ENABLE_PASSIVE_SKILLS_HELPER__
	if (IsPC())
		PSkillsManager::ComputePoints(this);
#endif

	ComputeBattlePoints();

	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP);
	}

	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP);
	}

	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);
	// @fixme118 part1
	int iCurHP = this->GetHP();
	int iCurSP = this->GetSP();

	m_pointsInstant.dwImmuneFlag = 0;

	for (int i = 0 ; i < WEAR_MAX_NUM; i++)
	{
		LPITEM pItem = GetWear(i);
		if (pItem)
		{
			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
		}
	}

#ifdef __ITEM_TOGGLE_SYSTEM__
	// Enabled ITEM_TOGGLE items
	for (uint32_t i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		const auto item = GetInventoryItem(i);
		if (!item || item->GetType() != ITEM_TOGGLE)
		{
			continue;
		}

		if (!item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
		{
			continue;
		}

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
		if (item->GetSubType() == TOGGLE_MOUNT && item->GetSocket(ITEM_SOCKET_TOGGLE_RIDING) == 1)
		{
			ComputeMountPoints();
			continue;
		}
#endif

#ifdef __PET_SYSTEM__
		if (item->GetSubType() == TOGGLE_PET)
		{
			if (GetPetSystem())
			{
				GetPetSystem()->RefreshBuffs();
				continue;
			}
		}
#endif

		{
			item->ModifyPoints(true);
		}
	}
#endif

	if (DragonSoul_IsDeckActivated())
	{
		for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
				i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
				{
					pItem->ModifyPoints(true);
				}
			}
		}
	}

	ComputeSkillPoints();

	RefreshAffect();

	CheckMaximumPoints();

#ifdef __ENABLE_DS_SET__
	if (DragonSoul_IsDeckActivated())
	{
		DragonSoul_HandleSetBonus();
	}
#endif

	// @fixme118 part2 (after petsystem stuff)
	if (IsPC())
	{
		if (this->GetHP() != iCurHP)
		{
			this->PointChange(POINT_HP, iCurHP - this->GetHP());
		}
		if (this->GetSP() != iCurSP)
		{
			this->PointChange(POINT_SP, iCurSP - this->GetSP());
		}
	}

	// Enable update packets for character again
	this->SetSkipUpdatePacket(false);

	PointsPacket();
	UpdatePacket();
}

void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "recovery_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL)   // <Factor>
	{
		return 0;
	}

	if (!ch->IsPC())
	{
		if (ch->IsAffectFlag(AFF_POISON))
		{
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
		}
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		if (ch->IsAffectFlag(AFF_BLEEDING))
		{
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
		}
#endif
		if (2493 == ch->GetMobTable().dwVnum)
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i = 1 ; i <= 4 ; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					regenPct += (val * cnt);

					break;
				}
			}

			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		if (2493 == ch->GetMobTable().dwVnum)
		{
			for (int i = 1 ; i <= 4 ; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val * cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		ch->CheckTarget();
		//ch->UpdateSectree();
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			return 3;
		}
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		if (ch->IsAffectFlag(AFF_BLEEDING))
		{
			return 3;
		}
#endif
		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
		{
			return PASSES_PER_SEC(3);
		}

		int iPercent = 0;
		int iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
	{
		return;
	}

	if (IsDead() || IsStun())
	{
		return;
	}

	if (IsNPC() && GetHP() >= GetMaxHP())
	{
		return;
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	int iSec = IsPC() ? 3 : (MAX(1, GetMobTable().bRegenCycle));
	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
	{
		return;
	}

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
	{
		return;
	}

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
	{
		return false;
	}

	if (GetMyShop())
	{
		return false;
	}

	/*
	   if (get_float_time() - m_fSyncTime < 0.2f)
	   return false;
	 */
	return true;
}

bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
	{
		return false;
	}

#ifdef __ABUSE_CONTROLLER_ENABLE__
	if (IsPC() && IsDead())
	{
		GetAbuseController()->DeadWalk();
		return false;
	}
#endif

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);

	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("cannot find tree at %d %d (name: %s)", x, y, GetName());
			GetDesc()->SetPhase(PHASE_CLOSE);
		}
		else
		{
			sys_err("no tree: %s %d %d %d", GetName(), x, y, GetMapIndex());
			Dead();
		}

		return false;
	}

#ifdef __ABUSE_CONTROLLER_ENABLE__
	if (IsPC())
	{
		spAbuseController controller = GetAbuseController();
		controller->VerifyCoordinates(new_tree, x, y, GetZ());
		controller->CheckSpeedhack(x, y);
	}
#endif

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

	if (GetSectree() != new_tree)
	{
		if (!IsNPC())
		{
			SECTREEID id = new_tree->GetID();
			SECTREEID old_id = GetSectree()->GetID();

			const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
			sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y,
					fDist);
		}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
	{
		MonsterLog("[IDLE] 정지");
	}

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
	if (GetX() == x && GetY() == y)
	{
		return false;
	}

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;


	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "대상추적" : "그냥이동");

		if (GetVictim())
		{
			//MonsterChat(MONSTER_CHAT_CHASE);
			MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}


DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
	{
		return dwMode;
	}

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
		case WEAPON_SWORD:
			dwMode = MOTION_MODE_ONEHAND_SWORD;
			break;

		case WEAPON_TWO_HANDED:
			dwMode = MOTION_MODE_TWOHAND_SWORD;
			break;

		case WEAPON_DAGGER:
			dwMode = MOTION_MODE_DUALHAND_SWORD;
			break;

		case WEAPON_BOW:
			dwMode = MOTION_MODE_BOW;
			break;

		case WEAPON_BELL:
			dwMode = MOTION_MODE_BELL;
			break;

		case WEAPON_FAN:
			dwMode = MOTION_MODE_FAN;
			break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		case WEAPON_CLAW:
			dwMode = MOTION_MODE_CLAW;
			break;
#endif
		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion * pkMotion = NULL;

	if (!GetMountVnum())
	{
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
		{
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
		}
	}

	if (pkMotion)
	{
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	}
	else
	{
		sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
										 (int) ((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
				GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
				m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

bool CHARACTER::Move(long x, long y)
{
	if (GetX() == x && GetY() == y)
	{
		return true;
	}

	if (test_server)
		if (m_bDetailLog)
		{
			sys_log(0, "%s position %u %u", GetName(), x, y);
		}

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int) GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

int CHARACTER::GetRealPoint(BYTE type) const
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(BYTE type, int val)
{
	m_points.points[type] = val;
}

int CHARACTER::GetPolymorphPoint(BYTE type) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
			case POINT_ST:
				if ((GetJob() == JOB_SHAMAN) || ((GetJob() == JOB_SURA) && (GetSkillGroup() == 2)))
				{
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
				}
				return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

			case POINT_HT:
				return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

			case POINT_IQ:
				return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

			case POINT_DX:
				return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}

int CHARACTER::GetPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;

	switch (type)
	{
	case POINT_STEAL_HP:
	case POINT_STEAL_SP:
		max_val = 50;
		break;
	}

	if (val > max_val)
	{
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);
	}

	return (val);
}

int CHARACTER::GetLimitPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
	int limit = INT_MAX;
	int min_limit = -INT_MAX;

	switch (type)
	{
	case POINT_ATT_SPEED:
		min_limit = 0;

		if (IsPC())
		{
			limit = 170;
		}
		else
		{
			limit = 250;
		}
		break;

	case POINT_MOV_SPEED:
		min_limit = 0;

		if (IsPC())
		{
			limit = 200;
		}
		else
		{
			limit = 250;
		}
		break;

	case POINT_STEAL_HP:
	case POINT_STEAL_SP:
		limit = 50;
		max_val = 50;
		break;

	case POINT_MALL_ATTBONUS:
	case POINT_MALL_DEFBONUS:
		limit = 20;
		max_val = 50;
		break;
	}

	if (val > max_val)
	{
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);
	}

	if (val > limit)
	{
		val = limit;
	}

	if (val < min_limit)
	{
		val = min_limit;
	}

	return (val);
}

void CHARACTER::SetPoint(BYTE type, int val)
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

#ifdef __EXTANDED_GOLD_AMOUNT__
int64_t CHARACTER::GetAllowedGold() const
{
	return max_gold;
}
#else
INT CHARACTER::GetAllowedGold() const
{
	if (GetLevel() <= 10)
	{
		return 100000;
	}
	else if (GetLevel() <= 20)
	{
		return 500000;
	}
	else
	{
		return 50000000;
	}
}
#endif
void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
	{
		PointChange(POINT_HP, GetMaxHP() - GetHP());
	}

	if (GetMaxSP() < GetSP())
	{
		PointChange(POINT_SP, GetMaxSP() - GetSP());
	}
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	void CHARACTER::PointChange(BYTE type, int64_t amount, bool bAmount, bool bBroadcast)
#else
	void CHARACTER::PointChange(BYTE type, int amount, bool bAmount, bool bBroadcast)
#endif
{
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t val = 0;
#else
	int val = 0;
#endif

	switch (type)
	{
	case POINT_NONE:
		return;

	case POINT_LEVEL:
		if (IsPC() && (GetLevel() + amount) > gPlayerMaxLevel)
		{
			return;
		}

		SetLevel(GetLevel() + amount);
		val = GetLevel();

		if (IsPC())
		{
			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());
		}
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		if (GetJob() == JOB_WOLFMAN)
		{
			if ((5 <= val) && (GetSkillGroup() != 1))
			{
				ClearSkill();
				// set skill group
				SetSkillGroup(1);
				// set skill points
				SetRealPoint(POINT_SKILL, GetLevel() - 1);
				SetPoint(POINT_SKILL, GetRealPoint(POINT_SKILL));
				PointChange(POINT_SKILL, 0);
				// update points (not required)
				// ComputePoints();
				// PointsPacket();
			}
		}
#endif
		PointChange(POINT_NEXT_EXP,	GetNextExp(), false);

#ifdef __ENABLE_LEVEL_INFORMATION__
		if (IsPC())
		{
			if (GetLevel() >= 55)
			{
				char szNoticeLevelUp[QUERY_MAX_LEN];
				snprintf(szNoticeLevelUp, sizeof(szNoticeLevelUp), LC_TEXT("[CH%d]: %s promoted [%u]. Congratulations!"), g_bChannel, GetName(), GetLevel());
				BroadcastNotice(szNoticeLevelUp);
			}
		}
#endif

		if (IsPC() && amount)
		{
			quest::CQuestManager::instance().LevelUp(GetPlayerID());

			LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

			if (GetGuild())
			{
				GetGuild()->LevelChange(GetPlayerID(), GetLevel());
			}

			if (GetParty())
			{
				GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
			}

#ifdef __ENABLE_BEGINNER_MANAGER__
			BeginnerHelper::instance().Broadcast(this);
#endif
		}
		break;

	case POINT_NEXT_EXP:
		val = GetNextExp();
		bAmount = false;
		break;

	case POINT_EXP:
	{
		if (amount > 0)
		{
			if (GetDungeon())
			{
				return;
			}

#ifdef __ENABLE_TELEPORT_SYSTEM__
			auto bValidate = CTeleportManager::instance().ValidateMap(this, this->GetMapIndex());
			if (!bValidate)
			{
				return;
			}
#endif
		}

		DWORD exp = GetExp();
		DWORD next_exp = GetNextExp();

		if (g_bChinaIntoxicationCheck)
		{
			if (IsOverTime(OT_NONE))
			{
				dev_log(LOG_DEB0, "<EXP_LOG> %s = NONE", GetName());
			}
			else if (IsOverTime(OT_3HOUR))
			{
				amount = (amount / 2);
				dev_log(LOG_DEB0, "<EXP_LOG> %s = 3HOUR", GetName());
			}
			else if (IsOverTime(OT_5HOUR))
			{
				amount = 0;
				dev_log(LOG_DEB0, "<EXP_LOG> %s = 5HOUR", GetName());
			}
		}


		if ((amount < 0) && (exp < (DWORD)(-amount)))
		{
			sys_log(1, "%s AMOUNT < 0 %d, CUR EXP: %d", GetName(), -amount, exp);
			amount = -exp;

			SetExp(exp + amount);
			val = GetExp();
		}
		else
		{
#ifdef __ENABLE_ANTY_EXP__
			auto bAntyExp = FindAffect(AFFECT_EXP_CURSE);
			if (bAntyExp && amount > 0)
			{
				return;
			}
#endif

			if (gPlayerMaxLevel <= GetLevel())
			{
				return;
			}

			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "You have gained %d exp.", amount);
			}

			DWORD iExpBalance = 0;

			if (exp + amount >= next_exp)
			{
				iExpBalance = (exp + amount) - next_exp;
				amount = next_exp - exp;

				SetExp(0);
				exp = next_exp;
			}
			else
			{
				SetExp(exp + amount);
				exp = GetExp();
			}

			DWORD q = DWORD(next_exp / 4.0f);
			int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

			if (iLevStep >= 4)
			{
				sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
				iLevStep = 4;
			}

			if (exp >= next_exp && iLevStep < 4)
			{
				for (int i = 0; i < 4 - iLevStep; ++i)
				{
					PointChange(POINT_LEVEL_STEP, 1, false, true);
				}
			}
			else if (exp >= q * 3 && iLevStep < 3)
			{
				for (int i = 0; i < 3 - iLevStep; ++i)
				{
					PointChange(POINT_LEVEL_STEP, 1, false, true);
				}
			}
			else if (exp >= q * 2 && iLevStep < 2)
			{
				for (int i = 0; i < 2 - iLevStep; ++i)
				{
					PointChange(POINT_LEVEL_STEP, 1, false, true);
				}
			}
			else if (exp >= q && iLevStep < 1)
			{
				PointChange(POINT_LEVEL_STEP, 1);
			}

			if (iExpBalance)
			{
				PointChange(POINT_EXP, iExpBalance);
			}

			val = GetExp();
		}
	}
	break;

	case POINT_LEVEL_STEP:
		if (amount > 0)
		{
			val = GetPoint(POINT_LEVEL_STEP) + amount;

			switch (val)
			{
			case 1:
			case 2:
			case 3:
				if ((GetLevel() <= g_iStatusPointGetLevelLimit) &&
						(GetLevel() <= gPlayerMaxLevel) ) // @fixme104
				{
					PointChange(POINT_STAT, 1);
				}
				break;

			case 4:
			{
				int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
				int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

				m_points.iRandomHP += iHP;
				m_points.iRandomSP += iSP;

				if (GetSkillGroup())
				{
					if (GetLevel() >= 5)
					{
						PointChange(POINT_SKILL, 1);
					}

					if (GetLevel() >= 9)
					{
						PointChange(POINT_SUB_SKILL, 1);
					}
				}

				PointChange(POINT_MAX_HP, iHP);
				PointChange(POINT_MAX_SP, iSP);
				PointChange(POINT_LEVEL, 1, false, true);

				val = 0;
			}
			break;
			}

#ifndef __DISABLE_DROP_POTIONS_WHEN_LV_UP__
			if (GetLevel() <= 10)
			{
				AutoGiveItem(27001, 2);
			}
			else if (GetLevel() <= 30)
			{
				AutoGiveItem(27002, 2);
			}
			else
			{
				AutoGiveItem(27002, 2);
			}
#endif

			PointChange(POINT_HP, GetMaxHP() - GetHP());
			PointChange(POINT_SP, GetMaxSP() - GetSP());
			PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

			SetPoint(POINT_LEVEL_STEP, val);
			SetRealPoint(POINT_LEVEL_STEP, val);

			Save();
			UpdatePacket();
		}
		else
		{
			val = GetPoint(POINT_LEVEL_STEP);
		}

		break;

	case POINT_HP:
	{
		if (IsDead() || IsStun())
		{
			return;
		}

		int prev_hp = GetHP();

		amount = MIN(GetMaxHP() - GetHP(), amount);
		SetHP(GetHP() + amount);
		val = GetHP();

		BroadcastTargetPacket();

		if (GetParty() && IsPC() && val != prev_hp)
		{
			GetParty()->SendPartyInfoOneToAll(this);
		}
	}
	break;

	case POINT_SP:
	{
		if (IsDead() || IsStun())
		{
			return;
		}

		amount = MIN(GetMaxSP() - GetSP(), amount);
		SetSP(GetSP() + amount);
		val = GetSP();
	}
	break;

	case POINT_STAMINA:
	{
		if (IsDead() || IsStun())
		{
			return;
		}

		int prev_val = GetStamina();
		amount = MIN(GetMaxStamina() - GetStamina(), amount);
		SetStamina(GetStamina() + amount);
		val = GetStamina();

		if (val == 0)
		{
			// Stamina
			SetNowWalking(true);
		}
		else if (prev_val == 0)
		{
			ResetWalking();
		}

		if (amount < 0 && val != 0)
		{
			return;
		}
	}
	break;

	case POINT_MAX_HP:
	{
		SetPoint(type, GetPoint(type) + amount);

		//SetMaxHP(GetMaxHP() + amount);
		int hp = GetRealPoint(POINT_MAX_HP);
		int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
		add_hp += GetPoint(POINT_MAX_HP);
		add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

		SetMaxHP(hp + add_hp);

		val = GetMaxHP();
	}
	break;

	case POINT_MAX_SP:
	{
		SetPoint(type, GetPoint(type) + amount);

		//SetMaxSP(GetMaxSP() + amount);
		int sp = GetRealPoint(POINT_MAX_SP);
		int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
		add_sp += GetPoint(POINT_MAX_SP);
		add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

		SetMaxSP(sp + add_sp);

		val = GetMaxSP();
	}
	break;

	case POINT_MAX_HP_PCT:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		PointChange(POINT_MAX_HP, 0);
		break;

	case POINT_MAX_SP_PCT:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		PointChange(POINT_MAX_SP, 0);
		break;

	case POINT_MAX_STAMINA:
		SetMaxStamina(GetMaxStamina() + amount);
		val = GetMaxStamina();
		break;

	case POINT_GOLD:
	{
		const int64_t maxAddAmount = GOLD_MAX - GetGold();
		if (amount > maxAddAmount)
		{
			char buf[256];
			snprintf(buf, sizeof(buf), "CUR %lld / ADD %lld", this->GetGold(), amount);
			LogManager::instance().CharLog(this, 0, "OVERFLOW_GOLD", buf);
			return;
		}

		SetGold(GetGold() + amount);
		val = GetGold();
	}
	break;

#ifdef __GAYA_SHOP_SYSTEM__
	case POINT_GAYA:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;
#endif

	case POINT_SKILL:
	case POINT_STAT:
	case POINT_SUB_SKILL:
	case POINT_STAT_RESET_COUNT:
	case POINT_HORSE_SKILL:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		SetRealPoint(type, val);
		break;

	case POINT_DEF_GRADE:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		PointChange(POINT_CLIENT_DEF_GRADE, amount);
		break;

	case POINT_CLIENT_DEF_GRADE:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	case POINT_ST:
	case POINT_HT:
	case POINT_DX:
	case POINT_IQ:
	case POINT_HP_REGEN:
	case POINT_SP_REGEN:
	case POINT_ATT_SPEED:
	case POINT_ATT_GRADE:
	case POINT_MOV_SPEED:
	case POINT_CASTING_SPEED:
	case POINT_MAGIC_ATT_GRADE:
	case POINT_MAGIC_DEF_GRADE:
	case POINT_BOW_DISTANCE:
	case POINT_HP_RECOVERY:
	case POINT_SP_RECOVERY:

	case POINT_ATTBONUS_HUMAN:	// 42
	case POINT_ATTBONUS_ANIMAL:	// 43
	case POINT_ATTBONUS_ORC:	// 44
	case POINT_ATTBONUS_MILGYO:	// 45
	case POINT_ATTBONUS_UNDEAD:	// 46
	case POINT_ATTBONUS_DEVIL:	// 47

	case POINT_ATTBONUS_MONSTER:
	case POINT_ATTBONUS_SURA:
	case POINT_ATTBONUS_ASSASSIN:
	case POINT_ATTBONUS_WARRIOR:
	case POINT_ATTBONUS_SHAMAN:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_ATTBONUS_WOLFMAN:
#endif

	case POINT_POISON_PCT:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_BLEEDING_PCT:
#endif
	case POINT_STUN_PCT:
	case POINT_SLOW_PCT:

	case POINT_BLOCK:
	case POINT_DODGE:

	case POINT_CRITICAL_PCT:
	case POINT_RESIST_CRITICAL:
	case POINT_PENETRATE_PCT:
	case POINT_RESIST_PENETRATE:
	case POINT_CURSE_PCT:

	case POINT_STEAL_HP:		// 48
	case POINT_STEAL_SP:		// 49

	case POINT_MANA_BURN_PCT:	// 50
	case POINT_DAMAGE_SP_RECOVER:	// 51
	case POINT_RESIST_NORMAL_DAMAGE:
	case POINT_RESIST_SWORD:
	case POINT_RESIST_TWOHAND:
	case POINT_RESIST_DAGGER:
	case POINT_RESIST_BELL:
	case POINT_RESIST_FAN:
	case POINT_RESIST_BOW:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_RESIST_CLAW:
#endif
	case POINT_RESIST_FIRE:
	case POINT_RESIST_ELEC:
	case POINT_RESIST_MAGIC:
#ifdef __ENABLE_SASH_SYSTEM__
	case POINT_ACCEDRAIN_RATE:
#endif
#ifdef __ENABLE_MAGIC_REDUCTION_SYSTEM__
	case POINT_RESIST_MAGIC_REDUCTION:
#endif
	case POINT_RESIST_WIND:
	case POINT_RESIST_ICE:
	case POINT_RESIST_EARTH:
	case POINT_RESIST_DARK:
	case POINT_REFLECT_MELEE:	// 67
	case POINT_REFLECT_CURSE:	// 68
	case POINT_POISON_REDUCE:	// 69
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_BLEEDING_REDUCE:
#endif
	case POINT_KILL_SP_RECOVER:	// 70
	case POINT_KILL_HP_RECOVERY:	// 75
	case POINT_HIT_HP_RECOVERY:
	case POINT_HIT_SP_RECOVERY:
	case POINT_MANASHIELD:
	case POINT_ATT_BONUS:
	case POINT_DEF_BONUS:
	case POINT_SKILL_DAMAGE_BONUS:
	case POINT_NORMAL_HIT_DAMAGE_BONUS:

	// DEPEND_BONUS_ATTRIBUTES
	case POINT_SKILL_DEFEND_BONUS:
	case POINT_NORMAL_HIT_DEFEND_BONUS:
#ifdef __ENABLE_12ZI_ELEMENT_ADD__
	case POINT_ATTBONUS_ELEC:
	case POINT_ATTBONUS_FIRE:
	case POINT_ATTBONUS_ICE:
	case POINT_ATTBONUS_WIND:
	case POINT_ATTBONUS_EARTH:
	case POINT_ATTBONUS_DARK:
	case POINT_ATTBONUS_INSECT:
	case POINT_ATTBONUS_DESERT:
	case POINT_ATTBONUS_CZ:
#endif
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;
	// END_OF_DEPEND_BONUS_ATTRIBUTES

	case POINT_PARTY_ATTACKER_BONUS:
	case POINT_PARTY_TANKER_BONUS:
	case POINT_PARTY_BUFFER_BONUS:
	case POINT_PARTY_SKILL_MASTER_BONUS:
	case POINT_PARTY_HASTE_BONUS:
	case POINT_PARTY_DEFENDER_BONUS:

	case POINT_RESIST_WARRIOR :
	case POINT_RESIST_ASSASSIN :
	case POINT_RESIST_SURA :
	case POINT_RESIST_SHAMAN :
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_RESIST_WOLFMAN :
#endif
	case POINT_RESIST_ALL:
	case POINT_ATTBONUS_METIN:
	case POINT_ATTBONUS_BOSS:
	case POINT_RESIST_MONSTER:
	case POINT_RESIST_BOSS:
	case POINT_DUNGEON_DAMAGE_BONUS:
	case POINT_DUNGEON_RECV_DAMAGE_BONUS:
	case POINT_AGGRO_MONSTER_BONUS:
	case POINT_DOUBLE_ITEM_DROP_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	case POINT_MALL_ATTBONUS:
	case POINT_MALL_DEFBONUS:
	case POINT_MALL_EXPBONUS:
	case POINT_MALL_ITEMBONUS:
	case POINT_MALL_GOLDBONUS:
	case POINT_MELEE_MAGIC_ATT_BONUS_PER:
		if (GetPoint(type) + amount > 100)
		{
			sys_err("MALL_BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
			amount = 100 - GetPoint(type);
		}

		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	// PC_BANG_ITEM_ADD
	case POINT_PC_BANG_EXP_BONUS :
	case POINT_PC_BANG_DROP_BONUS :
	case POINT_RAMADAN_CANDY_BONUS_EXP:
		SetPoint(type, amount);
		val = GetPoint(type);
		break;
	// END_PC_BANG_ITEM_ADD

	case POINT_EXP_DOUBLE_BONUS:	// 71
	case POINT_GOLD_DOUBLE_BONUS:	// 72
	case POINT_ITEM_DROP_BONUS:	// 73
	case POINT_POTION_BONUS:	// 74
		if (GetPoint(type) + amount > 100)
		{
			sys_err("BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
			amount = 100 - GetPoint(type);
		}

		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	case POINT_IMMUNE_STUN:		// 76
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		if (val)
		{
			// ChatPacket(CHAT_TYPE_INFO, "IMMUNE_STUN SET_BIT type(%u) amount(%d)", type, amount);
			SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
		}
		else
		{
			// ChatPacket(CHAT_TYPE_INFO, "IMMUNE_STUN REMOVE_BIT type(%u) amount(%d)", type, amount);
			REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
		}
		break;

	case POINT_IMMUNE_SLOW:		// 77
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		if (val)
		{
			SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
		}
		else
		{
			REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
		}
		break;

	case POINT_IMMUNE_FALL:	// 78
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		if (val)
		{
			SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
		}
		else
		{
			REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
		}
		break;

	case POINT_ATT_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_ATT_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_DEF_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_DEF_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_MAGIC_ATT_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_MAGIC_ATT_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_MAGIC_DEF_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_MAGIC_DEF_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_VOICE:
	case POINT_EMPIRE_POINT:
		//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
		val = GetRealPoint(type);
		break;

	case POINT_POLYMORPH:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		SetPolymorph(val);
		break;

	case POINT_MOUNT:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		MountVnum(val);
		break;

	case POINT_ENERGY:
	case POINT_COSTUME_ATTR_BONUS:
	{
		int old_val = GetPoint(type);
		SetPoint(type, old_val + amount);
		val = GetPoint(type);
		BuffOnAttr_ValueChange(type, old_val, val);
	}
	break;

	default:
		sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
		return;
	}

	if (IsPC())
	{
		switch (type)
		{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
			ComputeBattlePoints();
			break;
		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
		}
	}

	if (type == POINT_HP && amount == 0)
	{
		return;
	}

	if (IsSkipUpdatePacket())
	{
		return;
	}

	struct packet_point_change pack;

	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = m_vid;
	pack.type = type;
	pack.value = val;

	if (bAmount)
	{
		pack.amount = amount;
	}
	else
	{
		pack.amount = 0;
	}

	if (!bBroadcast)
	{
		if (IsPC())
		{
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
#ifdef __ADMIN_MANAGER__
			CAdminManager::instance().OnPlayerPointChange(this, type, val);
#endif
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

void CHARACTER::ApplyPoint(BYTE bApplyType, int iVal)
{
	// Check for custom apply compositions
	const auto it = APPLY_COMPOSITIONS.find(bApplyType);
	if (it != APPLY_COMPOSITIONS.end())
	{
		const auto& composition = it->second;

		for (const auto& apply : composition)
		{
			if (apply.first == bApplyType)
			{
				sys_err("Tried to apply %d on apply type %d. Endless loop!", apply.first, bApplyType);
				continue;
			}

			auto value = (apply.second * iVal); /// APPLY_COMPOSITION_REFERENCE_VALUE;

			ApplyPoint(apply.first, value);
		}

		// Compositions are not handled by the switch-case-statement
		return;
	}

	switch (bApplyType)
	{
	case APPLY_NONE:			// 0
		break;;

	case APPLY_CON:
		PointChange(POINT_HT, iVal);
		PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
		PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
		break;

	case APPLY_INT:
		PointChange(POINT_IQ, iVal);
		PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
		break;

	case APPLY_SKILL:
		// SKILL_DAMAGE_BONUS
	{
		// 00000000 00000000 00000000 00000000

		// vnum     ^ add       change
		BYTE bSkillVnum = (BYTE) (((DWORD)iVal) >> 24);
		int iAdd = iVal & 0x00800000;
		int iChange = iVal & 0x007fffff;

		sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

		if (0 == iAdd)
		{
			iChange = -iChange;
		}

		boost::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

		if (iter == m_SkillDamageBonus.end())
		{
			m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
		}
		else
		{
			iter->second += iChange;
		}
	}
		// END_OF_SKILL_DAMAGE_BONUS
	break;

	case APPLY_MAX_HP:
	case APPLY_MAX_HP_PCT:
	{
		int i = GetMaxHP();
		if (i == 0) { break; }
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		float fRatio = (float)GetMaxHP() / (float)i;
		PointChange(POINT_HP, GetHP() * fRatio - GetHP());
	}
	break;

	case APPLY_MAX_SP:
	case APPLY_MAX_SP_PCT:
	{
		int i = GetMaxSP();
		if (i == 0) { break; }
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		float fRatio = (float)GetMaxSP() / (float)i;
		PointChange(POINT_SP, GetSP() * fRatio - GetSP());
	}
	break;

	case APPLY_STR:
	case APPLY_DEX:
	case APPLY_ATT_SPEED:
	case APPLY_MOV_SPEED:
	case APPLY_CAST_SPEED:
	case APPLY_HP_REGEN:
	case APPLY_SP_REGEN:
	case APPLY_POISON_PCT:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case APPLY_BLEEDING_PCT:
#endif
	case APPLY_STUN_PCT:
	case APPLY_SLOW_PCT:
	case APPLY_CRITICAL_PCT:
	case APPLY_PENETRATE_PCT:
	case APPLY_ATTBONUS_HUMAN:
	case APPLY_ATTBONUS_ANIMAL:
	case APPLY_ATTBONUS_ORC:
	case APPLY_ATTBONUS_MILGYO:
	case APPLY_ATTBONUS_UNDEAD:
	case APPLY_ATTBONUS_DEVIL:
	case APPLY_ATTBONUS_WARRIOR:	// 59
	case APPLY_ATTBONUS_ASSASSIN:	// 60
	case APPLY_ATTBONUS_SURA:	// 61
	case APPLY_ATTBONUS_SHAMAN:	// 62
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case APPLY_ATTBONUS_WOLFMAN:
#endif
	case APPLY_ATTBONUS_MONSTER:	// 63
	case APPLY_STEAL_HP:
	case APPLY_STEAL_SP:
	case APPLY_MANA_BURN_PCT:
	case APPLY_DAMAGE_SP_RECOVER:
	case APPLY_BLOCK:
	case APPLY_DODGE:
	case APPLY_RESIST_SWORD:
	case APPLY_RESIST_TWOHAND:
	case APPLY_RESIST_DAGGER:
	case APPLY_RESIST_BELL:
	case APPLY_RESIST_FAN:
	case APPLY_RESIST_BOW:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case APPLY_RESIST_CLAW:
#endif
	case APPLY_RESIST_FIRE:
	case APPLY_RESIST_ELEC:
	case APPLY_RESIST_MAGIC:
	case APPLY_RESIST_WIND:
	case APPLY_RESIST_ICE:
	case APPLY_RESIST_EARTH:
	case APPLY_RESIST_DARK:
	case APPLY_REFLECT_MELEE:
	case APPLY_REFLECT_CURSE:
	case APPLY_ANTI_CRITICAL_PCT:
	case APPLY_ANTI_PENETRATE_PCT:
	case APPLY_POISON_REDUCE:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case APPLY_BLEEDING_REDUCE:
#endif
	case APPLY_KILL_SP_RECOVER:
	case APPLY_EXP_DOUBLE_BONUS:
	case APPLY_GOLD_DOUBLE_BONUS:
	case APPLY_ITEM_DROP_BONUS:
	case APPLY_POTION_BONUS:
	case APPLY_KILL_HP_RECOVER:
	case APPLY_IMMUNE_STUN:
	case APPLY_IMMUNE_SLOW:
	case APPLY_IMMUNE_FALL:
	case APPLY_BOW_DISTANCE:
	case APPLY_ATT_GRADE_BONUS:
	case APPLY_DEF_GRADE_BONUS:
	case APPLY_MAGIC_ATT_GRADE:
	case APPLY_MAGIC_DEF_GRADE:
	case APPLY_CURSE_PCT:
	case APPLY_MAX_STAMINA:
	case APPLY_MALL_ATTBONUS:
	case APPLY_MALL_DEFBONUS:
	case APPLY_MALL_EXPBONUS:
	case APPLY_MALL_ITEMBONUS:
	case APPLY_MALL_GOLDBONUS:
	case APPLY_SKILL_DAMAGE_BONUS:
	case APPLY_NORMAL_HIT_DAMAGE_BONUS:

	// DEPEND_BONUS_ATTRIBUTES
	case APPLY_SKILL_DEFEND_BONUS:
	case APPLY_NORMAL_HIT_DEFEND_BONUS:
	// END_OF_DEPEND_BONUS_ATTRIBUTES

	case APPLY_PC_BANG_EXP_BONUS :
	case APPLY_PC_BANG_DROP_BONUS :

	case APPLY_RESIST_WARRIOR :
	case APPLY_RESIST_ASSASSIN :
	case APPLY_RESIST_SURA :
	case APPLY_RESIST_SHAMAN :
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case APPLY_RESIST_WOLFMAN :
#endif
	case APPLY_ENERGY:
	case APPLY_DEF_GRADE:
	case APPLY_COSTUME_ATTR_BONUS:
	case APPLY_MAGIC_ATTBONUS_PER:
	case APPLY_MELEE_MAGIC_ATTBONUS_PER:

#ifdef __ENABLE_MAGIC_REDUCTION_SYSTEM__
	case APPLY_RESIST_MAGIC_REDUCTION:
#endif
#ifdef __ENABLE_SASH_SYSTEM__
	case APPLY_ACCEDRAIN_RATE:
#endif
#ifdef __ENABLE_12ZI_ELEMENT_ADD__
	case APPLY_ENCHANT_ELECT:
	case APPLY_ENCHANT_FIRE:
	case APPLY_ENCHANT_ICE:
	case APPLY_ENCHANT_WIND:
	case APPLY_ENCHANT_EARTH:
	case APPLY_ENCHANT_DARK:
	case APPLY_ATTBONUS_INSECT:
	case APPLY_ATTBONUS_DESERT:
	case APPLY_ATTBONUS_CZ:
#endif
	case APPLY_RESIST_ALL:
	case APPLY_ATTBONUS_BOSS:
	case APPLY_ATTBONUS_METIN:
	case APPLY_RESIST_MONSTER:
	case APPLY_RESIST_BOSS:
	case APPLY_DUNGEON_DAMAGE_BONUS:
	case APPLY_DUNGEON_RECV_DAMAGE_BONUS:
	case APPLY_AGGRO_MONSTER_BONUS:
	case APPLY_DOUBLE_ITEM_DROP_BONUS:
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		break;

	default:
		sys_err("Unknown apply type %d name %s", bApplyType, GetName());
		break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet)
{
	packet->header	= HEADER_GC_MOTION;
	packet->vid		= m_vid;
	packet->motion	= motion;

	if (victim)
	{
		packet->victim_vid = victim->GetVID();
	}
	else
	{
		packet->victim_vid = 0;
	}
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "save_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL)   // <Factor>
	{
		return 0;
	}
	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
	{
		return;
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
	{
		return;
	}

	if (IsPC())
	{
		return;
	}

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int) sizeof(chatbuf))
	{
		len = sizeof(chatbuf) - 1;
	}

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int) sizeof(chatbuf) - len)
	{
		len += (sizeof(chatbuf) - len) - 1;
	}
	else
	{
		len += len2;
	}

	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size		= sizeof(TPacketGCChat) + len;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = (DWORD)GetVID();
	pack_chat.bEmpire	= 0;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	pack_chat.locale	= -1;
#endif

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
void CHARACTER::ChatPacket(BYTE type, const std::string text, ...)
{
	LPDESC desc = GetDesc();
	if (!desc)
	{
		return;
	}

	const char* format;
	if (type != CHAT_TYPE_COMMAND)
	{
		format = LC::TranslateText(text, LC::TRANSLATION_TYPE_GAME, this->GetLocale());
	}
	else
	{
		format = text.c_str();
	}

	char chatBuffer[CHAT_MAX_LEN + 1];
	int chatLength;

	try
	{
		va_list args;
		va_start(args, text);
		chatLength = vsnprintf(chatBuffer, sizeof(chatBuffer), format, args);
		va_end(args);
	}
	catch (...)
	{
		sys_err("Failed to format chat [\"%s\"]!", format);
	}

	this->SendChatPacket(type, -1, chatLength, chatBuffer);
}

void CHARACTER::ChatPacket(BYTE type, BYTE locale, const std::string text, ...)
{
	LPDESC desc = GetDesc();
	if (!desc)
	{
		return;
	}

	const char* format;
	if (type != CHAT_TYPE_COMMAND)
	{
		format = LC::TranslateText(text, LC::TRANSLATION_TYPE_GAME, this->GetLocale());
	}
	else
	{
		format = text.c_str();
	}

	char chatBuffer[CHAT_MAX_LEN + 1];
	int chatLength;

	try
	{
		va_list args;
		va_start(args, text);
		chatLength = vsnprintf(chatBuffer, sizeof(chatBuffer), format, args);
		va_end(args);
	}
	catch (...)
	{
		sys_err("Failed to format chat [\"%s\"]!", format);
	}

	this->SendChatPacket(type, locale, chatLength, chatBuffer);
}

void CHARACTER::RawChatPacket(BYTE type, BYTE bLocale, const char* text, ...)
{
	LPDESC desc = GetDesc();
	if (!desc)
	{
		return;
	}

	char chatBuffer[CHAT_MAX_LEN + 1];
	int chatLength;

	try
	{
		va_list args;
		va_start(args, text);
		chatLength = vsnprintf(chatBuffer, sizeof(chatBuffer), text, args);
		va_end(args);
	}
	catch (...)
	{
		sys_err("Failed to format chat [\"%s\"]!", text);
	}

	this->SendChatPacket(type, bLocale, chatLength, chatBuffer);
}

void CHARACTER::RawChatPacket(BYTE type, const char* text, ...)
{
	LPDESC desc = GetDesc();
	if (!desc)
	{
		return;
	}

	char chatBuffer[CHAT_MAX_LEN + 1];
	int chatLength;

	try
	{
		va_list args;
		va_start(args, text);
		chatLength = vsnprintf(chatBuffer, sizeof(chatBuffer), text, args);
		va_end(args);
	}
	catch (...)
	{
		sys_err("Failed to format chat [\"%s\"]!", text);
	}

	this->SendChatPacket(type, -1, chatLength, chatBuffer);
}

void CHARACTER::SendChatPacket(BYTE type, int locale, int chatLength, const char* chat)
{
	if (!this->GetDesc())
	{
		return;
	}

	packet_chat packet;
	packet.header = HEADER_GC_CHAT;
	packet.size = sizeof(struct packet_chat) + chatLength;
	packet.type = type;
	packet.id = 0;
	packet.bEmpire = this->GetEmpire();
	packet.locale = locale;

	TEMP_BUFFER buffer;
	buffer.write(&packet, sizeof(packet));
	buffer.write(chat, chatLength);

	this->GetDesc()->Packet(buffer.read_peek(), buffer.size());
}
#else
void CHARACTER::ChatPacket(BYTE type, const char * format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
	{
		return;
	}

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size      = sizeof(struct packet_chat) + len;
	pack_chat.type      = type;
	pack_chat.id        = 0;
	pack_chat.bEmpire   = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
	{
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
	}
}
#endif

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Mining is finished."));
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
	{
		return;
	}

	// @fixme128
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
	{
		return;
	}

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
	{
		return;
	}

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Pickaxe in order to extract ore!"));
		return;
	}

	int count = number(5, 15);

	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

#ifdef __ABUSE_CONTROLLER_ENABLE__
void ELPlainCoord_GetRotatedPixelPosition(long centerX, long centerY, float distance, float rotDegree, long* pdstX, long* pdstY)
{
	float rotRadian = float(3.141592 * rotDegree / 180.0f);
	*pdstX = (long)(centerX + distance * float(sin((double)rotRadian)));
	*pdstY = (long)(centerY + distance * float(cos((double)rotRadian)));
}

bool CHARACTER::IS_VALID_FISHING_POSITION(int* returnPosx, int* returnPosy)
{
	int charX = GetX();
	int charY = GetY();


	LPSECTREE curWaterPostitionTree;

	long fX, fY;
	for (float fRot = 0.0f; fRot <= 180.0f; fRot += 10.0f) //mimics behaviour of client.
	{
		ELPlainCoord_GetRotatedPixelPosition(charX, charY, 600.0f, GetRotation() + fRot, &fX, &fY);
		curWaterPostitionTree = SECTREE_MANAGER::instance().Get(GetRotation(), fX, fY);
		if (curWaterPostitionTree && curWaterPostitionTree->IsAttr(fX, fY, ATTR_WATER))
		{
			*returnPosx = fX;
			*returnPosy = fY;
			return true;
		}
		//No idea if thats needed client uses it.
		ELPlainCoord_GetRotatedPixelPosition(charX, charY, 600.0f, GetRotation() - fRot, &fX, &fY);
		curWaterPostitionTree = SECTREE_MANAGER::instance().Get(GetMapIndex(), fX, fY);
		if (curWaterPostitionTree && curWaterPostitionTree->IsAttr(fX, fY, ATTR_WATER))
		{
			*returnPosx = fX;
			*returnPosy = fY;
			return true;
		}
	}

	return false;
}
#endif

void CHARACTER::fishing()
{
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int	x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot fish here."));
			return;
		}
#ifdef __ABUSE_CONTROLLER_ENABLE__
		int newPosx, newPosy;
		if (!IS_VALID_FISHING_POSITION(&newPosx, &newPosy))
		{
			ChatPacket(CHAT_TYPE_INFO, "You cannot do this there..");
			LogManager::instance().HackLog("FISH_BOT_LOCATION", this);
			return;
		}
#endif
	}

	if (!GetSectree())
	{
		return;
	}
	LPITEM rod = GetWear(WEAR_WEAPON);

	if (!rod || rod->GetType() != ITEM_ROD)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please choose a Fishing Pole."));
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Place the Bait on the Hook first."));
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
			{
				if (info->step == 0)
				{
					CFishing::instance().FishingFail(this);
					event_cancel(&m_pkFishingEvent);
					return;
				}

				info->hang_count -= 1;
			}

			// if (info)
			// CFishing::instance().Take(info, this);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't go fishing without a fishing pole!"));
	}

	// event_cancel(&m_pkFishingEvent);
}


bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
	{
		return;
	}

	if (IsDead())
	{
		return;
	}

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
	{
		MonsterLog("다음상태로어서가자");
	}
}


void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
	{
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	}
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange * pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, DWORD wVal)
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.parts[bPartPos] = wVal;
}

DWORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);
	return m_pointsInstant.parts[bPartPos];
}

DWORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
	case PART_MAIN:
		if (!IsPC())
		{
			return GetPart(PART_MAIN);
		}
		else
		{
			return m_pointsInstant.bBasePart;
		}

	case PART_HAIR:
		return GetPart(PART_HAIR);

#ifdef __ENABLE_SASH_SYSTEM__
	case PART_SASH:
		return GetPart(PART_SASH);
#endif

#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
	case PART_WEAPON:
		return GetPart(PART_WEAPON);
#endif

	default:
		return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		return false;
	}
	// END_OF_TRENT_MONSTER

	if (ch) // @fixme131
	{
		if (!battle_is_attackable(ch, this))
		{
			SendDamagePacket(ch, 0, DAMAGE_BLOCK);
			return false;
		}
	}

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
		{
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
		}

		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
		{
			return false;
		}

		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			if (m_pkChrSyncOwner == ch)
			{
				return true;
			}

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			static const timeval zero_tv = {0, 0};
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	TPacketGCOwnership pack;

	pack.bHeader	= HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID	= ch ? ch->GetVID() : 0;
	pack.dwVictimVID	= GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false);	// false
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
	{
		return true;
	}

	if (get_float_time() - m_fSyncTime >= 3.0f)
	{
		return true;
	}

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
	{
		return;
	}

	if (pkParty && m_pkParty)
	{
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));
	}

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

#ifdef __ENABLE_FIX_DUNGEON_PARTY_CRASH__
	if (m_pkDungeon && IsPC() && !pkParty)
	{
		SetDungeon(NULL);
	}
#else
	//if (m_pkDungeon && IsPC())
	//SetDungeon(NULL);
#endif

	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
		{
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		}
		else
		{
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		}

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD	dwGuestPID;
	DWORD	dwLeaderPID;

	TPartyJoinEventInfo()
		: dwGuestPID( 0 )
		, dwLeaderPID( 0 )
	{
	}
} ;

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "party_request_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
	{
		leader = leader->GetParty()->GetLeaderCharacter();
	}

	if (!leader)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot make a request because the group leader is not online."));
		return false;
	}

	if (m_pkPartyRequestEvent)
	{
		return false;
	}

	if (!IsPC() || !leader->IsPC())
	{
		return false;
	}

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
	{
		return false;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
	case PERR_NONE:
		break;

	case PERR_SERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return false;

	case PERR_DIFFEMPIRE:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot form a group with players from another kingdom."));
		return false;

	case PERR_DUNGEON:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite players while you are in a dungeon."));
		return false;

	case PERR_OBSERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
		return false;

	case PERR_LVBOUNDARY:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
		return false;

	case PERR_LOWLEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
		return false;

	case PERR_HILEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
		return false;

	case PERR_ALREADYJOIN:
		return false;

	case PERR_PARTYISFULL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
		return false;

	default:
		sys_err("Do not process party join error(%d)", errcode);
		return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD) GetVID());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You want to join %s's group."), leader->GetName());
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
	{
		return;
	}

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::DenyToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
	{
		return;
	}

	if (info->dwLeaderPID != GetPlayerID())
	{
		return;
	}

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
	{
		return;
	}

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::AcceptToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
	{
		return;
	}

	if (info->dwLeaderPID != GetPlayerID())
	{
		return;
	}

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
	{
		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is not in this group."));
	}
	else
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
		{
			return;
		}

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode)
		{
		case PERR_NONE:
			member->PartyJoin(this);
			return;
		case PERR_SERVER:
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
			break;
		case PERR_DUNGEON:
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite players while you are in a dungeon."));
			break;
		case PERR_OBSERVER:
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
			break;
		case PERR_LVBOUNDARY:
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
			break;
		case PERR_LOWLEVEL:
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
			break;
		case PERR_HILEVEL:
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
			break;
		case PERR_ALREADYJOIN:
			break;
		case PERR_PARTYISFULL:
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot join this group because it is already full."));
			break;
		}
		default:
			sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo * pInfo = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( pInfo == NULL )
	{
		sys_err( "party_invite_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You do not have the right to invite someone."));
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has declined your group invitation."), pchInvitee->GetName());
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
	case PERR_NONE:
		break;

	case PERR_SERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;

	case PERR_DIFFEMPIRE:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot form a group with players from another kingdom."));
		return;

	case PERR_DUNGEON:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite players while you are in a dungeon."));
		return;

	case PERR_OBSERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
		return;

	case PERR_LVBOUNDARY:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
		return;

	case PERR_LOWLEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
		return;

	case PERR_HILEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
		return;

	case PERR_ALREADYJOIN:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s is already in the group."), pchInvitee->GetName());
		return;

	case PERR_PARTYISFULL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
		return;

	default:
		sys_err("Do not process party join error(%d)", errcode);
		return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
	{
		return;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));


	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You do not have the right to invite someone."));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
	case PERR_NONE:
		break;

	case PERR_SERVER:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;

	case PERR_DUNGEON:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot accept an invitation into a dungeon."));
		return;

	case PERR_OBSERVER:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
		return;

	case PERR_LVBOUNDARY:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
		return;

	case PERR_LOWLEVEL:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
		return;

	case PERR_HILEVEL:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
		return;

	case PERR_ALREADYJOIN:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot accept the invitation."));
		return;

	case PERR_PARTYISFULL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot join this group because it is already full."));
		return;

	default:
		sys_err("ignore party join error(%d)", errcode);
		return;
	}


	if (GetParty())
	{
		pchInvitee->PartyJoin(this);
	}
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has declined your invitation."), pchInvitee->GetName());
	}
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has joined your group."), GetName());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has joined your Group."), pLeader->GetName());

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
	{
		return PERR_DIFFEMPIRE;
	}

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	int	level_limit = 30;
	return (abs(leader->GetLevel() - quest->GetLevel()) <= level_limit);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		return PERR_SERVER;
	}
	else if (pchLeader->GetDungeon())
	{
		return PERR_DUNGEON;
	}
	else if (pchGuest->IsObserverMode())
	{
		return PERR_OBSERVER;
	}
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
	{
		return PERR_LVBOUNDARY;
	}
	else if (pchGuest->GetParty())
	{
		return PERR_ALREADYJOIN;
	}
	else if (pchLeader->GetParty())
	{
		if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
		{
			return PERR_PARTYISFULL;
		}
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
	{
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));
	}

	if (m_pkDungeon == pkDungeon)
	{
		return;
	}

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
			{
				m_pkDungeon->DecPartyMember(GetParty(), this);
			}
			else
			{
				m_pkDungeon->DecMember(this);
			}
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
			{
				m_pkDungeon->IncPartyMember(GetParty(), this);
			}
			else
			{
				m_pkDungeon->IncMember(this);
			}
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

void CHARACTER::SetWarMap(CWarMap * pWarMap)
{
	if (m_pWarMap)
	{
		m_pWarMap->DecMember(this);
	}

	m_pWarMap = pWarMap;

	if (m_pWarMap)
	{
		m_pWarMap->IncMember(this);
	}
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
	{
		m_pWeddingMap->DecMember(this);
	}

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
	{
		m_pWeddingMap->IncMember(this);
	}
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL)
	{
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
		{
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);
		}

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*if (IsAffectFlag(AFF_JEONSIN))
		  RemoveAffect(SKILL_JEONSINBANGEO);*/
	}

	/*if (IsAffectFlag(AFF_GUNGON))
	  RemoveAffect(SKILL_GUNGON);*/

	// MINING
	mining_cancel();
	// END_OF_MINING

#ifdef __ADMIN_MANAGER__
	if (IsPC())
	{
		CAdminManager::instance().OnPlayerMove(this);
	}
	else
	{
		CAdminManager::instance().OnMonsterMove(this);
	}
#endif

#ifdef __ENABLE_PARTY_ATLAS__
	SendPartyPositionInfo();
#endif
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

	{
		if (pkChrCauser->GetMyShop() && pkChrCauser != this)
		{
			sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	{
		if (pkChrCauser->GetExchange())
		{
			sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

#ifdef __SHIP_DUNGEON_ENABLE__
	if (pkChrCauser->GetDungeon())
	{
		CShipDefendDungeonManager::instance().MoveInitialState(pkChrCauser, this);
	}
#endif

	if (IsPC())
	{
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			// 2005.03.17.myevan.
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true) { return; }

				//PREVENT_TRADE_WINDOW
				if (pkChrCauser == this)
				{
#ifdef __ENABLE_DELETE_SINGLE_STONE__
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen() || m_deleteSocket.open)
#else
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen())
#endif
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
						return;
					}
				}
				else
				{
#ifdef __ENABLE_DELETE_SINGLE_STONE__
					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen() || m_deleteSocket.open)
#else
					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen() )
#endif
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
						return;
					}

#ifdef __ENABLE_DELETE_SINGLE_STONE__
					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen()) || m_deleteSocket.open)
#else
					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen()))
#endif
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is already trading with another player."));
						return;
					}
				}
				//END_PREVENT_TRADE_WINDOW

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
				return;
			}

			if (test_server)
			{
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());
			}

			return;
		}
	}

	if (g_bChinaIntoxicationCheck)
	{
		if (pkChrCauser->IsOverTime(OT_3HOUR))
		{
			sys_log(0, "Teen OverTime : name = %s, hour = %d)", pkChrCauser->GetName(), 3);
			return;
		}
		else if (pkChrCauser->IsOverTime(OT_5HOUR))
		{
			sys_log(0, "Teen OverTime : name = %s, hour = %d)", pkChrCauser->GetName(), 5);
			return;
		}
	}

#ifdef __ENABLE_OFFLINE_SHOP__
	auto offlineShopPtr = COfflineShop::Get(GetKeepingOfflineShop());
	static const DWORD m_table[] =
	{
		30000, 30002, 30003, 30004, 30005, 30006, 30007, 30008
	};

	auto isShop = false;
	for (auto it : m_table) if (it == GetRaceNum()) { isShop = true; }
	if (offlineShopPtr && isShop)
	{
		offlineShopPtr->get()->AddViewer(pkChrCauser);
		return;
	}
#endif

	pkChrCauser->SetQuestNPCID(GetVID());

	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}


	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			//sys_err("%s.OnClickFailure(%s) : triggerOnClick.pFunc is EMPTY(pid=%d)",
			//			pkChrCauser->GetName(),
			//			GetName(),
			//			pkChrCauser->GetPlayerID());
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}

}

#ifdef __ADMIN_MANAGER__
	BYTE CHARACTER::GetGMLevel(bool bIgnoreTestServer) const
#else
	BYTE CHARACTER::GetGMLevel() const
#endif
{
#ifdef __ADMIN_MANAGER__
	if (bIgnoreTestServer && IsPC())
	{
		return GM::get_level(GetName(), GetDesc()->GetAccountTable().login, true);
	}
#endif

	if (test_server)
	{
		return GM_IMPLEMENTOR;
	}
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
#ifdef __ADMIN_MANAGER__
		m_pointsInstant.gm_level = GM::get_level(GetName(), GetDesc()->GetAccountTable().login);
#else
		m_pointsInstant.gm_level = gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
#endif
	}
	else
	{
		m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (m_pointsInstant.gm_level != GM_PLAYER)
	{
		return true;
	}
	if (test_server)
	{
		return true;
	}
	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
		{
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
		}
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
	{
		return;
	}

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.lHP = 0;
	p.lMaxHP = 0;
	p.isPoisoned = false;

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}

	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
	{
		return;
	}

	// CASTLE
	if (IS_CASTLE_MAP(GetMapIndex()) && !IsGM())
	{
		return;
	}
	// CASTLE

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
	}

	m_pkChrTarget = pkChrTarget;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;

	p.isPoisoned = false;

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);

		p.dwVID	= m_pkChrTarget->GetVID();
		p.lHP = 1;
		p.lMaxHP = 1;

		if (m_pkChrTarget->IsAffectFlag(AFF_POISON))
		{
			p.isPoisoned = true;
		}

		if (m_pkChrTarget->GetMaxHP() > 0)
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 || m_pkChrTarget->GetRaceNum() == 20102 || m_pkChrTarget->GetRaceNum() == 20103 ||
					m_pkChrTarget->GetRaceNum() == 20104 || m_pkChrTarget->GetRaceNum() == 20105 || m_pkChrTarget->GetRaceNum() == 20106 ||
					m_pkChrTarget->GetRaceNum() == 20107 || m_pkChrTarget->GetRaceNum() == 20108 || m_pkChrTarget->GetRaceNum() == 20109)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();

				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();

					if (iHorseMaxHealth)
					{
						p.lHP = iHorseHealth;
						p.lMaxHP = iHorseMaxHealth;
					}
				}
			}
			else
			{
				p.lHP = m_pkChrTarget->GetHP();
				p.lMaxHP = m_pkChrTarget->GetMaxHP();
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.lHP = 0;
		p.lMaxHP = 0;
	}

	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
	{
		return;
	}

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();

	p.lHP = GetHP();
	p.lMaxHP = GetMaxHP();
	p.isPoisoned = IsAffectFlag(AFF_POISON);

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
	{
		return;
	}

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
	{
		SetTarget(NULL);
	}
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log (0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
{
	if (!IsPC())
	{
		return false;
	}

	if (GetMapIndex() == lPrivateMapIndex)
	{
		Show(lPrivateMapIndex, x, y, 0);
		Stop();
		return true;
	}

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	return WarpSet(x, y, lPrivateMapIndex, lMapIndex, lAddr, wPort);
}

bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex, long lMapIndex, long lAddr, WORD wPort)
{
	//Send Supplementary Data Block if new map requires security packages in loading this map
	{
		long lCurAddr;
		long lCurMapIndex = 0;
		WORD wCurPort;

		CMapLocation::instance().Get(GetX(), GetY(), lCurMapIndex, lCurAddr, wCurPort);

		//do not send SDB files if char is in the same map
		if (lCurMapIndex != lMapIndex)
		{
			const TMapRegion* rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
			{
				DESC_MANAGER::instance().SendClientPackageSDBToLoadMap(GetDesc(), rMapRgn->strMapName.c_str());
			}
		}
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
	p.lAddr = lAddr;
#ifdef __ENABLE_NEWSTUFF__
	if (!g_stProxyIP.empty())
	{
		p.lAddr = inet_addr(g_stProxyIP.c_str());
	}
#endif
	p.wPort = wPort;

#ifdef __ENABLE_SWITCHBOT__
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	return true;
}

void CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex, long lAddr, WORD wPort)
{
	if (!IsPC())
	{
		return;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lPrivateMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lPrivateMapIndex);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
	p.lAddr = lAddr;
#ifdef __ENABLE_NEWSTUFF__
	if (!g_stProxyIP.empty())
	{
		p.lAddr = inet_addr(g_stProxyIP.c_str());
	}
#endif
	p.wPort = wPort;

#ifdef __ENABLE_SWITCHBOT__
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	{
		char buf[256];
		snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
		LogManager::instance().CharLog(this, 0, "WARP", buf);
	}

	return;
}

#define ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
void CHARACTER::WarpEnd()
{
	if (test_server)
	{
		sys_log(0, "WarpEnd %s", GetName());
	}

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
	{
		return;
	}

	int index = m_lWarpMapIndex;

	if (index > 10000)
	{
		index /= 10000;
	}

	if (!map_allow_find(index))
	{
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
#ifdef ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
		GoHome();
#else
		GetDesc()->SetPhase(PHASE_CLOSE);
#endif
		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		p.iLocale = GetLocale();
#endif
		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
	{
		return false;
	}

	int x, y;
	/*
	   float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	   float fx, fy;
	   GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	   x = GetX() + (int) fx;
	   y = GetY() + (int) fy;
	 */
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
	{
		return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
	{
		sys_log(0, "%s %p 포기하고 돌아가자! %d %d", GetName(), this, x, y);
	}

	if (GetParty())
	{
		GetParty()->SendMessage(this, PM_RETURN, x, y);
	}

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC())
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC())
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000)
				{
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
						{
							return true;
						}
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC())
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000)
			{
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
					{
						return true;
					}
			}
		}
	}

	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
			{
				return true;
			}
	}

	if (pkChr->IsState(pkChr->m_stateMove) &&
			GetMobBattleType() != BATTLE_TYPE_RANGE &&
			GetMobBattleType() != BATTLE_TYPE_MAGIC &&
			false == IsPet()
#ifdef __ENABLE_SHAMAN_SYSTEM__
		&& false == IsAutoShaman()
#endif
		)
	{
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if ( fMeetTime * yourSpeed <= 100000.0f )
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long) fYourMoveEstimateX;
				y += (long) fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX()) * (x - GetX()) + ((double)y - GetY()) * (y - GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
	{
		return false;
	}

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int) GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
			{
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			}
			else
			{
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);
			}

			dx = x + (int) fx;
			dy = y + (int) fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
			{
				break;
			}

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
			{
				break;
			}
		}

		if (!Goto(dx, dy))
		{
			return false;
		}
	}
	else
	{
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		if (!Goto(GetX() + (int) fx, GetY() + (int) fy))
		{
			return false;
		}
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox * CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
#ifdef __ADMIN_MANAGER__
	if (!GM::check_allow(GetGMLevel(), GM_ALLOW_USE_SAFEBOX))
	{
		ChatPacket(CHAT_TYPE_INFO, "You cannot do this with this gamemaster rank.");
		return;
	}
#endif

	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}
	else if (m_pkSafebox)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The Storeroom is already open."));
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime()  < PASSES_PER_SEC(10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have to wait 10 seconds before you can open the Storeroom again."));
		return;
	}
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
	{
		bLoaded = true;
	}

	if (!m_pkSafebox)
	{
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	}
	else
	{
		m_pkSafebox->ChangeSize(iSize);
	}

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
			{
				continue;
			}

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

#ifdef __TRANSMUTATION_SYSTEM__
			item->SetTransmutate(pItems->transmutate_id);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
			item->SetRefineElement(pItems->dwRefineElement);
#endif

			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
			{
				item->SetSkipSave(false);
			}
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//return;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
	{
		m_pkSafebox->ChangeSize(bSize);
	}

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
	{
		return;
	}

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	//END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

CSafebox * CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
	{
		bLoaded = true;
	}

	if (!m_pkMall)
	{
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	}
	else
	{
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);
	}

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
			{
				continue;
			}

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

#ifdef __TRANSMUTATION_SYSTEM__
			item->SetTransmutate(pItems->transmutate_id);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
			item->SetRefineElement(pItems->dwRefineElement);
#endif

			if (!m_pkMall->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
			{
				item->SetSkipSave(false);
			}
		}
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
	{
		return;
	}

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate & out)
{
	if (!GetParty())
	{
		return false;
	}

	memset(&out, 0, sizeof(out));

	out.header		= HEADER_GC_PARTY_UPDATE;
	out.pid		= GetPlayerID();
	if (GetMaxHP() <= 0) // @fixme136
	{
		out.percent_hp	= 0;
	}
	else
	{
		out.percent_hp	= MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	}
	out.role		= GetParty()->GetRole(GetPlayerID());

#ifdef __ENABLE_PARTY_UPDATE__
	out.bLeader = GetParty()->GetLeaderPID() == GetPlayerID();
#endif

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
										  GetPlayerID(),
										  NULL,
										  "SELECT size FROM safebox%s WHERE account_id = %u",
										  get_table_postfix(),
										  GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
			{
				MonsterLog("걷는다");
			}
			else
			{
				MonsterLog("뛴다");
			}
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking?"walking.":"running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
	{
		return;
	}
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	}
	else
	{
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	}
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
	{
		return;
	}
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel());

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	// @fixme104
	PointChange(POINT_STAT, (MINMAX(1, iLv, g_iStatusPointGetLevelLimit) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();
	UpdatePacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
	{
		return true;
	}

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
			AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
	{
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;
	}

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(50300);

	if (NULL != item)
	{
		extern const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);
		DWORD dwSkillVnum = 0;
		// 50% of getting random books or getting one of the same player's race
		if (!number(0, 1))
		{
			dwSkillVnum = GetRandomSkillVnum(GetJob());
		}
		else
		{
			dwSkillVnum = GetRandomSkillVnum();
		}
		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	__typeof(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy (p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::MonsterChat(BYTE bMonsterChatType)
{
	if (IsPC())
	{
		return;
	}

	char sbuf[256 + 1];

	if (IsMonster())
	{
		if (number(0, 60))
		{
			return;
		}

		snprintf(sbuf, sizeof(sbuf),
				 "(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
				 GetRaceNum(), GetRaceNum(), bMonsterChatType * 3 + number(1, 3));
	}
	else
	{
		if (bMonsterChatType != MONSTER_CHAT_WAIT)
		{
			return;
		}

		if (IsGuardNPC())
		{
			if (number(0, 6))
			{
				return;
			}
		}
		else
		{
			if (number(0, 30))
			{
				return;
			}
		}

		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
	}

	std::string text = quest::ScriptToString(sbuf);

	if (text.empty())
	{
		return;
	}

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size	= sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);

	PacketAround(buf.read_peek(), buf.size());
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_pQuestItem = item;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_pQuestItem = NULL;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	return m_pQuestItem;
}

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
	{
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);
	}

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
		{
			return value;
		}
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	if (dwRaceNum < MAIN_RACE_MAX_NUM)
#else
	if (dwRaceNum < JOB_MAX_NUM)
#endif
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
	{
		return;
	}

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
	{
		StopRiding();
	}

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

long CHARACTER::GetQuestFlag(const std::string& flag) const
{
	quest::CQuestManager& q = quest::CQuestManager::instance();

#ifdef __ENABLE_FIX_QUEST_FLAG__
	int lFlagValue = 0;
	DWORD dwPID = q.GetCurrentCharacterPtr() ? q.GetCurrentCharacterPtr()->GetPlayerID() : 0;
	LPDUNGEON lpDungeon = q.GetCurrentDungeon();
#endif

	quest::PC* pPC = q.GetPC(GetPlayerID());

#ifdef __ENABLE_FIX_QUEST_FLAG__
	if (pPC)
	{
		lFlagValue = pPC->GetFlag(flag);
	}

	// Restoring proper pointer
	if (dwPID)
	{
		q.GetPC(dwPID);
	}

	// Restoring saved dungeon
	if (lpDungeon)
		q.SelectDungeon(lpDungeon);

	return lFlagValue;
#else
	return pPC->GetFlag(flag);
#endif
}

void CHARACTER::SetQuestFlag(const std::string& flag, long value)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();

#ifdef __ENABLE_FIX_QUEST_FLAG__
	DWORD dwPID = q.GetCurrentCharacterPtr() ? q.GetCurrentCharacterPtr()->GetPlayerID() : 0;
	LPDUNGEON lpDungeon = q.GetCurrentDungeon();
#endif
	quest::PC* pPC = q.GetPC(GetPlayerID());

#ifdef __ENABLE_FIX_QUEST_FLAG__
	if (pPC)
	{
		pPC->SetFlag(flag, value);
	}

	// Restoring proper pointer
	if (dwPID)
	{
		q.GetPC(dwPID);
	}

	// Restoring saved dungeon
	if (lpDungeon)
		q.SelectDungeon(lpDungeon);

#else
	pPC->SetFlag(flag, value);
#endif
}

void CHARACTER::DetermineDropMetinStone()
{
#ifdef __ENABLE_NEWSTUFF__
	if (g_NoDropMetinStone)
	{
		m_dwDropMetinStone = 0;
		return;
	}
#endif

	static const DWORD c_adwMetin[] =
	{
#if defined(__ENABLE_WOLFMAN_CHARACTER__) && defined(__USE_WOLFMAN_STONES__)
		28012,
#endif
		28030,
		28031,
		28032,
		28033,
		28034,
		28035,
		28036,
		28037,
		28038,
		28039,
		//28040,
		28041,
		28042,
		28043,
#if defined(__ENABLE_MAGIC_REDUCTION_SYSTEM__) && defined(__USE_MAGIC_REDUCTION_STONES__)
		28044,
		28045,
#endif
	};
	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStoneDrop, aStoneDrop + STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinStone = 0;
	}
	else
	{
		const SStoneDropInfo & info = aStoneDrop[idx];
		m_bDropMetinStonePct = info.iDropPct;
		{
			m_dwDropMetinStone = c_adwMetin[number(0, sizeof(c_adwMetin) / sizeof(DWORD) - 1)];
			int iGradePct = number(1, 100);
			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel ++)
			{
				int iLevelGradePortion = info.iLevelPct[iStoneLevel];
				if (iGradePct <= iLevelGradePortion)
				{
					break;
				}
				else
				{
					iGradePct -= iLevelGradePortion;
					m_dwDropMetinStone += 100;
				}
			}
		}
	}
}

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid    = GetVID();
	for (int i = 0; i < WEAR_MAX_NUM; i++)
	{
		LPITEM item = GetWear(i);
		if (item)
		{
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
		}
		else
		{
			p.equips[i].vnum = 0;
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return ((iLeaderShip >= 20) || ((iLeaderShip >= 12) && ((m_dwLastDeadTime + 180) > get_dword_time())));
}

// #define ENABLE_MOUNT_ENTITY_REFRESH
#define ALTERNATIVE_MOUNT_REFRESH
void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
	{
		return;
	}

	if ((m_dwMountVnum != 0) && (vnum != 0)) //@fixme108 set recursively to 0 for eventuality
	{
		MountVnum(0);
	}

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
	{
		return;
	}

#ifdef ALTERNATIVE_MOUNT_REFRESH
	// Save pos first
	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();

	// Rerender entities
	RestartAtSamePos();

	// Revalidate combo and recompute points
	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();

	// Deprecated
	// Out of fucking logic
#else
#ifdef ENABLE_MOUNT_ENTITY_REFRESH
	// EncodeRemovePacket(this); // commented, otherwise it may warp you back
#endif
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

#ifdef ENABLE_MOUNT_ENTITY_REFRESH
		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
		{
			EncodeInsertPacket(entity);
		}
#else
		EncodeInsertPacket(entity);
#endif

#ifdef ENABLE_MOUNT_ENTITY_REFRESH
		if (!entity->IsObserverMode())
		{
			entity->EncodeInsertPacket(this);
		}
#endif
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
#endif
}

namespace
{
class FuncCheckWarp
{
public:
	FuncCheckWarp(LPCHARACTER pkWarp)
	{
		m_lTargetY = 0;
		m_lTargetX = 0;

		m_lX = pkWarp->GetX();
		m_lY = pkWarp->GetY();

		m_bInvalid = false;
		m_bEmpire = pkWarp->GetEmpire();

		char szTmp[64];

		if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
		{
			if (number(1, 100) < 5)
			{
				sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());
			}

			m_bInvalid = true;

			return;
		}

		m_lTargetX *= 100;
		m_lTargetY *= 100;

		m_bUseWarp = true;

		if (pkWarp->IsGoto())
		{
			LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
			m_lTargetX += pkSectreeMap->m_setting.iBaseX;
			m_lTargetY += pkSectreeMap->m_setting.iBaseY;
			m_bUseWarp = false;
		}
	}

	bool Valid()
	{
		return !m_bInvalid;
	}

	void operator () (LPENTITY ent)
	{
		if (!Valid())
		{
			return;
		}

		if (!ent->IsType(ENTITY_CHARACTER))
		{
			return;
		}

		LPCHARACTER pkChr = (LPCHARACTER) ent;

		if (!pkChr->IsPC())
		{
			return;
		}

		int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

		if (iDist > 300)
		{
			return;
		}

		if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
		{
			return;
		}

		if (pkChr->IsHack())
		{
			return;
		}

		if (!pkChr->CanHandleItem(false, true))
		{
			return;
		}

		if (m_bUseWarp)
		{
			pkChr->WarpSet(m_lTargetX, m_lTargetY);
		}
		else
		{
			pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
			pkChr->Stop();
		}
	}

	bool m_bInvalid;
	bool m_bUseWarp;

	long m_lX;
	long m_lY;
	long m_lTargetX;
	long m_lTargetY;

	BYTE m_bEmpire;
};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "warp_npc_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL)   // <Factor>
	{
		return 0;
	}

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
	{
		ch->GetSectree()->ForEachAround(f);
	}

	return passes_per_sec / 2;
}


void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
	{
		return;
	}

	if (!IsWarp() && !IsGoto())
	{
		return;
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
	{
		return 0;
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
	{
		return 0;
	}

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
	{
		return;
	}

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
	{
		return 0;
	}

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx) || IsGM())
		{
			if (CAN_ENTER_ZONE(this, mapIdx) || IsGM())
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
			return false;
		}
	}
	else
	{
		CCI * pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is not online."));
			return false;
		}

		//if (pcci->bChannel != g_bChannel)
		//{
		//	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player is playing on channel %d. (You are on channel %d.)"), pcci->bChannel, g_bChannel);
		//	return false;
		//}

		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex) && !IsGM())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex) && !IsGM())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			p.bAllowDungeon = IsGM();
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
			{
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
			}
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
	LPCHARACTER chRefineNPC = GetRefineNPC();
	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
#else
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
#endif
}

#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
LPCHARACTER CHARACTER::GetRefineNPC() const
{
	if (!m_dwRefineNPCVID)
	{
		return nullptr;
	}

	return CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
}
#endif

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
		{
			return iCost * iMultiply * 9 / 10;
		}

#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
		LPCHARACTER chRefineNPC = GetRefineNPC();
#else
		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
#endif
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
		{
			return iCost * iMultiply * 3;
		}

		return iCost * iMultiply;
	}
	else
	{
		return iCost;
	}
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);

#ifdef __BATTLE_PASS_ENABLE__
	if (iRemain < 0) // shouldnt happen
		iRemain = 0;

	CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, (uint64_t)iRemain });
#endif
}
// END_OF_ADD_REFINE_BUILDING

bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
	{
		bSendMsg = true;
	}

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After opening the Storeroom, you cannot go anywhere else for %d seconds."), limittime);
		}

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		}
		return true;
	}

	if (bCheckShopOwner)
	{
#ifdef __ENABLE_DELETE_SINGLE_STONE__
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen() || m_deleteSocket.open)
#else
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
#endif
		{
			if (bSendMsg)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot go elsewhere for a period of time after the trade."));
			}

			return true;
		}
	}
	else
	{
#ifdef __ENABLE_DELETE_SINGLE_STONE__
		if (GetExchange() || GetMyShop() || IsOpenSafebox() || IsCubeOpen() || m_deleteSocket.open)
#else
		if (GetExchange() || GetMyShop() || IsOpenSafebox() || IsCubeOpen())
#endif
		{
			if (bSendMsg)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot go elsewhere for a period of time after the trade."));
			}

			return true;
		}
	}

	//PREVENT_PORTAL_AFTER_EXCHANGE
	if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After trading you cannot go elsewhere for %d seconds."), limittime );
		}
		return true;
	}
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	//PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After trading you cannot go elsewhere for %d seconds."), limittime);
		}
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you cannot use a scroll for another %d seconds."), limittime);
		}
		return true;
	}
	//END_PREVENT_ITEM_COPY

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	if ((iPulse - m_deleteSocket.cooltime) < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you cannot use a scroll for another %d seconds."), limittime);
		}
		return true;
	}
#endif

	return false;
}

void CHARACTER::Say(const std::string & s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

bool CHARACTER::IsSiegeNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60 * 5);
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
	case MAIN_RACE_WARRIOR_M:
	case MAIN_RACE_SURA_M:
	case MAIN_RACE_ASSASSIN_M:
	case MAIN_RACE_SHAMAN_M:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case MAIN_RACE_WOLFMAN_M:
#endif
		return SEX_MALE;

	case MAIN_RACE_ASSASSIN_W:
	case MAIN_RACE_SHAMAN_W:
	case MAIN_RACE_WARRIOR_W:
	case MAIN_RACE_SURA_W:
		return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	if (GetMaxHP() <= 0) // @fixme136
	{
		return 0;
	}
	return (GetHP() * 100) / GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsBerserk;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsBerserk = mode;
	}
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) { return false; }

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
	{
		return 0;
	}

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		// write hack log
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);

		if (g_bEnableSpeedHackCrash)
		{
			// close connection
			LPDESC desc = ch->GetDesc();

			if (desc)
			{
				DESC_MANAGER::instance().DestroyDesc(desc);
				return 0;
			}
		}
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
	{
		return;
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60));
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) { return; }

	DESC	*desc = GetDesc();

	if (NULL == desc) { return; }
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) { return; }

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName	pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header		= HEADER_GC_GUILD;
	pack.subheader	= GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size		= sizeof(TPacketGCGuildName);
	pack.guildID	= pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "destroy_when_idle_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL)   // <Factor>
	{
		return 0;
	}

	if (ch->GetVictim())
	{
		return PASSES_PER_SEC(300);
	}

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
	{
		return;
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

void CHARACTER::ResetChatCounter()
{
	m_bChatCounter = 0;
}

BYTE CHARACTER::IncreaseChatCounter()
{
	return ++m_bChatCounter;
}

BYTE CHARACTER::GetChatCounter() const
{
	return m_bChatCounter;
}

bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
	{
		return false;
	}

	if ((iPulse - GetExchangeTime()) < limit_time)
	{
		return false;
	}

	if ((iPulse - GetMyShopTime()) < limit_time)
	{
		return false;
	}

	if ((iPulse - GetRefineTime()) < limit_time)
	{
		return false;
	}

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	if (((iPulse - m_deleteSocket.cooltime) < limit_time) || m_deleteSocket.open)
	{
		return false;
	}
#endif

#ifdef __ITEM_SHOP_ENABLE__
	if ((iPulse - GetItemShopTime()) < PASSES_PER_SEC(3))
	{
		return false;
	}
#endif

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
	{
		return false;
	}

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_MAX_LEVEL_CONST < GetLevel())
	{
		return 2500000000u;
	}
	else
	{
		return exp_table[GetLevel()];
	}
}

int	CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob);
}

#ifdef __ENABLE_TREASURE_BOX_LOOT__
void CHARACTER::SendTreasureBoxLoot(DWORD dwItemVnum)
{
	TPacketCGDefault packet(HEADER_GC_REQUEST_TREASURE_BOX_LOOT);
	packet.size += sizeof(DWORD);

	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwItemVnum);

	if (pGroup)
	{
		const size_t PacketSize = sizeof(pGroup->m_vecItems[0]) * pGroup->m_vecItems.size();
		packet.size += PacketSize;

		TEMP_BUFFER buff(PacketSize + sizeof(DWORD));
		buff.write(&pGroup->m_vecItems[0], PacketSize);
		buff.write(&dwItemVnum, sizeof(DWORD));

		GetDesc()->BufferedPacket(&packet, sizeof(TPacketCGDefault));
		GetDesc()->Packet(buff.read_peek(), buff.size());
	}
	else
	{
		TEMP_BUFFER buff(sizeof(DWORD));
		buff.write(&dwItemVnum, sizeof(DWORD));

		GetDesc()->BufferedPacket(&packet, sizeof(TPacketCGDefault));
		GetDesc()->Packet(buff.read_peek(), buff.size());
	}
}
#endif

#ifdef __RENEWAL_DEAD_PACKET__
#include "threeway_war.h"
DWORD CHARACTER::CalculateDeadTime(BYTE type)
{
	//generated from do_restart(cmd_general.cpp)
	if (!m_pkDeadEvent)
	{
		return 0;
	}

	int iTimeToDead = (event_time(m_pkDeadEvent) / passes_per_sec);

	if (type == REVIVE_TYPE_AUTO_TOWN)
	{
		return iTimeToDead - 7;
	}

	if (!test_server && type == REVIVE_TYPE_HERE && (!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (IsHack(false) && !CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
		{
			return iTimeToDead - (600 - g_nPortalLimitTime);
		}

		if (iTimeToDead > 590)
		{
			return iTimeToDead - 590;
		}
	}

	if (IsHack(false) && ((!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) || !CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex())))
	{
		return iTimeToDead - (600 - g_nPortalLimitTime);
	}

	if (iTimeToDead > 593)
	{
		return iTimeToDead - 593;
	}
	return 0;
}
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
EVENTINFO(channel_switch_info)
{
	DynamicCharacterPtr	pkCharacter;
	long				lAddr;
	WORD				wPort;
	int					iLeftSeconds;

	channel_switch_info()
		: pkCharacter(), lAddr(0), wPort(0), iLeftSeconds(0)
	{
	}
};

EVENTFUNC(channel_switch_event)
{
	channel_switch_info* info = dynamic_cast<channel_switch_info*>(event->info);
	if (!info)
	{
		sys_err("channel_switch_event :: info is a null pointer!");
		return 0;
	}

	LPCHARACTER pkCharacter = info->pkCharacter;
	if (!pkCharacter)
	{
		return 0;
	}

	if (info->iLeftSeconds > 0)
	{
		pkCharacter->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds left until channel switch."), info->iLeftSeconds);
		--info->iLeftSeconds;
	}
	else
	{
		pkCharacter->SetSwitchChannelEvent(NULL);
		pkCharacter->SwitchChannel(info->lAddr, info->wPort);
		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::SwitchChannel(long lAddr, WORD wPort)
{
	this->Stop();
	this->Save();

	if (this->GetSectree())
	{
		this->GetSectree()->RemoveEntity(this);
		this->ViewCleanup();

		this->EncodeRemovePacket(this);
	}

	TPacketGCWarp packet;
	packet.bHeader = HEADER_GC_WARP;
	packet.lX = this->GetX();
	packet.lY = this->GetY();
	packet.lAddr = lAddr;
	packet.wPort = wPort;

	this->GetDesc()->Packet(&packet, sizeof(TPacketGCWarp));
}

void CHARACTER::StartSwitchChannelEvent(long lAddr, WORD wPort)
{
	channel_switch_info* info = AllocEventInfo<channel_switch_info>();

	info->pkCharacter = this;
	info->lAddr = lAddr;
	info->wPort = wPort;
	if (this->IsPosition(POS_FIGHTING))
	{
		info->iLeftSeconds = 3;
	}
	else
	{
		info->iLeftSeconds = 3;
	}

	this->m_pkSwitchChannelEvent = event_create(channel_switch_event, info, 1);
}

bool CHARACTER::QuitSwitchChannelEvent()
{
	bool bWasEvent = false;
	if (this->m_pkSwitchChannelEvent)
	{
		event_cancel(&this->m_pkSwitchChannelEvent);
		bWasEvent = true;
	}

	this->SetSwitchChannelEvent(NULL);
	return bWasEvent;
}

void CHARACTER::SetSwitchChannelEvent(LPEVENT pkEvent)
{
	this->m_pkSwitchChannelEvent = pkEvent;
}

int CHARACTER::GetLeftTimeUntilNextChannelSwitch()
{
	return this->iNextChannelSwitchTime - time(0);
}
#endif

#ifdef __ADMIN_MANAGER__
int64_t CHARACTER::GetConvPoint(BYTE type) const
{
	switch (type)
	{
	case POINT_LEVEL:
		return GetLevel();
		break;

	case POINT_EXP:
		return GetExp();
		break;

	case POINT_NEXT_EXP:
		return GetNextExp();
		break;

	case POINT_HP:
		return GetHP();
		break;

	case POINT_MAX_HP:
		return GetMaxHP();
		break;

	case POINT_SP:
		return GetSP();
		break;

	case POINT_MAX_SP:
		return GetMaxSP();
		break;

	case POINT_GOLD:
		return GetGold();
		break;

	case POINT_STAMINA:
		return GetStamina();
		break;

	case POINT_MAX_STAMINA:
		return GetMaxStamina();
		break;
	}

	return GetPoint(type);
}

DWORD CHARACTER::GetAccountBanCount() const
{
	if (GetDesc())
	{
		return GetDesc()->GetAccountTable().accban_count;
	}

	return 0;
}
#endif

bool CHARACTER::CanTakeInventoryItem(LPITEM item, TItemPos* cell)
{
	int iEmpty = -1;
	if (item->IsDragonSoul())
	{
		cell->window_type = DRAGON_SOUL_INVENTORY;
		cell->cell = iEmpty = GetEmptyDragonSoulInventory(item);
	}
	else
	{
		cell->window_type = INVENTORY;
#ifndef __SPECIAL_STORAGE_ENABLE__
		cell->cell = iEmpty = GetEmptyInventory(item->GetSize());
#else
		cell->cell = iEmpty = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif
	}

	return iEmpty != -1;
}


#ifdef __ABUSE_CONTROLLER_ENABLE__
/*
Wallhack Detection
*/
EVENTFUNC(wallhack_check)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == nullptr)
	{
		sys_err("wallhack_check <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER Player = info->ch;

	if (!Player)
	{
		return 0;
	}

	LPDESC d = Player->GetDesc();

	if (!d)
	{
		return 0;
	}

	if (d->IsPhase(PHASE_HANDSHAKE) || d->IsPhase(PHASE_LOGIN) || d->IsPhase(PHASE_SELECT) || d->IsPhase(PHASE_DEAD) || d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}

	sys_log(2, "WALLHACK_CHECK: %s", Player->GetName());



	LPSECTREE CurrentTree = Player->GetSectree();
	if (!CurrentTree)
	{
		return PASSES_PER_SEC(1);
	}

	bool isWallHack = true;
	for (int32_t dx = -300; dx < 300; ++dx)
	{
		for (int32_t dy = -300; dy < 300; ++dy)
		{
			if (!CurrentTree->IsAttr(Player->GetX() + dx, Player->GetY() + dy, ATTR_BLOCK | ATTR_OBJECT))
			{
				isWallHack = false;
				break;
			}
		}
	}

	bool bCheck = true;

	//If we currently are in a dungeon
	if (Player->GetMapIndex() >= 10000 || Player->GetMapIndex() == 304 || Player->GetMapIndex() == 354)
	{
		bCheck = false;
	}

	if (bCheck)
	{
		if (isWallHack)
		{
			if (Player->GetLastMoveAblePosition().x == 0 || Player->GetLastMoveAblePosition().y == 0 || Player->GetLastMoveableMapIndex() == 0)
			{
				Player->GoHome();
				return 0;
			}
			std::string HackInfo = "S_WALL_HACK x" + std::to_string(Player->GetX());
			HackInfo += " y" + std::to_string(Player->GetY());
			HackInfo += " z" + std::to_string(Player->GetZ());
			HackInfo += " m" + std::to_string(Player->GetMapIndex());
			g_pLogManager->HackLog(HackInfo.c_str(), Player);
			Player->WarpSet(Player->GetLastMoveAblePosition().x, Player->GetLastMoveAblePosition().y, Player->GetLastMoveableMapIndex());
		}
		else
		{
			Player->SetLastMoveAblePosition(Player->GetXYZ());
			Player->SetLastMoveableMapIndex();
		}
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::StartCheckWallhack()
{
	if (m_pkCheckWallHackEvent)
	{
		return;
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckWallHackEvent = event_create(wallhack_check, info, PASSES_PER_SEC(1));
}
#endif

#ifdef __TRANSMUTATION_SYSTEM__
bool CHARACTER::GetTransmutationItems() const
{
	if (!transmutation_items)
	{
		return false;
	}

	if (!transmutation_items[0].item_id || !transmutation_items[1].item_id)
	{
		return false;
	}

	return true;
}

TransMutation_Struct* CHARACTER::GetTransmutationItem(BYTE nr) const
{
	if (!transmutation_items)
	{
		return nullptr;
	}

	if (nr < 2 && transmutation_items[nr].item_id)
	{
		return &transmutation_items[nr];
	}
	else
	{
		return nullptr;
	}
}

void CHARACTER::AddTransmutationItem(BYTE nr, DWORD item_id, WORD pos)
{
	if (nr > 1)
	{
		return;
	}

	if (!transmutation_items)
	{
		transmutation_items = new TransMutation_Struct[2];
		memset(transmutation_items, 0, sizeof(TransMutation_Struct) * 2);
	}

	transmutation_items[nr].item_id = item_id;
	transmutation_items[nr].pos = pos;
}

void CHARACTER::RemoveTransmutationItem(BYTE nr)
{
	if (nr > 1)
	{
		return;
	}

	if (transmutation_items)
	{
		memset(&transmutation_items[nr], 0, sizeof(transmutation_items[nr]));
	}
}

bool CHARACTER::FindTransMutationItem(DWORD item_id)
{
	if (!transmutation_items)
	{
		return false;
	}

	for (BYTE i = 0; i < 2; ++i)
	{
		if (transmutation_items[i].item_id == item_id)
		{
			return true;
		}
	}

	return false;
}

void CHARACTER::ClearTransmutation()
{
	if (transmutation_items)
	{
		delete[] transmutation_items;
		transmutation_items = nullptr;
	}
}
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
#include "find_letters_event.h"
void CHARACTER::FindLettersEventSendInfo()
{
	if (!GetDesc())
	{
		return;
	}

	std::string currentWord = CFindLetters::instance().GetWord();

	std::transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::toupper);
	std::transform(currentWord.begin(), currentWord.end(), currentWord.begin(), [](char ch) { return ch == ' ' ? '_' : ch; });

	bool bIsDifferent = false;
	for (int i = 0; i < FIND_LETTERS_SLOTS_NUM; ++i)
	{
		if (i >= currentWord.length())
		{
			continue;
		}

		if (currentWord[i] != char(m_letterSlots[i].bAsciiChar))
		{
			bIsDifferent = true;
			break;
		}
	}

	if (bIsDifferent)
	{
		for (int i = 0; i < FIND_LETTERS_SLOTS_NUM; i++)
		{
			m_letterSlots[i].bAsciiChar = CFindLetters::instance().GetLetterByPos(i);
			m_letterSlots[i].bIsFilled = 0;
		}
	}

	std::vector<TPlayerLetterReward> vecReward;
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(90601);
	if (pGroup)
	{
		TPlayerLetterReward tempReward;
		for (int i = 0; i < pGroup->GetGroupSize(); i++)
		{
			tempReward.dwVnum = pGroup->GetVnum(i);
			tempReward.bCount = pGroup->GetCount(i);

			vecReward.push_back(tempReward);
		}
	}

	TPacketGCFindLettersInfo pack;
	pack.bHeader = HEADER_GC_FIND_LETTERS_INFO;
	pack.wSize = sizeof(pack) + sizeof(TPlayerLetterReward) * vecReward.size();

	for (int i = 0; i < FIND_LETTERS_SLOTS_NUM; i++)
	{
		pack.letterSlots[i].bAsciiChar = m_letterSlots[i].bAsciiChar;
		pack.letterSlots[i].bIsFilled = m_letterSlots[i].bIsFilled;
	}

	GetDesc()->BufferedPacket(&pack, sizeof(pack));
	GetDesc()->Packet(&vecReward[0], sizeof(TPlayerLetterReward) * vecReward.size());
}

void CHARACTER::FindLettersAddLetter(int iLetterIndex, int iInventoryIndex)
{
	if (!GetDesc())
	{
		return;
	}

	if (iLetterIndex == -1 || iInventoryIndex == -1)
	{
		return;
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	if (iLetterIndex < FIND_LETTERS_SLOTS_NUM && iInventoryIndex >= 0 && iInventoryIndex < INVENTORY_AND_EQUIP_SLOT_MAX)
#else
	if (iLetterIndex < FIND_LETTERS_SLOTS_NUM && iInventoryIndex >= 0 && iInventoryIndex < INVENTORY_MAX_NUM)
#endif
	{
		if (!CanHandleItem())
		{
			return;
		}

		LPITEM pLetter = GetInventoryItem(iInventoryIndex);

		if (!pLetter)
		{
			return;
		}

#ifdef __SPECIAL_STORAGE_ENABLE__
		// We ought to skip equipped items
		if (pLetter->GetCell() >= INVENTORY_MAX_NUM && pLetter->GetCell() < SPECIAL_STORAGE_START_CELL)
		{
			return;
		}
#endif

		if (pLetter->IsExchanging() || pLetter->isLocked())
		{
			return;
		}

		if (pLetter->GetVnum() < 90500 || pLetter->GetVnum() > 90600)
		{
			return;
		}

		BYTE bLetterAscii = pLetter->GetVnum() % 100;
		if (bLetterAscii < 48 || (bLetterAscii > 57 && bLetterAscii < 65) || bLetterAscii > 90)
		{
			return;
		}

		if (m_letterSlots[iLetterIndex].bAsciiChar != bLetterAscii)
		{
			return;
		}

		if (m_letterSlots[iLetterIndex].bIsFilled)
		{
			return;
		}

		pLetter->SetCount(pLetter->GetCount() - 1);
		m_letterSlots[iLetterIndex].bIsFilled = 1;

		ChatPacket(CHAT_TYPE_COMMAND, "FindLettersRefresh %d %d %d", iLetterIndex, m_letterSlots[iLetterIndex].bAsciiChar, m_letterSlots[iLetterIndex].bIsFilled);

		bool isComplete = true;
		for (int i = 0; i < FIND_LETTERS_SLOTS_NUM; i++)
		{
			if (m_letterSlots[i].bAsciiChar == 95 || m_letterSlots[i].bAsciiChar == 0)
			{
				continue;
			}

			if (!m_letterSlots[i].bIsFilled)
			{
				isComplete = false;
			}
		}

		if (isComplete)
		{
			ChatPacket(CHAT_TYPE_COMMAND, "FindLettersFinish");

			for (int i = 0; i < FIND_LETTERS_SLOTS_NUM; i++)
			{
				m_letterSlots[i].bAsciiChar = CFindLetters::instance().GetLetterByPos(i);
				m_letterSlots[i].bIsFilled = 0;
			}

			DWORD dwBoxVnum = 90601;
			std::vector <DWORD> dwVnums;
			std::vector <DWORD> dwCounts;
			std::vector <LPITEM> item_gets(0);
			int count = 0;

			if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
			{
				for (int i = 0; i < count; i++)
				{
					if (item_gets[i])
					{
						if (dwCounts[i] > 1)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %s, %d units."), item_gets[i]->GetName(), dwCounts[i]);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box contains %s."), item_gets[i]->GetName());
						}
					}
				}
			}
		}
	}
}
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
int CHARACTER::GetSoulItemDamage(LPCHARACTER pkVictim, int iDamage, BYTE bSoulType)
{
	if (!pkVictim)
	{
		return 0;
	}

	if (!IsPC() || IsPolymorphed() || pkVictim->IsPC())
	{
		return 0;
	}

	if (bSoulType >= SOUL_MAX_NUM)
	{
		return 0;
	}

	const CAffect* pAffect = FindAffect(AFFECT_SOUL_RED + bSoulType);
	int iDamageAdd = 0;
	if (pAffect)
	{
		LPITEM soulItem = FindItemByID(pAffect->lSPCost);
		if (soulItem)
		{
			int iCurrentMinutes = (soulItem->GetSocket(2) / 10000);
			int iCurrentStrike = (soulItem->GetSocket(2) % 10000);

			int valueIndex = MINMAX(3, 2 + (iCurrentMinutes / 60), 5);
			float fDamageIncrease = float(soulItem->GetValue(valueIndex) / 10.0f);

			iDamageAdd = (fDamageIncrease * iDamage) - iDamage;

			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "CurrentDamage %d, add damage %d", iDamage, iDamageAdd);
			}

			int iNextStrikes = iCurrentStrike - 1;
			if (iNextStrikes <= 0)
			{
				DeactivateToggleItem(this, soulItem);
			}

			soulItem->SetSocket(2, (iCurrentMinutes * 10000 + iNextStrikes));
		}
	}

	return iDamageAdd;
}
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
void CHARACTER::SetIsShowTeamler(bool bIsShowTeamler)
{
	if (bIsShowTeamler == m_bIsShowTeamler)
	{
		return;
	}

	m_bIsShowTeamler = bIsShowTeamler;

	if (!IsPC())
	{
		return;
	}

	// update list
	if (m_bIsShowTeamler)
	{
		CHARACTER_MANAGER::instance().AddOnlineTeamler(GetName());
	}
	else
	{
		CHARACTER_MANAGER::instance().RemoveOnlineTeamler(GetName());
	}
	// update list p2p
	TPacketGGTeamlerStatus p2p_packet;
	p2p_packet.header = HEADER_GG_TEAMLER_STATUS;
	strlcpy(p2p_packet.szName, GetName(), sizeof(p2p_packet.szName));
	p2p_packet.is_online = m_bIsShowTeamler;
	P2P_MANAGER::instance().Send(&p2p_packet, sizeof(p2p_packet));

	ShowTeamlerPacket();
	GlobalShowTeamlerPacket(m_bIsShowTeamler);
}

void CHARACTER::ShowTeamlerPacket()
{
	TPacketGCShowTeamler packet;
	packet.header = HEADER_GC_SHOW_TEAMLER;
	packet.is_show = m_bIsShowTeamler;
	if (GetDesc())
	{
		GetDesc()->Packet(&packet, sizeof(packet));
	}
}

void CHARACTER::GlobalShowTeamlerPacket(bool bIsOnline)
{
	TPacketGCTeamlerStatus packet;
	packet.header = HEADER_GC_TEAMLER_STATUS;
	strlcpy(packet.szName, GetName(), sizeof(packet.szName));
	packet.is_online = bIsOnline;

	// send p2p
	{
		TPacketGGPlayerPacket p2p_pack;
		p2p_pack.header = HEADER_GG_PLAYER_PACKET;
		p2p_pack.size = sizeof(packet);
		p2p_pack.language = -1;

		TEMP_BUFFER buf;
		buf.write(&p2p_pack, sizeof(p2p_pack));
		buf.write(&packet, sizeof(packet));

		P2P_MANAGER::instance().Send(buf.read_peek(), buf.size());
	}

	// send to player on this core
	{
		const CHARACTER_MANAGER::NAME_MAP& rkPCMap = CHARACTER_MANAGER::Instance().GetPCMap();
		for (auto it = rkPCMap.begin(); it != rkPCMap.end(); ++it)
		{
			if (it->second->GetDesc())
			{
				if (it->second->GetDesc()->IsPhase(PHASE_GAME) || it->second->GetDesc()->IsPhase(PHASE_DEAD))
				{
					it->second->GetDesc()->Packet(&packet, sizeof(packet));
				}
			}
		}
	}
}
#endif

#ifdef __INVENTORY_BUFFERING__
void CHARACTER::SendBufferedInventoryPacket()
{
	if (!GetDesc())
	{
		// Well, no desc
		// Just disable it
		us_buffered_items.clear();
		bInvBuff = false;
	}

	// Intializing buffer
	TEMP_BUFFER buf{ 1024 * 1024, false };
	TPacketGCInventoryHeader packHdr{};
	packHdr.bHeader = HEADER_GC_ITEM_BUFFERED;
	packHdr.wSize = sizeof(packHdr) + sizeof(TPacketGCItemSet) * us_buffered_items.size();

	for (const auto& pItem : us_buffered_items)
	{
		TPacketGCItemSet pack{};
		pack.header = HEADER_GC_ITEM_SET;
		pack.Cell = TItemPos(pItem->GetWindow(), pItem->GetCell());
		pack.count = pItem->GetCount();
		pack.vnum = pItem->GetVnum();
		pack.flags = pItem->GetFlag();
		pack.anti_flags = pItem->GetAntiFlag();
		pack.highlight = (pack.Cell.window_type == DRAGON_SOUL_INVENTORY);
#ifdef __TRANSMUTATION_SYSTEM__
		pack.transmutate_id = pItem->GetTransmutate();
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		pack.dwRefineElement = pItem->GetRefineElement();
#endif

		std::memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
		std::memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

		buf.write(&pack, sizeof(pack));
	}

	for (const auto& pPair : us_buffered_items_logs)
#ifdef __ENABLE_NEW_LOGS_CHAT__
		CChatLogs::SendChatLogInformation(this, ELogsType::LOG_TYPE_CHEST, static_cast<int>(pPair.second.second), pPair.first);
#else
		ChatPacket(CHAT_TYPE_INFO, "You received %s in count of %d", pPair.second.first.c_str(), pPair.second.second);
#endif

	us_buffered_items_logs.clear();

	if (us_buffered_items.empty())
	{
		bInvBuff = false;
		return;
	}

	// Sending data
	GetDesc()->BufferedPacket(&packHdr, sizeof(packHdr));
	GetDesc()->LargePacket(buf.read_peek(), buf.size());

	// Data sent, switching off buffer
	us_buffered_items.clear();
	bInvBuff = false;
}

void CHARACTER::QuickOpenStack(LPITEM item, CountType iCount)
{
	if (item->GetType() != ITEM_GIFTBOX || item->GetType() == ITEM_USE && item->GetSubType() != USE_SPECIAL)
	{
		return;
	}

	// Setup buffering
	SetInventoryBuffer(true);

	// Keep on opening
	DWORD dwVID = item->GetVID();
	CountType wInitialCount = item->GetCount();

	CountType oCount = 0;

	while (item->GetCount())
	{
		bool bUsed = false;
		if (wInitialCount < iCount) //The number of items is less than required
		{
			break;
		}

		// Use, if function returns false, break
		bUsed = UseItemEx(item, NPOS);
		if (!bUsed)
		{
			break;
		}

		if (!ITEM_MANAGER::instance().FindByVID(dwVID)) // Item is missing, break
		{
			break;
		}

		if (item->GetCount() == wInitialCount) // Item is not consumed (cannot be opened), break
		{
			break;
		}

		if (iCount > 0)
		{
			oCount++;

			if (iCount == oCount) //When you open the required amount of items, break
			{
				break;
			}
		}

#ifdef __BATTLE_PASS_ENABLE__
		if (bUsed)
			CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_USE, item->GetVnum(), 1 });
#endif
	}

	// Release buffer
	SendBufferedInventoryPacket();
}

void CHARACTER::AddInventoryBufferLog(const std::string & sName, DWORD dwVnum, CountType wCount)
{
	if (us_buffered_items_logs.find(dwVnum) != us_buffered_items_logs.end())
		us_buffered_items_logs[dwVnum].second += wCount;
	else
		us_buffered_items_logs[dwVnum] = std::move(std::pair<std::string, int>(sName, wCount));
}
#endif

void CHARACTER::UpdateNextPickupTime()
{
	tt_next_pickup_time = get_global_time() + 1;
}

#ifdef __ENABLE_PARTY_ATLAS__
void CHARACTER::SendPartyPositionInfo(LPCHARACTER pkTarget, bool bClear)
{
	if (!IsPC())
	{
		return;
	}

	if (!GetParty())
	{
		return;
	}

	if (pkTarget == this)
	{
		return;
	}

	if (!pkTarget)
	{
		if (m_dwLastPartyPositionSent && get_dword_time() - m_dwLastPartyPositionSent < 2000)
		{
			return;
		}

		m_dwLastPartyPositionSent = get_dword_time();
	}

	TPacketGCPartyPosition pack;
	pack.bHeader = HEADER_GC_PARTY_POSITION;
	pack.dwPID = GetPlayerID();
	pack.dwX = GetX();
	pack.dwY = GetY();
	if (bClear)
	{
		pack.dwX = 0;
		pack.dwY = 0;
	}
	else
	{
		PIXEL_POSITION pxBasePos;
		if (SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(GetMapIndex(), pxBasePos))
		{
			pack.dwX -= pxBasePos.x;
			pack.dwY -= pxBasePos.y;
		}
	}

	if (pkTarget)
	{
		pkTarget->GetDesc()->Packet(&pack, sizeof(pack));
	}
	else
	{
		const CParty::TMemberMap& rkMap = GetParty()->GetMemberMap();
		for (auto elem : rkMap)
		{
			if (elem.second.pCharacter && elem.second.pCharacter->GetMapIndex() == GetMapIndex() && elem.second.pCharacter != this)
			{
				elem.second.pCharacter->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
	}
}

void CHARACTER::ClearPartyPositionInfo()
{
	SendPartyPositionInfo(NULL, true);
}
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
void CHARACTER::SetKeepingOfflineShop(uint32_t keepingOfflineShop)
{
	keepingOfflineShop_ = keepingOfflineShop;
}

uint32_t CHARACTER::GetKeepingOfflineShop() const
{
	return keepingOfflineShop_;
}

void CHARACTER::AddViewingOfflineShop(uint32_t id)
{
	viewingOfflineShops_.insert(id);
}

void CHARACTER::RemoveViewingOfflineShop(uint32_t id)
{
	viewingOfflineShops_.erase(id);
}

void CHARACTER::RemoveFromViewingOfflineShops()
{
	if (viewingOfflineShops_.size() < 1)
	{
		return;
	}

	auto shops = viewingOfflineShops_;
	for (const auto& id : shops)
	{
		auto shop = COfflineShop::Get(id);
		if (!shop)
		{
			continue;
		}

		shop->get()->RemoveViewer(this);
	}
}

void CHARACTER::SetOpeningOfflineShopState(bool isOpeningOfflineShop)
{
	isOpeningOfflineShop_ = isOpeningOfflineShop;
}

bool CHARACTER::IsOpeningOfflineShop() const
{
	return isOpeningOfflineShop_;
}

void CHARACTER::SetOfflineShopOpeningItem(CItem* item)
{
	offlineShopOpeningItem_ = item;
}

CItem* CHARACTER::GetOfflineShopOpeningItem() const
{
	return offlineShopOpeningItem_;
}
#endif // __ENABLE_OFFLINE_SHOP__

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
void CHARACTER::SetStateHorse()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
}

CHARACTER *CHARACTER::CreateToggleMount(CItem* item)
{
	m_toggleMount.reset();

	auto x = GetX();
	auto y = GetY();

	if (!GetDungeon())
	{
		x += number(-50, 50);
		y += number(-50, 50);
	}

	DWORD dwMountVnum = item->GetValue(ITEM_VALUE_MOUNT_VNUM);
	GetSpecialMount(dwMountVnum);

	CHARACTER* pkCharacter = CHARACTER_MANAGER::instance().SpawnMob(
								  dwMountVnum, GetMapIndex(), x, y, 0, false, -1, false);

	if (!pkCharacter)
	{
		sys_err("Failed to create mount character.");
		return nullptr;
	}

	std::string sName = GetName();

	sName += LC_TEXT("'s Mount");

	pkCharacter->SetName(sName);

	pkCharacter->SetEmpire(GetEmpire());
	pkCharacter->SetRider(this);
	pkCharacter->SetStateHorse();
	pkCharacter->MarkAsMount();

	pkCharacter->Show(GetMapIndex(), x, y);

	m_toggleMount.reset(pkCharacter);

	return m_toggleMount.get();
}

void CHARACTER::SetMountStartToggleItem(LPITEM item)
{
	m_dwMountToggleItemStartID = item->GetID();
}

void CHARACTER::CheckMountStartToggleItem()
{
	if (!m_dwMountToggleItemStartID)
	{
		return;
	}

	LPITEM item = FindItemByID(m_dwMountToggleItemStartID);
	if (!item)
	{
		return;
	}

	ActivateToggleItem(this, item);
}

void CHARACTER::GetSpecialMount(DWORD& dwVnum, bool bTakeOut)
{
	auto pItem = FindToggleItem(this, true, TOGGLE_MOUNT);

	// In case if item has not been found, and do not alter value
	if (!pItem) return;

	// Check if mask is applied; skip it if argument is provided
	auto pWearItem = (!bTakeOut) ? GetWear(WEAR_COSTUME_MOUNT) : nullptr;

	// alter value corresponding to mask status
	dwVnum = (pWearItem) ? pWearItem->GetValue(ITEM_VALUE_MOUNT_VNUM) : pItem->GetValue(ITEM_VALUE_MOUNT_VNUM);
}

void CHARACTER::RefreshMount(bool bTakeOut)
{
	auto pItem = FindToggleItem(this, true, TOGGLE_MOUNT);
	if (!pItem) return;

	DWORD dwVnum = pItem->GetValue(ITEM_VALUE_MOUNT_VNUM);
	GetSpecialMount(dwVnum, bTakeOut);

	//If pc's mounted, remount it!
	if (GetMountVnum())
	{
		MountVnum(dwVnum);
		return;
	}

	//If horse is only summoned, resummon new
	MountVnum(0);
	CreateToggleMount(pItem);
}

void CHARACTER::ComputeMountPoints()
{
	auto pItem = FindToggleItem(this, true, TOGGLE_MOUNT);
	if (!pItem) return;

	LPITEM pOverlayItem = GetWear(WEAR_COSTUME_MOUNT);

	// In case wrong item is placed
	if (pOverlayItem && (pOverlayItem->GetType() != ITEM_COSTUME && pOverlayItem->GetSubType() != COSTUME_MOUNT))
		return;

	// Applies
	for (const auto& pItem : { pItem, pOverlayItem })
	{
		if (!pItem)
		{
			continue;
		}

		auto pItemTable = pItem->GetProto();
		for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			if (pItemTable->aApplies[i].bType != APPLY_NONE)
			{
				ApplyPoint(pItemTable->aApplies[i].bType, pItemTable->aApplies[i].lValue);
			}
		}

		// Attrs
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pItem->GetAttributeType(i) != POINT_NONE)
			{
				ApplyPoint(pItem->GetAttributeType(i), pItem->GetAttributeValue(i));
			}
		}
	}
}
#endif

void CHARACTER::SetSkipUpdatePacket(bool skip)
{
	m_skipUpdatePacket = skip;
}

bool CHARACTER::IsSkipUpdatePacket()
{
	return m_skipUpdatePacket;
}

std::string CHARACTER::GetPlayerLink()
{
	std::string playerLink = "player:";

	playerLink += this->GetName();
	playerLink += ":";

	char buf[10];
	snprintf(buf, sizeof(buf), "%d", this->GetEmpire());
	playerLink += buf;

	return playerLink;
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
void CHARACTER::SetLocale(int locale)
{
	if (locale < 0 || locale >= LC_COUNT)
	{
		locale = LC_DEFAULT;
	}

	this->m_locale = locale;
}

int CHARACTER::GetLocale()
{
	return this->m_locale;
}
#endif

#ifdef __ENABLE_EQUIPMENT_SLOT_LOCK_SYSTEM__
int CHARACTER::UnlockEquipmentSlot(DWORD dwVnum)
{
	static const std::string sFlag("equipment_lock.{}");
	if (!dwVnum)
		return -1;

	auto iFind = m_EquipmentLockInfo.find(dwVnum);
	if (iFind == m_EquipmentLockInfo.end())
		return 0;

	// Nothing to update!
	if (GetQuestFlag(fmt::format(sFlag, iFind->second.second)))
		return -2;

	// Lets check if we have unlock item!
	if (CountSpecifyItem(dwVnum) < 1)
		return -3;

	RemoveSpecifyItem(dwVnum, 1);
	SetQuestFlag(fmt::format(sFlag, iFind->second.second), 1);

	// Lets call to the client, and update the slots!
	BroadcastEquipmentLockedSlot();
	return 1;
}

bool CHARACTER::EquipmentSlotStatus(BYTE bSlot)
{
	static const std::string sFlag("equipment_lock.{}");

	auto iFit = std::find_if(m_EquipmentLockInfo.begin(), m_EquipmentLockInfo.end(), [&bSlot](const std::pair<BYTE, std::pair<BYTE, std::string>>& rPair) { return rPair.second.first == bSlot; });
	if (iFit != m_EquipmentLockInfo.end())
	{
		return GetQuestFlag(fmt::format(sFlag, iFit->second.second));
	}

	return true;
}

void CHARACTER::BroadcastEquipmentLockedSlot()
{
	static const std::string sFlag("equipment_lock.{}");
	static const std::string sBroadcastClear("EquipmentLock_Clear");
	static const std::string sBroadcastInfo("EquipmentLock_Info {} {} {}");

	// We need to clear before sending!
	ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClear);

	for (const auto& [rKey, rValues] : m_EquipmentLockInfo)
	{
		ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastInfo, rKey, rValues.first, GetQuestFlag(fmt::format(sFlag, rValues.second))));
	}
}
#endif

void CHARACTER::BroadcastHCostume()
{
	for (const auto& [sKey, rValue] : m_HideCostume)
	{
		ChatPacket(CHAT_TYPE_COMMAND, fmt::format("BroadcastHideCostume_Info {} {}", sKey, static_cast<int>(HasCostumeFlag(rValue))));
	}
}


#ifdef __ENABLE_SHAMAN_SYSTEM__
void CHARACTER::SendAutoShamanSkill(DWORD dwSkillVnum, BYTE byLevel)
{
	TPacketGCShamanUseSkill pack{ HEADER_GC_AUTO_SHAMAN_SKILL, dwSkillVnum, GetVID(), byLevel };
	PacketView(&pack, sizeof(TPacketGCShamanUseSkill), this);
}

void CHARACTER::SendAutoShamanInformations()
{
	if (!GetShamanSystem())
		return;

	GetShamanSystem()->SendInformations();
}
#endif

