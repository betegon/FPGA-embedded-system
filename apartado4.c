#include <pthread.h>

#define NUMERO_HILOS 4


void lo_que_quiero_que_haga_el_hilo(){
    
    printf("soy hilo y hago cosass de hilos");

    // acabo el hilo y paso al siguiente
    pthread_exit(NULL);
}


int main (){

    // generar 4 hilos
    pthread_t hilos[4]; //Se crean "numero" hilos(11 en futbol)

    int i; // contador del ciclo
    int rc; // phtread_create nos devuelve  si ha ido bien y sino pues fue mal 


    
    for(;;) // for infinito que tenemos en el programa de bebidas
        
        // iremos haciendo estos ciclos siempre (por eso dentro del for infinito) primero hilo1, despues el 2,3,4 y vuelta a empezar
        for(i=0; i<NUMERO_HILOS; i++) 
        printf("En main: creo thread %ld\n", i);    
        // creo el hilo
        rc = pthread_create(&hilos[i], NULL, lo_que_quiero_que_haga_el_hilo, (void *)i);

}



/// como hacer que el resto de hilos esperen con semaforos??? DIAPOSITIVAS DE RAFA PONE:

/*
Es manejada por dos funciones atómicas:
– Wait. Decrementa el valor de la variable semáforo S y bloquea al proceso:
Wait(s)
{
Espera activa: cambiar if
por while y sin bloqueo
s=s-1;
if(s<0) Bloquear proceso que lo llamó;
}
– Signal. Lo aumenta y despierta al que esté bloqueado:
signal(s)
{
s=s+1;
if(s<=0) Desbloquear proceso parado;
}
• En la implementación del semáfor
*/

Wait(s){
s=s-1;
if(s<0) 
}

signal(s){
s=s+1;
if(s<=0) 
}



