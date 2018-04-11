/*************************************************************************
	> File Name: test.h
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月16日 星期五 09时51分30秒
 ************************************************************************/

#ifndef _TEST_H
#define _TEST_H
#include "common.h"
#include "trieIndex.h"
#include "lsh.h"

#define TABLESIZE 1024
#define COMMANDSIZE 512
#define LINKNODEBUFSIZE TABLESIZE*FLOWSPERPROCESS

int shmid;
void *shmptr;
pthread_t pro,con;


//extern vector<double> aVecParas[HASHFUNCNUM];
//extern double bParas[HASHFUNCNUM];
extern hashBucket hashTable[TABLESIZE];
extern flowRecordNode flowNodeBuf[];
extern flowRecordNode *unusedChainHead;    //initiated with &flowNodeBuf[0]
extern flowRecordNode *unusedChainTail;    //initiated with &flowNodeBuf[LINKNODEBUFSIZE-1]

extern flowRecord flowWaitExport[];
extern indexNode srcIpIndexNodeBuf[];
extern indexNode dstIpIndexNodeBuf[];
extern indexNode srcPortIndexNodeBuf[];
extern indexNode dstPortIndexNodeBuf[];
extern innerNode srcIpInnerNodeBuf[];
extern innerNode dstIpInnerNodeBuf[];
extern leafNode srcIpLeafNodeBuf[];
extern leafNode dstIpLeafNodeBuf[];
extern portIndexTable srcPortIndexTable;
extern portIndexTable dstPortIndexTable;
extern innerNode *srcIpRootNode;
extern innerNode *dstIpRootNode;
extern int inBase;
extern int outBase;
extern int currUsedSrcIpIndexNodes;
extern int currUsedDstIpIndexNodes;
extern int currUsedSrcIpInnerNodes;
extern int currUsedDstIpInnerNodes;
extern int currUsedSrcIpLeafNodes;
extern int currUsedDstIpLeafNodes;
extern int currUsedSrcPortIndexNodes;
extern int currUsedDstPortIndexNodes;

extern unsigned out;
extern int n;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond_p;
extern pthread_cond_t cond_c;
extern flowRecord flowBlock[BLOCKSIZE];

void *processFunc(void *arg);
char *processCommand(const char *opt,const char *name);
extern byte index();
extern void extractTuple4(tuple4vec *tvec,flowRecord rec);
extern void insertFlowsToBucket(flowRecord rec,int index);
extern void resort();
extern int myHash(tuple4vec vec);
extern int* exportDataAndIndex();
extern void *outputFlows(void *argv);
extern void *receiveData(void *argv);
extern void *produceFromFile(void *arg);

#endif
