#ifndef GLOBALDEFINE_H
#define GLOBALDEFINE_H

/***Software Setting**/
#define VERSION_SOFT  "2.1.0"

/*********************************************************************************************/
#define LLC_VOUT_REF  0.01f    /*init Voltage*/
#define LLC_VOUT_CMD  2.4f     /* Vbus Voltage(important) => 2.4*22.5 = 54V */
#define LLC_B0        0.5f      /* PI B0 */
#define LLC_B1        -0.485f;  /* PI B1 */

/*********************************************************************************************/
/**resonant frequency
 谐振频率 240k  最低200k  最高400k     4096M = 244ps   minfreq = 244ps*65536 = 16us = 62.5khz
 450k = 2.2222222us => 2222222ps /244ps = 9107
400K = 2.5US =>10245
300k = 3.333us => 13661
330k = 3.03us => 12418
280k = 3.57us =>14631
200k = 5us= 20490   100k = 10us = 10000000/244 = 40983   240k = 4166666ps/244 = 17077  17077*1.15 = 19638   0.417us
20490-9107 = 11383
1.8us /244 = 7377  8196/0.8 = 10245 *2 = 20500   100ns/244 = 410
1.7us /244 = 6967   16393/2 = 8196 -850-410 = 6936
*/
#define RESONANT_FREQ  14631     //280khz
#define HALF_RESONANT_FREQ  (RESONANT_FREQ/2)  //warn:do not change , only change RESONANT_FREQ
#define LLC_PWM_MINMUM_FREQ ((uint16_t)20490)  /* This defines the minimum LLC operating frequency, here 200kHz */
#define LLC_PWM_MAX_FREQ ((uint16_t)10245)  /* This defines the maxmum LLC operating frequency, here 400kHz */

/*********************************************************************************************/
#define OVP_VALUE (2.7f/3.3f*4095)        //OVP Voltage，only change 2.7f to other voltage . 2.7v=>2.7*22.5 = 60.75
#define OVP_REC_VALUE (2.6f/3.3f*4095)    //OVP recover value ,when voltage < 2.6V 
#define OCP_VALUE (2.2f/3.3f*4095)       //OCP Voltage，only change 2.2f to other voltage . 2.2v=>2.2*40 = 88 A
#define OCP_REC_VALUE (2/3.3f*4095)

/*********************************************************************************************/
/* Deadtime values for buck and boost stages, set to 200ns for both */
/* 230 x 868ps = 200ns */   ///Fdtg = fhrtim * 8 = 144m *8 = 1152m  1/1152 =  868   for 72mhz
/* 200ns/977ps = 205                     */  ///fdtg = 128*8 = 1024m  1/1024 = 977ps
#define DT_RISING       ((uint16_t)103)  //  200ns = 205
#define DT_FALLING      ((uint16_t)103)  //   100ns = 103
#define SYNC_DT_RISING   (DT_RISING*4)   //Warn:do not change, only change DT_RISING and DT_FALLING
#define SYNC_DT_FALLING   (DT_FALLING*4) //Warn:do not change, only change DT_RISING and DT_FALLING

#define SYNC_REFC_OFFTIME  ((uint16_t)40)  //40 = 10ns The time when synchronous rectification is turned off in advance relative to LLC
#define SYNC_REFC_ONTIME   ((uint16_t)40)  //40 = 10ns The time when synchronous rectification is turned on in advance relative to LLC



#define DEBUG           0    //For debug purposes only, do not enable
/**PGOODS
* 0.LLC start work do not wait GPIOA3 is high and openllc=1(from pc).
* 1.LLC only start when GPIOA3 is high and openllc = 1(from pc).
*/
#define PGOODS           1   

/**UART_START_ENABLE
* 0.LLC start work do not wait openllc=1(from pc) if PGOODS enable and GPIOA3 is high.
* 1.LLC only start when openllc=1(from pc) if PGOODS enable and GPIOA3 is high.
*/
#define UART_START_ENABLE        0

/**FAULT_AUTO_RECOVER
*0. LLC do not recover when llc fault is clear(vbus < recoverVoltage && Ia < recoverCurrent)
*1. LLC can recover(FAULT->IDLE) when llc fault is clear(vbus < recoverVoltage && Ia < recoverCurrent)
*/
#define FAULT_AUTO_RECOVER           0

/* This value is used to position the ADC sampling point before and after the
commutation on the synchronous rectification FETs */
/* It corresponds to 280ns (0x480/32) x (1/128MHz), slightly above ADC
conversion time */
#define ADC_CONVERSION_TIME     ((uint16_t)0x480)

#define OPEN_LOOP_SS    0
/* Private typedef -----------------------------------------------------------*/
#define DLL_CALIBRATIONTIMEOUT ((uint32_t)   10)        /* Timeout in ms */

/* Define the interrupt rate vs switching frequency */
#define REPETITION_RATE ((uint32_t)   5)  //6(n+1) PWM cycles generate an interrupt

/* Define the Over-curent blanking window */
/* 0x1000 x 217ps = 888ns */
#define OVER_CURRENT_BLANKING ((uint32_t) 0x1000)



























#endif
