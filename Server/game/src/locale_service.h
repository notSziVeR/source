#ifndef __LOCALE_SERVCICE__
#define __LOCALE_SERVCICE__

void InitializeSystemPaths();
void InitializeLocalizationFiles();
void LocaleService_LoadEmpireTextConvertTables();
void LocaleService_TransferDefaultSetting();

const std::string& GetBasePath();
const std::string& GetMapPath();
const std::string& GetQuestPath();

#endif