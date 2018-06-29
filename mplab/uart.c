#include "crypto.h"



//RF4 => U2RX
//RF5 => U2TX

//RASPBERRY PI 38400

void    init_uart()
{
    U2MODEbits.BRGH = 1;                // 1 => Baud Rate = 38400 (0=> Baud Rate = 9600)
    U2BRG = 130;
    U2MODEbits.SIDL = 0;                // Continue operation in SLEEP mode
    U2MODEbits.IREN = 0;                // IrDA is disabled
    U2MODEbits.RTSMD = 0;               // U1RTS pin is in Flow Control mode
    U2MODEbits.UEN = 0b00;              // U1TX, U1RX are enabled
    U2MODEbits.WAKE = 1;                // Wake-up enabled
    U2MODEbits.LPBACK = 0;              // Loopback mode is disabled
    U2MODEbits.RXINV = 0;               // U1RX IDLE state is '1'
    U2MODEbits.PDSEL = 0b00;            // 8-bit data, no parity
    U2MODEbits.STSEL = 0;               // 1 stop bit
    U2STAbits.UTXINV = 0;               // U1TX IDLE state is '1'
    U2MODEbits.ON = 1;                  // UART1 is enabled
    U2STAbits.URXEN = 1;                // UART1 receiver is enabled
    U2STAbits.UTXEN = 1;                // UART1 transmitter is enabled
}

void send_char(char c)
{
    U2STAbits.UTXEN = 1;                // Make sure transmitter is enabled
    while(U2STAbits.UTXBF);             // Wait while buffer is full
    __builtin_disable_interrupts();
	U2TXREG = c;                        // Transmit character
	while(U2STAbits.UTXBF);
    __builtin_enable_interrupts();
}

char read_char(void)
{
	char k;
    //PORTDbits.RD15 = 0;                // Optional RTS use
    __builtin_disable_interrupts();
	while(!U2STAbits.URXDA);             // Wait for information to be received
	k = U2RXREG;
	__builtin_enable_interrupts();
    //PORTDbits.RD15 = 1;
    return k;                      // Return received character
}

void send_string(char *s)
{
    int i;

    i = 0;
    while (s[i])
    {
        send_char(s[i]);
        i++;
    }
}

void base(u8 c)
{
    u8 base[16] = "0123456789ABCDEF";
    u8 hex[2];
 //   u8 tmp = c;


    hex[1] = base[c % 16];
    c = c / 16;
    hex[0] = base[c % 16];

    init_uart();
 //   if (tmp > 15)
    send_char(hex[0]);
    send_char(hex[1]);

    send_char(0x20); // espace
}

u8 size_nb(u32 nb) {
    u8 s = 0;
    while (nb > 0) {
        nb = nb / 10;
        s++;
    }
    return (s);
}
void base10(u32 nb)
{
    u8 base[16] = "0123456789ABCDEF";
 //   u8 b10[11];
 //   u8 tmp = c;
    u8 size = 0;
    if (nb == 0)
        size = 1;
    else
        size = size_nb(nb);
    u8 b10[size + 1];
    u8 i = size - 1;
    while (nb >= 1) {
        b10[i--] = base[nb % 10];
        nb = nb / 10;
    }

    b10[size] = '\0';

    init_uart();
 //   if (tmp > 15)
    i = 0;
    send_char('{');
    send_string("amount:");
    while (b10[i])
        send_char(b10[i++]);

    send_char('}'); // espace
}

void start_transaction(u8 amount[]) {
    //TMR2 = 0;
    //while (TMR2 < 20000);
    send_string("new_transaction");
	send_char('{');
    send_string("amount:");
	send_string(amount);
	send_char('}');
	//base10(amount); // on envoi le montant de la transaction
    
}