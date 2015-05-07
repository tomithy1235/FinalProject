/* 
 * File:   Sonar.h
 * Author: Timothy
 *
 * Created on April 12, 2015, 4:16 PM
 */

#ifndef SONAR_H
#define	SONAR_H

//#define TRIS_TRIG TRISBbits.TRISB8
#define RP8 RPOR4bits.RP8R
#define OCR5 22

//for testing:
//#define TRIS_LEFT_SONAR_IN TRISAbits.TRISA4
//#define LEFT_SONAR_IN PORTAbits.RA4
//#define CN_LEFT_SONAR CNEN1bits.CN0IE

//#define TRIS_RIGHT_SONAR_IN TRISBbits.TRISB5
//#define RIGHT_SONAR_IN PORTBbits.RB5
//#define CN_RIGHT_SONAR CNEN2bits.CN27IE

//for reals:
#define TRIS_LEFT_SONAR_IN TRISBbits.TRISB10
#define LEFT_SONAR_IN PORTBbits.RB10
#define CN_LEFT_SONAR CNEN2bits.CN16IE//for pin 21

//#define TRIS_RIGHT_SONAR_IN TRISBbits.TRISB11
//#define RIGHT_SONAR_IN PORTBbits.RB11
//#define CN_RIGHT_SONAR CNEN1bits.CN15IE//for pin 22



//#define ELEVENus (unsigned int) ((.000011*F_CY)/8.0 - 1)//for timer 3 (OCM)


void initSonar();
void sendPulseToSonar();
unsigned int distToObject(unsigned int timerVal);

#endif	/* SONAR_H */

