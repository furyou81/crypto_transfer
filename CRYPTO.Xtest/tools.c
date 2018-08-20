#include "crypto.h"

void	init_timer()
{
	TMR2 = 0;
    T2CONbits.ON = 1;
}

/* Delay ne peut depasser 50000 */
void	set_delay(u32 delay)
{
	if (delay > 50000 || delay == 0)
		return ;
	TMR2 = 0;
	while (TMR2 < delay);
}