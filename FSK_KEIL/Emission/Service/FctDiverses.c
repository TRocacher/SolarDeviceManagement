#include "FctDiverses.h"

/* =============================================
				DIVERS
 ============================================= */
/*
Fonction permettant d'attendre environ x ms
Procède par boucle while , donc peu précis.
Testé pour CPU à 72MHz
*/
void Delay_x_ms(int x)
{
	int i;
	int j;
	i=0;
	while (i<x)
	{
		j=0;
		while (j<11990)
		{
			j++;
		}
		i++;
	}		
}

