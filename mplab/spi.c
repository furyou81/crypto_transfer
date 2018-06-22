/*
 * File:   lcd_I2C.c
 * Author: bocal
 *
 * Created on May 20, 2018, 3:28 PM
 */

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
    wait_ready();;
    on();
    rSPI(SECTOR_ERRASE);
    rSPI((adress & (0xFF << 15)) >> 15);
    rSPI((adress & (0xFF << 7)) >> 7);
    rSPI(adress & (0xFF));
    
  //  TMR2 = 0;
  //  while (TMR2 < 10000);
    off();
	wait_ready();
   // TMR2 = 0;
    //while (TMR2 < 1000);
}


void chiperrase()
{
    on();
    rSPI(WREN);
    off();
    wait_ready();
    on();
    rSPI(ERRASE);
    off();
	wait_ready();

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
    //sector_erase(adress);
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
	wait_ready();
}

u8 readSPI(u32 address)
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
    return (0);
}

u8 checkSPI(u32 address)
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

    off();
    return (tab[0]);
}

void	read_zone(u32 depart, u32 fin)
{
	u32 i = depart;
	u8 ret;

	while (i < fin)
	{
		if (i % 20 == 0)
		{
			send_char('|');
			send_char(' ');
		}
		ret = readSPI(i);
		i++;
	}
}

u32		init_adr()
{
	u32 i = 0;
	u32 ret;

	while ((ret = checkSPI(i)) != 0xFF)
		i += 20;
	return (i);
}

void    get_date(u8 trans, u8 date[])
{
    u8 i = 0;
    while (i < 20)
    {
        date[i] = checkSPI(trans * 40 + i);
        i++;
    }
    date[i] = '\0';
}

void    get_amount(u8 trans, u8 tmp[])
{
    u8 i = 0;
    while (i < 20)
    {
        tmp[i] = checkSPI(trans * 40 + i + 20);
        i++;
    }
    tmp[i] = '\0';
}

void	write_line_SPI(u8 str[])
{
<<<<<<< HEAD
	u32 i = 0;
=======
	u8 i = 0;
>>>>>>> 50ae3133c708ee1f7b655807d9a3ae683639db74
	u32 adress = init_adr();
	while (str[i])
	{
		writeSPI(str[i], adress);
		i++;
		adress++;
	}
	while (i < 20)
	{
		writeSPI(' ', adress);
		adress++;
		i++;
	}
}

<<<<<<< HEAD
u32	count_max_transaction()
{
	u32 ret;
	u32 adr;
=======
u8	count_max_transaction()
{
	u8 ret;
	u8 adr;
>>>>>>> 50ae3133c708ee1f7b655807d9a3ae683639db74

	adr = init_adr();
	ret = adr / 40;
	return (ret);
}

void	flash(void) {
    initSPI();

    //read_status();
    change_status2();
	chiperrase();
<<<<<<< HEAD
	base(1);
	base(1);
	base(1);
	base(1);
	base(1);
=======
>>>>>>> 50ae3133c708ee1f7b655807d9a3ae683639db74
	write_line_SPI(" 12/12/2018         ");
	write_line_SPI(" 12,564878787 ETH   ");
	write_line_SPI(" 06/02/2018         ");
	write_line_SPI(" 53,564878787 ETH   ");
	write_line_SPI(" 09/08/2018         ");
	write_line_SPI(" 66,994878787 ETH   ");
	write_line_SPI(" 12/12/2018         ");
	write_line_SPI(" 12,564878787 ETH   ");
	write_line_SPI(" 06/02/2018         ");
	write_line_SPI(" 53,564878787 ETH   ");
	write_line_SPI(" 09/08/2018         ");
	write_line_SPI(" 66,994878787 ETH   ");
<<<<<<< HEAD
	write_line_SPI(" 10/08/2018         ");
	write_line_SPI(" 66,994878787 ETH   ");
	write_line_SPI(" 12/12/2018         ");
	write_line_SPI(" 12,564878787 ETH   ");
	write_line_SPI(" 06/02/2018         ");
	write_line_SPI(" 53,564878787 ETH   ");
	write_line_SPI(" 09/08/2018         ");
	write_line_SPI(" 66,994878787 ETH   ");
	write_line_SPI(" 12/12/2018         ");
	write_line_SPI(" 12,564878787 ETH   ");
	write_line_SPI(" 06/02/2018         ");
	write_line_SPI(" 53,564878787 ETH   ");
	write_line_SPI(" 09/08/2018         ");
	write_line_SPI(" 66,994878787 ETH   ");
	write_line_SPI(" 10/08/2018         ");
	write_line_SPI(" 66,994878787 ETH   ");
//read_zone(0, 300);
=======
	read_zone(0, 300);
>>>>>>> 50ae3133c708ee1f7b655807d9a3ae683639db74
}