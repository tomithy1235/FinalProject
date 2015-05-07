/*
 * File:   timer.c
 * Authors:
 *
 * Created on December 30, 2014, 8:07 PM
 */

#include "p24FJ64GA002.h"
#include "timer.h"

void delayTenthSec(int n)
{//delays a tenth of a second
    int i = 0;
    for(i=0;i<n;i++)
    {
        IFS0bits.T1IF = 0;
        T1CONbits.TON = 1;

        while(IFS0bits.T1IF == 0){ }
        T1CONbits.TON = 0;
        TMR1 = 0;
    }
}



void initTimer1()
//initializes timer1 (no interrupts)
{
    PR1 = (double)F_CY*0.1/256.0 - 1;//tenth second
    TMR1 = 0;
    T1CONbits.TCKPS = 0b11;
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 0;
}

void initTimer2()
{
    PR2 = ONE_SECOND/10;//quarter second
    TMR2 = 0;
    T2CONbits.TCKPS = 0b11;//256 prescaler
    IFS0bits.T2IF = 0;
    T2CONbits.TON = 0;

}

void initTimer3()
{//timer for PWM!
    T3CONbits.TCKPS = 1;//prescaler = 8
    TMR3 = 0;
    PR3 = (unsigned int) ((F_CY * .0001)/8.0 - 1); // = 183.32 = 183
    
}

void initTimer4()
{//used with timing length of input pulse from sonar
    PR4 = TENTH_SEC;//should be more than enough time than we should need
    T4CONbits.TCKPS = 3;//256 prescaler
    IFS1bits.T4IF = 0;
    T4CONbits.TON = 0;
    TMR4 = 0;
}

void initTimer5()
{//used with timing length between TRIG pulses for sonar
    PR5 = SIXTY_MS;
    TMR5 = 0;
    T5CONbits.TCKPS = 0b11;//256 prescaler
    IFS1bits.T5IF = 0;
    T5CONbits.TON = 1;//intentional! since the sendPulseToSonar() only goes when the flag is 1
}
