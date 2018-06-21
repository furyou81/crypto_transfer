#include "crypto.h"

extern Menu screen;
#define CONST 0b011110
#define SA0 0b0
#define WR 0b0
void    set_col(u8 a, u8 b, u8 c, u8 d)
{
    LATEbits.LATE0 = a;
    LATEbits.LATE1 = b;
    LATEbits.LATE2 = c;
    LATEbits.LATE3 = d;
}


u8    check_line1()
{
    u8 pin;

    pin = 0;
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if (screen == MAIN)
        {
            write_i2c('a');

            screen = MAKE_TRADE1;
        change_screen(screen);
        }
        return('1');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        screen = MAKE_TRADE2;
        change_screen(screen);
        return('2');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        screen = MAKE_TRADE3;
        change_screen(screen);
        return('3');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        return('A');
    }
    return('E');
}

u8    check_line2()
{
    u8 pin;

    pin = 0;
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        return('4');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        return('5');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        return('6');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        return('B');
    }
    return('E');
}

u8    check_line3()
{
    u8 pin;

    pin = 0;
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        return('7');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        return('8');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        return('9');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        return('C');
    }
    return('E');
}

u8    check_line4()
{
    u8 pin;

    pin = 0;
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
        return('*');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
        return('0');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
        return('#');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
        return('D');
    }
    return('E');
}