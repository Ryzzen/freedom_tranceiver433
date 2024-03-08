
#ifndef __KEYMODULE_H
#define __KEYMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f1xx_hal.h"

#include "nice.h"

typedef enum KEY_MODULE_TYPE_e {
	NICE;
	KEY_MODULE_MAX;
} KEY_MODULE_TYPE; 

typedef enum tranceivers_e {
	CC1101,
	TRANCEIVER_MAX
} tranceivers;

typedef enum param_e {
	ID,
	CHANNEL
} param;

// Key Module Interface
typedef struct keyModuleI_s {
	void 	(*Constructor)(void*);
	void	(*Destructor)(void*);
	void*	(*GetRfSettings)(void*);
	void	(*Bruteforce)(void*, uint32_t);
} KeyModuleI;

typedef void* (*KeyModuleFactory)(KEY_MODULE_TYPE);
	// keyModule module = GetModules(nb);
	// module.Init();
	// module.Bruteforce();

#ifdef __cplusplus
}
#endif

#endif /* __KEYMODULE_H */
