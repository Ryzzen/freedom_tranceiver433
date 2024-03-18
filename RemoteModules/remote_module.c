#include "remote_module.h"
#include "nice.h"

void InitRemoteModule(remoteModule* base, remoteModuleType type, tranceivers tranceiver)
{
	static remoteModuleFactory factory[REMOTE_MODULE_MAX] = {
        &Nice_Init
    };

	factory[type](base, tranceiver);
	return;
}
