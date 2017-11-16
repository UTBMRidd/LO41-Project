#ifndef COLIS
#define COLIS
typedef struct{
	int id;
	int clientID;
	int priorite;//De 0 à 5, plus le nombre est grand, plus le colis doit être livré vite
	int taille; //Valeurs possibles : De 0 à 5, plus le nombre est grand plus le colis est gros
}colis;

typedef struct llcol{
	colis* val;
	struct llcol* next;

}LLColis;

LLColis* addColis(LLColis* ll,int id,int clientID,int prio,int taille);
LLColis* removeById(LLColis* ll,int id);
void drawList(LLColis* ll);
void transfertColis(colis* d,colis* r);
#endif
