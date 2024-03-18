#include "remote_module.h"
#include "nice.h"

remoteModule NewRemoteModule(remoteModuleType type, tranceivers tranceiver)
{
	static remoteModuleFactory factory[REMOTE_MODULE_MAX] = {
        &Nice_New
    };

	remoteModule base;
	factory[type](&base, tranceiver);

	return base;
}
