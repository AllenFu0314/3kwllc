#ifndef UART_DEBUG_H
#define UART_DEBUG_H

#include "stdio.h"
#include "stm32f3xx_hal.h"
#include "main.h"

typedef struct
{
    float Vout;
    float I;
    uint16_t llc_freq;
    uint8_t SysState;
    uint8_t OV;
    uint8_t OC;
    uint8_t CompF;
    
}UART_TX_MESSAGE;

extern void Debug_UART(void);

#endif

