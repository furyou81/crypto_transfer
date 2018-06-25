#include "crypto.h"

extern Menu screen;
#define CONST 0b011110
#define SA0 0b0
#define WR 0b0

extern u8 pin[5];
extern u8 compteur;
u8 amount[13];
u8 aff_amount[14];
u8 index_amount = 0;
extern u8 nb_transaction;

void    init_amount()
{
    u8 i = 0;
    while(i < 13)
    {
        amount[i] = '0';
        i++;
    }
    amount[i] = '\0';

}

void    init_aff_amount()
{
    u8 i = 0;
    u8 j = 0;
    while (i < 14)
    {
        if (i == 4)
        {
            aff_amount[i] = ',';
            i++;
        }
        aff_amount[i] = amount[j];
        i++;
        j++;
    }
	aff_amount[i] = '\0';
}

void    decale_amount()
{
    u8 i = 0;
    while (i < 12)
    {
        amount[i] = amount[i + 1];
        i++;
    }
}

void    c_amount()
{
    u8 i = 12;
    while (i)
    {
        amount[i] = amount[i - 1];
        i--;
    }
    amount[0] = '0';
}

void    set_col(u8 a, u8 b, u8 c, u8 d)
{
    LATEbits.LATE0 = a;
    LATEbits.LATE1 = b;
    LATEbits.LATE2 = c;
    LATEbits.LATE3 = d;
}


u8    check_line1()
{
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if (screen == MAIN)
        {
            write_i2c('a');

            screen = AMOUNT;
        change_screen(screen);
        }
		else if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '1';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '1';
            index_amount++;
            change_screen(screen);
        }
		else if (screen == HISTORY)
		{
			screen = OLD_TRADES;
			change_screen(screen);
		}
		else if (screen == RESET)
		{
			chiperrase();
			screen = MAIN;
			change_screen(screen);
		}
		else if (screen == ACCOUNT)
		{
			screen = CLIENT;
			change_screen(screen);
		}
        return('1');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if (screen == MAIN)
        {
            write_i2c('a');

            screen = HISTORY;
        change_screen(screen);
        }
        else if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '2';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '2';
            index_amount++;
            change_screen(screen);
        }
		else if (screen == HISTORY)
		{
			screen = RESET;
			change_screen(screen);
		}
		else if (screen == RESET)
		{
			screen = HISTORY;
			change_screen(screen);
		}
        return('2');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '3';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '3';
            index_amount++;
            change_screen(screen);
        }
		else if (screen == MAIN)
        {
			screen = ACCOUNT;
            change_screen(screen);
        }
        return('3');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if (screen == AMOUNT && index_amount > 0)
        {
			aff_amount[4] = '.';
			start_transaction(aff_amount);
			init_aff_amount(aff_amount);
			init_amount(amount);
            screen = MAKE_TRADE1;
            change_screen(screen);
			index_amount = 0;
        }
        else if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur > 3)
        {
            screen = MAKE_TRADE2;
            change_screen(screen);
			if (screen == MAKE_TRADE1)
			{
				cmd_rfid("er2,9");
				init_interrupt_rfid();
			}
			else
			{

				init_interrupt_rfid();
			}
			init_pin(pin);
			compteur = 0;
        }
        return('A');
    }
    return('E');
}

u8    check_line2()
{
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '4';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '4';
            index_amount++;
            change_screen(screen);
        }
        return('4');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '5';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '5';
            index_amount++;
            change_screen(screen);
        }
        return('5');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '6';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '6';
            index_amount++;
            change_screen(screen);
        }
        return('6');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
		if (screen == OLD_TRADES && nb_transaction > 0)
		{
			nb_transaction--;
			change_screen(screen);
		}
        return('B');
    }
    return('E');
}

u8    check_line3()
{
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '7';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '7';
            index_amount++;
            change_screen(screen);
        }
        return('7');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '8';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '8';
            index_amount++;
            change_screen(screen);
        }
        return('8');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '9';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '9';
            index_amount++;
            change_screen(screen);
        }
        return('9');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if (screen == MAKE_TRADE1 && compteur > 0)
        {
            compteur--;
            pin[compteur] = 0xC4;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount > 0)
        {
            c_amount();
            index_amount--;
            change_screen(screen);
        }
        return('C');
    }
    return('E');
}

u8    check_line4()
{
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
        return('*');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
        if (screen == MAKE_TRADE1 && compteur < 4)
        {
            pin[compteur] = '0';
            compteur++;
            change_screen(screen);
        }
        else if (screen == AMOUNT && index_amount < 13)
        {
            decale_amount();
            amount[12] = '0';
            index_amount++;
            change_screen(screen);
        }
        return('0');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
			screen = MAIN;
			change_screen(screen);
			init_amount(amount);
			init_aff_amount(aff_amount);
			init_pin(pin);
			compteur = 0;
			index_amount = 0;
			send_string("reset");
        return('#');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB3 == BUT_DOWN)
    {
		u8 test = count_max_transaction();
		if (screen == OLD_TRADES && test - 1 > nb_transaction)
		{
			nb_transaction++;
			change_screen(screen);
		}
        return('D');
    }
    return('E');
}