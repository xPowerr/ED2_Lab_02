/*
 * File:   UART.c
 * Author: Gabriel Carrera 21216
 *
 * Created on July 26, 2023, 10:16 PM
 */

#include <xc.h>
#include "UART.h"

void UART_config(uint16_t baudrate){
    PIE1bits.RCIE = 1; // Habilitar interrupcion del receptor UART
    
    if (baudrate == 4800){
    SPBRG = 12; //valor para 4800 de baud rate

    TXSTAbits.SYNC = 0;         // Modo As�ncrono
    RCSTAbits.SPEN = 1;         // Habilitar UART

    TXSTAbits.TXEN = 1;         // Habilitamos la transmision
    PIR1bits.TXIF = 0;
    RCSTAbits.CREN = 1;         // Habilitamos la recepcion
    }
}

void UART_write_char (char character){
    TXSTAbits.TXEN = 1;         // Habilitamos la transmision
    PIR1bits.TXIF = 0;
}

char UART_read_char (void){
    RCSTAbits.CREN = 1;         // Habilitamos la recepcion
}