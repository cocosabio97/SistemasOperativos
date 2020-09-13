#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#define N_PARADAS  5   // numero de paradas de la ruta
#define EN_RUTA    0   // autobus en ruta
#define EN_PARADA  1   // autobus en la parada
#define MAX_USUARIOS 40  // capacidad del autobus
#define USUARIOS   4   // numero de usuarios

// estado inicial
int estado        = EN_RUTA;
int parada_actual = 0;     // parada en la que se encuentra el autobus
int n_ocupantes   = 0;     // ocupantes que tiene el autobus


// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS];  //= {0,0,...0};

// Otras definiciones globales (comunicacion y sincronizacion)


// variable del mutex
pthread_mutex_t mutex; 
// variables condicionales ??
pthread_cond_t autobus;
pthread_cond_t usuario;

void Autobus_En_Parada(){
/*Ajustar el estado y bloquear al autobus hasta que no haya pasajeros que
quieran bajar y/o subir la parada actual. Despues se pone en marcha*/
	pthread_mutex_lock(&mutex);
	estado = EN_PARADA;
	printf("El autobus se ha parado en la paradada: %d\n", parada_actual);
	pthread_cond_broadcast(&usuario);
	while(esperando_parada[parada_actual] > 0 || esperando_bajar[parada_actual] > 0){		
		pthread_cond_wait(&autobus, &mutex);	
	}	

	pthread_mutex_unlock(&mutex);
}



void Conducir_Hasta_Siguiente_Parada(){
//Establecer un Retardo que simule el trayecto y actualizar numero de parada 
	
	pthread_mutex_lock(&mutex);
	estado = EN_RUTA;
	pthread_mutex_unlock(&mutex);
	sleep(5);
	pthread_mutex_lock(&mutex);
	parada_actual++;
	if (parada_actual == N_PARADAS) {
		parada_actual = 0;
	}
	//estado = EN_PARADA;
	pthread_mutex_unlock(&mutex);

}

/*
void Conducir_Hasta_Siguiente_Parada(){
//Establecer un Retardo que simule el trayecto y actualizar numero de parada
	pthread_mutex_lock(&mutex);

	estado = EN_RUTA;

	//parada_actual = (parada_actual +1) % N_PARADAS;
	if(parada_actual == 4){
		parada_actual = 0;
	}else{
		parada_actual++;
	}
	printf("conduciendo hasta la siguente parada... \n");
	sleep(3);
	
	pthread_mutex_unlock(&mutex);
} */



void Subir_Autobus(int id_usuario, int origen){
/*El usuario indicara que quiere subir en la parada ’origen’, esperara a que
el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
proporcionar informacion de depuracion*/

	pthread_mutex_lock(&mutex);
	//printf("Soy %d y quiero subir en la parada %d \n",id_usuario , origen);
	esperando_parada[origen]++;

	pthread_cond_signal(&autobus);
	while(estado != EN_PARADA || parada_actual != origen){	
		pthread_cond_wait(&usuario,&mutex);
	}
	esperando_parada[origen]--;	
	n_ocupantes++;
	printf("Soy %d y quiero subir en la parada %d \n",id_usuario , origen);
	//pthread_cond_signal(&autobus);
	// usar numero de usuarios porque si hay 4 no se puede subir al bus
	pthread_mutex_unlock(&mutex);

}



void Bajar_Autobus(int id_usuario, int destino){
/*El usuario indicara que quiere bajar en la parada ’destino’, esperara a que
el autobus se pare en dicha parada y bajara. El id_usuario puede utilizarse para
proporcionar informacion de depuracion*/

	pthread_mutex_lock(&mutex);
	//printf("Soy %d y quiero bajarme en la parada %d \n",id_usuario , destino);
	esperando_bajar[destino]++;
	pthread_cond_signal(&autobus);
	while (estado != EN_PARADA || parada_actual != destino){
		
		pthread_cond_wait(&usuario,&mutex);		
		
	}
	esperando_bajar[destino]--;
	n_ocupantes--;
	printf("Soy %d y quiero bajarme en la parada %d \n",id_usuario , destino);
	//pthread_cond_signal(&autobus);
	pthread_mutex_unlock(&mutex);
}


void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus este en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estacion destino
	Bajar_Autobus(id_usuario, destino);
	
}
 

 
void* thread_autobus(void* args) {
	while (1) {
	// esperar a que los viajeros suban y bajen
	Autobus_En_Parada();

	// conducir hasta siguiente parada
	Conducir_Hasta_Siguiente_Parada();
	}
}


void* thread_usuario(void* arg) {
	int id_usuario = (int) arg;
	int a,b;
	
	// obtener el id del usario
	//id_usuario = pthread_self();
	//printf("Soy el usuario: %d\n", id_usuario );
	while (1) {
		a=rand()%N_PARADAS;
		do{
			b=rand()%N_PARADAS;
		} while(a==b);
		Usuario(id_usuario,a,b);
	}
}






int main(int argc, char* argv[]){
	// Definicion de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	int i;

	// variables de los hilos
	pthread_t users[USUARIOS];
	pthread_t bus;
	
	// Inicializamos el mutex
	pthread_mutex_init(&mutex,NULL);
	// Inicializamos las varibles de condicion
	pthread_cond_init(&autobus,NULL);
	pthread_cond_init(&usuario,NULL);
	
	/*	Crear el thread Autobus
		&autobus --> variable de tipo pthread_t
		thread_autobus--> funcion a ejecutar por el hilo*/
	
	for (i = 0; i<USUARIOS; i++){
	// Crear thread para el usuario i
	//	(void*)i --> es por si quieres pasarle a la funcion thread_usuario un argumento
		pthread_create(&users[i],NULL,thread_usuario,(void*)i);
	}

	pthread_create(&bus,NULL,thread_autobus,NULL);

	// Esperar terminacion de los hilos
	pthread_join(bus,NULL);
	for ( int i = 0; i<USUARIOS; i++){		
		pthread_join(users[i],NULL);
	}

	// Destruccion del mutex
	pthread_mutex_destroy(&mutex);
	// Destruccion de las variables de condicion
	pthread_cond_destroy(&autobus);
	pthread_cond_destroy(&usuario);

	return 0;
}

