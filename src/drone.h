#ifndef DRONE
#define DRONE

#include "position.h"
#include "colis.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>
#include <string.h>
typedef struct{
	pthread_t id;
	pthread_mutex_t mutex;
	int type;
	int batterie;
	position pos;
	position destination;
	int statut; /* 0=Disponible 1=En charge 2=Livre 3=rentreSansColis 4=rentreAvecColis 5=PlusDeTravail*/
	mqd_t canalCom;
	colis * livraison;
	sem_t* controle[2];
	
}drone;

void droneT(void* args);
void rechargerBatterie(drone* dr);
#endif
