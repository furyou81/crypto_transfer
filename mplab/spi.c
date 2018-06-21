/*
 * File:   lcd_I2C.c
 * Author: bocal
 *
 * Created on May 20, 2018, 3:28 PM
 */

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

#include "crypto.h"

#define READID 0x90
#define RDSR 0x05
#define WRITE  0x02
#define READ   0x03
#define WREN 0x06
#define WRDI 0x04
#define EWSR 0x50
#define WRSR 0x01
#define ERRASE 0x60
#define SECTOR_ERRASE 0x20

#define ADD_MANUFACTURER 0x00
#define ADD_DEVICE 0x01



void on()
{
    LATDbits.LATD6 = 0; //  The device is enabled by a high to low transition on CE#. CE# must remain low for the duration of any command sequence.
}

void off()
{
    LATDbits.LATD6 = 1;
}

void initSPI(void)
{
    TRISDbits.TRISD6 = 0; // CE / CS
    on();                     // Set CS high (idle state)

    IEC1bits.SPI2EIE = 0;       // SPI interrupts disabled
    IEC1bits.SPI2RXIE = 0;
    IEC1bits.SPI2TXIE = 0;

    SPI2CONbits.ON = 0;         // Turn off SPI module

    SPI2BUF = 0;                // Clear the receive buffer

    SPI2BRG = 3;                // FSCK = 2.5MHz

    SPI2STATbits.SPIROV = 0;    // Clear overflow flag


    /* SPI1CON settings */
    SPI2CONbits.FRMEN = 0;      // Framed SPI support is disabled
    SPI2CONbits.SIDL = 0;       // Continue operation in IDLE mode
    SPI2CONbits.DISSDO = 0;     // SDO1 pin is controlled by the module
    SPI2CONbits.MODE16 = 0;     // 8 bit mode
    SPI2CONbits.MODE32 = 0;     // 8 bit mode
    SPI2CONbits.CKP = 1;        // Idle state for clock is high, active state is low
    SPI2CONbits.CKE = 0;        // Output data changes on transition from idle to active
    SPI2CONbits.SSEN = 0;       // Not in slave mode
    SPI2CONbits.MSTEN = 1;      // Master mode
    SPI2CONbits.SMP = 1;        // Input data sampled at the end of data output time

    SPI2CONbits.ON = 1;         // Turn module on
    off();
}

u8 rSPI(u8 data)
{
    SPI2STATbits.SPIROV = 0; // reset overflow flag
    while (!SPI2STATbits.SPITBE); // while buf is not empty wait
    SPI2BUF = data;
    while (!SPI2STATbits.SPIRBF);   // Wait for transfer to be completed
    return (SPI2BUF);
}

u8 read_status()
{
    u8 status;
    on();
    rSPI(RDSR);
    status = rSPI(0x78); // nimporte quoi
    base(status);
    off();
    return (status);
    // u8 i;
   // for (i = 7; i >= 0; i--)
   // {
   //     send_char(((status & (1 << i)) >> i) + '0');
   // }
   // while(SPI2STATbits.SPIBUSY);

}

u8 read_status_uart()
{
    u8 status;
    on();
    rSPI(RDSR);
    status = rSPI(0x78); // nimporte quoi
    //base(status);
    off();
    return (status);
}

void wait_ready()
{
    u8 stat;

    stat = read_status_uart();
    while (stat & 0x01)
        stat = read_status_uart();
}

u8 readID()
{
    volatile u8 trash;
    u8 d;

    on();
    trash = rSPI(READID);
    trash = rSPI(0x0);
    trash = rSPI(0x0);
    trash = rSPI(ADD_MANUFACTURER);
    d = rSPI(0x8);
    off();
    base(d);
    return (d);
}

void change_status()
{
    on();
    rSPI(WREN);
    while(SPI2STATbits.SPIBUSY);
    rSPI(EWSR); //enable chage status
    off();
    while(SPI2STATbits.SPIBUSY);
    on();
    rSPI(WRSR);
    rSPI(0xE2);
    off();
}

void change_status2()
{
    on();
    rSPI(WRDI); // disable write
    //while(SPI2STATbits.SPIBUSY);
    off();
    wait_ready();
    on();
    rSPI(EWSR); //enable chage status
    off();
    //wait_ready();
    //while(SPI2STATbits.SPIBUSY);
    on();
    rSPI(WRSR);
    rSPI(0x00); // clear all bits to enable writting
    off();
    wait_ready();
}

void write_enable()
{
    on();
    rSPI(WREN);
    //while(SPI2STATbits.SPIBUSY);
    off();
}

void sector_erase(u32 adress)
{
    on();
        rSPI(WREN);
    off();
    TMR2 = 0;
    while (TMR2 < 1000);
    on();
    rSPI(SECTOR_ERRASE);
    rSPI((adress & (0xFF << 15)) >> 15);
    rSPI((adress & (0xFF << 7)) >> 7);
    rSPI(adress & (0xFF));
    while(SPI2STATbits.SPIBUSY);
  //  TMR2 = 0;
  //  while (TMR2 < 10000);
    off();
   // TMR2 = 0;
    //while (TMR2 < 1000);
}


void chiperrase()
{
    on();
    rSPI(WREN);
    off();
    TMR2 = 0;
    while (TMR2 < 1000);
    on();
    rSPI(ERRASE);
    while(SPI2STATbits.SPIBUSY);
    off();

}
void write_disable()
{
    on();
    rSPI(WRDI);
    while(SPI2STATbits.SPIBUSY);
    off();
}

void writeSPI(u8 data, u32 adress)
{
   // chiperrase();
    sector_erase(adress);
    wait_ready();
    on();
    volatile u8 trash;

   // change_status();
    rSPI(WREN);
    off();
    wait_ready();
    on();
    trash = rSPI(WRITE);
    trash = rSPI((adress & (0xFF << 15)) >> 15);
    trash = rSPI((adress & (0xFF << 7)) >> 7);
    trash = rSPI(adress & (0xFF));
    trash = rSPI(data);
    // while(SPI2STATbits.SPIBUSY);
    //write_disable();
    off();
}

u8 readSPI(u8 address)
{
    on();
    u8 tab[50];
    volatile u8 trash;
    trash = rSPI(READ);
    trash = rSPI((address & (0xFF << 15)) >> 15);
    trash = rSPI((address & (0xFF << 7)) >> 7);
    trash = rSPI(address & (0xFF));
    tab[0] = (rSPI(0x04)); // nimporte quoi
    //tab[1] = (rSPI(0x8));
    base(tab[0]);

    off();
    return 0;
}



int main(void) {
  //  TRISDbits.TRISD6 = 0; // CE / CS

    TMR2 = 0;
    T2CONbits.ON = 1;
    u8 ret;
    initSPI();

    read_status();
    change_status2();

    read_status();

    writeSPI(0xaa, 0x09);
    ret = readSPI(0x09);
    writeSPI(0xBB, 0x0a);
    ret = readSPI(0x0a);

    while(1)
    {
        ;
    }
    return (EXIT_SUCCESS);
}