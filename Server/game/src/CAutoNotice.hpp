#pragma once
#ifdef __ENABLE_AUTO_NOTICE__
namespace CAutoNoticeManager
{
	bool Initialize();
	bool LoadConfiguration(const std::string& filePath);
	void Reload();
	void Clear();

	void Process();

	void BroadcastMessage(const std::string& text);
}
#endif
