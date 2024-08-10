// AntyCheat
#include "StdAfx.h"
#include "AntyCheat.h"
#include "ModuleEncrypter.h"

#ifdef __THEMIDA__
	#include <ThemidaSDK.h>
#endif

std::string HashModule(std::string module)
{
#ifdef __THEMIDA__
	VM_START
#endif

	auto hash_value = CryptedModules.find(module);
	if (hash_value != CryptedModules.end())
	{
		module = hash_value->second;
	}

#ifdef __THEMIDA__
	VM_END
#endif

	return module;
}

