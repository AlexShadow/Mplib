/**
 *******************************************************************************
 * @file    ADC_Data_Read.c
 * @breaf   The demo functions of ADC for the
 *           TOSHIBA 'TMPM363' Device Series 
 * @version V1.000
 * @date    2010/07/13
 * 
 * THE SOURCE CODE AND ITS RELATED DOCUMENTATION IS PROVIDED "AS IS". TOSHIBA
 * CORPORATION MAKES NO OTHER WARRANTY OF ANY KIND, WHETHER EXPRESS, IMPLIED OR,
 * STATUTORY AND DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF MERCHANTABILITY,
 * SATISFACTORY QUALITY, NON INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * THE SOURCE CODE AND DOCUMENTATION MAY INCLUDE ERRORS. TOSHIBA CORPORATION
 * RESERVES THE RIGHT TO INCORPORATE MODIFICATIONS TO THE SOURCE CODE IN LATER
 * REVISIONS OF IT, AND TO MAKE IMPROVEMENTS OR CHANGES IN THE DOCUMENTATION OR
 * THE PRODUCTS OR TECHNOLOGIES DESCRIBED THEREIN AT ANY TIME.
 * 
 * TOSHIBA CORPORATION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGE OR LIABILITY ARISING FROM YOUR USE OF THE SOURCE CODE OR
 * ANY DOCUMENTATION, INCLUDING BUT NOT LIMITED TO, LOST REVENUES, DATA OR
 * PROFITS, DAMAGES OF ANY SPECIAL, INCIDENTAL OR CONSEQUENTIAL NATURE, PUNITIVE
 * DAMAGES, LOSS OF PROPERTY OR LOSS OF PROFITS ARISING OUT OF OR IN CONNECTION
 * WITH THIS AGREEMENT, OR BEING UNUSABLE, EVEN IF ADVISED OF THE POSSIBILITY OR
 * PROBABILITY OF SUCH DAMAGES AND WHETHER A CLAIM FOR SUCH DAMAGE IS BASED UPON
 * WARRANTY, CONTRACT, TORT, NEGLIGENCE OR OTHERWISE.
 * 
 * (C)Copyright TOSHIBA CORPORATION 2010 All rights reserved
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "tmpm361_adc.h"
#include "tmpm361_uart.h"

char tADC_Display[16];
uint32_t gADC_state = 0U;
ADC_ResultTypeDef gADC_Result;

#define UART UART0

void UART_Configuration(TSB_SC_TypeDef * SCx);
void UART_Print(TSB_SC_TypeDef * UARTx, char *string);
void ADC_Display(uint32_t e_Adc_Value);

void ADC_Data_Read(void)
{
    ADC_SWReset();
    ADC_SetInputChannel(ADC_AN_0);
    ADC_SetRepeatMode(ENABLE);
    ADC_SetVref(ENABLE);
    ADC_Start();
	UART_Configuration(UART);
    for (;;) {
        gADC_state = ADC_GetConvertState();
        if (DONE == gADC_state) {
            gADC_state = BUSY;
            gADC_Result = ADC_GetConvertResult(ADC_REG_08);
			ADC_Display(gADC_Result.ADCResultValue);
        }
    }
}

void ADC_Display(uint32_t e_Adc_Value)
{
    uint32_t e_volumn;

    e_volumn = (e_Adc_Value * 100U) / 0x03ffU;  /* Max ADC is 0x03ff      */

    tADC_Display[0] = 'A';      /* Disaply: "ADC Value:   "     */
    tADC_Display[1] = 'D';
    tADC_Display[2] = 'C';
    tADC_Display[3] = ' ';
    tADC_Display[4] = 'V';
    tADC_Display[5] = 'a';
    tADC_Display[6] = 'l';
    tADC_Display[7] = 'u';
    tADC_Display[8] = 'e';
    tADC_Display[9] = ':';

    /*  Display the value                           */
    if (e_volumn >= 100U) {
        tADC_Display[10] = (char) ((e_volumn / 100U) + 0x30U);
        tADC_Display[11] = (char) (((e_volumn / 10U) % 10U) + 0x30U);
        tADC_Display[12] = (char) ((e_volumn % 10U) + 0x30U);
    } else {
        tADC_Display[10] = ' ';
        if (e_volumn / 10U) {
            tADC_Display[11] = (char) ((e_volumn / 10U) + 0x30U);
        } else {
            tADC_Display[11] = ' ';
        }
        tADC_Display[12] = (char) ((e_volumn % 10U) + 0x30U);
    }
		tADC_Display[13] = '\r';
		tADC_Display[14] = '\n';
    tADC_Display[15] = 0U;
	UART_Print(UART, tADC_Display);

}

void UART_Print(TSB_SC_TypeDef * UARTx, char *string)
{
	  char *p = string;
	  int i;
	  while(*p)
	  {
		    UART_SetTxData(UARTx, (uint32_t)(*p));
		    p++;
		    for(i = 0; i < 1000; i ++);
	  }
}

void UART_Configuration(TSB_SC_TypeDef * SCx)
{
    UART_InitTypeDef myUART;
    if (SCx == UART0) {
        TSB_PE_CR_PE4C = 1;
        TSB_PE_FR2_PE4F2 = 1;
        TSB_PE_FR2_PE5F2 = 1;
        TSB_PE_IE_PE5IE = 1;
    } else {
		//do nothing
	}
    myUART.BaudRate = 115200U;
    myUART.DataBits = UART_DATA_BITS_8;
    myUART.StopBits = UART_STOP_BITS_1;
    myUART.Parity = UART_NO_PARITY;
    myUART.Mode = UART_ENABLE_TX;
    myUART.FlowCtrl = UART_NONE_FLOW_CTRL;

    UART_Enable(SCx);
    UART_Init(SCx, &myUART);
}
