#pragma once
#ifdef ENABLE_NEW_LOGS_CHAT
#include "StdAfx.h"
#include <unordered_map>
#include <mutex>

namespace net_logs_chat
{
#pragma pack(1)
struct GC_packet
{
	uint8_t header;

	int type_;
	int amount_;
	int value_;
};
#pragma pack()
};

enum class ELogsType : int
{
	LOG_TYPE_YANG,
	LOG_TYPE_PICKUP,
	LOG_TYPE_EXCHANGE,
	LOG_TYPE_SHOP,
	LOG_TYPE_CHEST,
	LOG_TYPE_GAYA,
};

enum class EHandlersType : int
{
	MAIN_HANDLER,
	LEFT_HANDLER,
};

enum ELogsChatConfig
{
	AVOID_FLOOD_MAX_LOGS = 50,
	MAX_LOGS_QUEUE = 1500,
};

/**
 * \brief
 */

class CPythonLogsChatModule : public CSingleton<CPythonLogsChatModule>
{
	struct TLogsInformation
	{
		int amount;
		int value;
		int time;
	};

public:
	CPythonLogsChatModule();
	virtual ~CPythonLogsChatModule();

	bool ReceivePacket();

public:
	void SetManagerHandler(const EHandlersType eNum, PyObject* handler);
	void InsertInformation(const ELogsType eNum, int amount, int value, int time);
	std::vector<std::pair<BYTE, TLogsInformation>> & GetInformation()
	{
		return logsContainer;
	};

	void SetResolution(bool resolution)
	{
		lowResolution = resolution;
	};

	bool GetResolution()
	{
		return lowResolution;
	};

private:
	PyObject* mainLogsHandler_;
	PyObject* leftLogsHandler_;
	bool	lowResolution;

private:
	std::vector<std::pair<BYTE, TLogsInformation>> logsContainer;
};
#endif
