/*
 * File:   led.c
 * Author: bocal
 *
 * Created on May 20, 2018, 3:28 PM
 */

#include "crypto.h"

void blue()
{
    LATBbits.LATB4 = 1;
    LATBbits.LATB7 = 0;
    LATBbits.LATB6 = 1;
    LATBbits.LATB5 = 1;
}

void green()
{
    LATBbits.LATB4 = 1;
    LATBbits.LATB7 = 1;
    LATBbits.LATB6 = 0;
    LATBbits.LATB5 = 1;
}

void red()
{
    LATBbits.LATB4 = 1;
    LATBbits.LATB7 = 1;
    LATBbits.LATB6 = 1;
    LATBbits.LATB5 = 0;
}

void pink()
{
    LATBbits.LATB4 = 1;
    LATBbits.LATB7 = 0;
    LATBbits.LATB6 = 1;
    LATBbits.LATB5 = 0;
}


int main(void) {
    TRISBbits.TRISB7 = 0; // RED
    TRISBbits.TRISB6 = 0; // GREEN
    TRISBbits.TRISB5 = 0; // BLUE
    TRISBbits.TRISB4 = 0; // ALIM
    LATBbits.LATB4 = 0;
    LATBbits.LATB7 = 1;
    LATBbits.LATB6 = 1;
    LATBbits.LATB5 = 1;
 
    while(1)
    {
           blue();
           pink();
         
    green();   blue();
    green();
    }
    return (EXIT_SUCCESS);
}