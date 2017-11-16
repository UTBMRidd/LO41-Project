#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include <signal.h>
#include <time.h>

void clientT(void* args){
	srand(time(NULL));
	client* c = (client*) args;
	pthread_mutex_lock(&c->mutex);
	waitForMessage(c->canalCom,reception,c);
	c->present = 1;
	printf("Client :%d\tJe suis à la maison\n",c->number);
	c->cible = 1;
	printf("Client :%d\tJ'ai posé ma cible\n",c->number);
	pthread_mutex_unlock(&c->mutex);
	while(c->servit == 0){
		if(c->occupe == 0){
			cibleRandom(c);
			presenceRandom(c);
		}
		sleep(1);
	}
	return NULL;
}
void cibleRandom(client* c){
	int ra = (rand()%20);
	if(ra==19){
		printf("Client :%d\tJe tonds ma pelouse, je dois enlever ma cible\n",c->number);
		pthread_mutex_lock(&c->mutex);
		c->cible=0;
		pthread_mutex_unlock(&c->mutex);
		sleep(10);
		printf("Client :%d\tJ'ai fini de tondre, je remets la cible\n",c->number);
		pthread_mutex_lock(&c->mutex);
		c->cible=1;
		pthread_mutex_unlock(&c->mutex);
	}

}
void presenceRandom(client* c){
	int ra = (rand()%20);
	if(ra==19){
		printf("Client :%d\tJe m'absente\n",c->number);
		pthread_mutex_lock(&c->mutex);
		c->present=0;
		pthread_mutex_unlock(&c->mutex);
		sleep(10);
		printf("Client :%d\tJe suis rentré\n",c->number);
		pthread_mutex_lock(&c->mutex);
		c->present=1;
		pthread_mutex_unlock(&c->mutex);
	}

}


void reception(void* args){
	client* c = (client*) args;
	pthread_mutex_lock(&c->mutex);
	c->occupe = 1;
	getValue(c->canalCom);
	printf("\nClient :%d\tle drone est chez moi\n",c->number);
	sem_post(&(c->controle[0]));
	sendValue(c->canalCom,c->cible,0);
	if(c->cible== 0){
		waitForMessage(c->canalCom,reception,c);
		c->occupe = 0;
		pthread_mutex_unlock(&c->mutex);
		return NULL;
	}
	sendValue(c->canalCom,c->present,0);
	if(c->present== 0){
		waitForMessage(c->canalCom,reception,c);
		c->occupe = 0;
		pthread_mutex_unlock(&c->mutex);
		return NULL;
	}
	sem_wait(&(c->controle[1]));
	c->livraison = getColis(c->canalCom);
	printf("Client :%d\tJ'ai reçu mon colis :%d\n\n",c->number,c->livraison->id);
	waitForMessage(c->canalCom,reception,c);
	c->occupe = 0;
	pthread_mutex_unlock(&c->mutex);
	return NULL;
	
}

