#ifndef __INC_METIN_II_ABUSE_H__
#define __INC_METIN_II_ABUSE_H__
#include <array>

class AbuseController
{
public:
	AbuseController(LPCHARACTER ch);

private:
	LPCHARACTER m_ch;
	DWORD m_lastUpdatePulse;

	/************************************************************************/
	/* Mob puller                                                           */
	/************************************************************************/
private:
	DWORD m_lastAttackAnalysis;
	DWORD m_lastSuspiciousAttackTime;

	DWORD m_nextAttackCooldown;
	DWORD m_nextDetectStart;

	struct PullInfo
	{
		PIXEL_POSITION mobPos;
		PIXEL_POSITION playerPos;
		bool isAggressive;
	};
	std::vector<PullInfo> m_vecPullInfo;

	void AnalyzeAttacks();
public:
	void InitiatedFight(const PIXEL_POSITION& mobPos, bool aggressive);
	void SuspiciousAttackCooldown();

	/************************************************************************/
	/* Ghost mode (Activities while dead)									*/
	/************************************************************************/
private:
	DWORD m_lastDeathTime;
	DWORD m_nextDeadWalkLog;
	DWORD m_nextDeadPickupLog;

public:
	void DeadWalk();
	void DeadPickup();

	void MarkDead() { m_lastDeathTime = get_dword_time(); }
	bool JustDied() const { return m_lastDeathTime + 1500 > get_dword_time(); }

	/************************************************************************/
	/* Wall hacking                                                         */
	/************************************************************************/
private:
	long m_whStartX;
	long m_whStartY;
	long m_whStartZ;
	int m_whHackCount;
	DWORD m_whLastDetection;
	DWORD m_whIgnoreUntil;
	int m_whContinuousInvalid;
	DWORD m_whNextDecrease;

public:
	void VerifyCoordinates(LPSECTREE tree, long x, long y, long z);

	/************************************************************************/
	/* Movement speed hacking                                               */
	/************************************************************************/
private:
	long m_mshStartX;
	long m_mshStartY;
	long m_mshStartZ;
	std::vector<double> m_mshRates;
	DWORD m_mshStartDetect;
	DWORD m_mshLastDetect;
	DWORD m_mshIgnoreUntil;
	DWORD m_mshFirstDetect;

public:
	bool CanPickupItem();
	void SignalMovement(DWORD lastStopTime);
	void CheckSpeedhack(long x, long y);
	void ResetMovementSpeedhackChecker(bool full = false);
	void Analyze();

	/************************************************************************/
	/* Fishbotting                                                          */
	/************************************************************************/
private:
	bool m_isSuspectedFishbot;
	int m_fishesLeftToBubbleHide;
	int m_bubbleHideTimesLeft;
	int m_bubbleHideTimes;

public:
	bool IsSuspectedFishbot() const { return m_isSuspectedFishbot; }
	bool IsHideFishingBubble() const;
	void SuspectFishbotBy(LPCHARACTER ch);
	void EndSuspectFishbot();
	void NotifyHiddenBubble();
	void NotifyFishedWithBubbleHide(bool valid, long ms);

	/************************************************************************/
	/* WaitDmg						                                        */
	/************************************************************************/
private:
	bool m_waitDmgSuspect;
	DWORD m_lastMoveAttackPacketTime;
	DWORD m_ignoreWaitDmgUntil;
	std::vector<DWORD> m_moveAttackTimes;
public:
	//Motion of attacking / combo / skill
	void ReceiveMoveAttackPacket() { m_lastMoveAttackPacketTime = get_dword_time(); };

	//Plain, raw attack packet.
	void ReceiveAttackPacket();

	/************************************************************************/
	/* Pickup bot                                                           */
	/************************************************************************/

private:
	int m_pickupBotSuspectCount;

public:
	void AutoPickupSuspect();
};

template <typename T>
const T getMedian(std::vector<T>& values, size_t vectorSize)
{
	T median = 0;

	if (vectorSize > 0)
	{
		//Sort the vector
		std::sort(values.begin(), values.end());

		//Extract the median
		if (vectorSize % 2 == 0)
		{
			median = (values[vectorSize / 2 - 1] + values[vectorSize / 2]) / 2;
		}
		else
		{
			median = values[(size_t)std::floor(vectorSize / 2)];
		}
	}

	return median;
}

template <typename T>
const T getAverage(std::vector<T>& values, size_t vectorSize)
{
	T average = 0;

	if (vectorSize > 0)
	{
		for (const auto value : values)
		{
			average += value;
		}

		average /= vectorSize;
	}

	return average;
}

typedef std::shared_ptr<AbuseController> spAbuseController;

#endif // !__INC_METIN_II_ABUSE_H__