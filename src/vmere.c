#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h> 
#include "vmere.h"
#include "utils.h"
#include "colis.h"
#include <string.h>
#include <semaphore.h>
#include <time.h>



vmere* creationVaisseauMere(int nbDrone,int nbColis,int nbClient){
	drone** drones = malloc(nbDrone * sizeof(drone));
	client** clients = malloc(nbClient * sizeof(client));
	mqd_t** messagerie = malloc(nbClient * sizeof(mqd_t));

	LLColis* cargaison = NULL;

	struct mq_attr	attr;
	attr.mq_flags = 0;
  	attr.mq_maxmsg = 10;
  	attr.mq_msgsize = 512;
  	attr.mq_curmsgs = 0;
	printf("Création d'un vaisseau mère\n");
	for(short i=0;i<nbColis;i++){
		int clientID = (rand()%nbClient);
		int priorite = (rand()%5) +1;
		int taille = (rand()%5) +1;
		cargaison = addColis(cargaison,i,clientID,priorite,taille);
	}
	printf("Cargaison chargé mon capitaine !\n");
	for(short i=0;i<nbClient;i++){
		clients[i] = (client*)malloc(sizeof(client));
		char* canalV=concat_num(i);
		messagerie[i] = mq_open(canalV, O_RDWR | O_CREAT, 0644, &attr);

		free(canalV);

		if (pthread_create(&clients[i]->id, NULL, clientT, clients[i])) { 
        		perror("Erreur création des clients"); 
        		exit(EXIT_FAILURE); 
    		}
		pthread_mutex_init(&clients[i]->mutex,NULL);
		pthread_mutex_lock(&clients[i]->mutex);
		clients[i]->canalCom = messagerie[i];
		clients[i]->number = i ;
		clients[i]->occupe = 0 ;
		clients[i]->pos.x = (rand()%6) ;
		clients[i]->pos.y = (rand()%6) ;
		sem_init(&(clients[i]->controle[0]), 0,-1);
		sem_init(&(clients[i]->controle[1]), 0,-1);
		pthread_mutex_unlock(&clients[i]->mutex);
	}
		printf("Clients pret\n");
	for(short i=0;i<nbDrone;i++){
		
		drones[i] = (drone*)malloc(sizeof(drone));
		if (pthread_create(&drones[i]->id, NULL, droneT, drones[i])) { 
        		perror("Erreur création des drones"); 
        		exit(EXIT_FAILURE); 
    		}
		pthread_mutex_init(&drones[i]->mutex,NULL);
		pthread_mutex_lock(&drones[i]->mutex);
    		drones[i]->batterie = 100;
		drones[i]->statut = 0;
		drones[i]->type = (rand()%5) +1;
		pthread_mutex_unlock(&drones[i]->mutex);
    }
	vmere* vaisseau = malloc(sizeof(vmere));
	vaisseau->drones = drones;
	vaisseau->clients = clients;
	vaisseau->cargaison = cargaison;
	vaisseau->messagerie = messagerie;
	vaisseau->nbClients = nbClient;
	vaisseau->nbColis = nbColis;
	vaisseau->nbDrones = nbDrone;

return vaisseau;
}
void deleteVaisseauMere(vmere* v){
	for(short i=0;i<v->nbClients;i++){
		mq_close(v->messagerie[i]);
		mq_unlink(concat_num(i));
		sem_destroy(&(v->clients[i]->controle[0]));
		sem_destroy(&(v->clients[i]->controle[1]));
		pthread_mutex_destroy(&v->clients[i]->mutex);
		free(v->clients[i]);
	}
	for(short i=0;i<v->nbDrones;i++){
		pthread_mutex_destroy(&v->drones[i]->mutex);
		free(v->drones[i]);
	}
	free(v->drones);
	free(v->clients);
	free(v->cargaison);
	free(v);
}
colis* getBestColis(int tailleDrone,LLColis* list){

	colis* choice = malloc(sizeof(colis));
	choice->priorite = -1;
	

	LLColis* elem = list;
	while(elem!=NULL){
		if(elem->val->taille <= tailleDrone){
			if(choice->priorite == -1){
				transfertColis(elem->val,choice);
			}
			else if(elem->val->priorite > choice->priorite){	
				transfertColis(elem->val,choice);
			}
			else if(elem->val->priorite == choice->priorite){
				if(elem->val->taille > choice->taille){
					transfertColis(elem->val,choice);
				}		
			}
			
			
		}
		elem = elem->next;
	}
	if(choice->priorite == -1){
		free(choice);
		choice = NULL;
	}
		return choice;

}

void assignationDesColis(vmere* v){
	int r = 0;
	while(r!=v->nbDrones){

		for(short i=0;i<v->nbDrones;i++){
			pthread_mutex_lock(&v->drones[i]->mutex);
			pthread_mutex_lock(&v->clients[i]->mutex);
			if(v->drones[i]->statut == 0){
				v->drones[i]->livraison = getBestColis(v->drones[i]->type,v->cargaison);
				if(v->drones[i]->livraison != NULL){

					v->drones[i]->canalCom = v->messagerie[v->drones[i]->livraison->clientID];
					v->drones[i]->controle[0] = &(v->clients[v->drones[i]->livraison->clientID]->controle[0]);
					v->drones[i]->controle[1] = &(v->clients[v->drones[i]->livraison->clientID]->controle[1]);
					v->drones[i]->destination.x = v->clients[v->drones[i]->livraison->clientID]->pos.x;
					v->drones[i]->destination.y = v->clients[v->drones[i]->livraison->clientID]->pos.y;
					v->cargaison = removeById(v->cargaison,v->drones[i]->livraison->id);
					
					if(v->clients[v->drones[i]->livraison->clientID]->occupe == 1){
						v->drones[i]->statut = 3;
					}
					else{
						v->clients[v->drones[i]->livraison->clientID]->occupe = 1;
						v->drones[i]->statut = 2;
					}
				}
				else{
					r++;
					v->drones[i]->statut = 5;
				}	
			}
			else if(v->drones[i]->statut == 4){
			
			v->cargaison = addColis(v->cargaison,v->drones[i]->livraison->id,v->drones[i]->livraison->clientID,v->drones[i]->livraison->priorite,v->drones[i]->livraison->taille);
			free(v->drones[i]->livraison);
			v->drones[i]->livraison = NULL;
			v->drones[i]->statut = 0;
			printf("RETOUR DU COLIS\n");
			}
			else if(v->drones[i]->statut == 3){
				if(v->clients[v->drones[i]->livraison->clientID]->occupe == 0){
					v->clients[v->drones[i]->livraison->clientID]->occupe = 1;
					v->drones[i]->statut = 2;
				}
			}
			pthread_mutex_unlock(&v->clients[i]->mutex);
			pthread_mutex_unlock(&v->drones[i]->mutex);
		}
	}
	for(short i=0;i<v->nbClients;i++){
		v->clients[i]->servit =1;
	}
}

int main(int argc, char *argv[])
{
	if(argc !=4){
		printf("Erreur format : ./Programme nbDrones nbColis nbClients\n");
		return EXIT_SUCCESS;
	}
	srand(time(NULL));
	vmere* v;
	v = creationVaisseauMere(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
	drawList(v->cargaison);
	assignationDesColis(v);
	drawList(v->cargaison);
	for(short i=0;i<v->nbDrones;i++){
		pthread_join(v->drones[i]->id, NULL);
	}
	for(short i=0;i<v->nbClients;i++){
		pthread_join(v->clients[i]->id, NULL);
	}
	printf("Suppression du vaisseau\n");
	deleteVaisseauMere(v);
	printf("Fin des opérations\n");
	

    return EXIT_SUCCESS;
}

