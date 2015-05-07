// ******************************************************************************************* //
// File:         finalPart1.c
// ******************************************************************************************* //

#include "p24fj64ga002.h"
#include "timer.h"
#include "finalHelper.h"
#include "pwm.h"
#include "adc.h"
#include "Sonar.h"
#include <stdio.h>

//#include "lcd.h"

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_OFF & ICS_PGx1 &
          FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
          IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )


typedef enum statesEnum 
{
    forward, turnLeft, turnRight, objectAvoidance, findLine, horizontalLine
} states;

//volatile int done = 0;
volatile int pulseDone = 0;
volatile states state = forward;
volatile unsigned int ls = 0;
volatile unsigned int rs = 0;
volatile unsigned int fs = 0;
volatile unsigned int bs = 0;

#define maxNoLineVal 580 //560
#define borderGoalVal 610
#define minOverLineVal 780

#define ONE_METER_TIME 336//TMR reg val representing one meter

#define MAX_SAFE_SPEED (unsigned int) 72
#define MIN_SPEED (unsigned int) 45
#define CORRECTION_FACTOR (double) 0.65 //0.5
#define SLOWDOWN_FACTOR (double) .8 //0.8

#define TURN_COUNT_THRESH 4//minimum number of times a turn must be detected before actually taking it
#define FIND_LINE_THRESH 200
#define HORIZ_COUNT_THRESH 500

int main(void)
{
    int i = 0;
    int objectDetected = 0;
//    int finishFound = 0;
    int inc = (MAX_SAFE_SPEED - MAX_SAFE_SPEED*CORRECTION_FACTOR)/10;//TODO: try MIN_SPEED
    int turnLeftCount = 0;
    int turnRightCount = 0;
    int findLineCount = 0;
    int horizCount = 0;

    //initButton();
    
    initTimer1();//for various delays (mostly delayTenthSec())
    initTimer2();//our 'have we been waiting too long' timer
    //note, I hijacked timer 2, since delayUs() is only for LCD setup.
    
    initTimer3();//for all OCM and PWM deeds
    T3CONbits.TON = 1;//IMPORTANT for all OCM and PWM!

    initPWM();//PWM for motors

    initMotors();//just sets MOTOR_EN tristate pins

    initADC();//Sets up IR input pins, configures analog input/channel scanning, turns ADC ON with INTERRUPTS!

    initSonar();//sets up OCM on output pulse, sets up ECHO input pins with interrupts,
    initTimer4();//For sonar input pulse timing
    initTimer5();//For sonar ensuring sufficient time between output pulses

    //initLCD();//inits and uses RB10 - RB15

    

    while(1)
    {
        switch (state)
        {
            case forward:
                //check if no line:
                if(rs < maxNoLineVal && ls < maxNoLineVal && bs < maxNoLineVal && fs < maxNoLineVal)
                {
                    if(findLineCount > FIND_LINE_THRESH)
                    {
                        state = findLine;
                        findLineCount = 0;
                    }
                    else
                        findLineCount++;
                }
                else
                    findLineCount = 0;

                //send pulse to sonar if we've waited long enough:
                if(IFS1bits.T5IF == 1){//if we've waited 60ms
                    IFS1bits.T5IF = 0;
                    sendPulseToSonar();
                    TMR5 = 0;
                    T5CONbits.TON = 1;
                }

                //Handling right sway:
                if(ls > borderGoalVal && rs < borderGoalVal)
                {//then we need to correct left (we are going right off the line)
                    motorControl(MAX_SAFE_SPEED*CORRECTION_FACTOR, MAX_SAFE_SPEED);
                }

                //Handling left sway:
                else if(rs > borderGoalVal && ls < borderGoalVal)
                {//then we need to correct right (we are going left off the line)
                    motorControl(MAX_SAFE_SPEED, MAX_SAFE_SPEED*CORRECTION_FACTOR);
                }

                else if (rs > minOverLineVal && ls > minOverLineVal)//for a horizontal line
                {
                    if(horizCount > HORIZ_COUNT_THRESH)
                    {
                        state = horizontalLine;
                        horizCount = 0;
                    }
                    else
                        horizCount++;
                }

                else
                {
                    horizCount = 0;
                    motorControl(MAX_SAFE_SPEED,MAX_SAFE_SPEED);//move forward
                }


                //left turn:
                if(ls > minOverLineVal && rs < minOverLineVal && bs > borderGoalVal && fs < maxNoLineVal)//left turn detected
                {
                    if(turnLeftCount > TURN_COUNT_THRESH)
                    {
                        state = turnLeft;
                        motorControl(MIN_SPEED,MIN_SPEED);
                        turnLeftCount = 0;
                        while(bs > minOverLineVal+20){}
                    }
                    else
                        turnLeftCount++;
                }
                else
                    turnLeftCount = 0;


                //right turn:
                if(rs > minOverLineVal && ls < minOverLineVal && bs > borderGoalVal && fs < maxNoLineVal)//right turn detected
                {
                    if(turnRightCount > TURN_COUNT_THRESH)
                    {
                        state = turnRight;
                        motorControl(MIN_SPEED,MIN_SPEED);
                        turnRightCount = 0;
                        while(bs > minOverLineVal+20){}
                    }
                    else
                        turnRightCount++;
                }
                else
                    turnRightCount = 0;



                //Sonar:
                if(pulseDone == 1)//if there's a sonar pulse to be read
                {

                    if(objectDetected == 1 && TMR4 <= (ONE_METER_TIME*0.4) )
                    {
                        state = objectAvoidance;
                        objectDetected = 0;
                    }

                    else if(TMR4 <= (ONE_METER_TIME*0.45))//so it double checks basically
                        objectDetected++;
                    else
                        objectDetected = 0;

                    pulseDone = 0;
                    TMR4 = 0;//no matter what
                }


                break;//end forward state


            case turnLeft:

                motorControl(-MIN_SPEED-8,MIN_SPEED+8);
                while(fs < borderGoalVal){}

                state = forward;
                break;


            case turnRight:

                motorControl(MIN_SPEED+8,-MIN_SPEED-8);
                while(fs < borderGoalVal){}

                state = forward;
                break;

                
            case findLine:
                motorControl(MAX_SAFE_SPEED-10,MAX_SAFE_SPEED+10);
                while(fs < minOverLineVal){}//until front sensor finds a line
                
                motorControl(0,0);
                delayTenthSec(2);

                motorControl(MIN_SPEED+5,MIN_SPEED+5);
                while(bs < minOverLineVal-20){}

                state = turnRight;
                break;


            case horizontalLine:

                motorControl((double)MAX_SAFE_SPEED*SLOWDOWN_FACTOR,(double)MAX_SAFE_SPEED*SLOWDOWN_FACTOR);
                
                if(fs > minOverLineVal)
                    state = forward;//should help with turning into square
                
                else if(bs > minOverLineVal)
                    state = turnLeft;//to handle when it's confused and turns around at corners

                else
                    state = turnRight;//sets default to turn right at a 3 way intersection

                break;


            //Sonar cases:
            case objectAvoidance:
                
                motorControl(MAX_SAFE_SPEED,(double)MAX_SAFE_SPEED*SLOWDOWN_FACTOR);//move around going right

                //curve out:
                for(i=0;i<10;i++)
                {
                    delayTenthSec(1);
                    motorControl(MAX_SAFE_SPEED, ((double)MAX_SAFE_SPEED*CORRECTION_FACTOR + inc*i) );
                }

                //curve back:
                for(i=0;i<10;i++)
                {
                    delayTenthSec(1);
                    motorControl((MAX_SAFE_SPEED - inc*i), MAX_SAFE_SPEED+8);
                }

                state = findLine;

                break;

//NOTE: This next section remains in the code but commented out because it is the code for one of the device requirements

//                case backAwayFromObject:
//                    //only really called for the requirement of Sonar device
//
//
//                    motorControl(-MAX_SAFE_SPEED,-MAX_SAFE_SPEED);
//
//                    //send pulse to sonar if we've wated long enough:
//                    if(IFS1bits.T5IF == 1){//if we've waited 60ms
//                        IFS1bits.T5IF = 0;
//                        sendPulseToSonar();
//                        TMR5 = 0;
//                        T5CONbits.TON = 1;
//                    }
//
//
//                    if(pulseDone == 1)//if there's a sonar pulse to be read
//                    {
//
//                        if(TMR4 > (ONE_METER_TIME*0.35))//so it double checks basically
//                            state = forward;
//
//                        pulseDone = 0;
//                        TMR4 = 0;//no matter what
//                    }
//
//                    break;

        }
        
    }
    return 0;
}


//IF initADC() is not commented out, then this shouldn't be commented out either!

void _ISR _ADC1Interrupt(void)
{
    IFS0bits.AD1IF = 0;//put flag down

    unsigned int *adcPtr;
    adcPtr = (unsigned int *) (&ADC1BUF0);

    //NOTE: this is assuming front is AN12, Left is AN11, Right is AN10, Back is AN9
    bs = ( (*adcPtr) + *(adcPtr + 4) + *(adcPtr + 8) + *(adcPtr + 12) ) / 4;
    rs = ( *(adcPtr+1) + *(adcPtr + 5) + *(adcPtr + 9) + *(adcPtr + 13) ) / 4;
    ls = ( *(adcPtr+2) + *(adcPtr + 6) + *(adcPtr + 10) + *(adcPtr + 14) ) / 4;
    fs = ( *(adcPtr+3) + *(adcPtr + 7) + *(adcPtr + 11) + *(adcPtr + 15) ) / 4;

//    done = 1;
}


//IF initSonar() is not commented out, then this shouldn't be commented out either!

void _ISR _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;//put flag down

    if(LEFT_SONAR_IN == 1)
    {
        pulseDone = -1;//to signify that pulse began
        T4CONbits.TON = 1;//start timing length of pulse
    }

    else if (LEFT_SONAR_IN == 0 && pulseDone == -1)
    {//only check these are 0 and when pulse has begun already. This SHOULD work, since they go up either way
        T4CONbits.TON = 0;//stop timing length of pulse
        pulseDone = 1;
    }
}