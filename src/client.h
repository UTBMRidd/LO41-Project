#ifndef CLIENT
#define CLIENT
#include "position.h"
#include "colis.h"
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <mqueue.h>
typedef struct{
	pthread_t id;
	int number;
	pthread_mutex_t mutex;
	int cible;
	int present;
	int occupe;
	position pos;
	int servit;
	mqd_t canalCom;
	colis * livraison;
	sem_t controle[2];
}client;

void clientT(void* args);
void reception(void* args);
void cibleRandom(client* c);
void presenceRandom(client* c);
#endif
