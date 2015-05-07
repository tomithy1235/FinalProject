
#include "Sonar.h"
#include "timer.h"
#include "p24fj64ga002.h"



void initSonar()
{//inits inputs from sonar (interrupts here too), OCM to sonar, and the timer that times input pulse

    unsigned int ELEVENus = (unsigned int) ((.000011*F_CY)/8.0 - 1);//for timer 3 (OCM)

    //init digital sonar inputs with interrupts:
    TRIS_LEFT_SONAR_IN = 1;
    //TRIS_RIGHT_SONAR_IN = 1;

    IEC1bits.CNIE = 1;//overall CN interrupt enable
    CN_LEFT_SONAR = 1;
    //CN_RIGHT_SONAR = 1;


    //init OCM on RP5:
    //TRIS_TRIG = 0;//output 
    OC5CONbits.OCTSEL = 1;//timer 3
    RP8 = OCR5;
    OC5R = 0;
    OC5RS = ELEVENus;//11us (supposed to be >=10us)

    return;
}

void sendPulseToSonar()
{
    //send OCM dual compare pulse:
    OC5CONbits.OCM = 0b100;//according to pg 135 of family manual, this should redo the pulse
    //now the interrupt will trigger the timer on AND off, and then we deal with it from there.
    return;
}

unsigned int distToObject(unsigned int timerVal)//might be worth not making this a function
{//returns distance in cm, given timerVal pulse length

    TMR4 = 0;//resets timer value (isn't a problem, as timerVal is a COPY of TMR4)
    
    //[from datasheet]: Test distance = (high level time×velocity of sound (340M/S) / 2
    return (unsigned int) (((timerVal+1)*256.0/F_CY) * 17000.0);//256 is the prescaler,
    //17000 for  cm/s
}
