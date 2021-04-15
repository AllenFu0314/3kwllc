#include "uart_debug.h"

extern UART_HandleTypeDef UartHandle;
static int16_t Debug_UART_Command_Comm(uint8_t UART_Cmd);
static int16_t Debug_UART_Command_Special( short UART_Cmd);
extern uint8_t u8ResetEnabled;
extern uint8_t open_llc;
uint8_t UART_Cmd=0;
uint8_t UART_DEBUG_START = 0;
uint8_t CMD_Start[] = {'Y' , 'Y' , 'Y' , 'Y' , 'Y'};
uint8_t nextCmd = 0;

UART_TX_MESSAGE uart_tx_message;

void Debug_UART()
{
    if(__HAL_USART_GET_FLAG(&UartHandle, USART_FLAG_ORE))
    {
        __HAL_USART_CLEAR_FLAG(&UartHandle,USART_CLEAR_OREF);
    }
    if(__HAL_USART_GET_FLAG(&UartHandle, USART_FLAG_RXNE))  //RX ready
    {
        UART_Cmd=(uint8_t) READ_REG((&UartHandle)->Instance->RDR);


        if(UART_DEBUG_START)
        {
            if (Debug_UART_Command_Comm(UART_Cmd) != 1)
            {
                if (Debug_UART_Command_Special(UART_Cmd) != 1)
                {
                }
            }
            if((UART_Cmd!='r') && (UART_Cmd!='R'))
            {
                // printf("UART_Cmd=%c\n",UART_Cmd);
            }
        }else
        {
            if(UART_Cmd == CMD_Start[nextCmd])
            {
                nextCmd++;
            }else
            {
                nextCmd = 0;
            }
            
            if(nextCmd == 5)
            {
                nextCmd = 0;
                UART_DEBUG_START = 1;
                printf("ack\n");
            }
            
        }
    }
}

int16_t Debug_UART_Command_Comm(uint8_t UART_Cmd)
{
    if(UART_Cmd=='f')
    {
        printf("");
        return 1;
    }
    else if(UART_Cmd=='s')
    {
        printf("openllc\n");
        open_llc = 1;
        return 1;
    }else if(UART_Cmd=='p')
    {
        printf("closellc\n");
        open_llc = 0;
        return 1;
    }else if(UART_Cmd=='q') //show system information
    {
        printf("Vout == %.2f\n",uart_tx_message.Vout );
        printf("Ia == %.2f\n" , uart_tx_message.I);
        printf("Sys == %d\n" , uart_tx_message.SysState);
        printf("Freq == %d\n" , uart_tx_message.llc_freq);
        printf("OV == %d\n" , uart_tx_message.OV);
        printf("OC == %d\n" , uart_tx_message.OC);
        printf("COMPF == %d\n" , uart_tx_message.CompF);
        return 1;
    }

    return 0;
}

int16_t Debug_UART_Command_Special( short UART_Cmd)
{
    if(UART_Cmd=='v')
    {
        printf("the SoftVersion == %s\n", VERSION_SOFT);
        return 1;
    }
    else if(UART_Cmd=='e')
    {
        printf("reset\n");
        u8ResetEnabled = 1;
        return 1;
    }

    if(UART_Cmd=='c')
    {
        printf("restart\n");
        UART_DEBUG_START = 0;
        nextCmd = 0;
        return 1;
    }
    if(UART_Cmd=='R')
    {

        return 1;
    }
    return 0;
}

void Sys_Restart(void)
{
    if (u8ResetEnabled != 1)
    {
//        WDT_WALLOW(WDT1);
//        WDT_FeedDog(WDT1);
    }
    else
    {
        printf(".");
    }
    /*
    			if (resetval.u8_RestartFlag == 1)
    			{
    				resetval.u32_RestartCountMs += 100;
    				if (resetval.u32_RestartCountMs > 300000)
    				{
    					resetval.u8_RestartFlag = 0;
    					resetval.u32_RestartCountMs = 0;
    				}
    			}
    			else
    			{
    				resetval.u32_RestartCountMs = 0;
    			}
    */
}
