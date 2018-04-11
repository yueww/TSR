/*************************************************************************
	> File Name: produceFlows.h
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月15日 星期一 15时06分55秒
 ************************************************************************/

#ifndef _PRODUCEFLOWS_H
#define _PRODUCEFLOWS_H

#include "common.h"

#define rDataCacheSize 5

typedef struct rawData{
    unsigned length;
    byte buf[BUFSIZE];
}rawData;

unsigned in;
int n;
pthread_mutex_t mutex;
pthread_cond_t cond_p;

rawData rDataCache[rDataCacheSize];
flowRecord flowBlock[BLOCKSIZE];

extern pthread_cond_t cond_c;

void *receiveData(void *argv);
void *produce(void *argv);
void *produceFromFile(void *arg);

#endif
