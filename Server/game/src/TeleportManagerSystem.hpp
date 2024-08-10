#pragma once
#include "Parser.hpp"

#ifdef __ENABLE_TELEPORT_SYSTEM__
class CTeleportManager : public singleton<CTeleportManager>
{
	public:
		// Initial Functions
		CTeleportManager() = default;
		virtual ~CTeleportManager() = default;

		void Create();
		void Clear();
		void Destroy();
		bool Load(const std::string& fileName);

		// Reload Function
		void Reload();

	public:
		void SendInformation(LPCHARACTER& ch);
		void Teleport(LPCHARACTER& ch, DWORD& dwIndex);

		// Validation if one can be on map
		bool ValidateMap(LPCHARACTER ch, DWORD dwMapIndex);

		const TeleportCollection::TeleportSet::Teleport* FindSet(const uint32_t id) const;

	protected:
		std::map<uint32_t, TeleportCollection::TeleportSet> sets_;
};
#endif
