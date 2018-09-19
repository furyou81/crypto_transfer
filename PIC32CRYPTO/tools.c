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

int ft_strlen(char str[])
{
    int i;

    i = 0;
    while (str[i])
        i++;
	return (i);
}

u8 size_nb(u32 nb) {
    u8 s = 0;
    while (nb > 0) {
        nb = nb / 10;
        s++;
    }
    return (s);
}