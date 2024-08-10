#include "StdAfx.h"
#ifdef __USE_CYTHON__
#include "PythonrootlibManager.h"
#include "../../Extern/Python2/Python.h"
#pragma comment (lib, "rootlib.lib")

struct rootlib_SMethodDef
{
	char* func_name;
	void (*func)();
};

PyMODINIT_FUNC initcff();
PyMODINIT_FUNC initcheck_bad_files();
PyMODINIT_FUNC initcolorInfo();
PyMODINIT_FUNC initconsoleModule();
PyMODINIT_FUNC initconstInfo();
PyMODINIT_FUNC initdebugInfo();
PyMODINIT_FUNC initdragon_soul_refine_settings();
PyMODINIT_FUNC initemotion();
PyMODINIT_FUNC initequipmentSet();
PyMODINIT_FUNC initexception();
PyMODINIT_FUNC initgame();
PyMODINIT_FUNC initgrid();
PyMODINIT_FUNC inithelper();
PyMODINIT_FUNC initinGameWiki();
PyMODINIT_FUNC initingamewikiconfig();
PyMODINIT_FUNC initinGameWikiUI();
PyMODINIT_FUNC initinterfaceModule();
PyMODINIT_FUNC initintroCreate();
PyMODINIT_FUNC initintroEmpire();
PyMODINIT_FUNC initintrointerface();
PyMODINIT_FUNC initintroLoading();
PyMODINIT_FUNC initintroLogin();
PyMODINIT_FUNC initintroLogo();
PyMODINIT_FUNC initintroSelect();
PyMODINIT_FUNC initintroTest();
PyMODINIT_FUNC inititemwrapper();
PyMODINIT_FUNC initlocaleInfo();
PyMODINIT_FUNC initmouseModule();
PyMODINIT_FUNC initmusicInfo();
PyMODINIT_FUNC initnetworkModule();
PyMODINIT_FUNC initplayerSettingModule();
PyMODINIT_FUNC initPrototype();
PyMODINIT_FUNC initpyaudio();
PyMODINIT_FUNC initqueuemanager();
PyMODINIT_FUNC initserverCommandParser();
PyMODINIT_FUNC initserverInfo();
PyMODINIT_FUNC initshortcutsConfig();
PyMODINIT_FUNC initstringCommander();
PyMODINIT_FUNC initsystem();
PyMODINIT_FUNC inittranslatemanager();
PyMODINIT_FUNC initui();
PyMODINIT_FUNC inituiadminmanager();
PyMODINIT_FUNC inituiAffectShower();
PyMODINIT_FUNC inituiAmuletSystem();
PyMODINIT_FUNC inituiAttachMetin();
PyMODINIT_FUNC inituibanpanel();
PyMODINIT_FUNC inituibattlepass();
PyMODINIT_FUNC inituibiologmanager();
PyMODINIT_FUNC inituiboxloot();
PyMODINIT_FUNC inituiCandidate();
PyMODINIT_FUNC inituiCharacter();
PyMODINIT_FUNC inituiChat();
PyMODINIT_FUNC inituiCommon();
PyMODINIT_FUNC inituicuberenewal();
PyMODINIT_FUNC inituidiscarditemwindow();
PyMODINIT_FUNC inituiDragonSoul();
PyMODINIT_FUNC inituidungeoninfo();
PyMODINIT_FUNC inituiDungeonTask();
PyMODINIT_FUNC inituiEquipmentDialog();
PyMODINIT_FUNC inituieventmanager();
PyMODINIT_FUNC inituiExchange();
PyMODINIT_FUNC inituifindletters();
PyMODINIT_FUNC inituiGameButton();
PyMODINIT_FUNC inituiGameOption();
PyMODINIT_FUNC inituigayasystem();
PyMODINIT_FUNC inituigmidleinterface();
PyMODINIT_FUNC inituiGuild();
PyMODINIT_FUNC inituiHandSwitcher();
PyMODINIT_FUNC inituiHelp();
PyMODINIT_FUNC inituiHunterMission();
PyMODINIT_FUNC inituiInventory();
PyMODINIT_FUNC inituiinventorymenu();
PyMODINIT_FUNC inituiitemopener();
PyMODINIT_FUNC inituiItemShopDialog();
PyMODINIT_FUNC inituiitemsplitter();
PyMODINIT_FUNC inituiLegendaryStones();
PyMODINIT_FUNC inituilogschat();
PyMODINIT_FUNC inituiluckybox();
PyMODINIT_FUNC inituiMapNameShower();
PyMODINIT_FUNC inituimarblemanager();
PyMODINIT_FUNC inituiMessenger();
PyMODINIT_FUNC inituiMiniMap();
PyMODINIT_FUNC inituiMultipleAncientShopBuy();
PyMODINIT_FUNC inituiMyShopDecoration();
PyMODINIT_FUNC inituiofflineshop();
PyMODINIT_FUNC inituiOption();
PyMODINIT_FUNC inituioxdialog();
PyMODINIT_FUNC inituiParty();
PyMODINIT_FUNC inituiPhaseCurtain();
PyMODINIT_FUNC inituiPickMoney();
PyMODINIT_FUNC inituiPlayerGauge();
PyMODINIT_FUNC inituiPointReset();
PyMODINIT_FUNC inituiPopupSystem();
PyMODINIT_FUNC inituipositionmanager();
PyMODINIT_FUNC inituipreview();
PyMODINIT_FUNC inituiPrivateShopBuilder();
PyMODINIT_FUNC inituiQuest();
PyMODINIT_FUNC inituirefactoredoptions();
PyMODINIT_FUNC inituiRefine();
PyMODINIT_FUNC inituirefineelement();
PyMODINIT_FUNC inituiremovestone();
PyMODINIT_FUNC inituiRestart();
PyMODINIT_FUNC inituiSafebox();
PyMODINIT_FUNC inituisashsystem();
PyMODINIT_FUNC inituiScalingOption();
PyMODINIT_FUNC inituiScriptLocale();
PyMODINIT_FUNC inituiselectitem();
PyMODINIT_FUNC inituiSelectMusic();
PyMODINIT_FUNC inituiSelectSkill();
PyMODINIT_FUNC inituiShamanSystem();
PyMODINIT_FUNC inituishipdefenddungeon();
PyMODINIT_FUNC inituiShop();
PyMODINIT_FUNC inituiShopSearch();
PyMODINIT_FUNC inituiswitchbot();
PyMODINIT_FUNC inituiSystem();
PyMODINIT_FUNC inituiSystemOption();
PyMODINIT_FUNC inituiTarget();
PyMODINIT_FUNC inituiTaskBar();
PyMODINIT_FUNC inituitechnicalmaintenance();
PyMODINIT_FUNC inituiteleportmanager();
PyMODINIT_FUNC inituiTip();
PyMODINIT_FUNC inituiTombola();
PyMODINIT_FUNC inituiToolTip();
PyMODINIT_FUNC inituitransmutation();
PyMODINIT_FUNC inituiUploadMark();
PyMODINIT_FUNC inituiWeb();
PyMODINIT_FUNC inituiWhisper();
PyMODINIT_FUNC inituizuopaneldialog();
PyMODINIT_FUNC initui_event();
PyMODINIT_FUNC initutil();
PyMODINIT_FUNC initutility();
PyMODINIT_FUNC initutils();
PyMODINIT_FUNC initvoicemanager();
PyMODINIT_FUNC initzuopanelhelper();

rootlib_SMethodDef rootlib_init_methods[] =
{
	{ "cff", initcff },
	{ "check_bad_files", initcheck_bad_files },
	{ "colorInfo", initcolorInfo },
	{ "consoleModule", initconsoleModule },
	{ "constInfo", initconstInfo },
	{ "debugInfo", initdebugInfo },
	{ "dragon_soul_refine_settings", initdragon_soul_refine_settings },
	{ "emotion", initemotion },
	{ "equipmentSet", initequipmentSet },
	{ "exception", initexception },
	{ "game", initgame },
	{ "grid", initgrid },
	{ "helper", inithelper },
	{ "inGameWiki", initinGameWiki },
	{ "ingamewikiconfig", initingamewikiconfig },
	{ "inGameWikiUI", initinGameWikiUI },
	{ "interfaceModule", initinterfaceModule },
	{ "introCreate", initintroCreate },
	{ "introEmpire", initintroEmpire },
	{ "introinterface", initintrointerface },
	{ "introLoading", initintroLoading },
	{ "introLogin", initintroLogin },
	{ "introLogo", initintroLogo },
	{ "introSelect", initintroSelect },
	{ "introTest", initintroTest },
	{ "itemwrapper", inititemwrapper },
	{ "localeInfo", initlocaleInfo },
	{ "mouseModule", initmouseModule },
	{ "musicInfo", initmusicInfo },
	{ "networkModule", initnetworkModule },
	{ "playerSettingModule", initplayerSettingModule },
	{ "Prototype", initPrototype },
	{ "pyaudio", initpyaudio },
	{ "queuemanager", initqueuemanager },
	{ "serverCommandParser", initserverCommandParser },
	{ "serverInfo", initserverInfo },
	{ "shortcutsConfig", initshortcutsConfig },
	{ "stringCommander", initstringCommander },
	{ "system", initsystem },
	{ "translatemanager", inittranslatemanager },
	{ "ui", initui },
	{ "uiadminmanager", inituiadminmanager },
	{ "uiAffectShower", inituiAffectShower },
	{ "uiAmuletSystem", inituiAmuletSystem },
	{ "uiAttachMetin", inituiAttachMetin },
	{ "uibanpanel", inituibanpanel },
	{ "uibattlepass", inituibattlepass },
	{ "uibiologmanager", inituibiologmanager },
	{ "uiboxloot", inituiboxloot },
	{ "uiCandidate", inituiCandidate },
	{ "uiCharacter", inituiCharacter },
	{ "uiChat", inituiChat },
	{ "uiCommon", inituiCommon },
	{ "uicuberenewal", inituicuberenewal },
	{ "uidiscarditemwindow", inituidiscarditemwindow },
	{ "uiDragonSoul", inituiDragonSoul },
	{ "uidungeoninfo", inituidungeoninfo },
	{ "uiDungeonTask", inituiDungeonTask },
	{ "uiEquipmentDialog", inituiEquipmentDialog },
	{ "uieventmanager", inituieventmanager },
	{ "uiExchange", inituiExchange },
	{ "uifindletters", inituifindletters },
	{ "uiGameButton", inituiGameButton },
	{ "uiGameOption", inituiGameOption },
	{ "uigayasystem", inituigayasystem },
	{ "uigmidleinterface", inituigmidleinterface },
	{ "uiGuild", inituiGuild },
	{ "uiHandSwitcher", inituiHandSwitcher },
	{ "uiHelp", inituiHelp },
	{ "uiHunterMission", inituiHunterMission },
	{ "uiInventory", inituiInventory },
	{ "uiinventorymenu", inituiinventorymenu },
	{ "uiitemopener", inituiitemopener },
	{ "uiItemShopDialog", inituiItemShopDialog },
	{ "uiitemsplitter", inituiitemsplitter },
	{ "uiLegendaryStones", inituiLegendaryStones },
	{ "uilogschat", inituilogschat },
	{ "uiluckybox", inituiluckybox },
	{ "uiMapNameShower", inituiMapNameShower },
	{ "uimarblemanager", inituimarblemanager },
	{ "uiMessenger", inituiMessenger },
	{ "uiMiniMap", inituiMiniMap },
	{ "uiMultipleAncientShopBuy", inituiMultipleAncientShopBuy },
	{ "uiMyShopDecoration", inituiMyShopDecoration },
	{ "uiofflineshop", inituiofflineshop },
	{ "uiOption", inituiOption },
	{ "uioxdialog", inituioxdialog },
	{ "uiParty", inituiParty },
	{ "uiPhaseCurtain", inituiPhaseCurtain },
	{ "uiPickMoney", inituiPickMoney },
	{ "uiPlayerGauge", inituiPlayerGauge },
	{ "uiPointReset", inituiPointReset },
	{ "uiPopupSystem", inituiPopupSystem },
	{ "uipositionmanager", inituipositionmanager },
	{ "uipreview", inituipreview },
	{ "uiPrivateShopBuilder", inituiPrivateShopBuilder },
	{ "uiQuest", inituiQuest },
	{ "uirefactoredoptions", inituirefactoredoptions },
	{ "uiRefine", inituiRefine },
	{ "uirefineelement", inituirefineelement },
	{ "uiremovestone", inituiremovestone },
	{ "uiRestart", inituiRestart },
	{ "uiSafebox", inituiSafebox },
	{ "uisashsystem", inituisashsystem },
	{ "uiScalingOption", inituiScalingOption },
	{ "uiScriptLocale", inituiScriptLocale },
	{ "uiselectitem", inituiselectitem },
	{ "uiSelectMusic", inituiSelectMusic },
	{ "uiSelectSkill", inituiSelectSkill },
	{ "uiShamanSystem", inituiShamanSystem },
	{ "uishipdefenddungeon", inituishipdefenddungeon },
	{ "uiShop", inituiShop },
	{ "uiShopSearch", inituiShopSearch },
	{ "uiswitchbot", inituiswitchbot },
	{ "uiSystem", inituiSystem },
	{ "uiSystemOption", inituiSystemOption },
	{ "uiTarget", inituiTarget },
	{ "uiTaskBar", inituiTaskBar },
	{ "uitechnicalmaintenance", inituitechnicalmaintenance },
	{ "uiteleportmanager", inituiteleportmanager },
	{ "uiTip", inituiTip },
	{ "uiTombola", inituiTombola },
	{ "uiToolTip", inituiToolTip },
	{ "uitransmutation", inituitransmutation },
	{ "uiUploadMark", inituiUploadMark },
	{ "uiWeb", inituiWeb },
	{ "uiWhisper", inituiWhisper },
	{ "uizuopaneldialog", inituizuopaneldialog },
	{ "ui_event", initui_event },
	{ "util", initutil },
	{ "utility", initutility },
	{ "utils", initutils },
	{ "voicemanager", initvoicemanager },
	{ "zuopanelhelper", initzuopanelhelper },
	{ NULL, NULL },
};

static PyObject* rootlib_isExist(PyObject *self, PyObject *args)
{
	char* func_name;

	if(!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name;i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			return Py_BuildValue("i", 1);
		}
	}
	return Py_BuildValue("i", 0);
}

static PyObject* rootlib_moduleImport(PyObject *self, PyObject *args)
{
	char* func_name;

	if(!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name;i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			rootlib_init_methods[i].func();
			if (PyErr_Occurred())
				return NULL;
			PyObject* m = PyDict_GetItemString(PyImport_GetModuleDict(), rootlib_init_methods[i].func_name);
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

static PyObject* rootlib_getList(PyObject *self, PyObject *args)
{
	int iTupleSize = 0;
	while (NULL != rootlib_init_methods[iTupleSize].func_name) {iTupleSize++;}

	PyObject* retTuple = PyTuple_New(iTupleSize);
	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		PyObject* retSubString = PyString_FromString(rootlib_init_methods[i].func_name);
		PyTuple_SetItem(retTuple, i, retSubString);
	}
	return retTuple;
}

void initrootlibManager()
{
	static struct PyMethodDef methods[] =
	{
		{"isExist", rootlib_isExist, METH_VARARGS},
		{"moduleImport", rootlib_moduleImport, METH_VARARGS},
		{"getList", rootlib_getList, METH_VARARGS},
		{NULL, NULL},
	};

	PyObject* m;
	m = Py_InitModule("rootlib", methods);
}
#endif
