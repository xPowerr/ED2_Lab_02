/*
 * File:   main.c
 * Author: Gabriel Carrera 21216
 *
 * Created on July 24, 2023, 5:26 PM
 */

// --------------- Palabra de Configuracion ---------------
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// --------------- Librerias ---------------
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "LCD.h"
#include "ADC_setup.h"
#include "UART.h"

// --------------- Frecuencia ---------------
#define _XTAL_FREQ 4000000


// --------------- Variables ---------------
int bandera = 0; // variable para el antirrebotes
//char button = 0; // variable para la libreria de IOCB
int adc_var = 0; // variable para almacenar el valor leido del ADC
int select = 0; // variable para el selector del muxeo del display
char display[3]; // array para caracteres de la lcd
int voltage1 = 0; // valor del mapeo del adc
int counter = 0; // variable para el contador del uart
char display_counter[1]; // array para caracteres de la lcd

uint8_t uni_volt = 0; // variable para unidades de voltaje
uint8_t dec_volt = 0; // variable para decimas de voltaje
uint8_t cen_volt = 0; // variable para centesimas de voltaje

// --------------- Prototipos ---------------
void setup(void); // funcion de configuracion
void cadena(char *cursor); //función para desplegar caracteres en la terminal UART
//void counter(void); // función del contador
//void setupADC(void);

// --------------- Loop principal ---------------
void main(void){
    setup(); // Llamada a la funcion de configuracion
    adc_init(0); // Se inicializa el ADC para el canal AN0
    UART_config(4800); // Se configura UART con baudrate de 4800
    
    //------------LCD------------------------
    Lcd_Init(); // Iniciar la LCD
    Lcd_Clear(); // Limpiar la LCD
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Pot1:");
    
    Lcd_Set_Cursor(1,8);
    Lcd_Write_String("Count:");
    
    while(1){
        
        voltage1 = map_adc_volt(adc_var, 0, 255, 0, 100);
        uni_volt = (voltage1*5) / 100;
        dec_volt = (voltage1*5)/10 % 10;
        cen_volt = (voltage1*5) % 10;
        
        Lcd_Set_Cursor(2,1);
        sprintf(display, "%d.%d%dV", uni_volt, dec_volt, cen_volt); // llenar arreglo de caracteres para enviar a lcd
        Lcd_Write_String(display);
        
        // Mostrar el valor del ADC en la LCD
        //Lcd_Write_Char('0' + (uni_volt)); // Muestra el dígito de las centenas
        //Lcd_Write_Char('0' + ((dec_volt))); // Muestra el dígito de las decenas
        //Lcd_Write_Char('0' + (cen_volt)); // Muestra el dígito de las unidades
        
        
        Lcd_Set_Cursor(2,9);
        sprintf(display_counter, "%03d", counter); // convertir la variable a caracter para la lcd
        Lcd_Write_String(display_counter);
        
        
        if (ADCON0bits.GO == 0) { // si la lectura del ADC se desactiva
            ADCON0bits.GO = 1; // activarla
            __delay_ms(50);
        }
        //PORTD = adc_read();
    }
}

// --------------- Rutina de  interrupciones ---------------
void __interrupt() isr(void){ // interrupciones
    if (PIR1bits.ADIF == 1){ // revisar bandera de interrupcion del puerto B
        adc_var = adc_read();
        PIR1bits.ADIF = 0; // limpiar bandera
    }
    if (PIR1bits.RCIF == 1){ //Revisar interrupcion del Receptor del UART
        PIR1bits.RCIF = 0; //Limpiar la bandera de interrupcion del receptor del UART
        if (RCREG == '+'){ //Revisar si se recibió un '+'
            if (counter == 255){ // hacer que no pase de 255, 8 bits
                counter = 0;
            }
            else {
            counter++; //Aumentar contador
            }
        }    
        if (RCREG == '-'){ //Revisar si se recibió un '-'
            if (counter == 0){ // hacer que no sea negativo, 8 bits
                counter = 255;
            }
            else {
            counter--; //Decrementar contador
            }
        }
        if (RCREG == 13){
            cadena("Voltaje Potenciometro: \r\n"); //Escribir en la terminal UART
            sprintf(display, "%d.%d%dV", uni_volt, dec_volt, cen_volt); //convertir variable a una cadena de caracteres
            cadena(display); //Mostrar variable en la terminal
            cadena("\n\r"); //Enter
        }
    }
}

// --------------- Setup General ---------------
void setup(void){
    
// --------------- Definir analogicas ---------------
    //ANSEL   =   0;
    //ANSELH  =   0;
    
// --------------- Configurar puertos --------------- 
    //TRISBbits.TRISB6 = 1; // RB6 como entrada
    //TRISBbits.TRISB7 = 1; // RB7 como entrada
    
    //TRISBbits.TRISB0 = 0;
    //TRISBbits.TRISB1 = 0;
    
    TRISB = 0; // Puerto B como salida - LCD
    TRISC = 0;
    TRISD = 0; // Puerto D como salida para RS y EN de LCD
    //TRISE = 0;
    
// --------------- Limpiar puertos ---------------    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;    
    
// --------------- Habilitar pullups --------------- 
    //OPTION_REGbits.nRBPU = 0; 
    //WPUBbits.WPUB6 = 1;
    //WPUBbits.WPUB7 = 1; 

// --------------- Banderas e interrupciones --------------- 
    INTCONbits.GIE = 1;   // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;  // Habilitar interrupciones de perifericas
    //INTCONbits.RBIE = 1;  // Habilitar interrupciones en PORTB
    
    //IOCBbits.IOCB6 = 1;   // Habilitar interrupciones en RB6
    //IOCBbits.IOCB7 = 1;   // Habilitar interrupciones en RB7
    
    // Utilizar la libreria para habilitar pullup e IOCB de cada boton deseado
    //ioc_init(7);
    //ioc_init(6);
    
    //INTCONbits.RBIF = 0;  // Limpiar bandera de interrupcion de PORTB
    
// --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b110 ; // establecerlo en 4 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador interno
}

//Funcion para mostrar texto
void cadena(char *cursor){
    while (*cursor != '\0'){//mientras el cursor sea diferente a nulo
        while (PIR1bits.TXIF == 0); //mientras que se este enviando no hacer nada
            TXREG = *cursor; //asignar el valor del cursor para enviar
            *cursor++;//aumentar posicion del cursor
    }
}