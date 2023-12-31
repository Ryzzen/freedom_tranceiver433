#include "stm32f1xx_hal.h"
#include "dwt_delay.h"

void DWT_Init(void)
{
    /* if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) { */
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    /* } */
}

void DWT_Delay(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
	uint32_t delayTicks = us * (SystemCoreClock / 1000000);

    while (DWT->CYCCNT - startTick < delayTicks);
}
