#include "key_module.h"

// returns a malloced module interface
keyModuleI* NewKeyModule(KEY_MODULE_TYPE type)
{
	static KeyModuleFactory factory[KEY_MODULE_MAX] = {
        &
    };

	keyModuleI* base = (keyModuleI*)(factory[type]());

	//malloc le bon module
	// le re
}
