
#ifndef __KEYMODULE_H
#define __KEYMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef enum KEY_MODULE_TYPE_e {
	NICE;
} KEY_MODULE_TYPE; 

typedef struct keyModule_s {
	void (*Init)(void*);
	void (*Bruteforce)(void*, uint32_t);
};
	// keyModule module = GetModules(nb);
	// module.Init();
	// module.Bruteforce();

#ifdef __cplusplus
}
#endif

#endif /* __KEYMODULE_H */
