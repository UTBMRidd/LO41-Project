#include "utils.h"
#include <string.h>
#include <stdio.h>
int nDigits(unsigned int n)
{
    int ret = 0;
 
    do
    {
        n /= 10;
        ret++;
    } while(n > 0);
 
    return ret;
}
char *concat_string(char *s1,char *s2){
     char *s3=NULL;
     s3=(char *)malloc((strlen(s1)+strlen(s2))*sizeof(char));
     strcpy(s3,s1);
     strcat(s3,s2);
     return s3;
}
char *concat_num(int i){
		char buffer[nDigits(i) +1];
		snprintf(buffer, 10, "%d", i);
     	return concat_string("/",buffer);
}
int getValue(mqd_t messageQ){
	char recv[512];
	ssize_t len_recv;
 	memset(recv, 0, 512);
  	len_recv = mq_receive(messageQ, recv, 512, NULL);
	return atoi(recv);
}
void sendValue(mqd_t messageQ,int value,int prio){
	char buffer[nDigits(value) +1];
	snprintf(buffer, 10, "%d", value);
  	mq_send(messageQ, buffer, strlen(buffer), prio);
}
void waitForMessage(mqd_t* mq, void* fct, void* args){
    struct sigevent sev;


    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = fct;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = args;
    if (mq_notify(mq, &sev) == -1)
        printf("CA marche po");

}

void sendColis(colis * original, mqd_t messageQ){
	sendValue(messageQ,original->id,0);
	sendValue(messageQ,original->clientID,0);
	sendValue(messageQ,original->priorite,0);
	sendValue(messageQ,original->taille,0);
	free(original);
	original = NULL;
}
colis* getColis(mqd_t messageQ){
	colis * final;
	final = malloc(sizeof(colis));
	final->id = getValue(messageQ);
	final->clientID = getValue(messageQ);
	final->priorite = getValue(messageQ);
	final->taille = getValue(messageQ);
	return final;
}




