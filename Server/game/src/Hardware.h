#ifndef __HARDWARE__H__
#define __HARDWARE__H__
#ifdef __HARDWARE_BAN__

#include <unordered_set>
#include <string>

#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

class CheckHardware : public singleton<CheckHardware>
{
	public:
		CheckHardware();
		virtual ~CheckHardware();
		void Destroy();
		void LoadBans();
		std::string DecodeHWID(const std::string & hdid);
		void AddBan(const std::string & id);
		void RemoveBan(std::string id);
		bool Check(const std::string & hdid) { return (bans_cointainer.find(hdid) == bans_cointainer.end()); }

	protected:
		std::unordered_set<std::string> bans_cointainer;
		CryptoPP::ECB_Mode< CryptoPP::AES >::Decryption spAES;
};
#endif
#endif

