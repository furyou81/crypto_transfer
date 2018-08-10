/* 
 * File:   newmain.c
 * Author: bocal
 *
 * Created on May 20, 2018, 12:30 AM
 */


// DEVCFG3
// USERID = No Setting
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
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
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = ON              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx3        // ICE/ICD Comm Channel Select (Communicate on PGEC3/PGED3)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)
//#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)

#include "types.h"
#include <p32xxxx.h>
#include <time.h>
#include <xc.h>
#include <sys/attribs.h>
#include "crypto.h"

u8 test[100];
extern u8 ras;
extern Menu screen;
extern u8 rep[];
extern u8 point;

int main(void)
{
	
    u32 compt = 0;
    init_keyboard();
	lcd();
	flash();
    screen = BOOT;
    change_screen(screen);
    ras = 0;
    init_uart();
    init_uart_rfid();
    point = 0;
    init_interrupt_ras();
    while (ras == 0)
    {
        send_string("start");
        TMR2 = 0;
        while (TMR2 < 50000);
        compt++;
        if (compt > 300)
        {
            screen = BOOT;
            change_screen(screen);
            point++;
            point %= 3;
            compt = 0;
        }
    }
    screen = MAIN;
    change_screen(screen);
    send_string("reset");
	//init_uart_rfid();
	//send_string_rfid("hola");
    while (1);
    return (EXIT_SUCCESS);

//	TRISBbits.TRISB15 = 0;
	//LATBbits.LATB15 = 0;
}

