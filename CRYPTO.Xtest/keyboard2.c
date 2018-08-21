#include "crypto.h"

extern Menu screen;
#define CONST 0b011110
#define SA0 0b0
#define WR 0b0

extern u8 pin[5];
extern u8 compteur;
u8 amount[13];
u8 aff_amount[14];
u8 amount_SPI[21];
u8 index_amount = 0;
extern u8 progress;
extern u8 nb_transaction;
extern u8 ras;
//u8 private_key[] = "0x6dc32f47f2c34b1d07d3cbeb1fd6a8b4354c520e1d1e0c8109d38c5f585f244e";
//u8 public_key[] = "0x961C0820ac2C7975C54f2225AfbECE63A3273Af3";
u8 private_key[100];
u8 public_key[100];
u8 rep[100];
u8 reponse[100];
u8 point = 0;
u8 pwd[5];

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

void    spi_amount()
{
    u8 i = 0;
    u8 k = 1;
    
    amount_SPI[0] = ' ';
    while (aff_amount[i])
    {
        amount_SPI[k] = aff_amount[i];
        i++;
        k++;
    }
    amount_SPI[k] = ' ';
    k++;
    amount_SPI[k] = 'E';
    k++;
    amount_SPI[k] = 'T';
    k++;
    amount_SPI[k] = 'H';
    k++;
    amount_SPI[k] = ' ';
    k++;
    amount_SPI[k] = '\0';
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
    LATCbits.LATC2 = a;
    LATCbits.LATC1 = b;
    LATCbits.LATC0 = c;
    LATBbits.LATB3 = d;
}

u8    check_line1()
{
    set_col(0, 1, 1, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if (screen == MAIN)
        {
            write_i2c('a');

            screen = AMOUNT;
			change_screen(screen);
        }
		else if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '1';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
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
			screen = CLIENT; // Creation d'un compte client (cle publique/privee).
			change_screen(screen);
		}
        else if (screen == SELLER)
        {
            screen = PRIVATE1;
            change_screen(screen);
            init_interrupt_ras();
			send_string("create_seller_account");
			send_string("private_key");
			while (ras == 0);
			ras = 0;
			ft_strcpy(private_key, rep);
            init_interrupt_ras();
            send_string("private_key");
			while (ras == 0);
			ras = 0;
			ft_strcpy(private_key, rep);
			init_interrupt_ras();
			send_string("public_key");
			while (ras == 0);
			ras = 0;
			ft_strcpy(public_key, rep);   
        }
        else if (screen == SETTINGS)
        {
            screen = ACCOUNT;
            change_screen(screen);
        }
		else if (screen = BALANCE_CHOICE)
		{
			send_string("balance_customer");
			screen = BALANCE;
			change_screen(screen);
		}
        return('1');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if (screen == MAIN)
        {
            write_i2c('a');

            screen = HISTORY;
			change_screen(screen);
        }
        else if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '2';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
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
        else if (screen == ACCOUNT)
        {
            
            screen = SELLER;
            change_screen(screen);
        }
        else if (screen == SELLER)
        {        
            screen = MAIN;
            change_screen(screen);
        }
        else if (screen == SETTINGS)
        {
            screen = WIFI;
			send_string("hotspot");
            change_screen(screen);
			init_interrupt_ras();
			ras = 0;
			while (ras == 0);
			ras = 0;
			ft_strcpy(reponse, rep);
			if (rep[0] != 'E')
				screen = CO_SUCCESS;
			else
				screen = CO_FAILED;
			change_screen(screen);
        }
		else if (screen = BALANCE_CHOICE)
		{
			send_string("balance_seller");
			init_interrupt_ras();
			ras = 0;
			while (ras == 0);
			ras = 0;
			ft_strcpy(reponse, rep);
			screen = BALANCE3;
			change_screen(screen);
		}
        return('2');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '3';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
        {
            decale_amount();
            amount[12] = '3';
            index_amount++;
            change_screen(screen);
        }
		else if (screen == MAIN)
        {
			screen = SETTINGS;
            change_screen(screen);
        }
        else if (screen == ACCOUNT)
        {
            screen = AMOUNT2;
            change_screen(screen);
        }
        else if (screen == SETTINGS)
        {
            screen = SHUT_DOWN;
            change_screen(screen);
            send_string("shut_down");
        }
        else if (screen == ACCOUNT)
        {
            screen = AMOUNT2;
            change_screen(screen);
        }
        return('3');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB2 == BUT_DOWN)
    {
        if (screen == AMOUNT && index_amount > 0)
        {
            if (screen == AMOUNT)
            {
                screen = MAKE_TRADE1;
                change_screen(screen);
            }
			aff_amount[4] = '.';
			start_transaction(aff_amount);
            spi_amount();
			init_aff_amount(aff_amount);
			init_amount(amount);
			index_amount = 0;
        }
        else if (((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur > 3) || (screen == AMOUNT2))
        {
			if (screen == MAKE_TRADE1 || screen == AMOUNT2)
			{
                u8 i = 0;
                Menu tmp = screen;

				set_key_reader(); // On set le lecteur RFID sur le code PIN qui vient d'etre saisi

                if (screen == AMOUNT2)
                {
                    aff_amount[4] = '.';
                    start_transaction(aff_amount);
                    spi_amount();
                    init_aff_amount(aff_amount);
                    init_amount(amount);
                    index_amount = 0;              
                }
				screen = MAKE_TRADE2;
				change_screen(screen);
				init_interrupt_rfid();
				init_uart_rfid();
				init_interrupt_ras();
				cmd_rfid("er1,5");
                u32 time = 0;
                TMR2 = 0;
				while (ras == 0)
                {
                    if (TMR2 > 50000)
                    {
                        time++;
                        TMR2 = 0;
                    }
                    if (time > 300)
                    {
                        time = 0;
                        point++;
                        point %= 3;
                        change_screen(TRANS);
                    }
                }
				ras = 0;
				ft_strcpy(reponse, rep);
                init_interrupt_ras();
                send_string("check_transaction");
                TMR2 = 0;
                point = 0;
    			while (ras == 0)
                {
                    if (TMR2 > 50000)
                    {
                        time++;
                        TMR2 = 0;
                    }
                    if (time > 300)
                    {
                        time = 0;
                        point++;
                        point %= 3;
                        change_screen(TRANS);
                    }
                }
				ras = 0;
				ft_strcpy(reponse, rep);
				screen = REP;
				change_screen(screen);
                init_interrupt_ras();
                while (i < 100)
                {
                    rep[i] = 0;
                    reponse[i] = 0;
                    i++;
                }
                send_string("date");
				while (ras == 0);
				ras = 0;
				ft_strcpy(reponse, rep);
                init_interrupt_ras();
                send_string("date");
				while (ras == 0);
				ras = 0;
				ft_strcpy(reponse, rep);
                if (tmp == MAKE_TRADE1)
                {
                    write_line_SPI(reponse);
                    write_line_SPI(amount_SPI);
                }
			}
			else if (screen == BALANCE)
			{
				set_key_reader(); // On set le lecteur RFID sur le code PIN qui vient d'etre saisi

				screen = BALANCE2;
				change_screen(screen);
				init_uart_rfid();
				cmd_rfid("er1,5");
				init_interrupt_rfid();
				init_interrupt_ras();
				ras = 0;
				while (ras == 0);
				ras = 0;
				ft_strcpy(reponse, rep);
				screen = BALANCE3;
				change_screen(screen);
			}
			else
			{
				set_key_reader(); // On set le lecteur RFID sur le code PIN qui vient d'etre saisi
				
				screen = CLIENT2;
				change_screen(screen);
				init_interrupt_ras();
				send_string("create_customer_account");
				send_string("private_key");
				while (ras == 0);
				ras = 0;
				ft_strcpy(private_key, rep);
                init_interrupt_ras();
                send_string("private_key");
				while (ras == 0);
				ras = 0;
				ft_strcpy(private_key, rep);
				init_uart_rfid();
				send_private_key(private_key);
                if (screen != ERROR)
                {
                    init_interrupt_ras();
                    send_string("public_key");
                    while (ras == 0);
                    ras = 0;
                    ft_strcpy(public_key, rep);
                    init_uart_rfid();
                    send_public_key(public_key);
                    screen = PRIVATE1;
                    change_screen(screen);
                }
				progress = 0;
			}
			init_pin(pin);
			compteur = 0;
        }
		else if (screen == PRIVATE1)
		{
			screen = PRIVATE2;
			change_screen(screen);
		}
		else if (screen == PRIVATE2)
		{
			screen = PRIVATE3;
			change_screen(screen);
		}
		else if (screen == PRIVATE3)
		{
			screen = PUBLIC1;
			change_screen(screen);
		}
		else if (screen == PUBLIC1)
		{
			screen = PUBLIC2;
			change_screen(screen);
		}
		else if (screen == PUBLIC2)
		{
			init_pin();
            send_string("reset");
			screen = MAIN;
			change_screen(screen);
		}
		else if (screen == ERROR)
		{
			init_pin();
			init_amount();
			init_aff_amount();
            send_string("reset");
			screen = MAIN;
			change_screen(screen);
		}
		else if (screen == REP)
		{
			init_pin();
			init_amount();
			init_aff_amount();
            send_string("reset");
			screen = MAIN;
			change_screen(screen);
		}
		else if (screen == CO_SUCCESS || screen == CO_FAILED || screen == BALANCE3)
		{
			screen = MAIN;
			change_screen(screen);
			send_string("reset");
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
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '4';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
        {
            decale_amount();
            amount[12] = '4';
            index_amount++;
            change_screen(screen);
        }
		else if (screen == ACCOUNT)
		{
			screen = BALANCE_CHOICE;
			change_screen(screen);
		}
        return('4');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB1 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '5';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
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
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '6';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
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
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '7';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
        {
            decale_amount();
            amount[12] = '7';
            index_amount++;
            change_screen(screen);
        }
        return('7');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '8';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
        {
            decale_amount();
            amount[12] = '8';
            index_amount++;
            change_screen(screen);
        }
        return('8');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur < 4)
        {
            pin[compteur] = '9';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
        {
            decale_amount();
            amount[12] = '9';
            index_amount++;
            change_screen(screen);
        }
        return('9');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB0 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == CLIENT || screen == BALANCE) && compteur > 0)
        {
            compteur--;
            pin[compteur] = 0xC4;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount > 0)
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
    if (PORTBbits.RB4 == BUT_DOWN)
    {
        return('*');
    }
    set_col(1, 0, 1, 1);
    if (PORTBbits.RB4 == BUT_DOWN)
    {
        if ((screen == MAKE_TRADE1 || screen == BALANCE || screen == CLIENT) && compteur < 4)
        {
            pin[compteur] = '0';
            compteur++;
            change_screen(screen);
        }
        else if ((screen == AMOUNT || screen == AMOUNT2) && index_amount < 13)
        {
            decale_amount();
            amount[12] = '0';
            index_amount++;
            change_screen(screen);
        }
        return('0');
    }
    set_col(1, 1, 0, 1);
    if (PORTBbits.RB4 == BUT_DOWN)
    {
        if (screen != SHUT_DOWN && screen != BOOT)
        {
			screen = MAIN;
			change_screen(screen);
			init_amount(amount);
			init_aff_amount(aff_amount);
			init_pin(pin);
			compteur = 0;
			index_amount = 0;
			send_string("reset");
        }
        return('#');
    }
    set_col(1, 1, 1, 0);
    if (PORTBbits.RB4 == BUT_DOWN)
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