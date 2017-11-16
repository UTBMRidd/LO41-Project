
#ifndef VMERE
#define VMERE

#include <pthread.h> 
#include "drone.h"
#include "client.h"
#include "colis.h"
#include <fcntl.h>
#include <sys/stat.h> 
#include <mqueue.h>

typedef struct{
	mqd_t** messagerie;
	drone** drones;
	client** clients;
	LLColis* cargaison;
	int nbClients;
	int nbDrones;
	int nbColis;
}vmere;

vmere* creationVaisseauMere(int nbDrone,int nbColis,int nbClient);
void deleteVaisseauMere(vmere* v);
colis* getBestColis(int tailleDrone,LLColis* list);
void assignationDesColis(vmere* v);
#endif
