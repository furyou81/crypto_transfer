/*
 * File:   led.c
 * Author: bocal
 *
 * Created on May 20, 2018, 3:28 PM
 */

#include "crypto.h"

void blue()
{
    LATBbits.LATB13 = 0;
    LATCbits.LATC7 = 1;
    LATBbits.LATB15 = 1;
}

void green()
{
    LATBbits.LATB13 = 1;
    LATCbits.LATC7 = 0;
    LATBbits.LATB15 = 1;
}

void red()
{
    LATBbits.LATB13 = 1;
    LATCbits.LATC7 = 1;
    LATBbits.LATB15 = 0;
}

void pink()
{
    LATBbits.LATB13 = 0;
    LATCbits.LATC7 = 1;
    LATBbits.LATB15 = 0;
}

void orange()
{
    LATBbits.LATB13 = 1;
    LATCbits.LATC7 = 0;
    LATBbits.LATB15 = 0;
}

void yellow()
{
    LATBbits.LATB13 = 0;
    LATCbits.LATC7 = 0;
    LATBbits.LATB15 = 1;
}

void init_led()
{
	TRISBbits.TRISB13 = 0; // RED
    TRISCbits.TRISC7 = 0; // GREEN
    TRISBbits.TRISB15 = 0; // BLUE
    LATBbits.LATB13 = 1;
    LATCbits.LATC7 = 1;
    LATBbits.LATB15 = 1;
}