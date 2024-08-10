#include <memory>
#include "stdafx.h"
#ifdef __HARDWARE_BAN__
#include "../../common/tables.h"
#include "db.h"
#include "Hardware.h"
#include <cryptopp/cryptlib.h>
#include <cryptopp/aes.h>
#include <cryptopp/hex.h>

// The key we use for that enc/dec
namespace
{
	const std::vector<unsigned char> vAESDecKey{ 'n', '5', 'm', 'K', 'z', '2', 'E', 'P', '4', 'W', 'k', 'm', 'p', '4', '5', 'k' };
}

CheckHardware::CheckHardware()
{
	spAES.SetKey(vAESDecKey.data(), vAESDecKey.size());
	bans_cointainer.clear();
	LoadBans();
}

CheckHardware::~CheckHardware()
{
	Destroy();
}

void CheckHardware::Destroy()
{
	bans_cointainer.clear();
}

std::string CheckHardware::DecodeHWID(const std::string & hdid)
{
	using namespace CryptoPP;
	std::string sDecryptedString;

	// Decrypting string
	try
	{
		// Dehexing string
		std::string sTemp;
		StringSource ss(hdid, true,
			new HexDecoder(
				new StringSink(sTemp)
			) // HexEncoder
		); // StringSource

		// Decrypting string
		StringSource ss1(sTemp, true,
			new StreamTransformationFilter(spAES,
				new StringSink(sDecryptedString)
			) // StreamTransformationFilter      
		); // StringSource
	}
	catch (const CryptoPP::Exception& e)
	{
		return "";
	}

	// Check if string contains at least real size in its block
	if (!sDecryptedString.size())
		return "";

	return sDecryptedString;
}

void CheckHardware::AddBan(const std::string & id)
{
	if (!id.size())
		return;

	delete DBManager::instance().DirectQuery("REPLACE INTO account.HardwareBan VALUES ('%s')", id.c_str());
	bans_cointainer.insert(id);
}

void CheckHardware::RemoveBan(std::string id)
{
	delete DBManager::instance().DirectQuery("DELETE FROM account.HardwareBan WHERE id = '%s'", id.c_str());
	if (bans_cointainer.find(id) != bans_cointainer.end())
		bans_cointainer.erase(bans_cointainer.find(id));
}

void CheckHardware::LoadBans()
{
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT id FROM account.HardwareBan"));

	if (msg->Get()->uiNumRows == 0)
		return;
	
	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(pRes)))
		bans_cointainer.insert(row[0]);
}
#endif

