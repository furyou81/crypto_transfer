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
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#include "types.h"
#include <p32xxxx.h>
#include <time.h>
#include <xc.h>
#include <sys/attribs.h>

#define BUT_DOWN    0
#define BUT_UP      1


int test;
u8 pin[5];
u8 compteur = 0;

void init_pin()
{
    u8 i = 0;
    while (i < 4)
    {
        pin[i] = 0xC4;
        i++;
    }
    pin[i] = '\0';
}

void check_input(u8 letter)
{
    //base(letter); // for uart debug
    if (letter == 'E') // TODO: Afficher erreur sur ecran
        return ;
   // if (letter == 'C' && compteur > 0)
   //     compteur--;
  //  else if (compteur < 4) {
  //      pin[compteur] = letter;
  //      compteur++;
  //  }
}

void __ISR(_CHANGE_NOTICE_VECTOR, IPL3SOFT) ChangeNoticeHandler(void)
{
    u8  res = 'E'; // code erreur
    if (PORTBbits.RB0 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line1();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    if (PORTBbits.RB1 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line2();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    if (PORTBbits.RB2 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line3();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    if (PORTBbits.RB3 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line4();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    PORTBbits.RB0;
    PORTBbits.RB1;
    PORTBbits.RB2;
    PORTBbits.RB3;
    IFS1bits.CNIF = 0; // Be sure to clear the CN interrupt status

    // flag before exiting the service routine.
}

void init_col()
{
    TRISEbits.TRISE0 = 0; //OUTPUT
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE3 = 0;

    LATEbits.LATE0 = 0; // OUTPUT 0
    LATEbits.LATE1 = 0;
    LATEbits.LATE2 = 0;
    LATEbits.LATE3 = 0;
}

void init_row()
{
    TRISBbits.TRISB0 = 1; //INPUT
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;

    CNPUEbits.CNPUE2 = 1;// internal pull up enabled
    CNPUEbits.CNPUE3 = 1;
    CNPUEbits.CNPUE4 = 1;
    CNPUEbits.CNPUE5 = 1;
}

void init_interrupt_keypad()
{
    PORTBbits.RB0; //read port
    PORTBbits.RB1;
    PORTBbits.RB2;
    PORTBbits.RB3;

    AD1PCFGbits.PCFG0 = 1; // pin analogue, set as a 'digital pin'
    AD1PCFGbits.PCFG1 = 1;
    AD1PCFGbits.PCFG2 = 1;
    AD1PCFGbits.PCFG3 = 1;

     __builtin_disable_interrupts(); //pas d'interruptions possible
    CNCONbits.ON = 1;
    CNENbits.CNEN2 = 1; // enable CN on RB0
    CNENbits.CNEN3 = 1;
    CNENbits.CNEN4 = 1;
    CNENbits.CNEN5 = 1;

    IPC6bits.CNIP = 3;
    IPC6bits.CNIS = 2;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    INTCONbits.MVEC = 1; // interrupt multivector enabled
    __builtin_enable_interrupts();
}

void init_keyboard()
{
    init_col();
    init_row();
    init_interrupt_keypad();
}

