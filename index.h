/*************************************************************************
	> File Name: index.h
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年02月05日 星期一 15时46分44秒
 ************************************************************************/

#ifndef _INDEX_H
#define _INDEX_H
#include "common.h"
#include "trieIndex.h"

#define THRESHOLD 0.96

flowRecord flowWaitExport[FLOWAMOUNTINMEM];
indexNode srcIpIndexNodeBuf[FLOWAMOUNTINMEM];
indexNode dstIpIndexNodeBuf[FLOWAMOUNTINMEM];
indexNode srcPortIndexNodeBuf[FLOWAMOUNTINMEM];
indexNode dstPortIndexNodeBuf[FLOWAMOUNTINMEM];
innerNode srcIpInnerNodeBuf[INNERNODENUM];
innerNode dstIpInnerNodeBuf[INNERNODENUM];
leafNode srcIpLeafNodeBuf[LEAFNODENUM];
leafNode dstIpLeafNodeBuf[LEAFNODENUM];
innerNode *srcIpRootNode;
innerNode *dstIpRootNode;
portIndexTable srcPortIndexTable;
portIndexTable dstPortIndexTable;
int inBase;
int outBase;

int currUsedSrcIpIndexNodes;
int currUsedDstIpIndexNodes;
int currUsedSrcPortIndexNodes;
int currUsedDstPortIndexNodes;
int currUsedSrcIpInnerNodes;
int currUsedSrcIpLeafNodes;
int currUsedDstIpInnerNodes;
int currUsedDstIpLeafNodes;

byte index();
leafNode* locateLeafNode(void *pTrieNode,flowRecord rec,int flag);
void recordIpIndexNum(leafNode *pLeafNode,flowRecord rec,int flag);
void recordPortIndexNum(flowRecord rec,int flag);
byte judgeExport();

#endif

