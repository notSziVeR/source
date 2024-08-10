#include "StdAfx.h"
#include "PythonNonPlayer.h"

#include "InstanceBase.h"
#include "PythonCharacterManager.h"

#include "PythonNetworkStream.h"

PyObject * nonplayerGetEventType(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualNumber))
	{
		return Py_BuildException();
	}

	BYTE iType = CPythonNonPlayer::Instance().GetEventType(iVirtualNumber);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetEventTypeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
	{
		return Py_BuildException();
	}

	BYTE iType = CPythonNonPlayer::Instance().GetEventTypeByVID(iVirtualID);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetLevelByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
	{
		return Py_BuildException();
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
	{
		return Py_BuildValue("i", -1);
	}

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
	{
		return Py_BuildValue("i", -1);
	}

	float fAverageLevel = pMobTable->bLevel;//(float(pMobTable->abLevelRange[0]) + float(pMobTable->abLevelRange[1])) / 2.0f;
	fAverageLevel = floor(fAverageLevel + 0.5f);
	return Py_BuildValue("i", int(fAverageLevel));
}

PyObject * nonplayerGetGradeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
	{
		return Py_BuildException();
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
	{
		return Py_BuildValue("i", -1);
	}

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
	{
		return Py_BuildValue("i", -1);
	}

	return Py_BuildValue("i", pMobTable->bRank);
}


PyObject * nonplayerGetMonsterName(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	return Py_BuildValue("s", rkNonPlayer.GetMonsterName(iVNum));
}

PyObject * nonplayerLoadNonPlayerData(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer::Instance().LoadNonPlayerData(szFileName);
	return Py_BuildNone();
}

PyObject* nonplayerGetVnumByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
	{
		return Py_BuildException();
	}

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
	{
		return Py_BuildValue("i", -1);
	}

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
	{
		return Py_BuildValue("i", -1);
	}

	return Py_BuildValue("i", pMobTable->dwVnum);
}

#ifdef ENABLE_TARGET_MONSTER_LOOT
PyObject* nonplayerGetRaceFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
	{
		return Py_BuildException();
	}

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
	{
		return Py_BuildValue("i", -1);
	}

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	return Py_BuildValue("i", rkNonPlayer.GetRaceFlag(pInstance->GetVirtualNumber()));
}
#endif

PyObject* nonplayerIsMonsterRaceFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BadArgument();
	}
	int iRaceFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iRaceFlag))
	{
		return Py_BadArgument();
	}

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable((DWORD)iVNum);

	if (!pMobTable)
	{
		return Py_BuildValue("b", false);
	}

	return Py_BuildValue("b", IS_SET(pMobTable->dwRaceFlag, iRaceFlag));
}

PyObject* nonplayerGetMonsterResist(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
	{
		return Py_BuildException();
	}

	BYTE bResistNum;
	if (!PyTuple_GetInteger(poArgs, 1, &bResistNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterResist(iVnum, bResistNum));
}

#ifdef ENABLE_12ZI_ELEMENT_ADD
PyObject* nonplayerMonsterHasRaceFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	char* szRaceFlag;
	if (!PyTuple_GetString(poArgs, 1, &szRaceFlag))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	return Py_BuildValue("b", rkNonPlayer.MonsterHasRaceFlag(iVNum, szRaceFlag));
}
#endif

#ifdef ZUO_PANEL_ENABLE
PyObject* nonplayerGetMonstersByName(PyObject* poSelf, PyObject* poArgs)
{
	char* name;
	if (!PyTuple_GetString(poArgs, 0, &name))
		return Py_BadArgument();

	char* func_name;
	if (!PyTuple_GetString(poArgs, 1, &func_name))
		return Py_BadArgument();

	std::unordered_set<DWORD> s_banned;
	int ban_count = PyTuple_Size(poArgs);

	if (ban_count)
	{
		for (int i = 2; i < ban_count; ++i)
		{
			int race_num = 0;
			if (PyTuple_GetInteger(poArgs, i, &race_num))
				s_banned.insert(race_num);
		}
	}

	std::map<DWORD, std::string> mobs;
	if (std::string(func_name) == "RetZuoMonsters")
		mobs = CPythonNonPlayer::Instance().GetMonsterByName(name, true, true);
	else
		mobs = CPythonNonPlayer::Instance().GetMonsterByName(name, false, false, s_banned);

	for (std::map<DWORD, std::string>::iterator it = mobs.begin(); it != mobs.end(); it++)
		PyCallClassMemberFunc(CPythonNetworkStream::instance().m_apoPhaseWnd[CPythonNetworkStream::instance().PHASE_WINDOW_GAME], func_name, Py_BuildValue("(si)", (it->second).c_str(), it->first));

	return Py_BuildNone();
}
#endif

#ifdef INGAME_WIKI
PyObject* nonplayerGetMonsterDataByNamePart(PyObject* poSelf, PyObject* poArgs)
{
	char* szNamePart;
	if (!PyTuple_GetString(poArgs, 0, &szNamePart))
	{
		return Py_BadArgument();
	}

	std::tuple<const char*, int> mobData = CPythonNonPlayer::Instance().GetMonsterDataByNamePart(szNamePart);
	return Py_BuildValue("(si)", std::get<0>(mobData), std::get<1>(mobData));
}

PyObject* nonplayerBuildWikiSearchList(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNonPlayer::Instance().BuildWikiSearchList();
	return Py_BuildNone();
}

/*
	Some systems need these functions, and you can probably already have them, if
		you already have ignore the lines below
*/

PyObject* nonplayerGetMonsterImmuneFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->dwImmuneFlag);
}

PyObject* nonplayerGetMonsterLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->bLevel);
}

PyObject* nonplayerGetMonsterRaceFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->dwRaceFlag);
}

PyObject* nonplayerIsMonsterStone(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->bType == CPythonNonPlayer::STONE);
}

PyObject* nonplayerGetMonsterDamage(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("ii", pkTab->dwDamageRange[0], pkTab->dwDamageRange[1]);
}

PyObject* nonplayerGetMonsterMaxHP(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->dwMaxHP);
}

PyObject* nonplayerGetMonsterExp(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->dwExp);
}

PyObject* nonplayerGetMonsterResistValue(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		return Py_BuildException();
	}

	if (iType < 0 || iType > CPythonNonPlayer::MOB_RESISTS_MAX_NUM)
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("i", pkTab->cResists[iType]);
}

PyObject* nonplayerGetMonsterGold(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	const CPythonNonPlayer::TMobTable* pkTab = rkNonPlayer.GetTable(iVNum);

	return Py_BuildValue("ii", pkTab->dwGoldMin, pkTab->dwGoldMax);
}
/*
	-----------------------------------------------------------------------------------------
*/
#endif

PyObject* nonplayerMonsterExists(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BadArgument();
	}

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable((DWORD)iVNum);

	if (!pMobTable)
	{
		return Py_BuildValue("b", false);
	}

	return Py_BuildValue("b", true);
}

#ifdef ENABLE_TARGET_MONSTER_LOOT
PyObject* nonplayerGetMonsterRegen(PyObject* poSelf, PyObject* poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
	{
		return Py_BadArgument();
	}

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable((DWORD)iVNum);

	if (!pMobTable)
	{
		return Py_BuildValue("ii", -1, -1);
	}

	return Py_BuildValue("ii", pMobTable->bRegenCycle, pMobTable->bRegenPercent);
}
#endif

PyObject* nonplayerGetEnchant(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
	{
		return Py_BuildException();
	}

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
	{
		return Py_BuildValue("i", -1);
	}

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());
	if (!pMobTable)
	{
		return Py_BuildValue("i", -1);
	}

	BYTE enchant;
	if (!PyTuple_GetByte(poArgs, 1, &enchant))
	{
		return Py_BuildException();
	}

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
	return Py_BuildValue("i", rkNonPlayer.GetEnchant(pInstance->GetVirtualNumber(), enchant));
}

void initNonPlayer()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetEventType",				nonplayerGetEventType,				METH_VARARGS },
		{ "GetEventTypeByVID",			nonplayerGetEventTypeByVID,			METH_VARARGS },
		{ "GetLevelByVID",				nonplayerGetLevelByVID,				METH_VARARGS },
		{ "GetGradeByVID",				nonplayerGetGradeByVID,				METH_VARARGS },
		{ "GetMonsterName",				nonplayerGetMonsterName,			METH_VARARGS },

		{ "MonsterExists",				nonplayerMonsterExists,				METH_VARARGS },

		{ "LoadNonPlayerData",			nonplayerLoadNonPlayerData,			METH_VARARGS },

		{ "GetVnumByVID",				nonplayerGetVnumByVID,				METH_VARARGS },

#ifdef ENABLE_TARGET_MONSTER_LOOT
		{ "GetRaceFlag",				nonplayerGetRaceFlag,				METH_VARARGS },
#endif

		{ "GetMonsterRaceFlag",			nonplayerGetMonsterRaceFlag,		METH_VARARGS },
		{ "IsMonsterRaceFlag",			nonplayerIsMonsterRaceFlag,			METH_VARARGS },
		{ "GetMonsterResist",			nonplayerGetMonsterResist,			METH_VARARGS },

#ifdef ENABLE_12ZI_ELEMENT_ADD
		{ "MonsterHasRaceFlag",			nonplayerMonsterHasRaceFlag,		METH_VARARGS },
#endif

#ifdef ZUO_PANEL_ENABLE
		{ "GetMonstersByName",			nonplayerGetMonstersByName,			METH_VARARGS },
#endif

#ifdef INGAME_WIKI
		{ "GetMonsterDataByNamePart",	nonplayerGetMonsterDataByNamePart,	METH_VARARGS },
		{ "BuildWikiSearchList",		nonplayerBuildWikiSearchList,		METH_VARARGS },

		/*
			Some systems need these functions, and you can probably already have them, if
				you already have ignore the lines below
		*/
		{ "GetMonsterLevel",			nonplayerGetMonsterLevel,			METH_VARARGS },
		{ "GetMonsterRaceFlag",			nonplayerGetMonsterRaceFlag,		METH_VARARGS },
		{ "IsMonsterStone",				nonplayerIsMonsterStone,			METH_VARARGS },
		{ "GetMonsterDamage",			nonplayerGetMonsterDamage,			METH_VARARGS },
		{ "GetMonsterMaxHP",			nonplayerGetMonsterMaxHP,			METH_VARARGS },
		{ "GetMonsterExp",				nonplayerGetMonsterExp,				METH_VARARGS },

		{ "GetMonsterImmuneFlag",		nonplayerGetMonsterImmuneFlag,		METH_VARARGS },
		{ "GetMonsterResistValue",		nonplayerGetMonsterResistValue,		METH_VARARGS },
		{ "GetMonsterGold",				nonplayerGetMonsterGold,			METH_VARARGS },
		/*
			-----------------------------------------------------------------------------------------
		*/
#endif

#ifdef ENABLE_TARGET_MONSTER_LOOT
		{ "GetMonsterRegen",			nonplayerGetMonsterRegen,			METH_VARARGS },
#endif

		{ "GetEnchant",					nonplayerGetEnchant,				METH_VARARGS },

		{ NULL,							NULL,								NULL		 },
	};

	PyObject * poModule = Py_InitModule("nonplayer", s_methods);

	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_NONE",		CPythonNonPlayer::ON_CLICK_EVENT_NONE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_BATTLE",		CPythonNonPlayer::ON_CLICK_EVENT_BATTLE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_SHOP",		CPythonNonPlayer::ON_CLICK_EVENT_SHOP);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_TALK",		CPythonNonPlayer::ON_CLICK_EVENT_TALK);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_VEHICLE",		CPythonNonPlayer::ON_CLICK_EVENT_VEHICLE);

	PyModule_AddIntConstant(poModule, "PAWN", 0);
	PyModule_AddIntConstant(poModule, "S_PAWN", 1);
	PyModule_AddIntConstant(poModule, "KNIGHT", 2);
	PyModule_AddIntConstant(poModule, "S_KNIGHT", 3);
	PyModule_AddIntConstant(poModule, "BOSS", 4);
	PyModule_AddIntConstant(poModule, "KING", 5);

	PyModule_AddIntConstant(poModule, "MOB_RESIST_SWORD", CPythonNonPlayer::MOB_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_TWOHAND", CPythonNonPlayer::MOB_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_DAGGER", CPythonNonPlayer::MOB_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BELL", CPythonNonPlayer::MOB_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FAN", CPythonNonPlayer::MOB_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BOW", CPythonNonPlayer::MOB_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FIRE", CPythonNonPlayer::MOB_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_ELECT", CPythonNonPlayer::MOB_RESIST_ELECT);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_MAGIC", CPythonNonPlayer::MOB_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_WIND", CPythonNonPlayer::MOB_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_POISON", CPythonNonPlayer::MOB_RESIST_POISON);
	PyModule_AddIntConstant(poModule, "MOB_RESISTS_MAX_NUM", CPythonNonPlayer::MOB_RESISTS_MAX_NUM);

#ifdef INGAME_WIKI
	PyModule_AddIntConstant(poModule, "IMMUNE_STUN", CItemData::IMMUNE_STUN);
	PyModule_AddIntConstant(poModule, "IMMUNE_SLOW", CItemData::IMMUNE_SLOW);
	PyModule_AddIntConstant(poModule, "IMMUNE_CURSE", CItemData::IMMUNE_CURSE);
	PyModule_AddIntConstant(poModule, "IMMUNE_POISON", CItemData::IMMUNE_POISON);
	PyModule_AddIntConstant(poModule, "IMMUNE_TERROR", CItemData::IMMUNE_TERROR);
	PyModule_AddIntConstant(poModule, "IMMUNE_FLAG_MAX_NUM", CItemData::IMMUNE_FLAG_MAX_NUM);

	PyModule_AddIntConstant(poModule, "MOB_RESIST_SWORD", CPythonNonPlayer::MOB_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_TWOHAND", CPythonNonPlayer::MOB_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_DAGGER", CPythonNonPlayer::MOB_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BELL", CPythonNonPlayer::MOB_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FAN", CPythonNonPlayer::MOB_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BOW", CPythonNonPlayer::MOB_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FIRE", CPythonNonPlayer::MOB_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_ELECT", CPythonNonPlayer::MOB_RESIST_ELECT);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_MAGIC", CPythonNonPlayer::MOB_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_WIND", CPythonNonPlayer::MOB_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_POISON", CPythonNonPlayer::MOB_RESIST_POISON);
	PyModule_AddIntConstant(poModule, "MOB_RESISTS_MAX_NUM", CPythonNonPlayer::MOB_RESISTS_MAX_NUM);

	PyModule_AddIntConstant(poModule, "RACE_FLAG_ANIMAL", CPythonNonPlayer::RACE_FLAG_ANIMAL);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_UNDEAD", CPythonNonPlayer::RACE_FLAG_UNDEAD);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DEVIL", CPythonNonPlayer::RACE_FLAG_DEVIL);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_HUMAN", CPythonNonPlayer::RACE_FLAG_HUMAN);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ORC", CPythonNonPlayer::RACE_FLAG_ORC);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_MILGYO", CPythonNonPlayer::RACE_FLAG_MILGYO);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_INSECT", CPythonNonPlayer::RACE_FLAG_INSECT);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_FIRE", CPythonNonPlayer::RACE_FLAG_FIRE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ICE", CPythonNonPlayer::RACE_FLAG_ICE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_DESERT", CPythonNonPlayer::RACE_FLAG_DESERT);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_TREE", CPythonNonPlayer::RACE_FLAG_TREE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_ELEC", CPythonNonPlayer::RACE_FLAG_ATT_ELEC);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_FIRE", CPythonNonPlayer::RACE_FLAG_ATT_FIRE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_ICE", CPythonNonPlayer::RACE_FLAG_ATT_ICE);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_WIND", CPythonNonPlayer::RACE_FLAG_ATT_WIND);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_EARTH", CPythonNonPlayer::RACE_FLAG_ATT_EARTH);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_ATT_DARK", CPythonNonPlayer::RACE_FLAG_ATT_DARK);
	PyModule_AddIntConstant(poModule, "RACE_FLAG_MAX_NUM", CPythonNonPlayer::RACE_FLAG_MAX_NUM);
#endif

	PyModule_AddIntConstant(poModule, "GET_MAX_ENCHANT", CPythonNonPlayer::MOB_ENCHANTS_MAX_NUM);
}