#ifndef INITTIMER_H
#define	INITTIMER_H

#define F_CY 14745600.0


#define TENTH_SEC F_CY*0.1/256.0 - 1
#define SIXTY_MS F_CY*.06/256.0 - 1
#define TEN_MS .01*F_CY/256.0 - 1
#define ONE_SECOND F_CY/256 - 1

//void delayUs(unsigned int delay);
void initTimer2();
void initTimer1();
void initTimer3();
void initTimer4();
void initTimer5();
void delayTenthSec(int n);

#endif	/* INITTIMER_H */

