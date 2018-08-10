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


/*
	1. Turn on the I2C module by setting the ON bit (I2CxCON<15>) to ?1?.
	2. Assert a Start condition on SDAx and SCLx.
	3. Send the I2C device address byte to the slave with a write indication.
	4. Wait for and verify an Acknowledge from the slave.
	5. Send the serial memory address high byte to the slave.
	6. Wait for and verify an Acknowledge from the slave.
	7. Send the serial memory address low byte to the slave.
	8. Wait for and verify an Acknowledge from the slave.
	9. Assert a Repeated Start condition on SDAx and SCLx.
	10. Send the device address byte to the slave with a read indication.
	11. Wait for and verify an Acknowledge from the slave.
	12. Enable master reception to receive serial memory data.
	13. Generate an ACK or NACK condition at the end of a received byte of data.
	14. Generate a Stop condition on SDAx and SCLx.
*/
void init_i2c()
{
//	DEVCFG1bits.FWDTEN = 0;
	TRISBbits.TRISB8 = 1;
    TRISBbits.TRISB9 = 1;
	CFGCONbits.IOLOCK = 0;
	PMD5bits.I2C1MD = 0;
	// 1. Turn on the I2C module by setting the ON bit (I2CxCON<15>) to ?1?.
	I2C1CONbits.ON = 0b1;  // 1. Turn on the I2C module by setting the ON bit
	// 2. Assert a Start condition on SDAx and SCLx.
	I2C1CONbits.SEN = 0b1; /*This bit is automatically cleared by MCU*/
	// 3. Send the I2C device address byte to the slave with a write indication.
    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
    // 4. Wait for and verify an Acknowledge from the slave.
	
	/* When I2C1 is enabled, all digital output-only
		functions and all analog functions on pin RA0 and
		RA1 do not function correctly.

		Work around
		Disable slew rate control of the I2C1 module by
		setting the DISSLW bit (I2C1CON<9>) = 1.
	 */
    I2C1CONbits.DISSLW = 1;
	
    I2C1BRG = 0x26;  /*Set Baud Rate Generator*/

	I2C1CONbits.SEN = HIGH; /*This bit is automatically cleared by MCU*/
    while (I2C1CONbits.SEN == HIGH); /*wait until start condition finishes*/


   /* LATCbits.LATC6 = 0; //reset
    TMR2 = 0;
    while (TMR2 < 1000);
    LATCbits.LATC6 = 1;
    TMR2 = 0;
    while (TMR2 < 500);*/

   // start_i2c();
    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
    write_i2c(0x3a);
    write_i2c(0x39);
    write_i2c(0x72); // setting contrast 70 a 7f
    write_i2c(0x54); //set contrst 2 50 a 5F
    write_i2c(0x6d);
    write_i2c(0x0c);
    write_i2c(0x01);


    stop_i2c(); // 14. Generate a Stop condition on SDAx and SCLx.
}


void start_i2c()
{
    I2C1CONbits.SEN = HIGH; /*This bit is automatically cleared by MCU*/
      int j;
    /*Test if START condition is completed - test interrupt I2C1MIF in IFS1*/
    while (I2C1CONbits.SEN == HIGH) /*wait until start condition finishes*/
    {
        ;
    }
}

void stop_i2c()
{
    I2C1CONbits.PEN = 1;
}



/*
	1. The software loads the I2CxTRN register with the data byte to transmit.
	2. Writing the I2CxTRN register sets the buffer full flag bit, TBF (I2CxSTAT<0>).
	3. The data byte is shifted out the SDAx pin until all eight bits are transmitted. Each bit of
	address/data will be shifted out onto the SDAx pin after the falling edge of SCLx.
	4. On the ninth SCLx clock, the I2C master shifts in the ACK bit from the slave device and
	writes its value into the ACKSTAT bit (I2CxSTAT<15>).
	5. The I2C master generates the master interrupt at the end of the ninth SCLx clock cycle
*/
void write_i2c(u8 c)
{
    I2C1TRN = c;

    /*Wait for transmit buffer empty - indicates that write is completed*/
    while (I2C1STATbits.TBF == 0b1); // Transmit in progress; I2CxTRN register is full (8-bits of data)

    /* Wait for ACK signal from device - bit will clear when ACK is received */
	while (I2C1STATbits.ACKSTAT == 1);
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
    write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
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

    /*Initialize I2C Peripheral*/
    //SYSTEMConfigPerformance(8000000);
    init_i2c();

    init_keyboard();
    start_i2c();
        write_i2c((CONST << 2 ) | (SA0 << 1) | WR);
       // write_i2c(0x81);
       write_i2c(0xfa); // dire qu'on veut ecrire dans la DDRAM
        //write_i2c(0x13);
       init_pin();
       init_amount();
       choose_screen(screen);
       stop_i2c();
}
