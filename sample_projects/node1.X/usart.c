#include <xc.h>
#include "usart.h"

void usart_init(unsigned int BAUDRATE)
{
    unsigned long int DIVIDER;
    DIVIDER = ((int) (FOSC_ / (16UL * BAUDRATE) - 1));
    if (DIVIDER > 255)
    {
        BAUDCON1bits.BRG16 = 1; // 16-bit baud rate generator
        SPBRG = DIVIDER; // Hýz deðeri SPBRG'ye yükleniyor
        SPBRGH1 = DIVIDER >> 8;
    }
    else
    {
        BAUDCON1bits.BRG16 = 0; // 8-bit baud rate generator
        SPBRG = DIVIDER; // Hýz deðeri SPBRG'ye yükleniyor   
    }

    RX_PIN = 1; // Seri iletiþim pinleri giriþ olarak tanýmlanýyor
    TX_PIN = 0;

    // Gönderim kaydedicisi ayarlanýyor
    TXSTAbits.TX9 = 0; // 8-bit transmission
    TXSTAbits.TXEN = 1; // Transmit enabled
    TXSTAbits.SYNC = 0; // Asynchronous mode
    TXSTAbits.BRGH = 1; // High speed
    // Alim kaydedicisi ayarlaniyor
    RCSTAbits.RX9 = 0; // 8-bit reception
    RCSTAbits.SPEN = 1; // enable serial port (configures RX/DT and TX/CK pins as serial port pins)
    RCSTAbits.CREN = 1; // enable receiver

}

void putch(unsigned char byte) //printf fonksiyonu, putch'a byte byte veri basar
{
    // Tek byte gönder
    while (!TXIF); // Transfer tamponu boþ mu
    TXREG = byte; // Tek byte gönderiliyor
}

unsigned char getch(void)
{
    // Tek byte al
    while (!RCIF); // Alým tamponu boþ mu
    return RCREG; // Tek byte alýnýyor
}

void usart_isr_init()
{
    //  USART interrupts configuration
    RCONbits.IPEN = 0; // disable interrupt priority
    INTCONbits.GIE = 1; // enable interrupts
    INTCONbits.PEIE = 1; // enable peripheral interrupts.
    PIE1bits.RCIE = 1; // enable USART receive interrupt
}
