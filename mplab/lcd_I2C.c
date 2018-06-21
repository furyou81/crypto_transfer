/*
 * File:   lcd_I2C.c
 * Author: bocal
 *
 * Created on May 20, 2018, 3:28 PM
 */

#include "crypto.h"
#define HIGH 1
#define CONST 0b011110
#define SA0 0b0
#define WR 0b0
/*
 *
 */

u8 bug = 0;
u8 state = 0;
Menu screen = MAIN;

void init_i2c()
{
    TRISGbits.TRISG2 = 1;
    TRISGbits.TRISG3 = 1;
    I2C1CONbits.DISSLW = 1; /*disable slew rate for 100 kHz*/
    I2C1BRG = 0x26;  /*Set Baud Rate Generator*/
    I2C1CONbits.ON = 1;

    LATDbits.LATD0=0; //reset
    TMR2 = 0;
    while (TMR2 < 1000);
    LATDbits.LATD0 = 1;
    TMR2 = 0;
    while (TMR2 < 500);

    start_i2c();
    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
    write_i2c(0x3a);
    //write_i2c(0x1e);
    write_i2c(0x39);
    //write_i2c(0x1c);
    write_i2c(0x72); // setting contrast 70 a 7f
    write_i2c(0x54); //set contrst 2 50 a 5F
    write_i2c(0x6d);
    write_i2c(0x0c);
  //  write_i2c(0x0e); // cursor at current position
  //  write_i2c(0x0f); // blinking
    write_i2c(0x01);
    stop_i2c();
}


void start_i2c()
{
    I2C1CONbits.SEN = HIGH; /*This bit is automatically cleared by MCU*/
      int j;
    /*Test if START condition is completed - test interrupt I2C1MIF in IFS1*/
    while (I2C1CONbits.SEN == HIGH) /*wait until start condition finishes*/
    {
        ;
       // j = 1;
    }
}

void stop_i2c()
{
    I2C1CONbits.PEN = 1;
}

void write_i2c(u8 c)
{
    I2C1TRN = c;
    // IFS0bits.I2C1SIF = 0;
    /*Wait for transmit buffer empty - indicates that write is completed*/
    while (I2C1STATbits.TBF == HIGH) //Transmit Buffer Full Status bit //Transmit in progress; I2CxTRN is full (8-bits of data)
    {
        ;
     //   j = 1;
    }
    /*Wait for ACK signal from device - bit will clear when ACK is received*/
//   while (I2C1STATbits.ACKSTAT == 1)
//    {
//        int  j = 1;
//    }
}

void writeEeprom(void)
{
    start_i2c();

    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
    /*while (!IFS0bits.I2C1SIF)
    {
        j = 1;
    }*/
     /*Page write*/
   // int i = 0;
    /*24LC02 can only write up to 8 bytes at a time*/
   //   I2C1TRN = 'a';
   /*Wait for transmit buffer empty - indicates that write is completed*/
 //   while (I2C1STATbits.TBF == HIGH);
    /*Generate ACK event*/
    //while (I2C1CONbits.ACKEN == 1);
    stop_i2c();
}

void write_words(char str[], u8 new_line, u8 rank)
{
    u8 i = 0;
    u8 aff = 0;

    while (str[i])
    {
        if (i == 1 && bug == 0)
        {
            write_i2c('e');
            aff = 1;
        }
        write_i2c(str[i]);
        i++;
    }
    if (aff == 1)
        i++;
    if (rank > 0)
        i += 3;
    while (new_line > 0)
    {
       while (i < 21)
       {
          write_i2c(' ');
          i++;
       }
       new_line--;
       i = 1;
    }
}


/*Max tab[10]*/
void  write_nb(u32 nb)
{
    u8 i = 0;
    u8 tab[100];
    u32 tmp = nb;

    while (tmp)
    {
        tmp = tmp / 10;
        i++;
    }
    tab[i] = 0;
    i--;
    while (nb > 0)
    {
       tab[i] = (nb % 10) + 48;
       nb = nb / 10;
       i--;
    }
    write_words(tab, 0, 0);
}

void write_line(char str[], u8 new_line, u32 rank)
{
    u8 i = 0;

    if (rank > 0)
    {
        write_nb(rank);
        write_i2c('.');
    }
    write_words(str, new_line, rank);
    bug = 1;
}

void bruit()
{

    write_line("BRUIT", 1, 0);
}

void change_screen(Menu str)
{
    bug = 0;
    start_i2c();
    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
//    write_i2c(0x02);
 //   write_i2c(0x40);
    write_i2c(0xfa); // dire qu'on veut ecrire dans la DDRAM
    //write_i2c(0x13);
    choose_screen(str);
    stop_i2c();
}

void choose_screen(Menu str)
{
    if (str == MAIN)
    {
        // write : str | new_line | rank
        write_line("HOME", 1, 0); // string + nombre de lignes a sauter
        write_line("MAKE TRADE", 1, 1);
        write_line("OLD TRADES", 1, 2);
        write_line("ADD KEY", 1, 3);
    }
    else if (str == MAKE_TRADE1)
    {
        // write : str | new_line | rank
        write_line("MAKE TRADE", 2, 0);
        write_line("ACCOUNT NUM?", 1, 0);
    }
    else if (str == MAKE_TRADE2)
    {
        // write : str | new_line | rank
        write_line("MAKE TRADE", 2, 0);
        write_line("LOOKING FOR CARD...", 1, 0);
    }
    else if (str == MAKE_TRADE3)
    {
        // write : str | new_line | rank
        write_line("MAKE TRADE", 2, 0);
        write_line("SCANNING CARD", 1, 0);
    }
    else if (str == OLD_TRADES)
    {
        // write : str | new_line | rank
        write_line("OLD TRADES", 1, 0);
    }
    else
    {
        write_line("ADD_KEY", 1, 0);

    }
}



int main(void) {
    TRISDbits.TRISD0 = 0;


    TMR2 = 0;
    T2CONbits.ON = 1;

    /*Initialize I2C Peripheral*/
    //SYSTEMConfigPerformance(8000000);
    init_i2c();

    init_keyboard();
    start_i2c();
    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
    write_i2c(0xfa); // dire qu'on veut ecrire dans la DDRAM
    int i = 0;
    while(1) {
        if (screen == MAIN)
            change_screen(screen);
        stop_i2c();
    }
    return (EXIT_SUCCESS);
}