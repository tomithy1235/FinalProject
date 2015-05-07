#include "p24FJ64GA002.h"
#include "adc.h"

void initADC()
{//initializes ADC

    TRISBbits.TRISB15 = 1;//set as inputs
    TRISBbits.TRISB14 = 1;
    TRISBbits.TRISB13 = 1;
    TRISBbits.TRISB12 = 1;

    AD1PCFGbits.PCFG9 = 0;  //Set pins 23-26 to analog
    AD1PCFGbits.PCFG10 = 0;
    AD1PCFGbits.PCFG11 = 0;
    AD1PCFGbits.PCFG12 = 0;

    AD1CON1bits.FORM = 0;//sets output mode as int
    AD1CON1bits.SSRC = 7;//auto-convert
    AD1CON1bits.ASAM = 1;//auto-sampling

    AD1CON2bits.VCFG = 0;//uses Vdd and V_R- = Vss
    AD1CON2bits.SMPI = 15;//every 16 sample/convert sequence, interrupt is called
    AD1CON2bits.ALTS = 0;//DON'T use mux B
    
    AD1CON3bits.ADRC = 0;//use system clock
    AD1CON3bits.SAMC = 1;// use 1 * T_AD
    AD1CON3bits.ADCS = 10;//use 11 * T_CY
    AD1CHS = 0;             //Configure input channels, sets negative input as V_R-
    
    AD1CON2bits.CSCNA = 1;//turn on channel scanning
    
    AD1CSSLbits.CSSL9 = 1;  // enable AN9 for scanning
    AD1CSSLbits.CSSL10 = 1;  // enable AN10 for scanning
    AD1CSSLbits.CSSL11 = 1;  // enable AN11 for scanning
    AD1CSSLbits.CSSL12 = 1;  // enable AN12 for scanning

    ADC_ON = 1; // Turn on A/D

    IEC0bits.AD1IE = 1;     //Enable the interrupt
    IFS0bits.AD1IF = 0;     //Put the flag down
}
