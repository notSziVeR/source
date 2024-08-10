#pragma once
#ifdef __TECHNICAL_MAINTENANCE_ENABLE__

class CTechnicalMaintenance : public singleton<CTechnicalMaintenance>
{
	public:
		CTechnicalMaintenance();
		virtual ~CTechnicalMaintenance();
		void Destroy();

	public:
		void RegisterMaintenance(const std::string & _sMaintenanceName, const time_t & _ttMaintenanceTime, bool bBroadcast = false);
		void DelayMaintenance(const std::string & _sMaintenanceName, const time_t & ttNewTime, bool bBroadcast = false);
		void CancelMaintenance(const std::string & _sMaintenanceName, bool bBroadcast = false);
		void BroadcastAlert();
		void BroadcastMaintenanceInfo(LPCHARACTER ch = nullptr);
		bool FindMaintenance(const std::string & _sMaintenanceName);
		void RequestMaintenanceInfo();
		void SendMaintenanceInfo(LPDESC d);

	private:
		std::string sMaintenanceName;
		std::string sMaintenanceTime;
		time_t ttMaintenanceTime;
};
#endif

