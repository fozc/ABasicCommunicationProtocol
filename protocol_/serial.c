/*
 * File:   serial.c
 * Author: Fatih OZCAN
 *
 * Created on 25 Ocak 2017 Çar?amba, 15:43
 */
#include <xc.h>
#include <string.h>
#include "serial.h"
#include "system.h"

#define BUFFER_SIZE 128
#define FRAME_START 0x01  
#define FRAME_END   0x04  
#define FRAME_DLE   0x10  

typedef enum
{
    COM_STATE_IDLE,
    COM_STATE_COLLECT_DATA,
    COM_STATE_FRAME_ESC,
    COM_STATE_GET_BCC,
    COM_STATE_PACKAGE_READY
} com_state_t;

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t index;
    uint8_t bcc;
    com_state_t com_state;
} serial_t;

serial_t serial;

char my_memcmp(const void *Ptr1, const void *Ptr2, uint16_t Count)
{
    char v = 0;
    char *p1 = (char *) Ptr1;
    char *p2 = (char *) Ptr2;

    while (Count-- > 0 && v == 0)
    {
        v = *(p1++) - *(p2++);
    }

    return v;
}

void reset_serial_com(void)
{
    memset(serial.buffer, 0x00, serial.index);
    serial.index = 0;
    serial.bcc = 0;
    serial.com_state = COM_STATE_IDLE;
}

void serial_interrupt_handler(uint8_t data)
{
    switch (serial.com_state)
    {
    case COM_STATE_IDLE:

        if (data == FRAME_START) /* Paket baslangici alindi. Gelen verileri topla */
            serial.com_state = COM_STATE_COLLECT_DATA;
        break;

    case COM_STATE_FRAME_ESC:

        serial.buffer[serial.index++] = data; /* Frame ESC verisinden sonra pakette bulunan gercek data gelir */
        serial.com_state = COM_STATE_COLLECT_DATA; /* Bir sonraki veri icin tekrar ilgili case e git */
        if (serial.index >= BUFFER_SIZE) /* Buffer size dolarsa iletisimi resetle */
        {
            reset_serial_com();
            return;
        }
        break;

    case COM_STATE_COLLECT_DATA:

        if (data == FRAME_DLE) /* Paket icinde baslangic, bitis verisi var !!! */
        {
            serial.com_state = COM_STATE_FRAME_ESC;
            return;
        }
        else if (data == FRAME_END) /* Paket sonu */
        {
            serial.com_state = COM_STATE_PACKAGE_READY;
            return;
        }
        else if (data == FRAME_START) /* Paket baslangici. Bu paket yarida kaldi. Yeni paket alinacak */
        {
            reset_serial_com();
            serial.com_state = COM_STATE_COLLECT_DATA; /* Yeni pakete baslaniyor,*/
            return;
        }
        else
            serial.buffer[serial.index++] = data; /* Paket datasi */


        if (serial.index >= BUFFER_SIZE) /* Buffer size dolarsa iletisimi resetle */
        {
            reset_serial_com();
            return;
        }
        break;

    case COM_STATE_PACKAGE_READY:
        //Paket hazirsa paket islenene veya timeout zamani dolana kadar yeni veri alma.
        break;

    default:
        break;
    }
}

uint8_t check_package_bcc(void)
{
    uint8_t bcc = 0;

    for (uint8_t i = 0; i < serial.index - 1; i++)
    {
        if (serial.buffer[i] == FRAME_START || serial.buffer[i] == FRAME_END || serial.buffer[i] == FRAME_DLE)
            bcc ^= FRAME_DLE;

        bcc ^= serial.buffer[i];
    }

    printf("Hesp. BCC: %x ", bcc);
    serial.bcc = bcc;
    if (bcc != serial.buffer[serial.index - 1])
    {
        printf(" ERR.\r\n");
        return 0;
    }
    printf(" OK.\r\n");
    return 1;
}

void serial_send_data(char data)
{
    while (!TXSTA1bits.TRMT); //TX buffer doluysa bekle
    TX1REG = data;
}

void serial_send_package(const char *buff, uint8_t size)
{
    uint8_t bcc = 0;

    serial_send_data(FRAME_START); // Paket basliyor

    for (uint8_t i = 0; i < size; i++) //Paket verileri
    {
        if (buff[i] == FRAME_START || buff[i] == FRAME_END || buff[i] == FRAME_DLE)
        {
            bcc ^= FRAME_DLE;
            serial_send_data(FRAME_DLE);
        }

        bcc ^= buff[i];
        serial_send_data(buff[i]);
    }

    if (bcc == FRAME_START || bcc == FRAME_END || bcc == FRAME_DLE) /* BCC degeri herhangi bir deger olabilir !*/
    {
        //bcc ^= FRAME_ESC;
        serial_send_data(FRAME_DLE);
    }
    serial_send_data(bcc); //BCC
    serial_send_data(FRAME_END); //Paket sonu
}

uint8_t is_package_ready(void)
{
    if (serial.com_state != COM_STATE_PACKAGE_READY)
        return 0;
    return 1;
}

void serial_rx_process(void)
{
    const char led1_on[] =  "LED1_ON";
    const char led1_off[] = "LED1_OFF";
    const char get_info[] = "GET_INFO";

    if (!is_package_ready()) // Bekleyen hazir paket var mi ?
        return;

    if (!check_package_bcc())
    {
        reset_serial_com();
        return;
    }
    printf("|--------------------------|\r\n");
    printf("Alinan Data: %s BCC: %x\r\n", serial.buffer, serial.buffer[serial.index - 1]);


    if (my_memcmp(serial.buffer, led1_on, sizeof (led1_on) - 1) == 0)
    {
        LED1_ON();
        reset_serial_com();
        return;
    }

    if (my_memcmp(serial.buffer, led1_off, sizeof (led1_off) - 1) == 0)
    {
        LED1_OFF();
        reset_serial_com();
        return;
    }

    if (my_memcmp(serial.buffer, get_info, sizeof (get_info) - 1) == 0)
    {
        send_info();
        reset_serial_com();
        return;
    }

    reset_serial_com();
    
}

void LED1_ON(void)
{
    printf("LED1_ON\r\n");
    LATB1 = 1;
}

void LED1_OFF(void)
{
    printf("LED1_OFF\r\n");
    LATB1 = 0;
}

void send_info(void)
{
    char buff[] = "INFO:Mcu: PIC18F46K22 Compiler: XC8\r\n";

    serial_send_package(buff, sizeof (buff) - 1);
}

 