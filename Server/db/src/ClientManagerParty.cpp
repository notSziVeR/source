// vim:ts=4 sw=4
#include "stdafx.h"
#include "ClientManager.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"

void CClientManager::QUERY_PARTY_CREATE(CPeer* peer, TPacketPartyCreate* p)
{
#ifdef __ENABLE_PARTY_UPDATE__
	TPartyMap& pm = m_map_pkChannelParty[1];
#else
	TPartyMap& pm = m_map_pkChannelParty[peer->GetChannel()];
#endif

	if (pm.find(p->dwLeaderPID) == pm.end())
	{
		pm.insert(make_pair(p->dwLeaderPID, TPartyMember()));
#ifdef __ENABLE_PARTY_UPDATE__
		ForwardPacket(HEADER_DG_PARTY_CREATE, p, sizeof(TPacketPartyCreate));
#else
		ForwardPacket(HEADER_DG_PARTY_CREATE, p, sizeof(TPacketPartyCreate), peer->GetChannel(), peer);
#endif
		sys_log(0, "PARTY Create [%lu]", p->dwLeaderPID);
	}
	else
	{
		sys_err("PARTY Create - Already exists [%lu]", p->dwLeaderPID);
	}
}

void CClientManager::QUERY_PARTY_DELETE(CPeer* peer, TPacketPartyDelete* p)
{
#ifdef __ENABLE_PARTY_UPDATE__
	TPartyMap& pm = m_map_pkChannelParty[1];
#else
	TPartyMap& pm = m_map_pkChannelParty[peer->GetChannel()];
#endif
	itertype(pm) it = pm.find(p->dwLeaderPID);

	if (it == pm.end())
	{
		sys_err("PARTY Delete - Non exists [%lu]", p->dwLeaderPID);
		return;
	}

	pm.erase(it);
#ifdef __ENABLE_PARTY_UPDATE__
	ForwardPacket(HEADER_DG_PARTY_DELETE, p, sizeof(TPacketPartyDelete));
#else
	ForwardPacket(HEADER_DG_PARTY_DELETE, p, sizeof(TPacketPartyDelete), peer->GetChannel(), peer);
#endif
	sys_log(0, "PARTY Delete [%lu]", p->dwLeaderPID);
}

void CClientManager::QUERY_PARTY_ADD(CPeer* peer, TPacketPartyAdd* p)
{
#ifdef __ENABLE_PARTY_UPDATE__
	TPartyMap& pm = m_map_pkChannelParty[1];
#else
	TPartyMap& pm = m_map_pkChannelParty[peer->GetChannel()];
#endif
	itertype(pm) it = pm.find(p->dwLeaderPID);

	if (it == pm.end())
	{
		sys_err("PARTY Add - Non exists [%lu]", p->dwLeaderPID);
		return;
	}

	if (it->second.find(p->dwPID) == it->second.end())
	{
		it->second.insert(std::make_pair(p->dwPID, TPartyInfo()));
#ifdef __ENABLE_PARTY_UPDATE__
		ForwardPacket(HEADER_DG_PARTY_ADD, p, sizeof(TPacketPartyAdd));
#else
		ForwardPacket(HEADER_DG_PARTY_ADD, p, sizeof(TPacketPartyAdd), peer->GetChannel(), peer);
#endif
		sys_log(0, "PARTY Add [%lu] to [%lu]", p->dwPID, p->dwLeaderPID);
	}
	else
	{
		sys_err("PARTY Add - Already [%lu] in party [%lu]", p->dwPID, p->dwLeaderPID);
	}
}

void CClientManager::QUERY_PARTY_REMOVE(CPeer* peer, TPacketPartyRemove* p)
{
#ifdef __ENABLE_PARTY_UPDATE__
	TPartyMap& pm = m_map_pkChannelParty[1];
#else
	TPartyMap& pm = m_map_pkChannelParty[peer->GetChannel()];
#endif
	itertype(pm) it = pm.find(p->dwLeaderPID);

	if (it == pm.end())
	{
		sys_err("PARTY Remove - Non exists [%lu] cannot remove [%lu]", p->dwLeaderPID, p->dwPID);
		return;
	}

	itertype(it->second) pit = it->second.find(p->dwPID);

	if (pit != it->second.end())
	{
		it->second.erase(pit);
#ifdef __ENABLE_PARTY_UPDATE__
		ForwardPacket(HEADER_DG_PARTY_REMOVE, p, sizeof(TPacketPartyRemove));
#else
		ForwardPacket(HEADER_DG_PARTY_REMOVE, p, sizeof(TPacketPartyRemove), peer->GetChannel(), peer);
#endif
		sys_log(0, "PARTY Remove [%lu] to [%lu]", p->dwPID, p->dwLeaderPID);
	}
	else
	{
		sys_err("PARTY Remove - Cannot find [%lu] in party [%lu]", p->dwPID, p->dwLeaderPID);
	}
}

void CClientManager::QUERY_PARTY_STATE_CHANGE(CPeer* peer, TPacketPartyStateChange* p)
{
#ifdef __ENABLE_PARTY_UPDATE__
	TPartyMap& pm = m_map_pkChannelParty[1];
#else
	TPartyMap& pm = m_map_pkChannelParty[peer->GetChannel()];
#endif
	itertype(pm) it = pm.find(p->dwLeaderPID);

	if (it == pm.end())
	{
		sys_err("PARTY StateChange - Non exists [%lu] cannot state change [%lu]", p->dwLeaderPID, p->dwPID);
		return;
	}

	itertype(it->second) pit = it->second.find(p->dwPID);

	if (pit == it->second.end())
	{
		sys_err("PARTY StateChange - Cannot find [%lu] in party [%lu]", p->dwPID, p->dwLeaderPID);
		return;
	}

	if (p->bFlag)
	{
		pit->second.bRole = p->bRole;
	}
	else
	{
		pit->second.bRole = 0;
	}

#ifdef __ENABLE_PARTY_UPDATE__
	ForwardPacket(HEADER_DG_PARTY_STATE_CHANGE, p, sizeof(TPacketPartyStateChange));
#else
	ForwardPacket(HEADER_DG_PARTY_STATE_CHANGE, p, sizeof(TPacketPartyStateChange), peer->GetChannel(), peer);
#endif
	sys_log(0, "PARTY StateChange [%lu] at [%lu] from %d %d", p->dwPID, p->dwLeaderPID, p->bRole, p->bFlag);
}

void CClientManager::QUERY_PARTY_SET_MEMBER_LEVEL(CPeer* peer, TPacketPartySetMemberLevel* p)
{
#ifdef __ENABLE_PARTY_UPDATE__
	TPartyMap& pm = m_map_pkChannelParty[1];
#else
	TPartyMap& pm = m_map_pkChannelParty[peer->GetChannel()];
#endif
	itertype(pm) it = pm.find(p->dwLeaderPID);

	if (it == pm.end())
	{
		sys_err("PARTY SetMemberLevel - Non exists [%lu] cannot level change [%lu]", p->dwLeaderPID, p->dwPID);
		return;
	}

	itertype(it->second) pit = it->second.find(p->dwPID);

	if (pit == it->second.end())
	{
		sys_err("PARTY SetMemberLevel - Cannot find [%lu] in party [%lu]", p->dwPID, p->dwLeaderPID);
		return;
	}

	pit->second.bLevel = p->bLevel;

#ifdef __ENABLE_PARTY_UPDATE__
	ForwardPacket(HEADER_DG_PARTY_SET_MEMBER_LEVEL, p, sizeof(TPacketPartySetMemberLevel));
#else
	ForwardPacket(HEADER_DG_PARTY_SET_MEMBER_LEVEL, p, sizeof(TPacketPartySetMemberLevel), peer->GetChannel());
#endif
	sys_log(0, "PARTY SetMemberLevel pid [%lu] level %d", p->dwPID, p->bLevel);
}
