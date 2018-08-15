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

u8 nb_transaction = 0;
u8 bug = 0;
Menu screen = MAIN;
u8 progress = 0;
extern u8 pin[5];
extern u8 amount[13];
extern u8 aff_amount[14];
extern u8 private_key[];
extern u8 public_key[];
extern u8 rep[];
extern u8 reponse[];
extern u8 point;

void init_i2c()
{

 //   TRISBbits.TRISB8 = 1;
 //   TRISBbits.TRISB9 = 1;
    I2C1CONbits.DISSLW = 1; /*disable slew rate for 100 kHz*/
    I2C1BRG = 0x26;  /*Set Baud Rate Generator*/
    I2C1CONbits.ON = 1;

    LATCbits.LATC6 = 0; //reset
    TMR2 = 0;
    while (TMR2 < 1000);
    LATCbits.LATC6 = 1;
    TMR2 = 0;
    while (TMR2 < 500);

    start_i2c();
    //write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
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

	start_i2c();
	write_i2c(0xfa);
	stop_i2c();

	//TMR2 = 0;
	//while(TMR2 < 500);


	//start_i2c();

	/*write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
	write_i2c(0xfa);
    stop_i2c();*/

	//TMR2 = 0;
	//while(TMR2 < 500);

	//start_i2c();
	//write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
	/*write_i2c('H');
	write_i2c('A');
	write_i2c('M');
	write_i2c('A');
	write_i2c('U');
	write_i2c('p');
	write_i2c('Y');*/
	//write_line("few seconds . . .", 2, 0);
	//choose_screen(screen);
	//stop_i2c();
	//while(1);

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
	  TMR2 = 0;
   while (TMR2 < 200);
   write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
}

void stop_i2c()
{
    I2C1CONbits.PEN = 1;
	TMR2 = 0;
	while(TMR2 < 350);
}

void write_i2c(u8 c)
{
	while (I2C1STATbits.TRSTAT == HIGH) //Transmit Buffer Full Status bit //Transmit in progress; I2CxTRN is full (8-bits of data)
    {
        ;
     //   j = 1;
    }
    I2C1TRN = c;
    // IFS0bits.I2C1SIF = 0;
    /*Wait for transmit buffer empty - indicates that write is completed*/
    while (I2C1STATbits.TBF == HIGH) //Transmit Buffer Full Status bit //Transmit in progress; I2CxTRN is full (8-bits of data)
    {
        ;
     //   j = 1;
    }
    /*Wait for ACK signal from device - bit will clear when ACK is received*/
   while (I2C1STATbits.ACKSTAT == 1);
//    {
//        int  j = 1;
//    }

   TMR2 = 0;
   while (TMR2 < 250);
}

void writeEeprom(void)
{
    start_i2c();

    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
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

int ft_strlen(char str[])
{
    int i;

    i = 0;
    while (str[i])
        i++;
	return (i);
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

void    write_pin()
{
    u8 i = 0;

    while (i < 4)
    {
        if (pin[i] == 0xC4)
            write_i2c(pin[i]);
        else
            write_i2c('*');
        i++;
    }
}

void	progress_bar(char *tmp)
{
	u8 i = 0;

	while (i < 20)
	{
		if (i < progress)
			tmp[i] = 0x1F;
		else
			tmp[i] = ' ';
		i++;
	}
	tmp[i] = '\0';
}

void	line_priv(char *tmp, u8 k)
{
	u8 i = 0;

	while (i < 20 && private_key[i + k * 20])
	{
		tmp[i] = private_key[i + k * 20];
		i++;
	}
	tmp[i] = '\0';
}

void	line_pub(char *tmp, u8 k)
{
	u8 i = 0;

	while (i < 20 && public_key[i + k * 20])
	{
		tmp[i] = public_key[i + k * 20];
		i++;
	}
	tmp[i] = '\0';
}

void choose_screen(Menu str)
{
	u8 tmp[21];

    if (str == MAIN)
    {
        // write : str | new_line | rank
        write_line("HOME", 1, 0); // string + nombre de lignes a sauter
        write_line("MAKE TRADE", 1, 1);
        write_line("HISTORY", 1, 2);
		write_line("SETTINGS", 1, 3);
    }
    else if (str == SETTINGS)
    {
        write_line("SETTINGS", 1, 0); // string + nombre de lignes a sauter
        write_line("ACCOUNT", 1, 1);
        write_line("WIFI", 1, 2);
		write_line("SHUT DOWN", 1, 3);
    }
    else if (str == BOOT)
    {
        write_line("   ", 1, 0); // string + nombre de lignes a sauter
        write_line(" Please wait a", 1, 0);
        if (point == 0)
            write_line("few seconds . ", 2, 0);
        else if (point == 1)
            write_line("few seconds . . ", 2, 0);
        else if (point == 2)
            write_line("few seconds . . .", 2, 0);
    }
    else if (str == MAKE_TRADE1)
    {
        // write : str | new_line | rank
        write_line("   ", 1, 0);
        write_line("  PIN:   ", 0, 0);
        write_pin();
        write_line("      ", 0, 0);
        write_line("", 1, 0);
        write_line("        A to ACCEPT", 1, 0);
    }
    else if (str == MAKE_TRADE2)
    {
        // write : str | new_line | rank
        write_line(" MAKE TRADE", 2, 0);
        write_line("LOOKING FOR CARD...", 2, 0);
    }
    else if (str == MAKE_TRADE3)
    {
        // write : str | new_line | rank
        write_line("  ", 2, 0);
        write_line("SCANNING CARD", 2, 0);
    } 
    else if (str == AMOUNT)
    {
        // write : str | new_line | rank
        write_line("  AMOUNT:", 2, 0);
        init_aff_amount();
        write_line(aff_amount, 0, 0);
        write_line(" ETH", 0, 0);
        write_line("          A to ACCEPT", 1, 0);
    }
    else if (str == OLD_TRADES)
    {
		if (nb_transaction == 0)
            write_line("  ", 1, 0);
        else
            write_line("            B to UP", 1, 0);
		if (count_max_transaction() > 0)
		{
			get_date(nb_transaction, tmp);
			write_line(tmp, 0, 0);
		}
		else
			write_line("      NONE", 1, 0);
		if (count_max_transaction() > 0)
		{
			get_amount(nb_transaction, tmp);
			write_line(tmp, 0, 0);
		}
		else
			write_line(" ", 1, 0);
		if (count_max_transaction() - 1 > nb_transaction)
            write_line("         D to DOWN", 1, 0);
		else
			write_line("   ", 1, 0);
    }
	else if (str == HISTORY)
	{
		write_line("HISTORY:", 1, 0);
        write_line("OLD TRADES", 1, 1);
        write_line("RESET", 2, 2);
	}
	else if (str == RESET)
	{
		write_line("Are you sure?:", 1, 0);
        write_line("YES", 1, 1);
        write_line("NO", 2, 2);
	}
    else if (str == SELLER)
	{
		write_line(" It will erase your", 1, 0);
        write_line(" old account:", 1, 0);
         write_line("YES", 1, 1);
        write_line("NO", 1, 2);
	}
	else if (str == ACCOUNT)
	{
		write_line("  ", 1, 0);
		write_line("CUSTOMER", 1, 1);
        write_line("SELLER", 1, 2);
		write_line("REFUND", 1, 3);
	}
	else if (str == CLIENT)
	{
		write_line("   ", 1, 0);
        write_line("  PIN:   ", 0, 0);
        write_pin();
        write_line("      ", 0, 0);
        write_line("", 1, 0);
        write_line("        A to ACCEPT", 1, 0);
	}
	else if (screen == CLIENT2)
	{
		write_line("   ", 1, 0);
		write_line("LOOKING FOR CARD...", 1, 0);
		progress_bar(tmp);
		write_line(tmp, 2, 0);
	}
	else if (screen == PRIVATE1)
	{
		write_line("   ", 1, 0);
		write_line("Please save priv key", 0, 0);
		write_line("Can't be recovered", 1, 0);
		write_line("    A to continue", 1, 0);
	}
	else if (screen == PRIVATE2)
	{
		write_line("   ", 1, 0);
		line_priv(tmp, 0);
		write_line(tmp, 0, 0);
		line_priv(tmp, 1);
		write_line(tmp, 0, 0);
		write_line("    A to continue", 1, 0);
	}
	else if (screen == PRIVATE3)
	{
		write_line("   ", 1, 0);
		line_priv(tmp, 2);
		write_line(tmp, 0, 0);
		line_priv(tmp, 3);
		write_line(tmp, 1, 0);
		write_line("   A to continue", 1, 0);
	}
	else if (screen == PUBLIC1)
	{
		write_line("   ", 1, 0);
		write_line("Please save publ key", 0, 0);
		write_line("Can't be recovered", 1, 0);
		write_line("    A to continue", 1, 0);
	}
	else if (screen == PUBLIC2)
	{
		write_line("   ", 1, 0);
		line_pub(tmp, 0);
		write_line(tmp, 0, 0);
		line_pub(tmp, 1);
		write_line(tmp, 0, 0);
		write_line("    A to continue", 1, 0);
	}
	else if (screen == PUBLIC3)
	{
		write_line("   ", 1, 0);
		line_pub(tmp, 2);
		write_line(tmp, 2, 0);
		write_line("   A to continue", 1, 0);
	}
    else if (screen == TRANS)
	{
		write_line("   ", 1, 0);
		write_line("Transaction in", 1, 0);
        if (point == 0)
            write_line("progress . ", 2, 0);
        else if (point == 1)
            write_line("progress . . ", 2, 0);
        else if (point == 2)
            write_line("progress . . .", 2, 0);
	}
	else if (screen == REP)
	{
		write_line("   ", 1, 0);
		write_line(reponse, 2, 0);
		write_line("   A to continue", 1, 0);
	}
    else if (str == AMOUNT2)
    {
        // write : str | new_line | rank
        write_line("  REFUND:", 2, 0);
        init_aff_amount();
        write_line(aff_amount, 0, 0);
        write_line(" ETH", 0, 0);
        write_line("          A to ACCEPT", 1, 0);
    }
    else if (str == SHUT_DOWN)
    {
        write_line("   ", 1, 0);
        write_line(" You can remove", 1, 0);
        write_line("the power supply", 2, 0);
    }
	else if (str == WIFI)
	{
		write_line("   ", 1, 0);
        write_line("go to CRYPTO:", 1, 0);
		write_line("pasword: raspberry  ", 0, 0);
        write_line("host: 192.168.220.1 ", 0, 0);
	}
	else if (str == CO_SUCCESS)
	{
		write_line("   ", 1, 0);
        write_line("Connection SUCCESS", 2, 0);
		write_line("    A to continue", 1, 0);
	}
	else if (str == CO_FAILED)
	{
		write_line("   ", 1, 0);
        write_line("Connection FAILED", 2, 0);
		write_line("    A to continue", 1, 0);
	}
	else
    {
        write_line("  ERROR", 3, 0);
		write_line("   A to continue", 1, 0);
    }
}

void change_screen(Menu str)
{
	u8 tmp[20];

    bug = 0;
    init_i2c();
    start_i2c();
    //write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
//    write_i2c(0x02);
 //   write_i2c(0x40);
    //write_i2c(0x01);
    write_i2c(0xfa); // dire qu'on veut ecrire dans la DDRAM
    //write_i2c(0x13);
	if (screen != OLD_TRADES)
		nb_transaction = 0;
	choose_screen(str);
    stop_i2c();
}

void lcd(void) {
    TRISCbits.TRISC6 = 0;


    TMR2 = 0;
    T2CONbits.ON = 1;
	TMR2 = 0;
	while (TMR2 < 20000);
	int c = 0;
	while (c < 1000)
	{
		TMR2 = 0;
		while (TMR2 < 20000);
		c++;
	}


    /*Initialize I2C Peripheral*/

    init_i2c();

    init_keyboard();

	init_pin();
	init_amount();

	/*start_i2c();
	write_i2c(0xfa);
	stop_i2c();*/

	start_i2c();
	choose_screen(screen);
    stop_i2c();
	//   while(1);
}
