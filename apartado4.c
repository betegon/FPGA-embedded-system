#include <stdio.h>
#include <pthread.h>


// Direcciones de los registros
#define INSERT_BASE 0xC5400000
#define VALOR_BASE  0xC5400004
#define TOTAL_BASE  0xC5400008
#define RESTO_BASE  0xC540000c

// Direcciones de memoria
#define LED_BASE    0x81420000
#define BOTON_BASE  0x81400000
#define SWITCH_BASE 0x81440000
#define SEG7_BASE  	0xce200000
#define SEG71_BASE  0xce200004
#define SEG72_BASE  0xce200008
#define SEG73_BASE  0xce20000c

// Valor de cada moneda
#define VALOR_MONEDA1 2
#define VALOR_MONEDA2 20
#define VALOR_MONEDA3 100

// Valor de cada bebida
#define VALOR_BEBIDA1 168
#define VALOR_BEBIDA2 198
#define VALOR_BEBIDA3 214

// Numero inicial de monedas en el monedero.
#define MONEDA1 5
#define MONEDA2 9
#define MONEDA3 9

// Numero de hilos
#define NUMERO_HILOS 4

// variables globales
int *LEDS 		= (int *)LED_BASE;
int *BOTONES 	= (int *)BOTON_BASE;
int *SWITCHES 	= (int *)SWITCH_BASE;
int *SEG7 		= (int *)SEG7_BASE;
int *SEG71		= (int *)SEG71_BASE;
int *SEG72 		= (int *)SEG72_BASE;
int *SEG73 		= (int *)SEG73_BASE;
int *INSERT 	= (int *)INSERT_BASE;
int *VALOR  	= (int *)VALOR_BASE;
int *TOTAL  	= (int *)TOTAL_BASE;
int *RESTO  	= (int *)RESTO_BASE;

// Global para que todos los hilos puedan acceder al mutex
pthread_mutex_t mutex;


/*
*
* MONEDAS:
* 	* 2   cts - boton1
* 	* 20  cts - boton2
* 	* 100 cts -boton 3
*
*/

int	ct2 	= MONEDA1;
int	ct20 	= MONEDA2;
int	ct100 	= MONEDA2;


/*
*Funcion: restar_moneda
-----------------------
*Resta una moneda del monedero
*/

void restar_moneda(){

	 if(((*BOTONES) & 0x1) == 1 ){
		cargar_valor();
		*INSERT = VALOR_MONEDA1;
		ct2 -= 1;
		*SEG71=ct2;

		while(((*BOTONES) & 0x1) == 1 );
	}

	else if(((*BOTONES) & 0x2) == 2 ){
		cargar_valor();
		*INSERT = VALOR_MONEDA2;
		ct20 -= 1;
		*SEG72=ct20;
		while(((*BOTONES) & 0x2) == 2 );
	}

	else if(((*BOTONES) & 0x4) == 4 ){
		cargar_valor();
		*INSERT = VALOR_MONEDA3;
		ct100 -= 1;
		*SEG73=ct100;
		while(((*BOTONES) & 0x4) == 4 );
	}
}


/*
*funcion: recargar_monedero
*--------------------------
*Suma las monedas que se van introduciendo al monedero
*/

void recargar_monedero(){
	
	if(((*BOTONES) & 0x1) == 1 ){
		ct2 += 1;
		*SEG71 = ct2;
		while(((*BOTONES) & 0x1) == 1 );
	}

	else if(((*BOTONES) & 0x2) == 2 ){
		ct20 += 1;
		*SEG72=ct20;
		while(((*BOTONES) & 0x2) == 2 );
	}

	else if(((*BOTONES) & 0x4) == 4 ){
		ct100 += 1;
		*SEG73=ct100;
		while(((*BOTONES) & 0x4) == 4 );
	}
}


/*
*funcion: cargar_valor
*---------------------
*Carga el valor correspondiente en funcion de que switch se ponga 1.
*/
void cargar_valor(){

	// AND primer, segundo y tercer switch: (0001, 0010, 0100)
	if ((((*SWITCHES) & 0x01) == 0x01)){
		*VALOR = VALOR_BEBIDA1; 
		}
	if ((((*SWITCHES) & 0x02) == 0x02)){
		*VALOR = VALOR_BEBIDA2;
	}
	if ((((*SWITCHES) & 0x04) == 0x04)){
		*VALOR = VALOR_BEBIDA3;
	}
}

void *funcion_hilo(){

	int bebida_num, tot, resto_anterior;
	pthread_mutex_lock(&mutex); // Bloquear el acceso
	
	bebida_num = 0;
	
	while(bebida_num < 1){		
		if(((*SWITCHES) & 0x7f) == 0){
			recargar_monedero();
		}
		else{
			restar_moneda();
			tot = *TOTAL;	
			LEDS = tot; 

			if (((*RESTO) & 0xff) == 0 && (resto_anterior !=0)){	
			bebida_num += 1;
			}
		resto_anterior= (*RESTO);
		}
	}	
	
	//desbloqueo
	pthread_mutex_unlock(&mutex); 
	// acaba el hilo y da paso al siguiente
    pthread_exit(NULL);
}



int main (){

	int bebida_num, tot, resto_anterior,i;
    pthread_t hilos[NUMERO_HILOS];	// generar 4 hilos
	pthread_attr_t attr; // atributos de los hilos.

    int rc; // para lo que devuelva pthread_create.

	*SEG71 = ct2;
	*SEG72 = ct20;
	*SEG73 = ct100;
	*INSERT 	= 0;
	*LEDS   	= 0;
	*SEG7   	= 0;
	bebida_num 	= 0;
	resto_anterior= 0;

	// inicializar mutex y atributos.
	pthread_mutex_init(&mutex, NULL); 
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


	for(;;){	
		for(i=0;i<NUMERO_HILOS;i++){
			bebida_num = 0;
			*SEG7 = i + 1;// mostrar en el primer caracter el hilo correspondiente
			
			rc = pthread_create(&hilos[i], NULL, funcion_hilo, (void *)i);
			pthread_join(hilos[i], NULL);
		}
	}
	
	
	// Desinicializa el mutex
	pthread_mutex_destroy(&mutex); 
	pthread_exit(NULL);
	return 0;
}
