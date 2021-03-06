/*
 * File:   main.c
 * Author: bocal
 *
 * Created on May 20, 2018, 3:28 PM
 */
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_4         // System PLL Output Clock Divider (PLL Divide by 4)

// DEVCFG1
#pragma config FNOSC = FRCPLL           // Oscillator Selection Bits (Fast RC Osc with PLL)
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = ON            // CLKO Output Signal Active on the OSCO Pin (Enabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#include "crypto.h"

int main(void)
{
    TMR2 = 0;
    T2CONbits.ON = 1;
    init_uart_rfid(); // communication avec le lecteur rfid
    init_uart(); // communication avec la raspberry pi
  
    start_transaction(45896);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
    TMR2 = 0;
    while (TMR2 < 10000);
//    start_transaction(785);
    while(1)
    {
        ;
    }
    return (EXIT_SUCCESS);
}