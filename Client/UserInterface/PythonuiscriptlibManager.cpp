#include "StdAfx.h"
#ifdef __USE_CYTHON__
#include "PythonuiscriptlibManager.h"
#include "../../Extern/Python2/Python.h"
#pragma comment (lib, "uiscriptlib.lib")

struct uiscriptlib_SMethodDef
{
	char* func_name;
	void (*func)();
};

PyMODINIT_FUNC initacceptguildwardialog();
PyMODINIT_FUNC initacce_absorbwindow();
PyMODINIT_FUNC initadminmanagerwindow();
PyMODINIT_FUNC initAmuletInformation_Window();
PyMODINIT_FUNC initatlaswindow();
PyMODINIT_FUNC initattachstonedialog();
PyMODINIT_FUNC initban_panel();
PyMODINIT_FUNC initbattlepass();
PyMODINIT_FUNC initbeltinventorywindow();
PyMODINIT_FUNC initbiologmanageralert();
PyMODINIT_FUNC initbiologmanagerwindow();
PyMODINIT_FUNC initbuildguildbuildingwindow();
PyMODINIT_FUNC initbuyfailpopupdialog();
PyMODINIT_FUNC initchangegradenamedialog();
PyMODINIT_FUNC initchangepassworddialog();
PyMODINIT_FUNC initcharacterwindow();
PyMODINIT_FUNC initCostumeWindow();
PyMODINIT_FUNC initcreatecharacterwindow();
PyMODINIT_FUNC initcuberenewalwindow();
PyMODINIT_FUNC initdeclareguildwardialog();
PyMODINIT_FUNC initdiscarditemwindow();
PyMODINIT_FUNC initdoubleinputdialogwithdescription();
PyMODINIT_FUNC initDungeonInfo_Main();
PyMODINIT_FUNC initDungeonInfo_Ranking();
PyMODINIT_FUNC initDungeonTaskWindow();
PyMODINIT_FUNC initequipmentdialog();
PyMODINIT_FUNC initexchangedialog_new();
PyMODINIT_FUNC initexpandedmoneytaskbar();
PyMODINIT_FUNC initexpandedtaskbar();
PyMODINIT_FUNC initfindletterswindow();
PyMODINIT_FUNC initgameoptiondialog();
PyMODINIT_FUNC initgameoptiondialog_formobile();
PyMODINIT_FUNC initgamewindow();
PyMODINIT_FUNC initgaya_crafting();
PyMODINIT_FUNC initgaya_market();
PyMODINIT_FUNC initgmidlepanel();
PyMODINIT_FUNC initguildwindow();
PyMODINIT_FUNC initguildwindow_baseinfopage();
PyMODINIT_FUNC initguildwindow_boardpage();
PyMODINIT_FUNC initguildwindow_gradepage();
PyMODINIT_FUNC initguildwindow_guildinfopage();
PyMODINIT_FUNC initguildwindow_guildinfopage_eu();
PyMODINIT_FUNC initguildwindow_guildskillpage();
PyMODINIT_FUNC initguildwindow_memberpage();
PyMODINIT_FUNC initHandSwitcher_Window();
PyMODINIT_FUNC inithelpwindow();
PyMODINIT_FUNC inithelpwindow2();
PyMODINIT_FUNC initHunterMission_Window();
PyMODINIT_FUNC initidlepanelanswer();
PyMODINIT_FUNC initimekor();
PyMODINIT_FUNC initinputdialog();
PyMODINIT_FUNC initinputdialogofflineshop();
PyMODINIT_FUNC initinputdialogwithdescription();
PyMODINIT_FUNC initinputdialogwithdescription2();
PyMODINIT_FUNC initinventorymenu();
PyMODINIT_FUNC initinventorywindow();
PyMODINIT_FUNC inititemshop_main();
PyMODINIT_FUNC inititem_opener();
PyMODINIT_FUNC inititem_splitter();
PyMODINIT_FUNC initlegendary_stones_crafting_minerals();
PyMODINIT_FUNC initlegendary_stones_crafting_shards();
PyMODINIT_FUNC initlegendary_stones_crafting_stones();
PyMODINIT_FUNC initlegendary_stones_refine();
PyMODINIT_FUNC initloadingwindow();
PyMODINIT_FUNC initloginwindow();
PyMODINIT_FUNC initluckyboxwindow();
PyMODINIT_FUNC initmaintenance_alert();
PyMODINIT_FUNC initmaintenance_panel();
PyMODINIT_FUNC initmallwindow();
PyMODINIT_FUNC initmarblemanagerwindow();
PyMODINIT_FUNC initmarklistwindow();
PyMODINIT_FUNC initmessengerwindow();
PyMODINIT_FUNC initminimap();
PyMODINIT_FUNC initmoneyinputdialog();
PyMODINIT_FUNC initmoneyinputdialogofflineshop();
PyMODINIT_FUNC initmousebuttonwindow();
PyMODINIT_FUNC initmusiclistwindow();
PyMODINIT_FUNC initmyshopdecorationwindow();
PyMODINIT_FUNC initoffline_shop_builder();
PyMODINIT_FUNC initoffline_shop_editor();
PyMODINIT_FUNC initoffline_shop_manager();
PyMODINIT_FUNC initoffline_shop_viewer();
PyMODINIT_FUNC initoxdialog();
PyMODINIT_FUNC initpartymemberinfoboard();
PyMODINIT_FUNC initpassworddialog();
PyMODINIT_FUNC initpickmoneydialog();
PyMODINIT_FUNC initpopupdialog();
PyMODINIT_FUNC initpopupsystem_window();
PyMODINIT_FUNC initpositionsmanagerwindow();
PyMODINIT_FUNC initpreviewwindow();
PyMODINIT_FUNC initprivateshopbuilder();
PyMODINIT_FUNC initquestdialog();
PyMODINIT_FUNC initquestioncopybonusdialog();
PyMODINIT_FUNC initquestiondialog();
PyMODINIT_FUNC initquestiondialog2();
PyMODINIT_FUNC initquestiondialog3();
PyMODINIT_FUNC initquestiondialogadmin();
PyMODINIT_FUNC initquestiondialogmultiline();
PyMODINIT_FUNC initquestiondialogofflineshop();
PyMODINIT_FUNC initquestiondialogsort();
PyMODINIT_FUNC initrefactoredoptions();
PyMODINIT_FUNC initrefinedialog();
PyMODINIT_FUNC initrefineelementchange();
PyMODINIT_FUNC initrefineelementdialog();
PyMODINIT_FUNC initrestartdialog();
PyMODINIT_FUNC initrightmousebuttonwindow();
PyMODINIT_FUNC initsafeboxwindow();
PyMODINIT_FUNC initsash_absorption();
PyMODINIT_FUNC initsash_combination();
PyMODINIT_FUNC initscalingoption_main();
PyMODINIT_FUNC initSelectCharacterWindow();
PyMODINIT_FUNC initselectempirewindow();
PyMODINIT_FUNC initselectitemwindow();
PyMODINIT_FUNC initselectskillwindow();
PyMODINIT_FUNC initshopdialog();
PyMODINIT_FUNC initshopsearchwindow();
PyMODINIT_FUNC initspecialstorage();
PyMODINIT_FUNC initsystemdialog();
PyMODINIT_FUNC initsystemdialog_forportal();
PyMODINIT_FUNC initsystemoptiondialog();
PyMODINIT_FUNC inittaskbar();
PyMODINIT_FUNC initteleport_manager_window();
PyMODINIT_FUNC inittimewizard_dialog();
PyMODINIT_FUNC inittombola_window();
PyMODINIT_FUNC inittransmutation();
PyMODINIT_FUNC initwebloadingwindow();
PyMODINIT_FUNC initwebwindow();
PyMODINIT_FUNC initwhisperdialog();
PyMODINIT_FUNC initzuopanelhelper();
PyMODINIT_FUNC initzuo_event();

uiscriptlib_SMethodDef uiscriptlib_init_methods[] =
{
	{ "acceptguildwardialog", initacceptguildwardialog },
	{ "acce_absorbwindow", initacce_absorbwindow },
	{ "adminmanagerwindow", initadminmanagerwindow },
	{ "AmuletInformation_Window", initAmuletInformation_Window },
	{ "atlaswindow", initatlaswindow },
	{ "attachstonedialog", initattachstonedialog },
	{ "ban_panel", initban_panel },
	{ "battlepass", initbattlepass },
	{ "beltinventorywindow", initbeltinventorywindow },
	{ "biologmanageralert", initbiologmanageralert },
	{ "biologmanagerwindow", initbiologmanagerwindow },
	{ "buildguildbuildingwindow", initbuildguildbuildingwindow },
	{ "buyfailpopupdialog", initbuyfailpopupdialog },
	{ "changegradenamedialog", initchangegradenamedialog },
	{ "changepassworddialog", initchangepassworddialog },
	{ "characterwindow", initcharacterwindow },
	{ "CostumeWindow", initCostumeWindow },
	{ "createcharacterwindow", initcreatecharacterwindow },
	{ "cuberenewalwindow", initcuberenewalwindow },
	{ "declareguildwardialog", initdeclareguildwardialog },
	{ "discarditemwindow", initdiscarditemwindow },
	{ "doubleinputdialogwithdescription", initdoubleinputdialogwithdescription },
	{ "DungeonInfo_Main", initDungeonInfo_Main },
	{ "DungeonInfo_Ranking", initDungeonInfo_Ranking },
	{ "DungeonTaskWindow", initDungeonTaskWindow },
	{ "equipmentdialog", initequipmentdialog },
	{ "exchangedialog_new", initexchangedialog_new },
	{ "expandedmoneytaskbar", initexpandedmoneytaskbar },
	{ "expandedtaskbar", initexpandedtaskbar },
	{ "findletterswindow", initfindletterswindow },
	{ "gameoptiondialog", initgameoptiondialog },
	{ "gameoptiondialog_formobile", initgameoptiondialog_formobile },
	{ "gamewindow", initgamewindow },
	{ "gaya_crafting", initgaya_crafting },
	{ "gaya_market", initgaya_market },
	{ "gmidlepanel", initgmidlepanel },
	{ "guildwindow", initguildwindow },
	{ "guildwindow_baseinfopage", initguildwindow_baseinfopage },
	{ "guildwindow_boardpage", initguildwindow_boardpage },
	{ "guildwindow_gradepage", initguildwindow_gradepage },
	{ "guildwindow_guildinfopage", initguildwindow_guildinfopage },
	{ "guildwindow_guildinfopage_eu", initguildwindow_guildinfopage_eu },
	{ "guildwindow_guildskillpage", initguildwindow_guildskillpage },
	{ "guildwindow_memberpage", initguildwindow_memberpage },
	{ "HandSwitcher_Window", initHandSwitcher_Window },
	{ "helpwindow", inithelpwindow },
	{ "helpwindow2", inithelpwindow2 },
	{ "HunterMission_Window", initHunterMission_Window },
	{ "idlepanelanswer", initidlepanelanswer },
	{ "imekor", initimekor },
	{ "inputdialog", initinputdialog },
	{ "inputdialogofflineshop", initinputdialogofflineshop },
	{ "inputdialogwithdescription", initinputdialogwithdescription },
	{ "inputdialogwithdescription2", initinputdialogwithdescription2 },
	{ "inventorymenu", initinventorymenu },
	{ "inventorywindow", initinventorywindow },
	{ "itemshop_main", inititemshop_main },
	{ "item_opener", inititem_opener },
	{ "item_splitter", inititem_splitter },
	{ "legendary_stones_crafting_minerals", initlegendary_stones_crafting_minerals },
	{ "legendary_stones_crafting_shards", initlegendary_stones_crafting_shards },
	{ "legendary_stones_crafting_stones", initlegendary_stones_crafting_stones },
	{ "legendary_stones_refine", initlegendary_stones_refine },
	{ "loadingwindow", initloadingwindow },
	{ "loginwindow", initloginwindow },
	{ "luckyboxwindow", initluckyboxwindow },
	{ "maintenance_alert", initmaintenance_alert },
	{ "maintenance_panel", initmaintenance_panel },
	{ "mallwindow", initmallwindow },
	{ "marblemanagerwindow", initmarblemanagerwindow },
	{ "marklistwindow", initmarklistwindow },
	{ "messengerwindow", initmessengerwindow },
	{ "minimap", initminimap },
	{ "moneyinputdialog", initmoneyinputdialog },
	{ "moneyinputdialogofflineshop", initmoneyinputdialogofflineshop },
	{ "mousebuttonwindow", initmousebuttonwindow },
	{ "musiclistwindow", initmusiclistwindow },
	{ "myshopdecorationwindow", initmyshopdecorationwindow },
	{ "offline_shop_builder", initoffline_shop_builder },
	{ "offline_shop_editor", initoffline_shop_editor },
	{ "offline_shop_manager", initoffline_shop_manager },
	{ "offline_shop_viewer", initoffline_shop_viewer },
	{ "oxdialog", initoxdialog },
	{ "partymemberinfoboard", initpartymemberinfoboard },
	{ "passworddialog", initpassworddialog },
	{ "pickmoneydialog", initpickmoneydialog },
	{ "popupdialog", initpopupdialog },
	{ "popupsystem_window", initpopupsystem_window },
	{ "positionsmanagerwindow", initpositionsmanagerwindow },
	{ "previewwindow", initpreviewwindow },
	{ "privateshopbuilder", initprivateshopbuilder },
	{ "questdialog", initquestdialog },
	{ "questioncopybonusdialog", initquestioncopybonusdialog },
	{ "questiondialog", initquestiondialog },
	{ "questiondialog2", initquestiondialog2 },
	{ "questiondialog3", initquestiondialog3 },
	{ "questiondialogadmin", initquestiondialogadmin },
	{ "questiondialogmultiline", initquestiondialogmultiline },
	{ "questiondialogofflineshop", initquestiondialogofflineshop },
	{ "questiondialogsort", initquestiondialogsort },
	{ "refactoredoptions", initrefactoredoptions },
	{ "refinedialog", initrefinedialog },
	{ "refineelementchange", initrefineelementchange },
	{ "refineelementdialog", initrefineelementdialog },
	{ "restartdialog", initrestartdialog },
	{ "rightmousebuttonwindow", initrightmousebuttonwindow },
	{ "safeboxwindow", initsafeboxwindow },
	{ "sash_absorption", initsash_absorption },
	{ "sash_combination", initsash_combination },
	{ "scalingoption_main", initscalingoption_main },
	{ "SelectCharacterWindow", initSelectCharacterWindow },
	{ "selectempirewindow", initselectempirewindow },
	{ "selectitemwindow", initselectitemwindow },
	{ "selectskillwindow", initselectskillwindow },
	{ "shopdialog", initshopdialog },
	{ "shopsearchwindow", initshopsearchwindow },
	{ "specialstorage", initspecialstorage },
	{ "systemdialog", initsystemdialog },
	{ "systemdialog_forportal", initsystemdialog_forportal },
	{ "systemoptiondialog", initsystemoptiondialog },
	{ "taskbar", inittaskbar },
	{ "teleport_manager_window", initteleport_manager_window },
	{ "timewizard_dialog", inittimewizard_dialog },
	{ "tombola_window", inittombola_window },
	{ "transmutation", inittransmutation },
	{ "webloadingwindow", initwebloadingwindow },
	{ "webwindow", initwebwindow },
	{ "whisperdialog", initwhisperdialog },
	{ "zuopanelhelper", initzuopanelhelper },
	{ "zuo_event", initzuo_event },
	{ NULL, NULL },
};

static PyObject* uiscriptlib_isExist(PyObject *self, PyObject *args)
{
	char* func_name;

	if(!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != uiscriptlib_init_methods[i].func_name;i++)
	{
		if (0 == _stricmp(uiscriptlib_init_methods[i].func_name, func_name))
		{
			return Py_BuildValue("i", 1);
		}
	}
	return Py_BuildValue("i", 0);
}

static PyObject* uiscriptlib_moduleImport(PyObject *self, PyObject *args)
{
	char* func_name;

	if(!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != uiscriptlib_init_methods[i].func_name;i++)
	{
		if (0 == _stricmp(uiscriptlib_init_methods[i].func_name, func_name))
		{
			uiscriptlib_init_methods[i].func();
			if (PyErr_Occurred())
				return NULL;
			PyObject* m = PyDict_GetItemString(PyImport_GetModuleDict(), uiscriptlib_init_methods[i].func_name);
			if (m == NULL) {
				PyErr_SetString(PyExc_SystemError,
					"dynamic module not initialized properly");
				return NULL;
			}
			Py_INCREF(m);
			return Py_BuildValue("S", m);
		}
	}
	return NULL;
}

static PyObject* uiscriptlib_getList(PyObject *self, PyObject *args)
{
	int iTupleSize = 0;
	while (NULL != uiscriptlib_init_methods[iTupleSize].func_name) {iTupleSize++;}

	PyObject* retTuple = PyTuple_New(iTupleSize);
	for (int i = 0; NULL != uiscriptlib_init_methods[i].func_name; i++)
	{
		PyObject* retSubString = PyString_FromString(uiscriptlib_init_methods[i].func_name);
		PyTuple_SetItem(retTuple, i, retSubString);
	}
	return retTuple;
}

void inituiscriptlibManager()
{
	static struct PyMethodDef methods[] =
	{
		{"isExist", uiscriptlib_isExist, METH_VARARGS},
		{"moduleImport", uiscriptlib_moduleImport, METH_VARARGS},
		{"getList", uiscriptlib_getList, METH_VARARGS},
		{NULL, NULL},
	};

	PyObject* m;
	m = Py_InitModule("uiscriptlib", methods);
}
#endif
