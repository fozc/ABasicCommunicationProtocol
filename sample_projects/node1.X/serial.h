/* 
 * File:   serial.h
 * Author: F.ArGe
 *
 * Created on 25 Ocak 2017 Çar?amba, 15:43
 */

#ifndef SERIAL_H
#define	SERIAL_H

#include <stdint.h>

void    reset_serial_com           (void);
void    serial_interrupt_handler   (uint8_t data);
void    serial_send_data           (char data);
void    serial_tx_process          (const char *buff, uint8_t size);
uint8_t is_package_ready           (void);
void    serial_rx_process          (void);
void    LED1_ON                    (void);
void    LED1_OFF                   (void);
void    send_info                  (void);
#endif	/* SERIAL_H */

