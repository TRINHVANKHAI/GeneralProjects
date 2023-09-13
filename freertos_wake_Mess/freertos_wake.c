/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "fsl_rdc.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#if defined(FSL_FEATURE_SOC_PORT_COUNT) && (FSL_FEATURE_SOC_PORT_COUNT > 0)
#include "fsl_port.h"
#endif
#include "fsl_uart.h"
#if configUSE_TICKLESS_IDLE == 2
#include "fsl_gpt.h"
#endif
#include "fsl_gpc.h"
#include "rdc_dbg.h"
#include "fsl_mu.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
/* clang-format off */
#define tickless_task_PRIORITY   ( configMAX_PRIORITIES - 2 )
#define SW_task_PRIORITY   ( configMAX_PRIORITIES - 1 )
#define TIME_DELAY_SLEEP      5000

/* Interrupt priorities. */
#define BOARD_SW_NAME "GPIO3_IO19"
#define BOARD_SW_GPIO GPIO3
#define BOARD_SW_GPIO_PIN 19U
#define BOARD_SW_IRQ  GPIO3_Combined_16_31_IRQn
#define SW_NVIC_PRIO 2



/* clang-format on */
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void vPortGptIsr(void);
IRQn_Type vPortGetGptIrqn(void);
GPT_Type *vPortGetGptBase(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static void Tickless_task(void *pvParameters);
static void SW_task(void *pvParameters);

SemaphoreHandle_t xSWSemaphore = NULL;

volatile uint8_t external_wake = 0;


/*******************************************************************************
 * Code
 ******************************************************************************/

#if configUSE_TICKLESS_IDLE == 2
/*!
 * @brief Interrupt service fuction of GPT timer.
 *
 * This function to call low power timer ISR
 */
void GPT1_IRQHandler(void)
{
    vPortGptIsr();
}

/*!
 * @brief Fuction of GPT timer.
 *
 * This function to return GPT timer base address
 */

GPT_Type *vPortGetGptBase(void)
{
    return GPT1;
}

/*!
 * @brief Fuction of GPT timer.
 *
 * This function to return GPT timer interrupt number
 */

IRQn_Type vPortGetGptIrqn(void)
{
    return GPT1_IRQn;
}
#endif

#define APP_PowerUpSlot (5U)
#define APP_PowerDnSlot (6U)

#define RDC_DISABLE_A53_ACCESS 0xFC
void BOARD_PeripheralRdcSetting(void)
{
    rdc_domain_assignment_t assignment = {0};
    rdc_periph_access_config_t periphConfig;

    assignment.domainId = BOARD_DOMAIN_ID;

    /* Only configure the RDC if the RDC peripheral write access is allowed. */
    if ((0x1U & RDC_GetPeriphAccessPolicy(RDC, kRDC_Periph_RDC, assignment.domainId)) != 0U)
    {
        RDC_SetMasterDomainAssignment(RDC, kRDC_Master_SDMA3_PERIPH, &assignment);
        RDC_SetMasterDomainAssignment(RDC, kRDC_Master_SDMA3_BURST, &assignment);
        RDC_SetMasterDomainAssignment(RDC, kRDC_Master_SDMA3_SPBA2, &assignment);

        RDC_GetDefaultPeriphAccessConfig(&periphConfig);
        /* Do not allow the A53 domain(domain0) to access the following peripherals */
        periphConfig.policy = RDC_DISABLE_A53_ACCESS;
        periphConfig.periph = kRDC_Periph_SAI3;
        RDC_SetPeriphAccessConfig(RDC, &periphConfig);
        periphConfig.periph = kRDC_Periph_UART4;
        RDC_SetPeriphAccessConfig(RDC, &periphConfig);
        periphConfig.periph = kRDC_Periph_GPT1;
        RDC_SetPeriphAccessConfig(RDC, &periphConfig);
        periphConfig.periph = kRDC_Periph_SDMA3;
        RDC_SetPeriphAccessConfig(RDC, &periphConfig);
        periphConfig.periph = kRDC_Periph_I2C3;
        RDC_SetPeriphAccessConfig(RDC, &periphConfig);
        periphConfig.periph = kRDC_Periph_PDM;
        RDC_SetPeriphAccessConfig(RDC, &periphConfig);
    }
}

#if 0
void Reset_Handler(void) {
    while(1);
}
void NMI_Handler(void) {
    while(1);
}
void HardFault_Handler(void) {
    while(1);
}
void MemManage_Handler(void) {
    while(1);
}
void BusFault_Handler(void) {
    while(1);
}
void UsageFault_Handler(void) {
    while(1);
}
#endif

/*!
 * @brief Main function
 */
 volatile char cmd=0;
 #define KEY_CHAR_SPACE 0x20
int main(void)
{
    int i;

/* Define the init structure for the input switch pin */
#ifdef BOARD_SW_NAME
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput,
        0,
#if defined(FSL_FEATURE_SOC_IGPIO_COUNT) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0)
        kGPIO_IntRisingEdge,
#endif
    };
#endif
#if configUSE_TICKLESS_IDLE == 2
    gpt_config_t gptConfig;
#endif

    /* M7 has its local cache and enabled by default,
     * need to set smart subsystems (0x28000000 ~ 0x3FFFFFFF)
     * non-cacheable before accessing this address region */
    BOARD_InitMemory();

    /* Board specific RDC settings */
    BOARD_RdcInit();
    BOARD_PeripheralRdcSetting();
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
#if configUSE_TICKLESS_IDLE == 2
    //CLOCK_SetRootMux(kCLOCK_RootGpt1, kCLOCK_GptRootmuxSysPll1Div20); /* Set GPT1 source to SysPLL1 Div20 40MHZ */
    //CLOCK_SetRootDivider(kCLOCK_RootGpt1, 1U, 5U);                    /* Set root clock to 40MHZ / 5 = 8MHZ */
    
    CLOCK_SetRootMux(kCLOCK_RootGpt1, kCLOCK_GptRootmuxOsc24M);      /* Set GPT1 source to Osc24 MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootGpt1, 1U, 1U);			/* Set root clock to 24MHZ / 3 = 8MHZ */
    GPT_GetDefaultConfig(&gptConfig);

    /* Initialize GPT module */
    GPT_Init(GPT1, &gptConfig);

    /* Divide GPT clock source frequency by 1 inside GPT module */
    GPT_SetClockDivider(GPT1, 1);

    /* Enable GPT Output Compare1 interrupt */
    GPT_EnableInterrupts(GPT1, kGPT_OutputCompare1InterruptEnable);

    /* Enable at the Interrupt */
    EnableIRQ(GPT1_IRQn);
#endif


    /*
     * In order to wakeup M7 from LPM, all PLLCTRLs need to be set to "NeededRun"
     */
    for (i = 0; i != 39; i++)
    {
        CCM->PLL_CTRL[i].PLL_CTRL = kCLOCK_ClockNeededRun;
    }


    GPC_Init(BOARD_GPC_BASEADDR, APP_PowerUpSlot, APP_PowerDnSlot);
    //GPC_EnableIRQ(BOARD_GPC_BASEADDR, BOARD_MU_IRQ_NUM);
    GPC_EnableIRQ(BOARD_GPC_BASEADDR, GPT1_IRQn);
    MU_Init(MUB);

    PRINTF("Press any key to start the example\r\n");
    cmd = GETCHAR();
    PRINTF("CMD=%x\r\n", cmd);
    //RDC_DBG_PrintAllMasterAccessPolicy();
    //RDC_DBG_PrintAllMemAccessPolicy();
    RDC_DBG_PrintAllPeriphAccessPolicy();
    /* Print a note to terminal. */
    PRINTF("Tickless Demo example \r\n");
    PRINTF("Build Time: %s--%s \r\n", __DATE__, __TIME__);
#ifdef BOARD_SW_NAME
    PRINTF("Press or turn on %s to wake up the CPU\r\n", BOARD_SW_NAME);

/* Init input switch GPIO. */
#if defined(FSL_FEATURE_SOC_PORT_COUNT) && (FSL_FEATURE_SOC_PORT_COUNT > 0)
#if (defined(FSL_FEATURE_PORT_HAS_NO_INTERRUPT) && FSL_FEATURE_PORT_HAS_NO_INTERRUPT)
    GPIO_SetPinInterruptConfig(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, kGPIO_InterruptFallingEdge);
#else
    PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN, kPORT_InterruptFallingEdge);
#endif
#endif

#if defined(__CORTEX_M)
    NVIC_SetPriority(BOARD_SW_IRQ, SW_NVIC_PRIO);
#else
    GIC_SetPriority(BOARD_SW_IRQ, BOARD_SW_GIC_PRIO);
#endif

    EnableIRQ(BOARD_SW_IRQ);
    GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);
#if defined(FSL_FEATURE_SOC_IGPIO_COUNT) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0)
    GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
    GPIO_PortEnableInterrupts(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#endif
#endif

    /*Create tickless task*/
    if (xTaskCreate(Tickless_task, "Tickless_task", configMINIMAL_STACK_SIZE + 100, NULL, tickless_task_PRIORITY,
                    NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(SW_task, "Switch_task", configMINIMAL_STACK_SIZE + 100, NULL, SW_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    
    GPC_EnableIRQ(BOARD_GPC_BASEADDR, BOARD_SW_IRQ);
    PRINTF("\r\nTick count :\r\n");
    /*Task Scheduler*/
    vTaskStartScheduler();
    for (;;)
        ;
}

/* Tickless Task */
static void Tickless_task(void *pvParameters)
{
    for (;;)
    {
        if (external_wake==1) {
        	PRINTF("Wake A core .. ");
        	if (cmd == KEY_CHAR_SPACE) {
			if (kStatus_Success != MU_TriggerInterrupts(MUB, kMU_GenInt0InterruptTrigger)) {
			    PRINTF("previous signal has not been processed\r\n");
			} else {
			    PRINTF("sent GIR OK!\r\n");
			}
        	} else {
        	    MU_SendMsg ( MUB, 1, cmd );
		    PRINTF("msg sent!\r\n");
        	}
        	external_wake = 0;
        }
        
        PRINTF("%d\r\n", xTaskGetTickCount());
        vTaskDelay(TIME_DELAY_SLEEP);
    }
}

/* Switch Task */
static void SW_task(void *pvParameters)
{
    xSWSemaphore = xSemaphoreCreateBinary();
    for (;;)
    {
        if (xSemaphoreTake(xSWSemaphore, portMAX_DELAY) == pdTRUE)
        {
            PRINTF("CPU woken up by external interrupt\r\n");
        }
    }
}
/*!
 * @brief Interrupt service fuction of switch.
 *
 * This function to wake up CPU
 */
#ifdef BOARD_SW_NAME

void GPIO3_Combined_16_31_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

/* Clear external interrupt flag. */
#ifdef BOARD_SW_DELAY
    volatile uint32_t i = 0;
    for (i = 0; i < 10000000; ++i)
    {
        __NOP(); /* delay */
    }
#if (defined(FSL_FEATURE_PORT_HAS_NO_INTERRUPT) && FSL_FEATURE_PORT_HAS_NO_INTERRUPT)
    /* Clear external interrupt flag. */
    GPIO_GpioClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#else
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#endif
    if (1 == GPIO_PinRead(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN))
    {
        xSemaphoreGiveFromISR(xSWSemaphore, &xHigherPriorityTaskWoken);
    }
#else
#if (defined(FSL_FEATURE_PORT_HAS_NO_INTERRUPT) && FSL_FEATURE_PORT_HAS_NO_INTERRUPT)
    /* Clear external interrupt flag. */
    GPIO_GpioClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#else
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#endif
    //xSemaphoreGiveFromISR(xSWSemaphore, &xHigherPriorityTaskWoken);
    external_wake = 1;
    PRINTF("ISR: CPU woken up by external interrupt\r\n");
#endif
}
#endif
