/*************************************************************************
	> File Name: processFlows.h
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月16日 星期二 10时19分34秒
 ************************************************************************/

#ifndef _PROCESSFLOWS_H
#define _PROCESSFLOWS_H
#include "common.h"
#include "trieIndex.h"

unsigned out;
pthread_cond_t cond_c;
flowRecord flows[FLOWSPERPROCESS];

extern int shmid;
extern void *shmptr;
extern int n;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond_p;
extern flowRecord flowBlock[];
extern flowRecord flowWaitExport[];
extern indexNode srcIpIndexNodeBuf[];
extern indexNode dstIpIndexNodeBuf[];
extern indexNode srcPortIndexNodeBuf[];
extern indexNode dstPortIndexNodeBuf[];
extern innerNode srcIpInnerNodeBuf[];
extern innerNode dstIpInnerNodeBuf[];
extern innerNode *srcIpRootNode;
extern innerNode *dstIpRootNode;
extern leafNode srcIpLeafNodeBuf[LEAFNODENUM];
extern leafNode dstIpLeafNodeBuf[LEAFNODENUM];
extern int inBase;
extern int outBase;
extern int currUsedSrcIpInnerNodes;
extern int currUsedDstIpInnerNodes;
extern portIndexTable srcPortIndexTable;
extern portIndexTable dstPortIndexTable;
extern int currUsedSrcIpIndexNodes;
extern int currUsedDstIpIndexNodes;
extern int currUsedSrcPortIndexNodes;
extern int currUsedDstPortIndexNodes;
extern int currUsedSrcIpInnerNodes;
extern int currUsedSrcIpLeafNodes;
extern int currUsedDstIpInnerNodes;
extern int currUsedDstIpLeafNodes;

void *outputFlows(void *argv);
extern void resort();
extern byte index();
extern int* exportDataAndIndex();

#endif
