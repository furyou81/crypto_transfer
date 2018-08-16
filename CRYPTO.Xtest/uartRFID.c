#include "crypto.h"

extern Menu screen;
extern u8 progress;
extern u8 rep[];
u8 done = 0;
u8 ras = 0;
u8 key = 0;
//RF2 => U1RX
//RF3 => U1TX



void    init_uart_rfid()
{
	RPC5R = 0b0001; // U1TX
	U1RXR = 0b0010; // RPA4
    U1MODEbits.BRGH = 1;                // 1 => Baud Rate = 38400 (0=> Baud Rate = 9600)
    U1BRG = 130;
    U1MODEbits.SIDL = 0;                // Continue operation in SLEEP mode
    U1MODEbits.IREN = 0;                // IrDA is disabled
   // U1MODEbits.RTSMD = 0;               // U1RTS pin is in Flow Control mode
    U1MODEbits.UEN = 0b00;              // U1TX, U1RX are enabled
    U1MODEbits.WAKE = 1;                // Wake-up enabled
    U1MODEbits.LPBACK = 0;              // Loopback mode is disabled
    U1MODEbits.RXINV = 0;               // U1RX IDLE state is '1'
    U1MODEbits.PDSEL = 0b00;            // 8-bit data, no parity
    U1MODEbits.STSEL = 0;               // 1 stop bit
    U1STAbits.UTXINV = 0;               // U1TX IDLE state is '1'
    U1MODEbits.ON = 1;                  // UART1 is enabled
    U1STAbits.URXEN = 1;                // UART1 receiver is enabled
    U1STAbits.UTXEN = 1;                // UART1 transmitter is enabled
}

void send_char_rfid(char c)
{
    U1STAbits.UTXEN = 1;                // Make sure transmitter is enabled
    while(U1STAbits.UTXBF);             // Wait while buffer is full
     __builtin_disable_interrupts(); //pas d'interruptions possible
	U1TXREG = c;
	while(U1STAbits.UTXBF);
	__builtin_enable_interrupts();
	TMR2 = 0;
	while (TMR2 < 10000);// Transmit character
}

char read_char_rfid(void)
{
	char k;

    __builtin_disable_interrupts();
	while(!U1STAbits.URXDA);             // Wait for information to be received
	k = U1RXREG;
	__builtin_enable_interrupts();
    return k;
}

void send_string_rfid(char *s)
{
    int i;

    i = 0;
    while (s[i])
    {
        send_char_rfid(s[i]);
        i++;
    }
}

void cmd_rfid(char *cmd)
{
    send_string_rfid(cmd);
    send_char_rfid(13); // ENTER
    //send_char_rfid('e');
    //send_char_rfid('r');
    //send_char_rfid('2');
    //send_char_rfid(',');
    //send_char_rfid('8');
    //send_char_rfid(13); // ENTER
    //TMR2 = 0;
    //while (TMR2 < 1000);
}

void read_rfid()
{
    u8 c;
    u8 buf[500];
    u32 i = 0;
	u8 chevron = 0;

   while ((c = read_char_rfid()))
   {
	   /*if (c == '>')
		   chevron++;
	   if (screen == MAKE_TRADE2 && c == '>')
		   break ;
	   if (screen == CLIENT2 && chevron <= 1)
		   continue ;
	    if (screen == CLIENT2 && chevron >= 3)
			break ;*/
       buf[i] = c;
	   if (i > 4 && buf[i] == ':' && buf[i - 1] == 'R' && buf[i - 2] == 'R' && buf[i - 3] == 'E' && screen == CLIENT2)
	   {
		   screen = ERROR;
		   change_screen(screen);
		   done = 1;
		   break ;
	   }
       i++;
       if (i == 499)
           break ;
       //send_char(c);
   }
    buf[i] = '\0';
    i = 0;
    if (screen != ERROR)
    {
        while (buf[i])
            send_char(buf[i++]);
    }
}

void	read_ras(char *buf)
{
    u8 c;
    u32 i = 0;

	
	//while (ras == 0);
	//ras = 0;
	while ((c = read_char()) && i < 100)
	{
		buf[i] = c;
		i++;
	}
    buf[i] = '\0';
}

void	ft_strcpy(u8 *dst, u8 *src)
{
	u8 i = 0;

	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

void	send_private_key(char *buf)
{
	u8 compteur = 1;
	u8 i = 0;
	u8 k = 0;

	done = 0;
	
	while (compteur < 3)
	{
		k = i;
		
		send_string_rfid("ew");
		send_char_rfid(compteur + 48);
		send_char_rfid(',');
		while (i < 32 + k && buf[i])
		{
			send_char_rfid(buf[i]);
			i++;
		}
		send_char_rfid(13);
		init_interrupt_rfid();
		while (done == 0);
		done = 0;
		if (screen == ERROR)
			break ;
		compteur++;
		progress += 6;
		change_screen(screen);
	}
	
}

void	send_public_key(char *buf)
{
	u8 compteur = 4;
	u8 i = 0;
	u8 k = 0;

	while (compteur < 6)
	{
		k = i;

		send_string_rfid("ew");
		send_char_rfid(compteur + 48);
		send_char_rfid(',');
		while (i < 32 + k && buf[i])
		{
			send_char_rfid(buf[i]);
			i++;
		}
		send_char_rfid(13);
		init_interrupt_rfid();
		while (done == 0);
		done = 0;
		if (screen == ERROR)
			break ;
		compteur++;
		progress += 6;
		change_screen(screen);
	}
	
}

void base_rfid(u8 c)
{
    u8 base[16] = "0123456789ABCDEF";
    u8 hex[2];
 //   u8 tmp = c;


    hex[1] = base[c % 16];
    c = c / 16;
    hex[0] = base[c % 16];

    init_uart_rfid();
 //   if (tmp > 15)
    send_char_rfid(hex[0]);
    send_char_rfid(hex[1]);

    send_char_rfid(0x20); // espace
}

void send_transaction_amount(u32 amount) {
    base10(amount);
}

void __ISR(_UART1_VECTOR, IPL6SOFT) IntUart2Handler(void) {

    read_rfid();
    if (screen == MAKE_TRADE2)
    {
        screen = TRANS;
        change_screen(screen);
    }
	done = 1;
    IEC1bits.U1RXIE = 0; // disable interrupt on UART1
    IFS1bits.U1RXIF = 0;
}

void init_interrupt_rfid() {
     __builtin_disable_interrupts(); //pas d'interruptions possible
    U1STAbits.URXISEL = 0x0;
   // U2STAbits.
    
    IEC1bits.U1RXIE = 1; // enable interrupt on UART1
    IPC8bits.U1IP = 6; // set priority
   // IPC6bits.U1IS = 2;
    IFS1bits.U1RXIF = 0; // clear interrupt flag
    INTCONbits.MVEC = 1; // interrupt multivector enabled
    __builtin_enable_interrupts();
}

void __ISR(_UART2_VECTOR, IPL6SOFT) IntUart3Handler(void) {
	ras = 1;
	read_ras(rep);
	IEC1bits.U2RXIE = 0; // disable interrupt on UART1
	IFS1bits.U2RXIF = 0;
}

void init_interrupt_ras() {
	u8 i = 0;
     __builtin_disable_interrupts(); //pas d'interruptions possible
    U2STAbits.URXISEL = 0x0;
   // U2STAbits.
	while (i < 100)
	{
		rep[i] = 0;
		i++;
	}
    IEC1bits.U2RXIE = 1; // enable interrupt on UART1
    IPC9bits.U2IP = 6; // set priority
   // IPC6bits.U1IS = 2;
    IFS1bits.U2RXIF = 0; // clear interrupt flag
    INTCONbits.MVEC = 1; // interrupt multivector enabled
    __builtin_enable_interrupts();
}

