#include "crypto.h"

extern Menu screen;

//RF2 => U1RX
//RF3 => U1TX



void    init_uart_rfid()
{
    U1MODEbits.BRGH = 1;                // 1 => Baud Rate = 38400 (0=> Baud Rate = 9600)
    U1BRG = 129;
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
    U1TXREG = c;                        // Transmit character
}

char read_char_rfid(void)
{
    //PORTDbits.RD15 = 0;                // Optional RTS use
    while(!U1STAbits.URXDA);             // Wait for information to be received
    //PORTDbits.RD15 = 1;
    return U1RXREG;                      // Return received character
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

   while ((c = read_char_rfid()) && c != '\0')
   {
       buf[i] = c;
       i++;
       if (i == 499)
           break ;
       //send_char(c);
   }
    buf[i] = '\0';
    i = 0;
    while (buf[i])
        send_char(buf[i++]);
}

void read_ras()
{
    u8 c;
    u8 buf[500];
    u32 i = 0;

   while ((c = read_char()) && c != '\0')
   {
       buf[i] = c;
       i++;
       if (i == 499)
           break ;
       //send_char(c);
   }
    buf[i] = '\0';
	//cmd_rfid();
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
	screen = MAIN;
	change_screen(screen);
         __builtin_disable_interrupts(); //pas d'interruptions possible
    IEC0bits.U1RXIE = 0; // disable interrupt on UART1
    __builtin_enable_interrupts();
    IFS0bits.U1RXIF = 0;
}

void init_interrupt_rfid() {
     __builtin_disable_interrupts(); //pas d'interruptions possible
    U1STAbits.URXISEL = 0x0;
   // U2STAbits.
    
    IEC0bits.U1RXIE = 1; // enable interrupt on UART1
    IPC6bits.U1IP = 6; // set priority
   // IPC6bits.U1IS = 2;
    IFS0bits.U1RXIF = 0; // clear interrupt flag
    INTCONbits.MVEC = 1; // interrupt multivector enabled
    __builtin_enable_interrupts();
}

