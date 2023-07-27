/* 
 * File:   Libreria para UART
 * Author: Gabriel Carrera 21216
 * Comments: A
 * Revision history: A 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UART
#define	UART

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>

void UART_config (uint16_t baudrate); // hacer la configuracion de TX y RX
void UART_write_char (char character);  // escribir desde UART
char UART_read_char (void);  // Leer con uart

#endif	/* UART_H */