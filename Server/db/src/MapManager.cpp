#include "stdafx.h"
#include "MapManager.h"

CMapManager::CMapManager()
{
	this->m_mapMapsByChannel.clear();
}

CMapManager::~CMapManager()
{
	for (TMapsByChannelMap::iterator itMapsByChannel = this->m_mapMapsByChannel.begin();
			itMapsByChannel != this->m_mapMapsByChannel.end();
			++itMapsByChannel)
	{
		for (TMapHostInfoByMapIndexMap::iterator itMapHostInfoByMapIndex = itMapsByChannel->second->begin();
				itMapHostInfoByMapIndex != itMapsByChannel->second->end();
				++itMapHostInfoByMapIndex)
		{
			delete itMapHostInfoByMapIndex->second;
		}
		delete itMapsByChannel->second;
	}

	this->m_mapMapsByChannel.clear();
}

void CMapManager::AddMap(BYTE byChannel, long lMapIndex, const char* c_szHostIP, WORD wHostPort)
{
	if (this->GetMapHostInfo(byChannel, lMapIndex))
	{
		sys_err("CMapManager::AddMap :: Duplicate entry for map %ld for channel %hhu.", lMapIndex, byChannel);
		return;
	}

	TMapsByChannelMap::iterator itMapsByChannel = this->m_mapMapsByChannel.find(byChannel);
	if (itMapsByChannel == this->m_mapMapsByChannel.end())
	{
		this->m_mapMapsByChannel.insert(TMapsByChannelMap::value_type(byChannel, new TMapHostInfoByMapIndexMap()));
		itMapsByChannel = this->m_mapMapsByChannel.find(byChannel);
		if (itMapsByChannel == this->m_mapMapsByChannel.end())
		{
			sys_err("CMapManager::AddMap :: Failed to add map holding host info by map index to by channel map.");
			return;
		}
	}

	itMapsByChannel->second->insert(TMapHostInfoByMapIndexMap::value_type(lMapIndex, new TMapHostInfo(c_szHostIP, wHostPort)));
	sys_log(0, "CMapManager::AddMap :: Added map %ld for channel %hhu on host %s by port %hu.", lMapIndex, byChannel, c_szHostIP, wHostPort);
}

CMapManager::TMapHostInfo* CMapManager::GetMapHostInfo(BYTE byChannel, long lMapIndex)
{
	TMapsByChannelMap::iterator itMapsByChannel = this->m_mapMapsByChannel.find(byChannel);
	if (itMapsByChannel == this->m_mapMapsByChannel.end())
	{
		return NULL;
	}

	TMapHostInfoByMapIndexMap::iterator itMapHostInfoByMapIndex = itMapsByChannel->second->find(lMapIndex);
	if (itMapHostInfoByMapIndex == itMapsByChannel->second->end())
	{
		return NULL;
	}

	return itMapHostInfoByMapIndex->second;
}