/*
 * File:   main.c
 * Author: Fatih OZCAN
 *
 * Created on 25 Ocak 2017 Çar?amba, 15:39
 */

#include "system.h"
#include "usart.h"
#include "serial.h"

void interrupt ISR()
{
    char i;
    di();

    //    if(RCSTA1bits.OERR)
    //    {
    //        i = RCREG1;
    //        RCSTA1bits.OERR = 0;
    //        RCSTA1bits.CREN = 0; 
    //        RCSTA1bits.CREN = 1; // enable receiver
    //    }
    //    if(RCSTA1bits.FERR)
    //    {
    //        i = RCREG1;
    //    }
    if (RC1IF)
    {
        i = RCREG1;
        //         TX1REG = i;
        serial_interrupt_handler(i);
    }
    ei();
}

const char a[] = "Birinci cihaz\r\n";

void main(void)
{
    TRISC = 0b10000000;
    ANSELC = 0;
    
    ANSELB = 0;

    TRISB = 0x00;
    LATB0 = 1;
//    __delay_ms(100);
    usart_init(9600U);
    usart_isr_init();

    printf("|---Node_1 Started---|\r\n");



    while (1)
    {
        serial_rx_process();
    }


    return;
}
