#pragma once

class CPythonSystem : public CSingleton<CPythonSystem>
{
public:
	enum EWindow
	{
		WINDOW_STATUS,
		WINDOW_INVENTORY,
		WINDOW_ABILITY,
		WINDOW_SOCIETY,
		WINDOW_JOURNAL,
		WINDOW_COMMAND,

		WINDOW_QUICK,
		WINDOW_GAUGE,
		WINDOW_MINIMAP,
		WINDOW_CHAT,

		WINDOW_MAX_NUM,
	};

	enum
	{
		FREQUENCY_MAX_NUM  = 75,
		RESOLUTION_MAX_NUM = 100
	};

#ifdef ENABLE_HIDE_EFFECTS
	enum EHideEffects
	{
		HIDE_EFFECTS_GENERAL = (1 << 0),
		HIDE_EFFECTS_SKILLS = (1 << 1),
		HIDE_EFFECTS_BUFFS = (1 << 2),
	};
#endif

#ifdef ENABLE_HIDE_OBJECTS
	enum EHideObjects
	{
		HIDE_OBJECTS_MOUNTS = (1 << 0),
		HIDE_OBJECTS_PETS = (1 << 1),
		HIDE_OBJECTS_SHOPS = (1 << 2),
	};
#endif

	typedef struct SResolution
	{
		DWORD	width;
		DWORD	height;
		DWORD	bpp;		// bits per pixel (high-color = 16bpp, true-color = 32bpp)

		DWORD	frequency[20];
		BYTE	frequency_count;
	} TResolution;

	typedef struct SWindowStatus
	{
		int		isVisible;
		int		isMinimized;

		int		ixPosition;
		int		iyPosition;
		int		iHeight;
	} TWindowStatus;

	typedef struct SConfig
	{
		DWORD			width;
		DWORD			height;
		DWORD			bpp;
		DWORD			frequency;

		bool			is_software_cursor;
		bool			is_object_culling;
		int				iDistance;
		int				iShadowLevel;

		FLOAT			music_volume;
		BYTE			voice_volume;

		int				gamma;

		int				isSaveID;
		char			SaveID[20];

		bool			bWindowed;
		bool			bDecompressDDS;
		bool			bNoSoundCard;
		bool			bUseDefaultIME;
		BYTE			bSoftwareTiling;
		bool			bViewChat;
		bool			bAlwaysShowName;
		bool			bShowDamage;
		bool			bShowSalesText;
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
		bool			bShowMobAIFlag;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
		bool			bShowMobLevel;
#endif
#ifdef ENABLE_HIDE_EFFECTS
		DWORD			dwEffectsFlag;
#endif
#ifdef ENABLE_HIDE_OBJECTS
		DWORD			dwObjectsFlag;
#endif
#ifdef ENABLE_NEW_FILTERS
		bool			PickUpSolo;
		bool			IgnoreWeapon;
		bool			IgnoreArmor;
		bool			IgnoreHelmet;
		bool			IgnoreShield;
		bool			IgnoreBracelets;
		bool			IgnoreShoes;
		bool			IgnoreNecklets;
		bool			IgnoreEarrings;
		bool			IgnoreOthers;
#endif
	} TConfig;

public:
	CPythonSystem();
	virtual ~CPythonSystem();

	void Clear();
	void SetInterfaceHandler(PyObject * poHandler);
	void DestroyInterfaceHandler();

	// Config
	void							SetDefaultConfig();
	bool							LoadConfig();
	bool							SaveConfig();
	void							ApplyConfig();
	void							SetConfig(TConfig * set_config);
	TConfig *						GetConfig();
	void							ChangeSystem();

	// Interface
	bool							LoadInterfaceStatus();
	void							SaveInterfaceStatus();
	bool							isInterfaceConfig();
	const TWindowStatus &			GetWindowStatusReference(int iIndex);

	DWORD							GetWidth();
	DWORD							GetHeight();
	DWORD							GetBPP();
	DWORD							GetFrequency();
	bool							IsSoftwareCursor();
	bool							IsWindowed();
	bool							IsViewChat();
	bool							IsAlwaysShowName();
	bool							IsShowDamage();
	bool							IsShowSalesText();
	bool							IsUseDefaultIME();
	bool							IsNoSoundCard();
	bool							IsAutoTiling();
	bool							IsSoftwareTiling();
	void							SetSoftwareTiling(bool isEnable);
	void							SetViewChatFlag(int iFlag);
	void							SetAlwaysShowNameFlag(int iFlag);
	void							SetShowDamageFlag(int iFlag);
	void							SetShowSalesTextFlag(int iFlag);
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
	bool							IsShowMobAIFlag();
	void							SetShowMobAIFlagFlag(int iFlag);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
	bool							IsShowMobLevel();
	void							SetShowMobLevelFlag(int iFlag);
#endif
#ifdef ENABLE_HIDE_EFFECTS
	void							SetHideEffectsFlag(DWORD iFlag, bool isHide);
	bool							IsHideEffects(DWORD dwHideFlag);
#endif
#ifdef ENABLE_HIDE_OBJECTS
	void							SetHideObjectsFlag(DWORD iFlag, bool isHide);
	bool							IsHideObjects(DWORD dwHideFlag);
#endif
#ifdef ENABLE_NEW_FILTERS
	bool							IsIgnoring(int iFlag);
	void							SetIgnoring(int iFlag, bool value);
	bool							IsVectorPickupMode();
	void							SwitchPickupMode();
#endif

	// Window
	void							SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

	// SaveID
	int								IsSaveID();
	const char *					GetSaveID();
	void							SetSaveID(int iValue, const char * c_szSaveID);

	/// Display
	void							GetDisplaySettings();

	int								GetResolutionCount();
	int								GetFrequencyCount(int index);
	bool							GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp);
	bool							GetFrequency(int index, int freq_index, OUT DWORD *frequncy);
	int								GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
	int								GetFrequencyIndex(int res_index, DWORD frequency);
	bool							isViewCulling();

	// Sound
	float							GetMusicVolume();
	int								GetSoundVolume();
	void							SetMusicVolume(float fVolume);
	void							SetSoundVolumef(float fVolume);

	int								GetDistance();
	int								GetShadowLevel();
	void							SetShadowLevel(unsigned int level);

protected:
	TResolution						m_ResolutionList[RESOLUTION_MAX_NUM];
	int								m_ResolutionCount;

	TConfig							m_Config;
	TConfig							m_OldConfig;

	bool							m_isInterfaceConfig;
	PyObject *						m_poInterfaceHandler;
	TWindowStatus					m_WindowStatus[WINDOW_MAX_NUM];
};