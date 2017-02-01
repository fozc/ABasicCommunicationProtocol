#ifndef USART_H
#define USART_H

//#define BAUD 9600		//Boudrate
#define FOSCA 4000000UL	//Kristal h�z�
#define NINE_MODE 0     //9 bit ileti�im kullan�lacaksa 1 yap�lmal�
#define HIGH_SPEED 1	//H�zl� ileti�im yap�lacaksa 1 yap�lmal�

#define RX_PIN TRISC7	//Seri port pinleri
#define TX_PIN TRISC6

//#define DIVIDER       ((int)(FOSC/(16UL * BAUDRATE) -1));

extern void usart_init(unsigned int BAUDRATE);
extern void putch(unsigned char byte);
extern unsigned char getch(void);
extern void usart_isr_init(void);

#endif /* USART_H */