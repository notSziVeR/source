#include "StdAfx.h"

#ifdef ENABLE_NEW_LOGS_CHAT
#include "PythonLogsChatModule.h"
#include "PythonNetworkStream.h"
#include "PythonApplication.h"

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CPythonLogsChatModule::CPythonLogsChatModule()
{
	logsContainer.reserve(MAX_LOGS_QUEUE);
}

CPythonLogsChatModule::~CPythonLogsChatModule()
	= default;

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

void CPythonLogsChatModule::SetManagerHandler(const EHandlersType eNum, PyObject * handler)
{
	switch (eNum)
	{
	case EHandlersType::MAIN_HANDLER:
		mainLogsHandler_ = handler;
		break;

	case EHandlersType::LEFT_HANDLER:
		leftLogsHandler_ = handler;
		break;
	}
}

void CPythonLogsChatModule::InsertInformation(const ELogsType eNum, int amount, int value, int time)
{
	TLogsInformation log { 0 };
	log.amount = amount;
	log.value = value;
	log.time = time;

	logsContainer.insert(logsContainer.begin(), std::make_pair(static_cast<BYTE>(eNum), log));
	if (logsContainer.size() > MAX_LOGS_QUEUE)
		logsContainer.pop_back();

	PyObject* tuple = Py_BuildValue("iii", amount, value, time);

	if (mainLogsHandler_)
		PyCallClassMemberFunc(mainLogsHandler_, "BINARY_HandlingLogInformation",
							  Py_BuildValue("(iO)", eNum, tuple));

	if (leftLogsHandler_)
		PyCallClassMemberFunc(leftLogsHandler_, "BINARY_HandlingLogInformation",
							  Py_BuildValue("(iO)", eNum, tuple));
}

bool CPythonLogsChatModule::ReceivePacket()
{
	net_logs_chat::GC_packet headerPacket;
	if (!CPythonNetworkStream::Instance().Recv(sizeof(headerPacket), &headerPacket))
	{
		TraceError("ChatLogs bad size!");
		return false;
	}

	CPythonLogsChatModule::Instance().InsertInformation(static_cast<ELogsType>(headerPacket.type_), headerPacket.amount_, headerPacket.value_, CPythonApplication::Instance().GetServerTimeStamp());
	return true;
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| Python interface
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| General Functions
\*******************************************************************/

PyObject* logsUpdateLogsChatHandler(PyObject * poSelf, PyObject * poArgs)
{
	int type;
	if (!PyTuple_GetInteger(poArgs, 0, &type))
	{
		return Py_BadArgument();
	}

	PyObject* mClass;
	if (!PyTuple_GetObject(poArgs, 1, &mClass))
	{
		return Py_BadArgument();
	}

	CPythonLogsChatModule::Instance().SetManagerHandler(static_cast<EHandlersType>(type), mClass);

	return Py_BuildNone();
}

PyObject* logsInsertInformation(PyObject* poSelf, PyObject* poArgs)
{
	BYTE ty_;
	if (!PyTuple_GetByte(poArgs, 0, &ty_))
	{
		return Py_BadArgument();
	}

	int am_;
	if (!PyTuple_GetInteger(poArgs, 1, &am_))
	{
		return Py_BadArgument();
	}

	int val_;
	if (!PyTuple_GetInteger(poArgs, 2, &val_))
	{
		return Py_BadArgument();
	}

	int time_;
	if (!PyTuple_GetInteger(poArgs, 3, &time_))
	{
		return Py_BadArgument();
	}

	CPythonLogsChatModule::Instance().InsertInformation(static_cast<ELogsType>(ty_), am_, val_, time_);

	return Py_BuildNone();
}

PyObject* logsGetInformation(PyObject* poSelf, PyObject* poArgs)
{
	auto& vec = CPythonLogsChatModule::Instance().GetInformation();

	PyObject* tuple = PyTuple_New(vec.size());

	int index = 0;
	for (const auto & kv : vec)
	{
		PyObject* child = PyTuple_New(2);
		PyTuple_SetItem(child, 0, Py_BuildValue("i", kv.first));
		PyTuple_SetItem(child, 1, Py_BuildValue("iii", kv.second.amount, kv.second.value, kv.second.time));
		PyTuple_SetItem(tuple, index++, child);
	}

	return Py_BuildValue("O", tuple);
}

PyObject* logsSetLowRsesoulution(PyObject* poSelf, PyObject* poArgs)
{
	bool res_;
	if (!PyTuple_GetBoolean(poArgs, 0, &res_))
	{
		return Py_BadArgument();
	}

	CPythonLogsChatModule::Instance().SetResolution(res_);
	return Py_BuildNone();
}

PyObject* logsGetLowResolution(PyObject* poSelf, PyObject* poArgs)
{
	auto res = CPythonLogsChatModule::Instance().GetResolution();

	return Py_BuildValue("b", res);
}

void initLogsChatModule()
{
	static PyMethodDef s_methods[] =
	{
		{ "UpdateLogChatHandler",	logsUpdateLogsChatHandler,	METH_VARARGS },
		{ "InsertInformation",		logsInsertInformation,		METH_VARARGS },
		{ "GetLogsInformations",	logsGetInformation,			METH_VARARGS },

		{ "SetLowResolutionSystem",	logsSetLowRsesoulution,		METH_VARARGS },
		{ "GetLowResolutionSystem", logsGetLowResolution,		METH_VARARGS },
		{ NULL, NULL, NULL }
	};

	PyObject* module = Py_InitModule("logsChat", s_methods);

	PyModule_AddIntConstant(module, "MAIN_HANDLER",			static_cast<int>(EHandlersType::MAIN_HANDLER));
	PyModule_AddIntConstant(module, "LEFT_HANDLER",			static_cast<int>(EHandlersType::LEFT_HANDLER));

	PyModule_AddIntConstant(module, "LOG_TYPE_YANG",		static_cast<int>(ELogsType::LOG_TYPE_YANG));
	PyModule_AddIntConstant(module, "LOG_TYPE_PICKUP",		static_cast<int>(ELogsType::LOG_TYPE_PICKUP));
	PyModule_AddIntConstant(module, "LOG_TYPE_EXCHANGE",	static_cast<int>(ELogsType::LOG_TYPE_EXCHANGE));
	PyModule_AddIntConstant(module, "LOG_TYPE_SHOP",		static_cast<int>(ELogsType::LOG_TYPE_SHOP));
	PyModule_AddIntConstant(module, "LOG_TYPE_CHEST",		static_cast<int>(ELogsType::LOG_TYPE_CHEST));
	PyModule_AddIntConstant(module, "LOG_TYPE_GAYA",		static_cast<int>(ELogsType::LOG_TYPE_GAYA));

	PyModule_AddIntConstant(module, "AVOID_FLOOD_MAX_LOGS", static_cast<int>(ELogsChatConfig::AVOID_FLOOD_MAX_LOGS));
	PyModule_AddIntConstant(module, "MAX_LOGS_QUEUE",		static_cast<int>(ELogsChatConfig::MAX_LOGS_QUEUE));
};
#endif
