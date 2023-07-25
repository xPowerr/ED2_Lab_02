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
#include "LCD.h"
#include "ADC_setup.h"

// --------------- Frecuencia ---------------
#define _XTAL_FREQ 8000000


// --------------- Variables ---------------
int bandera = 0; // variable para el antirrebotes
//char button = 0; // variable para la libreria de IOCB
int adc_var = 0; // variable para almacenar el valor leido del ADC
int select = 0; // variable para el selector del muxeo del display

// --------------- Prototipos ---------------
void setup(void);
//void counter(void); // función del contador
//void setupADC(void);

// --------------- Loop principal ---------------
void main(void){
    setup();
    adc_init(0);
    
    //------------LCD------------------------
    Lcd_Init(); // Iniciar la LCD
    Lcd_Clear(); // Limpiar la LCD
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("ADC");
    
    while(1){
        
        Lcd_Set_Cursor(2,1);
        // Mostrar el valor del ADC en la LCD
        Lcd_Write_Char('0' + (adc_var / 100)); // Muestra el dígito de las centenas
        Lcd_Write_Char('0' + ((adc_var / 10) % 10)); // Muestra el dígito de las decenas
        Lcd_Write_Char('0' + (adc_var % 10)); // Muestra el dígito de las unidades
        
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
    
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    
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
    OSCCONbits.IRCF = 0b111 ; // establecerlo en 8 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador interno
}

// --------------- Setup del ADC ---------------
//void setupADC(void){
    
    //adc_init(0);
    
    //ADCON0bits.ADCS1 = 0; // Fosc/2        
    //ADCON0bits.ADCS0 = 0; // =======      
    
    //ADCON1bits.VCFG1 = 0; // Referencia VSS (0 Volts)
    //ADCON1bits.VCFG0 = 0; // Referencia VDD (5 Volts)
    
    //ADCON1bits.ADFM = 0;  // Justificado hacia izquierda
    
    //ADCON0bits.CHS0 = 0; // Canal AN0     
    
    //ADCON0bits.ADON = 1; // Habilitamos el ADC
    //__delay_us(100); //delay de 100 us
//}

// --------------- Funciones ---------------    
void counter(void){ 
    if (PORTBbits.RB6 == 0){ // revisar si se presiono el botón de incrementar
        bandera = 1;} // activar bandera
    if (PORTBbits.RB6 == 1 && bandera == 1){ // revisar si se dejo de presionar el botón y la bandera está en 1
        PORTD--; // incrementar
        bandera = 0; // limpiar bandera
    }
    if (PORTBbits.RB7 == 0){ // revisar si se presiono el botón de decrementar
        bandera = 2;} // activar bandera
    if (PORTBbits.RB7 == 1 && bandera == 2){ // revisar si se dejo de presionar el botón y la bandera está en 1
        PORTD++; // decrementar el puerto
        bandera = 0; // limpiar bandera
    }
}    
