#ifndef USART_H
#define USART_H

#define FOSC_ 4000000UL	//Kristal hizi

#define RX_PIN TRISC7	//Seri port pinleri
#define TX_PIN TRISC6

//#define DIVIDER       ((int)(FOSC/(16UL * BAUDRATE) -1));

void          usart_init        (unsigned int BAUDRATE);
void          putch             (unsigned char byte);
unsigned char getch             (void);
void          usart_isr_init    (void);


#endif /* USART_H */