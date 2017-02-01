/*
 * File:   main.c
 * Author: Fatih OZCAN
 *
 * Created on 25 Ocak 2017 Çar?amba, 15:39
 */

#include "system.h"
#include "serial.h"
#include "usart.h"

void interrupt ISR()
{
    if (RC1IF)
    {
        serial_interrupt_handler(RCREG);
    }
}
 
void main(void)
{
    TRISC = 0b10000000;
    ANSELC = 0;
    
    ANSELB = 0;
    TRISB = 0x00;
    LATB0 = 1;
    
    usart_init(9600U);
    usart_isr_init();
 
    
    while (1)
    {
        serial_send_package("GET_INFO", 8);
        __delay_ms(150);__delay_ms(150);__delay_ms(150);
        __delay_ms(150);__delay_ms(150);__delay_ms(150);
        serial_send_package("LED1_ON", 8);
        __delay_ms(150);__delay_ms(150);__delay_ms(150);
        __delay_ms(150);__delay_ms(150);__delay_ms(150);
        serial_send_package("LED1_OFF", 9);
        __delay_ms(150);__delay_ms(150);
        serial_rx_process();
    }


    return;
}
