#pragma once

class CMapManager :
	public singleton<CMapManager>
{

public:
	typedef struct SMapHostInfo
	{
		char	szIP[16];
		WORD	wPort;

		SMapHostInfo(const char* c_szIP, WORD wPort)
		{
			memcpy(this->szIP, c_szIP, sizeof(this->szIP));
			this->wPort = wPort;
		}
	} TMapHostInfo;

private:
	typedef std::map<long, TMapHostInfo*> TMapHostInfoByMapIndexMap;
	typedef std::map<BYTE, TMapHostInfoByMapIndexMap*> TMapsByChannelMap;

private:
	TMapsByChannelMap	m_mapMapsByChannel;

public:
	CMapManager();
	virtual ~CMapManager();

	void			AddMap(BYTE byChannel, long lMapIndex, const char* c_szHostIP, WORD wHostPort);
	TMapHostInfo*	GetMapHostInfo(BYTE byChannel, long lMapIndex);
};
