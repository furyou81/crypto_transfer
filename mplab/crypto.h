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

    MAIN, MAKE_TRADE1, MAKE_TRADE2, MAKE_TRADE3, OLD_TRADES, ADD_KEY

};

void    init_uart();

void send_char(char c);

char read_char(void);

void base(u8 c);

#endif	/* CRYPTO_H */

