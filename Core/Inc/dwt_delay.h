
#ifndef __DWT_DELAY_H
#define __DWT_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void DWT_Init(void);
void DWT_Delay(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif /* __DWT_DELAY_H */
