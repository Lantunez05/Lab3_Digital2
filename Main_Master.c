/* 
 * File:   Main_Master.c
 * Author: Luis Antunez
 *
 * Created on 27 de julio de 2023, 02:39 AM
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
#include "SPI.h"
#include "LCD.h"

#define _XTAL_FREQ 8000000
#define RS RE0
#define EN RE1
#define D4 RD4
#define D5 RB5
#define D6 RD6
#define D7 RD7

char s1_pot [10];
char s2_pot [10];
char cont [10];

// Prototipos
void setup (void);

void main(void)
{
    setup();
    Lcd_Init();
    int pot_s1 = 0;
    int pot_s2 = 0;
    unsigned int a;
    while (1)
    {
        //-----------Primer esclavo--------------------------------------------
        PORTCbits.RC2 = 0;       //Slave Select
       __delay_ms(5);
       
        spiWrite(1);
        pot_s1 = spiRead();
        
        __delay_ms(1);
       
       PORTCbits.RC2 = 1;       //Slave Deselect 
       //---------------Segundo Esclavo--------------------------------------------------
       PORTCbits.RC1 = 0;       //Slave Select
       __delay_ms(5);
       
       spiWrite(1);
       pot_s2 = spiRead();
       //unsigned int cont = spiRead();
       __delay_ms(1);
       
       PORTCbits.RC1 = 1;       //Slave Deselect
       
       //------------LCD----------------------------
       Lcd_Clear();
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String("S1:");
       Lcd_Set_Cursor(1,4);
       sprintf(s1_pot, "%d\r", pot_s1);
       Lcd_Write_String(s1_pot);
       Lcd_Set_Cursor(2,1);
       Lcd_Write_String("S2:");
       Lcd_Set_Cursor(2,4);
       sprintf(s2_pot, "%d\r", pot_s2);
       Lcd_Write_String(s2_pot);
       Lcd_Set_Cursor(1,12);
       Lcd_Write_String("Cont:");
       __delay_ms(2000);
       
      
    }
}

void setup(void)
{
    
    ANSEL = 0;
    ANSELH = 0;
    TRISC2 = 0;
    TRISC1 = 0;
    TRISE = 0;
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    PORTCbits.RC2 = 1;
    PORTCbits.RC1 = 1;
    
    // Oscilador 
    OSCCONbits.IRCF =0b111; 
    OSCCONbits.SCS = 1; 
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1; 
    
    //adc_init(1);
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    return;
}