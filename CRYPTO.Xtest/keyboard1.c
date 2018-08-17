#define BUT_DOWN    0
#define BUT_UP      1
#include "crypto.h"


int test;
u8 pin[5];
u8 compteur = 0;
extern u8 pwd[5];

void init_pin()
{
    u8 i = 0;
    while (i < 4)
    {
        pin[i] = 0xC4;
        i++;
    }
    pin[i] = '\0';
}

void check_input(u8 letter)
{
    //base(letter); // for uart debug
    if (letter == 'E') // TODO: Afficher erreur sur ecran
        return ;
   // if (letter == 'C' && compteur > 0)
   //     compteur--;
  //  else if (compteur < 4) {
  //      pin[compteur] = letter;
  //      compteur++;
  //  }
}

void __ISR(_CHANGE_NOTICE_VECTOR, IPL3SOFT) ChangeNoticeHandler(void)
{
	    u8  res = 'E'; // code erreur
    if (PORTBbits.RB2 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line1();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    if (PORTBbits.RB1 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line2();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    if (PORTBbits.RB0 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line3();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    if (PORTBbits.RB4 == BUT_UP)
    {
          test = 0;
    }
    else
    {
        res = check_line4();
        check_input(res);
        set_col(0, 0, 0, 0);
    }

    PORTBbits.RB2;
    PORTBbits.RB1;
    PORTBbits.RB0;
    PORTBbits.RB4;
	IFS1bits.CNBIF = 0;
//    IFS1bits.CNIF = 0; // Be sure to clear the CN interrupt status

    // flag before exiting the service routine.
}

void init_col() // ok
{
    TRISCbits.TRISC2 = 0; //OUTPUT
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC0 = 0;
    TRISBbits.TRISB3 = 0;

    LATCbits.LATC2 = 0; // OUTPUT 0
    LATCbits.LATC1 = 0;
    LATCbits.LATC0 = 0;
    LATBbits.LATB3 = 0;
}

void init_row() //ok
{
    TRISBbits.TRISB2 = 1; //INPUT
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB4 = 1;

    CNPUBbits.CNPUB2 = 1;// internal pull up enabled
    CNPUBbits.CNPUB1 = 1;
    CNPUBbits.CNPUB0 = 1;
    CNPUBbits.CNPUB4 = 1;
}

void init_interrupt_keypad() //ok
{


    PORTBbits.RB2; //read port
    PORTBbits.RB1;
    PORTBbits.RB0;
    PORTBbits.RB4;

	//ANSELCbits.ANSC0 = 0;
	//ANSELCbits.ANSC1 = 0;
	//ANSELCbits.ANSC2 = 0;
	//ANSELBbits.ANSB3 = 0;

	ANSELBbits.ANSB2 = 0;
	ANSELBbits.ANSB1 = 0;
	ANSELBbits.ANSB0 = 0;
//NSELBbits.ANSB4 = 0;
	
    //AD1PCFGbits.PCFG0 = 1; // pin analogue, set as a 'digital pin'
    //AD1PCFGbits.PCFG1 = 1;
    //AD1PCFGbits.PCFG2 = 1;
    //AD1PCFGbits.PCFG3 = 1;

     __builtin_disable_interrupts(); //pas d'interruptions possible
    //CNCONBbits.ON = 1;
    CNENBbits.CNIEB2 = 1; // enable CN on RB0
    CNENBbits.CNIEB1 = 1;
    CNENBbits.CNIEB0 = 1;
    CNENBbits.CNIEB4 = 1;

	CNCONBbits.ON = 1;

	IPC8bits.CNIP = 3;
	IPC8bits.CNIS = 2;

    //IPC6bits.CNIP = 3;
    //IPC6bits.CNIS = 2;
    
	IFS1bits.CNBIF = 0;
	IEC1bits.CNBIE = 1;
	//IFS1bits.CNIF = 0;
    //IEC1bits.CNIE = 1;
    INTCONbits.MVEC = 1; // interrupt multivector enabled
    __builtin_enable_interrupts();
}

void init_keyboard()
{
    init_col();
    init_row();
    init_interrupt_keypad();
}

