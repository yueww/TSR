/*************************************************************************
	> File Name: flowResort.h
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月31日 星期三 10时19分40秒
 ************************************************************************/

#ifndef _FLOWRESORT_H
#define _FLOWRESORT_H
#include "common.h"
#include "lsh.h"

#define BUCKETSIZE 2*FLOWSPERPROCESS
#define LINKNODEBUFSIZE TABLESIZE*FLOWSPERPROCESS

hashBucket hashTable[TABLESIZE];
flowRecordNode flowNodeBuf[LINKNODEBUFSIZE];
flowRecordNode *unusedChainHead;    //initiated with &flowNodeBuf[0]
flowRecordNode *unusedChainTail;    //initiated with &flowNodeBuf[LINKNODEBUFSIZE-1]
extern flowRecord flows[];
extern flowRecord flowWaitExport[];
extern int inBase;
extern int outBase;
int insertedNodeNum;

//extern flowRecord flowBlock[];

void extractTuple4(tuple4vec *tvec,flowRecord rec);
void insertFlowsToBucket(flowRecord rec,int index);
void exportResortedFlows(int index);
void resort();
int isMatch(flowRecord *rec1,flowRecord *rec2);

#endif
