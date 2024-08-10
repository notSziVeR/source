#include "StdAfx.h"

#ifdef ENABLE_SHOP_SEARCH
#include "PythonShopSearch.h"

PyObject * shopsearchGetResultPageMaxNum(PyObject * poSelf, PyObject * poArgs)
{
	int maxNum = CPythonShopSearch::Instance().GetResultPageMaxNum();
	return Py_BuildValue("i", maxNum);
}

PyObject * shopsearchGetResultItemMaxNum(PyObject * poSelf, PyObject * poArgs)
{
	int maxNum = CPythonShopSearch::Instance().GetResultItemMaxNum();
	return Py_BuildValue("i", maxNum);
}

PyObject * shopsearchGetResultItemID(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("(ii)", item ? item->offlineID.first : 0, item ? item->offlineID.second : 0);
}

PyObject * shopsearchGetResultItemOwnerID(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->owner : 0);
}

PyObject * shopsearchGetResultItemVnum(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->vnum : 0);
}

PyObject * shopsearchGetResultItemCount(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->count : 0);
}

PyObject * shopsearchGetResultItemSocket(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}
	int socketIdx;
	if (!PyTuple_GetInteger(poArgs, 1, &socketIdx))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->alSockets[socketIdx] : 0);
}

PyObject * shopsearchGetResultItemAttribute(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}
	int attrIdx;
	if (!PyTuple_GetInteger(poArgs, 1, &attrIdx))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("ii", item ? item->aAttr[attrIdx].bType : 0, item ? item->aAttr[attrIdx].sValue : 0);
}

PyObject * shopsearchGetResultItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);

	return Py_BuildValue("L", item ? (int64_t)item->price : 0);
}

PyObject * shopsearchGetResultItemAvgPriceLevel(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->avgPriceLevel : 0);
}

PyObject * shopsearchGetResultItemEndTime(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->endTime : 0);
}

PyObject * shopsearchGetSoldPriceAverage(PyObject * poSelf, PyObject * poArgs)
{
	int day;
	if (!PyTuple_GetInteger(poArgs, 0, &day))
	{
		return Py_BadArgument();
	}

	const TShopSearchSoldItemInfo* soldInfo = CPythonShopSearch::Instance().GetSoldItemInfo(day);
	return Py_BuildValue("L", soldInfo ? (long long)soldInfo->averagePrice : 0LL);
//	return Py_BuildValue("K", soldInfo ? soldInfo->averagePrice : 0);//try
}

PyObject * shopsearchGetSoldItemCount(PyObject * poSelf, PyObject * poArgs)
{
	int day;
	if (!PyTuple_GetInteger(poArgs, 0, &day))
	{
		return Py_BadArgument();
	}

	const TShopSearchSoldItemInfo* soldInfo = CPythonShopSearch::Instance().GetSoldItemInfo(day);
	return Py_BuildValue("L", soldInfo ? soldInfo->count : 0LL);
}

PyObject* shopsearchGetResultTransmutate(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->transmutate_id : 0);
}

PyObject* shopsearchGetResultRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	const TShopSearchClientItem* item = CPythonShopSearch::Instance().GetResultItem(index);
	return Py_BuildValue("i", item ? item->dwRefineElement : 0);
}

void initShopSearch()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetResultPageMaxNum",			shopsearchGetResultPageMaxNum,			METH_VARARGS },
		{ "GetResultItemMaxNum",			shopsearchGetResultItemMaxNum,			METH_VARARGS },
		{ "GetResultItemID",				shopsearchGetResultItemID,				METH_VARARGS },
		{ "GetResultItemOwnerID",			shopsearchGetResultItemOwnerID,			METH_VARARGS },
		{ "GetResultItemVnum",				shopsearchGetResultItemVnum,			METH_VARARGS },
		{ "GetResultItemCount",				shopsearchGetResultItemCount,			METH_VARARGS },
		{ "GetResultItemSocket",			shopsearchGetResultItemSocket,			METH_VARARGS },
		{ "GetResultItemAttribute",			shopsearchGetResultItemAttribute,		METH_VARARGS },
		{ "GetResultItemPrice",				shopsearchGetResultItemPrice,			METH_VARARGS },
		{ "GetResultItemAvgPriceLevel",		shopsearchGetResultItemAvgPriceLevel,	METH_VARARGS },
		{ "GetResultItemEndTime",			shopsearchGetResultItemEndTime,			METH_VARARGS },

		{ "GetSoldPriceAverage",			shopsearchGetSoldPriceAverage,			METH_VARARGS },
		{ "GetSoldItemCount",				shopsearchGetSoldItemCount,				METH_VARARGS },

		{ "GetResultItemTransmutate",		shopsearchGetResultTransmutate,			METH_VARARGS },
		{ "GetResultItemRefineElement",		shopsearchGetResultRefineElement,		METH_VARARGS },

		{ NULL,								NULL,									NULL		 },
	};

	PyObject * poModule = Py_InitModule("shopsearch", s_methods);

	PyModule_AddIntConstant(poModule, "SOLD_ITEM_INFO_COUNT",	SHOPSEARCH_SOLD_ITEM_INFO_COUNT);

	PyModule_AddIntConstant(poModule, "BUY_SUCCESS",			SHOPSEARCH_BUY_SUCCESS);
	PyModule_AddIntConstant(poModule, "BUY_NOT_EXIST",			SHOPSEARCH_BUY_NOT_EXIST);
	PyModule_AddIntConstant(poModule, "BUY_PRICE_CHANGE",		SHOPSEARCH_BUY_PRICE_CHANGE);
	PyModule_AddIntConstant(poModule, "BUY_TIMEOUT",			SHOPSEARCH_BUY_TIMEOUT);
	PyModule_AddIntConstant(poModule, "BUY_NO_PEER",			SHOPSEARCH_BUY_NO_PEER);
	PyModule_AddIntConstant(poModule, "BUY_UNKNOWN_ERROR",		SHOPSEARCH_BUY_UNKNOWN_ERROR);

	PyModule_AddIntConstant(poModule, "SORT_RANDOM",			SHOPSEARCH_SORT_RANDOM);
	PyModule_AddIntConstant(poModule, "SORT_ASC",				SHOPSEARCH_SORT_ASC);
	PyModule_AddIntConstant(poModule, "SORT_DESC",				SHOPSEARCH_SORT_DESC);
	PyModule_AddIntConstant(poModule, "SORT_MAX_NUM",			SHOPSEARCH_SORT_MAX_NUM);

	PyModule_AddIntConstant(poModule, "AVG_PRICE_GOOD",			SHOPSEARCH_AVG_PRICE_GOOD);
	PyModule_AddIntConstant(poModule, "AVG_PRICE_NORMAL",		SHOPSEARCH_AVG_PRICE_NORMAL);
	PyModule_AddIntConstant(poModule, "AVG_PRICE_BAD",			SHOPSEARCH_AVG_PRICE_BAD);
	PyModule_AddIntConstant(poModule, "AVG_PRICE_WORST",		SHOPSEARCH_AVG_PRICE_WORST);
}
#endif
