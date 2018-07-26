/* 
 * File:   crypto.h
 * Author: bocal
 *
 * Created on May 20, 2018, 3:17 AM
 */

#ifndef CRYPTO_H
#define	CRYPTO_H
#include "types.h"
#include <p32xxxx.h>
#include <time.h>
#include <xc.h>
#include <sys/attribs.h>


#define BUT_DOWN    0
#define BUT_UP      1

#ifdef	__cplusplus
extern "C" {

#endif




#ifdef	__cplusplus
}
#endif

typedef enum Menu Menu;

enum Menu

{
    MAIN, MAKE_TRADE1, MAKE_TRADE2, MAKE_TRADE3, AMOUNT, OLD_TRADES, HISTORY,
	CLIENT, SELLER, CLIENT2, SUCCED, SELLER2, CREATE, DELETE, BALANCE, RESET,
	ACCOUNT, PRIVATE1, PRIVATE2, PRIVATE3, PUBLIC1, PUBLIC2, PUBLIC3, ERROR,
    REP, TRANS, AMOUNT2, SETTINGS, SHUT_DOWN, BOOT
};

void    init_uart();

void send_char(char c);

char read_char(void);

void base(u8 c);

#endif	/* CRYPTO_H */

