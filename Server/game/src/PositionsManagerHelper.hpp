#pragma once
#include "stdafx.h"
#ifdef __ENABLE_SAVE_POSITION__

class PositionsManagerHelper : public singleton<PositionsManagerHelper>
{
	public:
		enum EPositionsManagerHelper : int
		{
			MAX_SAVE_POSITIONS = 10,
		};

		enum class EPositionsCommandInterpreter : int
		{
			INFO_INITIALIZE,
			INFO_POSITIONS,
			INFO_POSITIONS_CLEAR,
		};

	public:
		PositionsManagerHelper();
		~PositionsManagerHelper();

		void	EmitPositions(LPCHARACTER& ch);
		void	InterpreterCommand(LPCHARACTER& ch, const EPositionsCommandInterpreter& broadType, BYTE bPos = 0, DWORD mapIndex = 0, int localX = 0, int localY = 0, int globalX = 0, int globalY = 0);

		void	SavePosition(LPCHARACTER& ch, BYTE bPos);
		void	ClearPosition(LPCHARACTER& ch, BYTE bPos);
		void	TeleportPosition(LPCHARACTER& ch, BYTE bPos);
};
#endif
