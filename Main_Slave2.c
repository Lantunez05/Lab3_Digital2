/* 
 * File:   Main_Slave2.c
 * Author: Luis Antunez
 *
 * Created on 27 de julio de 2023, 03:24 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
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

#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include "ADC.h"
#include "SPI.h"

#define _XTAL_FREQ 8000000
uint8_t ver, cont;
#define contador 0
// Prototipos
void setup (void);
void __interrupt() isr(void){
    if (ADIF)
    {
        adc_change_channel(1);
        PORTA = adc_get_channel();          // Seleccion de canal
        PORTB  = adc_read();                // Representar el valor del adce en el puerto B
        ADIF =0;                            // Apagar la bandera de interrupcion
        return;
    }
   if(SSPIF == 1){
        ver = spiRead();
        if (ver == contador)
        {
            spiWrite(PORTB);
            __delay_ms(1);
        }
        else
        {
            spiWrite(cont);
            __delay_ms(1);
        }
        
        SSPIF = 0;
    }
}
void main(void) {
    setup();
    while(1){
        
        if(ADCON0bits.GO == 0)
        {
           __delay_ms(10);
            ADCON0bits.GO = 1;  
        }
        
        if (RE0 == 1)
        {
            cont++;
        }
        else if (RE1 == 1)
        {
            cont--;
        }
        
       __delay_ms(250);
    }
    return;
}

void setup(void)
{
    ANSEL = 0b00000011;
    ANSELH = 0;
    
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    
    adc_init(1);
    // Oscilador 
    OSCCONbits.IRCF =0b111; 
    OSCCONbits.SCS = 1; 
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupci?n MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupci?n MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    return;
}


