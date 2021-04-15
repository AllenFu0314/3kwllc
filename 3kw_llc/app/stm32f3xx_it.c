
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_it.h"
#include "stdio.h"
#include "string.h"

extern LLC_CAL llc_cal;
extern ADC_HandleTypeDef AdcHandle1;
extern COMP_HandleTypeDef   Comp2Handle;
extern uint32_t Vout;
extern uint32_t Ia;
extern uint8_t open_llc;
//extern uint8_t fg_OV ;
//extern uint8_t fg_OC;
volatile uint8_t nothing;
uint16_t debounce_fault;
uint16_t IdleToRun;
uint16_t RunToIdle;
uint8_t SyncRefc_flag = 0;
uint8_t llc_flag;

/** @addtogroup STM32F3xx_HAL_Examples
  * @{
  */

/** @addtogroup HRTIM/HRTIM_LLC_HalfBridge
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}


void HRTIM1_Master_IRQHandler(void)
{
    //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
    /*
        Get AdcValue of Vout(Vbus) and Ia
    */
    Vout = HAL_ADCEx_InjectedGetValue(&AdcHandle1, ADC_INJECTED_RANK_1);
    Ia =  HAL_ADCEx_InjectedGetValue(&AdcHandle1, ADC_INJECTED_RANK_2);
    llc_cal.Vout = Vout *3.3f/4095;
    llc_cal.Ia = Ia*3.3f/4095;
    static uint16_t CurrentPeriod;
    /*clear int event*/
    __HAL_HRTIM_MASTER_CLEAR_IT(&hhrtim, HRTIM_MASTER_IT_MREP);

    switch(DemoState)
    {
    case IDLE:
        HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                     HRTIM_OUTPUT_TA1
                                     | HRTIM_OUTPUT_TA2
                                     | HRTIM_OUTPUT_TB1
                                     | HRTIM_OUTPUT_TB2);
#if PGOODS == 1
#if UART_START_ENABLE == 1
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)&&open_llc)
#else
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))
#endif
        {
            if(++IdleToRun >= 1500) {
                DemoState = RUN;
                IdleToRun = 0;
            }

        } else {
            IdleToRun = 0;
        }
#else
        DemoState = RUN;
#endif
        break;

    case RUN:
#if PGOODS == 1
#if UART_START_ENABLE == 1

        if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)||!open_llc)
#else
        if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))
#endif
        {
            if(++RunToIdle >= 10 ) {

                HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                             HRTIM_OUTPUT_TA1
                                             | HRTIM_OUTPUT_TA2
                                             | HRTIM_OUTPUT_TB1
                                             | HRTIM_OUTPUT_TB2);

                DemoState = IDLE;
                RunToIdle =0;
                memset((uint16_t*)&llc_cal, 0, sizeof(llc_cal));
                llc_cal.Vref =LLC_VOUT_REF;
                llc_cal.Vref_cmd = LLC_VOUT_CMD;
                llc_cal.flag_ssOver = 0;
                llc_cal.B0 = 0.5;
                llc_cal.B1 = -0.485;
                llc_cal.Freq = LLC_PWM_MAX_FREQ;
                llc_cal.Openloop_ssValue = LLC_PWM_MAX_FREQ;
                __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_MASTER, llc_cal.Freq);
                CurrentPeriod = llc_cal.Freq;
                /* Adjust Turn-on and turn-off time for SR1 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, SYNC_DT_RISING+SYNC_REFC_ONTIME);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_2, ((CurrentPeriod>>1)-SYNC_REFC_OFFTIME));

                /* Adjust Turn-on and turn-off time for SR2 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_3, ((CurrentPeriod>>1)+SYNC_DT_RISING+SYNC_REFC_ONTIME));
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_REFC_OFFTIME));

                /* Adjust new sampling points for SR1 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3,  20);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_4,  850 + ADC_CONVERSION_TIME);

                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2,  CurrentPeriod>>2);
                __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_B,  ((CurrentPeriod>>1)-SYNC_DT_RISING) + ADC_CONVERSION_TIME);

                /* Adjust new sampling points for SR2 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_2,  ((CurrentPeriod>>1)+850) - ADC_CONVERSION_TIME);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3,  ((CurrentPeriod>>1)+850) + ADC_CONVERSION_TIME);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_DT_RISING) - ADC_CONVERSION_TIME);
                __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, (CurrentPeriod-SYNC_DT_RISING) + ADC_CONVERSION_TIME);

                break;
            }
        } else {
            RunToIdle = 0;
        }
#endif
#if DEBUG ==1
        CurrentPeriod = __HAL_HRTIM_GETPERIOD(&hhrtim, HRTIM_TIMERINDEX_MASTER);
        /* Decrease the LLC period down to half the init value */
        if (CurrentPeriod < (LLC_PWM_MAX_FREQ*4))
        {
            CurrentPeriod++;
        }
        else  /* Re-start ramp from initial period */
        {
            /* Update duty cycle (CMP1) and ADC triggering point (CMP2)*/
            CurrentPeriod = LLC_PWM_MAX_FREQ;
        }
        /* Set new LLC frequency */
        __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_MASTER, CurrentPeriod);

        /* Adjust Turn-on and turn-off time for SR1 */
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, (15 * CurrentPeriod)/100);
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_2, (35 * CurrentPeriod)/100);

        /* Adjust Turn-on and turn-off time for SR2 */
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_3, (65 * CurrentPeriod)/100);
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_4, (85 * CurrentPeriod)/100);

        /* Adjust new sampling points for SR1 */
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3, (15 * CurrentPeriod)/100 - ADC_CONVERSION_TIME);
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_4, (15 * CurrentPeriod)/100 + ADC_CONVERSION_TIME);
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2, (35 * CurrentPeriod)/100 - ADC_CONVERSION_TIME);
        __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, (35 * CurrentPeriod)/100 + ADC_CONVERSION_TIME);

        /* Adjust new sampling points for SR2 */
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_2, (65 * CurrentPeriod)/100 - ADC_CONVERSION_TIME);
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3, (65 * CurrentPeriod)/100 + ADC_CONVERSION_TIME);
        __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_4, (85 * CurrentPeriod)/100 - ADC_CONVERSION_TIME);
        __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, (85 * CurrentPeriod)/100 + ADC_CONVERSION_TIME);



#else

#if OPEN_LOOP_SS ==1
        if(llc_cal.Openloop_ssValue >= LLC_PWM_MINMUM_FREQ) {
            llc_cal.flag_ssOver = 1;
        } else
        {
            llc_cal.Openloop_ssValue+=5;
            llc_cal.flag_ssOver = 0;
        }

#else
        if(llc_cal.Vref>=llc_cal.Vref_cmd)
        {
            llc_cal.flag_ssOver = 1;
        } else
        {
            llc_cal.Vref+=0.004f;
            llc_cal.flag_ssOver =0;
        }
#endif
        //llc_cal.Vref = (llc_cal.Vref>=llc_cal.Vref_cmd)?llc_cal.Vref_cmd:(llc_cal.Vref+0.002f);


        LLC_Cal();

        if(llc_cal.Vloop<1.3f)
        {
            llc_cal.Freq = LLC_PWM_MAX_FREQ;
        } else
        {
            llc_cal.Freq = LLC_PWM_MAX_FREQ+((llc_cal.Vloop-1.3f)*(LLC_PWM_MINMUM_FREQ-LLC_PWM_MAX_FREQ))/2.7f;
        }


#if OPEN_LOOP_SS ==1
        llc_cal.Freq = llc_cal.Freq >llc_cal.Openloop_ssValue ? llc_cal.Openloop_ssValue : llc_cal.Freq;
#endif
        CurrentPeriod = llc_cal.Freq;

        /* Set new LLC frequency */
        __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_MASTER, llc_cal.Freq);
        /*
        小于300k 关同步整流pwm 大于330k 开同步整流
        //300k = 3.333us => 13661
        //330k = 3.03us => 12418
        */
//     if(llc_cal.Freq < 12418){
//         SyncRefc_flag = 0;
//     }else if(llc_cal.Freq > 13661){
//         SyncRefc_flag = 1;
//     }
        /*not used current*/
        SyncRefc_flag = 1;

        if(llc_cal.Freq <RESONANT_FREQ)
        {
            /* Adjust Turn-on and turn-off time for SR1 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, SYNC_DT_RISING+SYNC_REFC_ONTIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_2, ((CurrentPeriod>>1)-SYNC_REFC_OFFTIME));

            /* Adjust Turn-on and turn-off time for SR2 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_3, ((CurrentPeriod>>1)+SYNC_DT_RISING+SYNC_REFC_ONTIME));
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_REFC_OFFTIME));

            /* Adjust new sampling points for SR1 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3,  20);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_4,  850 + ADC_CONVERSION_TIME);
            // __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2,  ((CurrentPeriod>>1)-410) - ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2,  CurrentPeriod>>2);
            //__HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_B,  ((CurrentPeriod>>1)-410) + ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_B,  ((CurrentPeriod>>1)-SYNC_DT_RISING) + ADC_CONVERSION_TIME);

            /* Adjust new sampling points for SR2 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_2,  ((CurrentPeriod>>1)+850) - ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3,  ((CurrentPeriod>>1)+850) + ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_DT_RISING) - ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, (CurrentPeriod-SYNC_DT_RISING) + ADC_CONVERSION_TIME);
        } else
        {
            //6967/2 =
            /* Adjust Turn-on and turn-off time for SR1 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, SYNC_DT_RISING+SYNC_REFC_ONTIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_2, HALF_RESONANT_FREQ-SYNC_REFC_OFFTIME);

            /* Adjust Turn-on and turn-off time for SR2 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_3, ((CurrentPeriod>>1)+SYNC_DT_RISING+SYNC_REFC_ONTIME));
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_4, ((CurrentPeriod>>1)+HALF_RESONANT_FREQ-SYNC_REFC_OFFTIME));

            /* Adjust new sampling points for SR1 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3,  20);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_4,  850 + ADC_CONVERSION_TIME);
            //__HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2,  ((CurrentPeriod>>1)-410) - ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2,  CurrentPeriod>>2);
            __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_B,  ((CurrentPeriod>>1)-SYNC_DT_RISING) + ADC_CONVERSION_TIME);

            /* Adjust new sampling points for SR2 */
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_2,  ((CurrentPeriod>>1)+850) - ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3,  ((CurrentPeriod>>1)+850) + ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_DT_RISING) - ADC_CONVERSION_TIME);
            __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, (CurrentPeriod-SYNC_DT_RISING) + ADC_CONVERSION_TIME);
        }


        /*SoftStart over , if PI result < 1.25 && LLC Vbus>limit into Skip cycle mode*/
        if(llc_cal.flag_ssOver)
        {
            if((llc_cal.Vloop<1.25f)&&(llc_cal.Vout>=2.4f))
            {
                HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                             HRTIM_OUTPUT_TA1
                                             | HRTIM_OUTPUT_TA2
                                             | HRTIM_OUTPUT_TB1
                                             | HRTIM_OUTPUT_TB2);
            } else if(llc_cal.Vloop>1.3f)
            {
                HAL_HRTIM_WaveformOutputStart(&hhrtim,
                                              HRTIM_OUTPUT_TA1
                                              | HRTIM_OUTPUT_TA2);
                if(SyncRefc_flag == 1) {
                    HAL_HRTIM_WaveformOutputStart(&hhrtim,
                                                  HRTIM_OUTPUT_TB1
                                                  | HRTIM_OUTPUT_TB2);
                } else {
                    HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                                 HRTIM_OUTPUT_TB1
                                                 | HRTIM_OUTPUT_TB2);
                }

            }
        }

        if(!llc_cal.flag_ssOver)
        {
            HAL_HRTIM_WaveformOutputStart(&hhrtim,
                                          HRTIM_OUTPUT_TA1
                                          | HRTIM_OUTPUT_TA2);
            if(SyncRefc_flag == 1) {
                HAL_HRTIM_WaveformOutputStart(&hhrtim,
                                              HRTIM_OUTPUT_TB1
                                              | HRTIM_OUTPUT_TB2);
            } else {
                HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                             HRTIM_OUTPUT_TB1
                                             | HRTIM_OUTPUT_TB2);
            }
            nothing =0;
        }

#endif
        break;

    case FAULT:
        nothing =1;

        HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                     HRTIM_OUTPUT_TA1
                                     | HRTIM_OUTPUT_TA2
                                     | HRTIM_OUTPUT_TB1
                                     | HRTIM_OUTPUT_TB2);
#if FAULT_AUTO_RECOVER == 1

        if(!HAL_COMP_GetOutputLevel(&Comp2Handle)&&!fg_OC&&!fg_OV)
        {
            if(++debounce_fault >=10000)
            {
                memset((uint16_t*)&llc_cal, 0, sizeof(llc_cal));
                llc_cal.Vref =LLC_VOUT_REF;
                llc_cal.Vref_cmd = LLC_VOUT_CMD;
                llc_cal.flag_ssOver = 0;
                llc_cal.B0 = 0.5;
                llc_cal.B1 = -0.485;
                llc_cal.Freq = LLC_PWM_MAX_FREQ;
                llc_cal.Openloop_ssValue = LLC_PWM_MAX_FREQ;
                __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_MASTER, llc_cal.Freq);
                CurrentPeriod = llc_cal.Freq;
                /* Adjust Turn-on and turn-off time for SR1 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, SYNC_DT_RISING+SYNC_REFC_ONTIME);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_2, ((CurrentPeriod>>1)-SYNC_REFC_OFFTIME));

                /* Adjust Turn-on and turn-off time for SR2 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_3, ((CurrentPeriod>>1)+SYNC_DT_RISING+SYNC_REFC_ONTIME));
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_REFC_OFFTIME));

                /* Adjust new sampling points for SR1 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3,  20);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_4,  850 + ADC_CONVERSION_TIME);

                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_MASTER, HRTIM_COMPAREUNIT_2,  CurrentPeriod>>2);
                __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_B,  ((CurrentPeriod>>1)-SYNC_DT_RISING) + ADC_CONVERSION_TIME);

                /* Adjust new sampling points for SR2 */
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_2,  ((CurrentPeriod>>1)+850) - ADC_CONVERSION_TIME);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3,  ((CurrentPeriod>>1)+850) + ADC_CONVERSION_TIME);
                __HAL_HRTIM_SETCOMPARE(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_4, (CurrentPeriod-SYNC_DT_RISING) - ADC_CONVERSION_TIME);
                __HAL_HRTIM_SETPERIOD(&hhrtim, HRTIM_TIMERINDEX_TIMER_C, (CurrentPeriod-SYNC_DT_RISING) + ADC_CONVERSION_TIME);
                __HAL_HRTIM_CLEAR_IT(&hhrtim, HRTIM_IT_FLT1);
                __HAL_HRTIM_TIMER_CLEAR_FLAG(&hhrtim, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_DLYPRT);
                DemoState = IDLE;
                debounce_fault = 0;
    
            }
        } else
        {
            debounce_fault = 0;
        }

#endif

        break;
    default:
        /* Something wrong happened, shut-down all switches */
        HAL_HRTIM_WaveformOutputStop(&hhrtim,
                                     HRTIM_OUTPUT_TA1
                                     | HRTIM_OUTPUT_TA2
                                     | HRTIM_OUTPUT_TB1
                                     | HRTIM_OUTPUT_TB2);
        break;


    }
}



