#pragma once

#include "Packet.h"
#include "AbstractChat.h"

class CWhisper
{
public:
	typedef struct SChatLine
	{
		CGraphicTextInstance Instance;

		SChatLine()
		{
		}
		~SChatLine()
		{
			Instance.Destroy();
		}

		static void DestroySystem();

		static SChatLine* New();
		static void Delete(SChatLine* pkChatLine);

		static CDynamicPool<SChatLine> ms_kPool;
	} TChatLine;

	typedef std::deque<TChatLine*> TChatLineDeque;
	typedef std::list<TChatLine*> TChatLineList;

public:
	CWhisper();
	~CWhisper();

	void Destroy();

	void SetPosition(float fPosition);
	void SetBoxSize(float fWidth, float fHeight);
	void AppendChat(int iType, const char* c_szChat);
	void Render(float fx, float fy);
#ifdef ENABLE_RENEWAL_EXCHANGE
	void SetDownwardsRendering(bool newState) { m_drawDownwards = newState; }
#endif

protected:
	void __Initialize();
	void __ArrangeChat();

protected:
	float m_fLineStep;
	float m_fWidth;
	float m_fHeight;
	float m_fcurPosition;

#ifdef ENABLE_RENEWAL_EXCHANGE
	bool m_drawDownwards;
#endif

	TChatLineDeque m_ChatLineDeque;
	TChatLineList m_ShowingChatLineList;

public:
	static void DestroySystem();

	static CWhisper* New();
	static void Delete(CWhisper* pkWhisper);

	static CDynamicPool<CWhisper>		ms_kPool;
};

class CPythonChat : public CSingleton<CPythonChat>, public IAbstractChat
{
public:
	enum EWhisperType
	{
		WHISPER_TYPE_CHAT               = 0,
		WHISPER_TYPE_NOT_EXIST          = 1,
		WHISPER_TYPE_TARGET_BLOCKED     = 2,
		WHISPER_TYPE_SENDER_BLOCKED     = 3,
		WHISPER_TYPE_ERROR              = 4,
		WHISPER_TYPE_GM                 = 5,
#ifdef OFFLINE_MESSAGE_SYSTEM
		WHISPER_TYPE_MESSAGE_GONE = 6,
		WHISPER_TYPE_MESSAGE_LIMIT_REACHED = 7,
		WHISPER_TYPE_MESSAGE_PLAYER_NOT_EXIST = 8,
		WHISPER_TYPE_MESSAGE_SENT = 9,
#endif
		WHISPER_TYPE_SYSTEM             = 0xFF
	};

	enum EBoardState
	{
		BOARD_STATE_VIEW,
		BOARD_STATE_EDIT,
		BOARD_STATE_LOG,
	};

	enum
	{
		CHAT_LINE_MAX_NUM = 300,
		CHAT_LINE_COLOR_ARRAY_MAX_NUM = 3,
	};

	typedef struct SChatLine
	{
		int iType;
#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		int iLocale;
#endif
		float fAppendedTime;
		D3DXCOLOR aColor[CHAT_LINE_COLOR_ARRAY_MAX_NUM];
#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		CGraphicExpandedImageInstance FlagInstance;
		bool FlagScaled;
		int FlagWidth;
#endif
		CGraphicTextInstance Instance;
		SChatLine();
		virtual ~SChatLine();

		void SetColor(DWORD dwID, DWORD dwColor);
		void SetColorAll(DWORD dwColor);
		D3DXCOLOR & GetColorRef(DWORD dwID);
		static void DestroySystem();

		static SChatLine* New();
		static void Delete(SChatLine* pkChatLine);

		static CDynamicPool<SChatLine> ms_kPool;
	} TChatLine;

	typedef struct SWaitChat
	{
		int iType;
		std::string strChat;

		DWORD dwAppendingTime;
	} TWaitChat;

	typedef std::deque<TChatLine*> TChatLineDeque;
	typedef std::list<TChatLine*> TChatLineList;

	typedef std::map<std::string, CWhisper*> TWhisperMap;
	typedef std::set<std::string> TIgnoreCharacterSet;
	typedef std::list<TWaitChat> TWaitChatList;

	typedef struct SChatSet
	{
		int					m_ix;
		int					m_iy;
		int					m_iHeight;
		int					m_iStep;
		float				m_fEndPos;

		int					m_iBoardState;
		std::vector<int>	m_iMode;

		TChatLineList		m_ShowingChatLineList;

		bool CheckMode(DWORD dwType)
		{
			if (dwType >= m_iMode.size())
			{
				return false;
			}

			return m_iMode[dwType] ? true : false;
		}

		SChatSet()
		{
			m_iBoardState = BOARD_STATE_VIEW;

			m_ix = 0;
			m_iy = 0;
			m_fEndPos = 1.0f;
			m_iHeight = 0;
			m_iStep = 15;

			m_iMode.clear();
			m_iMode.resize(ms_iChatModeSize, 1);
		}

		static int ms_iChatModeSize;
	} TChatSet;

	typedef std::map<int, TChatSet> TChatSetMap;

public:
	CPythonChat();
	virtual ~CPythonChat();

	void SetChatColor(UINT eType, UINT r, UINT g, UINT b);
#ifdef ENABLE_RENEWAL_WHISPER
	void DestroyWhisper();
#endif
	void Destroy();
	void Close();

	int CreateChatSet(DWORD dwID);
	void Update(DWORD dwID);
	void Render(DWORD dwID);
	void RenderWhisper(const char * c_szName, float fx, float fy);

	void SetBoardState(DWORD dwID, int iState);
	void SetPosition(DWORD dwID, int ix, int iy);
	void SetHeight(DWORD dwID, int iHeight);
	void SetStep(DWORD dwID, int iStep);
	void ToggleChatMode(DWORD dwID, int iMode);
	void EnableChatMode(DWORD dwID, int iMode);
	void DisableChatMode(DWORD dwID, int iMode);
	void SetEndPos(DWORD dwID, float fPos);

	int  GetVisibleLineCount(DWORD dwID);
	int  GetEditableLineCount(DWORD dwID);
	int  GetLineCount(DWORD dwID);
	int  GetLineStep(DWORD dwID);

	// Chat
#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	void AppendChat(int iType, int locale, const char* c_szChat);
#endif

	void AppendChat(int iType, const char * c_szChat);

	void AppendChatWithDelay(int iType, const char * c_szChat, int iDelay);
	void ArrangeShowingChat(DWORD dwID);

	// Ignore
	void IgnoreCharacter(const char * c_szName);
	BOOL IsIgnoreCharacter(const char * c_szName);

	// Whisper
	CWhisper * CreateWhisper(const char * c_szName);
	void AppendWhisper(int iType, const char * c_szName, const char * c_szChat);
	void ClearWhisper(const char * c_szName);
	BOOL GetWhisper(const char * c_szName, CWhisper ** ppWhisper);
	void InitWhisper(PyObject * ppyObject);

protected:
	void __Initialize();
	void __DestroyWhisperMap();

	TChatLineList * GetChatLineListPtr(DWORD dwID);
	TChatSet * GetChatSetPtr(DWORD dwID);

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	void UpdateFlagInstance(TChatSet* pChatSet, TChatLine* pChatLine, int& curX, int curY);
#endif
	void UpdateViewMode(DWORD dwID);
	void UpdateEditMode(DWORD dwID);
	void UpdateLogMode(DWORD dwID);

	DWORD GetChatColor(int iType);

protected:
	TChatLineDeque						m_ChatLineDeque;
	TChatLineList						m_ShowingChatLineList;
	TChatSetMap							m_ChatSetMap;
	TWhisperMap							m_WhisperMap;
	TIgnoreCharacterSet					m_IgnoreCharacterSet;
	TWaitChatList						m_WaitChatList;

	D3DXCOLOR m_akD3DXClrChat[CHAT_TYPE_MAX_NUM];
};