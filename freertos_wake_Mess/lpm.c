/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Compiler includes. */
#if defined(__ICCARM__)
#include <intrinsics.h>
#endif

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#if configUSE_TICKLESS_IDLE == 2
#include "fsl_gpt.h"
#else
#include "fsl_device_registers.h"
#endif

#include "lpm.h"
#include "fsl_gpc.h"
#include "fsl_debug_console.h"
#include "board.h"

#define BOARD_GPC_BASEADDR GPC
/*
 * Clock Speed of M core
 */
typedef enum lpm_mcore_clock_speed
{
    LPM_MCORE_HIGH_FREQ,
    LPM_MCORE_LOW_FREQ
} LPM_MCORE_CLOCK_SPEED;
void LPM_MCORE_ChangeMCoreClock(LPM_MCORE_CLOCK_SPEED target);

/*
 * LPM state of M core
 */
typedef enum lpm_power_status_mcore
{
    LPM_MCORE_STATE_RUN,
    LPM_MCORE_STATE_WAIT,
    LPM_MCORE_STATE_STOP,
} LPM_POWER_STATUS_MCORE;
void LPM_MCORE_SetPowerStatus(GPC_Type *base, LPM_POWER_STATUS_MCORE targetPowerMode);

void LPM_MCORE_ChangeMCoreClock(LPM_MCORE_CLOCK_SPEED target)
{
    /* Change CCM Root to change M7 clock*/
    switch (target)
    {
        case LPM_MCORE_LOW_FREQ:
            if (CLOCK_GetRootMux(kCLOCK_RootM7) != kCLOCK_M7RootmuxOsc24M)
            {
                CLOCK_SetRootMux(kCLOCK_RootM7, kCLOCK_M7RootmuxOsc24M);
                CLOCK_SetRootDivider(kCLOCK_RootM7, 1U, 1U);
            }
            break;
        case LPM_MCORE_HIGH_FREQ:
            if (CLOCK_GetRootMux(kCLOCK_RootM7) != kCLOCK_M7RootmuxSysPll1)
            {
                CLOCK_SetRootDivider(kCLOCK_RootM7, 1U, 1U);
                CLOCK_SetRootMux(kCLOCK_RootM7, kCLOCK_M7RootmuxSysPll1); /* switch cortex-m7 to SYSTEM PLL1 */
            }
            break;
        default:
            break;
    }
}

void LPM_MCORE_SetPowerStatus(GPC_Type *base, LPM_POWER_STATUS_MCORE targetPowerMode)
{
    gpc_lpm_config_t config;
    config.enCpuClk              = false;
    config.enFastWakeUp          = false;
    config.enDsmMask             = false;
    config.enWfiMask             = false;
    config.enVirtualPGCPowerdown = true;
    config.enVirtualPGCPowerup   = true;
    switch (targetPowerMode)
    {
        case LPM_MCORE_STATE_RUN:
            GPC->LPCR_M7 = GPC->LPCR_M7 & (~GPC_LPCR_M7_LPM0_MASK);
            break;
        case LPM_MCORE_STATE_WAIT:
            GPC_EnterWaitMode(GPC, &config);
            break;
        case LPM_MCORE_STATE_STOP:
            GPC_EnterStopMode(GPC, &config);
            break;
        default:
            break;
    }
}

void LPM_MCORE_PreSleepProcessing(void)
{
    DbgConsole_Deinit();
}

void LPM_MCORE_PostSleepProcessing(void)
{
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE,
                    BOARD_DEBUG_UART_CLK_FREQ);
}

extern uint32_t SystemCoreClock; /* in Kinetis SDK, this contains the system core clock speed */

/*
 * GPT timer base address and interrupt number
 */

#if configUSE_TICKLESS_IDLE == 2
extern GPT_Type *vPortGetGptBase(void);
extern IRQn_Type vPortGetGptIrqn(void);
#endif /* configUSE_TICKLESS_IDLE */

/*-----------------------------------------------------------*/

/*
 * The number of SysTick increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 2
static uint32_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE == 2
static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The number of GPT increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 2
static uint32_t ulLPTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The flag of GPT is occurs or not.
 */
#if configUSE_TICKLESS_IDLE == 2
static volatile bool ulLPTimerInterruptFired = false;
#endif /* configUSE_TICKLESS_IDLE */

#if configUSE_TICKLESS_IDLE == 2
void vPortGptIsr(void)
{
    ulLPTimerInterruptFired = true;
    /* Clear interrupt flag.*/
    GPT_ClearStatusFlags(GPT1, kGPT_OutputCompare1Flag);
}

void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods;
    TickType_t xModifiableIdleTime;
    GPT_Type *pxGptBase;

    pxGptBase = vPortGetGptBase();
    if (pxGptBase == 0)
        return;
    /* Make sure the SysTick reload value does not overflow the counter. */
    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
    {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }
    if (xExpectedIdleTime == 0)
        return;

    /* Calculate the reload value required to wait xExpectedIdleTime
    tick periods.  -1 is used because this code will execute part way
    through one of the tick periods. */
    ulReloadValue = (ulLPTimerCountsForOneTick * (xExpectedIdleTime - 1UL));

    /* Stop the GPT and systick momentarily.  The time the GPT and systick is stopped for
    is accounted for as best it can be, but using the tickless mode will
    inevitably result in some tiny drift of the time maintained by the
    kernel with respect to calendar time. */
    GPT_StopTimer(pxGptBase);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __disable_irq();
    __DSB();
    __ISB();

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep)
    {
        /* Restart from whatever is left in the count register to complete
        this tick period. */
        SysTick->LOAD = SysTick->VAL;

        /* Restart SysTick. */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        /* Reset the reload register to the value required for normal tick
        periods. */
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

        /* Re-enable interrupts - see comments above __disable_irq()
        call above. */
        __enable_irq();
    }
    else
    {
        /* Set the new reload value. */
        GPT_SetOutputCompareValue(pxGptBase, kGPT_OutputCompare_Channel1, ulReloadValue);

        /* Enable GPT. */
        GPT_StartTimer(pxGptBase);

        /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
        set its parameter to 0 to indicate that its implementation contains
        its own wait for interrupt or wait for event instruction, and so wfi
        should not be executed again.  However, the original expected idle
        time variable must remain unmodified, so a copy is taken. */
        xModifiableIdleTime = xExpectedIdleTime;
        
        LPM_MCORE_ChangeMCoreClock(LPM_MCORE_LOW_FREQ);
        LPM_MCORE_SetPowerStatus(BOARD_GPC_BASEADDR, LPM_MCORE_STATE_STOP);
        LPM_MCORE_PreSleepProcessing();
        //>>>>>>>>>>>>>>>>>>>
        
        configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
        if (xModifiableIdleTime > 0)
        {
            __DSB();
            __WFI();
            __ISB();
        }
        configPOST_SLEEP_PROCESSING(xExpectedIdleTime);
        
        //<<<<<<<<<<<<<<<<<<<
        LPM_MCORE_PostSleepProcessing();
        LPM_MCORE_ChangeMCoreClock(LPM_MCORE_HIGH_FREQ);
        LPM_MCORE_SetPowerStatus(BOARD_GPC_BASEADDR, LPM_MCORE_STATE_RUN);
        
        
        ulLPTimerInterruptFired = false;

        /* Re-enable interrupts - see comments above __disable_irq()
        call above. */
        __enable_irq();
        __NOP();
        if (ulLPTimerInterruptFired)
        {
            /* The tick interrupt handler will already have pended the tick
            processing in the kernel.  As the pending tick will be
            processed as soon as this function exits, the tick value
            maintained by the tick is stepped forward by one less than the
            time spent waiting. */
            ulCompleteTickPeriods   = xExpectedIdleTime - 1UL;
            ulLPTimerInterruptFired = false;
        }
        else
        {
            /* Something other than the tick interrupt ended the sleep.
            Work out how long the sleep lasted rounded to complete tick
            periods (not the ulReload value which accounted for part
            ticks). */
            ulCompleteTickPeriods = (GPT_GetCurrentTimerCount(pxGptBase)) / ulLPTimerCountsForOneTick;
        }

        /* Stop GPT when CPU waked up then set SysTick->LOAD back to its standard
        value.  The critical section is used to ensure the tick interrupt
        can only execute once in the case that the reload register is near
        zero. */
        GPT_StopTimer(pxGptBase);
        portENTER_CRITICAL();
        {
            SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
            vTaskStepTick(ulCompleteTickPeriods);
            SysTick->LOAD = ulTimerCountsForOneTick - 1UL;
        }
        portEXIT_CRITICAL();
    }
}

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void vPortSetupTimerInterrupt(void)
{
    /* Calculate the constants required to configure the tick interrupt. */
    ulTimerCountsForOneTick   = (configCPU_CLOCK_HZ / configTICK_RATE_HZ);
    ulLPTimerCountsForOneTick = (configGPT_CLOCK_HZ / configTICK_RATE_HZ);
    if (ulLPTimerCountsForOneTick != 0)
    {
        xMaximumPossibleSuppressedTicks = portMAX_32_BIT_NUMBER / ulLPTimerCountsForOneTick;
    }
    else
    {
        /* ulLPTimerCountsForOneTick is zero, not allowed state */
        while (1)
            ;
    }
    NVIC_EnableIRQ(vPortGetGptIrqn());

    /* Configure SysTick to interrupt at the requested rate. */
    SysTick->LOAD = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
    SysTick->VAL  = 0UL;
    SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
}
#endif /* #if configUSE_TICKLESS_IDLE */
