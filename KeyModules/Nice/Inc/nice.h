#ifndef __NICE_H
#define __NICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f1xx_hal.h"

#include "key_module.h"

#define NICE_PACKETSIZE 5
#define NICE_MAX_ID 1024
#define NICE_MAX_CHANNEL 4

#define PWM_SIZE 3
#define PWM_TRUE 0b011
#define PWM_FALSE 0b001
#define ARRAY_TO_BIT(array, bit, size) ((array[bit / (8 * size)] >> (((8 * size) - 1) - bit)) & 1)
#define PWM_BIT_ENCODE(bit) (bit ? PWM_TRUE : PWM_FALSE)

#define SET_BIT_ARRAY(out, index, value) (out[((index) - ((index) % 8)) / 8] |= (value&1) << (7-((index) % 8)))

/* TODO: make the PWM encoding it's own utility */

typedef struct niceModule_s {
	niceTranceivers tranceiver;
	uint16_t id;
	uint8_t channel;

	void* (*GetRfSettings)(niceModule*);
	void* (*Bruteforce)(niceModule*, uint32_t field);
} niceModule;


static void* Nice_Bruteforce(void* _this, uint32_t field)
static void* Nice_GetRfSettings(niceModule* this);

void Nice_Init(niceModule* this, Tranceivers tranceiver, uint16_t id, uint8_t channel)

#ifdef __cplusplus
}
#endif

#endif /* __NICE_H */
