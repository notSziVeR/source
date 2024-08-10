namespace GM
{
extern void init(DWORD* adwAdminConfig);
extern void insert(const tAdminInfo& c_rInfo);
extern void remove(const char* name);
extern unsigned char get_level(const char* name, const char* account = NULL, bool ignore_test_server = false);
extern void clear();

extern bool check_allow(unsigned char bGMLevel, DWORD dwCheckFlag);
extern bool check_account_allow(const std::string& stAccountName, DWORD dwCheckFlag);
}
