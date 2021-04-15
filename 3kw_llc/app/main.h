
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "uart_debug.h"
#include "globaldefine.h"
/* Exported types ------------------------------------------------------------*/
typedef volatile enum { IDLE,RUN, FAULT} DemoModeTypeDef;

/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_6
#define USARTx_TX_GPIO_PORT              GPIOB
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_7
#define USARTx_RX_GPIO_PORT              GPIOB
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler


/**************************Global Def*******************************/


/**************************Struct Def*******************************/

typedef struct
{
    uint32_t PWMfreq;
    uint32_t PWMPeriod;
    uint16_t Freq_max;
    uint16_t Freq_min;
    volatile uint8_t flag_ssOver;
    volatile  uint16_t Freq_ss;
    uint16_t Freq_100k;
    uint16_t Openloop_ssValue;
    volatile  uint16_t Freq;
    float Vref;
    float Vref_cmd;
    float Vdiff;
    float Vloop_last;
    volatile float Vloop;
    float Vdiff_last;
    float Duty1;
    float Vout;
    float Vout_last;
    float Vout_last2;
    float Duty1_last;

    float B0;
    float B1;
    float Ia;
    float Ib;
    float IPRI1;
    float IPRI2;
} LLC_CAL;


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

static void GPIO_LLC_HB_outputs_Config(void);
static void HRTIM_Config(void);
static void ADC_Config(void);
static void COMP2_Config(void);
static void DAC1_CH2_Config(void);
static void UART_Config(void);

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------- */

extern DemoModeTypeDef DemoState;
extern HRTIM_HandleTypeDef hhrtim;
extern void LLC_Cal(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
