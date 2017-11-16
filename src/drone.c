#include <stdio.h>
#include <stdlib.h>
#include "drone.h"



void droneT(void* args){
	drone* d = (drone*) args;
	printf("Je suis le drone tout neuf de taille %d\n",d->type);

	while(d->statut != 5){
		if(d->statut == 0){

			printf("J'attends du travail\n");
			sleep(2);
		}
		else if(d->statut == 1){
			rechargerBatterie(d);
		}
		else if(d->statut == 2){
			printf("J'ai du travail\n");
			livraison(d);
		}
		else if(d->statut == 3){
			pthread_mutex_lock(&d->mutex);
			d->destination.x=0; 
			d->destination.y=0;
			/*Retour au vaisseau mère*/
			seRendreSurPosition(d);
			d->statut = 1;
			pthread_mutex_unlock(&d->mutex);
		}
	}

	printf("Plus de travail\n");
	return NULL;
}

void livraison(drone* d){

	pthread_mutex_lock(&d->mutex);
	seRendreSurPosition(d);
	printf("Je suis chez le client : %d\n",d->livraison->clientID);
	sendValue(d->canalCom,42,0);
	sem_wait(d->controle[0]);
	int cible = getValue(d->canalCom);
	if(cible == 0){
		printf("La cible n'est pas posé, je rentre\n");
		d->statut = 3;
		pthread_mutex_unlock(&d->mutex);
		return NULL;
	}
	int dispo = getValue(d->canalCom);
	if(dispo == 0){
		printf("Le client n'est pas là, je rentre\n");
		d->statut = 3;
		pthread_mutex_unlock(&d->mutex);
		return NULL;
	}
	sendColis(d->livraison,d->canalCom);
	sem_post(d->controle[1]);
	sleep(2);
	d->statut =3;
	pthread_mutex_unlock(&d->mutex);
	return NULL;

}
void seRendreSurPosition(drone* d){
	printf("En voyage\n");
	while(d->pos.x != d->destination.x){
		d->batterie = d->batterie -1;
		if(d->pos.x < d->destination.x){
			d->pos.x += 1;
			sleep(1);
		}
		if(d->pos.x > d->destination.x){
			d->pos.x -= 1;
			sleep(1);
		}
	}
	while(d->pos.y != d->destination.y){
		d->batterie = d->batterie -1;
		if(d->pos.y < d->destination.y){
			d->pos.y += 1;
			sleep(1);
		}
		if(d->pos.y > d->destination.y){
			d->pos.y -= 1;
			sleep(1);
		}
	}
	printf("Le drone est arrivé sur place\n");
}
void rechargerBatterie(drone* dr){
	printf("La batterie est à %d %\n",dr->batterie);
	pthread_mutex_lock(&dr->mutex);
	while(dr->batterie < 100){
		sleep(0.5);
		dr->batterie += 1;
	}
	printf("Batterie rechargé à %d %\n",dr->batterie);
	dr->statut = 0;
	pthread_mutex_unlock(&dr->mutex);
}

