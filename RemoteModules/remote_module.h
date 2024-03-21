
#ifndef __KEYMODULE_H
#define __KEYMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define CHECK_OBJ if (!super || !super->this) return
#define S_FREE(a) free(a); a = NULL

typedef enum remoteModuleType_e {
	NICE,
	REMOTE_MODULE_MAX
} remoteModuleType; 

typedef enum tranceivers_e {
	CC1101,
	TRANCEIVER_MAX
} tranceivers;


// Key Module Interface
typedef struct remoteModule_s {
	void		(*Destructor)(struct remoteModule_s*);
	void*		(*GetRfSettings)(struct remoteModule_s*);
	void		(*AutoGeneratePacket)(struct remoteModule_s*, uint32_t, uint8_t*, size_t);
	uint32_t	(*GeneratePacket)(struct remoteModule_s*, void*, uint8_t*, size_t);
	void		(*SetPacket)(struct remoteModule_s*, void*);

	void	*this;
} remoteModule;

typedef void (*remoteModuleFactory)(remoteModule*, tranceivers);

void InitRemoteModule(remoteModule*, remoteModuleType, tranceivers);

#ifdef __cplusplus
}
#endif

#endif /* __KEYMODULE_H */
