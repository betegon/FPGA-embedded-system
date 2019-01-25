#include <stdio.h>

// Direcciones de los registros
#define INSERT_BASE 0xC5400000;
#define VALOR_BASE  0xC5400004;
#define TOTAL_BASE  0xC5400008;
#define RESTO_BASE  0xC540000c;

// Direcciones de memoria
#define LED_BASE    0x81420000;
#define BOTON_BASE  0x81400000;
#define SWITCH_BASE 0x81440000;
#define SEG7_BASE   0xce200000;

// Valor de cada moneda
#define VALOR_MONEDA1 2;
#define	VALOR_MONEDA2 20;
#define VALOR_MONEDA3 100;

// Valor de la bebida
#define VALOR_BEBIDA 220;

// variables globales
int *INSERT     = (int *)INSERT_BASE;
int *VALOR      = (int *)VALOR_BASE;
int *TOTAL      = (int *)TOTAL_BASE;
int *RESTO      = (int *)RESTO_BASE;

int *LEDS       = (int *)LED_BASE;
int *BOTONES    = (int *)BOTON_BASE;
int *SWITCHES   = (int *)SWITCH_BASE;
int *SEG7       = (int *)SEG7_BASE;

int main (){

	int tot, bebida_num,resto_anterior;

	// se carga valor con lo que cuesta la bebida
	*VALOR  = VALOR_BEBIDA;
	*INSERT = 0;
	*LEDS   = 0;
	*SEG7   = 0;
	
	bebida_num   = 0;
	resto_anterior = 0;
	for(;;){
		tot = *TOTAL; 
		*LEDS = tot; 

		// AND primer, segundo y tercer switch (0001, 0010, 01000)
		if(((*BOTONES) & 0x1) == 1 ){ 
			*INSERT = VALOR_MONEDA1;  
			while(((*BOTONES) & 0x1) == 1 );
		}

		else if(((*BOTONES) & 0x2) == 2 ){ 
			*INSERT = VALOR_MONEDA2;  
			while(((*BOTONES) & 0x2) == 2 );
		}

		else if(((*BOTONES) & 0x4) == 4 ){ 
			*INSERT = VALOR_MONEDA3;  
			while(((*BOTONES) & 0x4) == 4 );
		}

		if (((*RESTO) & 0xff) == 0 && (resto_anterior !=0)){ 
			bebida_num += 1;
			*SEG7 = bebida_num;
		}

		resto_anterior= (*RESTO);
	}
	
	return 0;
}
