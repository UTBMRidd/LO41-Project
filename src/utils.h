#ifndef UTILS
#define UTILS
#include <fcntl.h>
#include <sys/stat.h> 
#include <mqueue.h>
#include "colis.h"
char *concat_string(char *s1,char *s2);
int nDigits(unsigned int n);
char *concat_num(int i);
int getValue(mqd_t messageQ);
void sendValue(mqd_t messageQ,int value,int prio);
void waitForMessage(mqd_t* mq, void* fct,void* args);
void sendColis(colis * original, mqd_t messageQ);
colis* getColis(mqd_t messageQ);
#endif
