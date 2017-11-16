#include "colis.h"
#include <stdlib.h>
  
LLColis* addColis(LLColis* ll,int id,int clientID,int prio,int taille)
{
    	LLColis* nouvelElement = malloc(sizeof(LLColis));
	nouvelElement->val=malloc(sizeof(colis));
    	nouvelElement->val->id = id;
	nouvelElement->val->clientID = clientID;
    	nouvelElement->val->priorite = prio;
    	nouvelElement->val->taille = taille;
	nouvelElement->next = NULL;

    if(ll == NULL)
    {
        return nouvelElement;
    }
    else
    {       
	LLColis* temp=ll;

        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = nouvelElement;
        return ll;
    }
}


LLColis* removeById(LLColis* ll,int id)
{
    if(ll == NULL)
        return NULL;
 
    if(ll->val->id == id)
    {
        LLColis* tmp = ll->next;
        free(ll);
        tmp = removeById(tmp, id);
        return tmp;
    }
    else
    {
        ll->next = removeById(ll->next, id);
        return ll;
    }
}

void drawList(LLColis* ll){
printf("Nouvelle Liste:\n \n");
if(ll == NULL){
	printf("La liste est vide\n");
}else{
LLColis* tmp = ll;
printf("ID : %d\tClient : %d\tPriorite : %d\tTaille : %d\n",tmp->val->id,tmp->val->clientID,tmp->val->priorite,tmp->val->taille);
while(tmp->next != NULL){
	tmp = tmp->next;
	printf("ID : %d\tClient : %d\tPriorite : %d\tTaille : %d\n",tmp->val->id,tmp->val->clientID,tmp->val->priorite,tmp->val->taille);


	}


	}


}
void transfertColis(colis* d,colis* r){

r->id = d->id;
r->clientID = d->clientID;
r->priorite = d->priorite;
r->taille = d->taille;
}



