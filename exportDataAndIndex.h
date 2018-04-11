/*************************************************************************
	> File Name: exportDataAndIndex.h
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月05日 星期一 16时36分13秒
 ************************************************************************/

#ifndef _EXPORTDATAANDINDEX_H
#define _EXPORTDATAANDINDEX_H
#include "common.h"
#include "myToolFunc.h"
#include "trieIndex.h"
#include <queue>

using namespace std;

typedef struct trieNodeOffset{
    int offset[256];
}trieNodeOffset;

/*
typedef struct portOffsetTable{
    int offset[65536];
}portOffsetTable;
*/

int trieNodeNum;
int trieNodeArrIndex;
trieNodeOffset trieNodeArr[INNERNODENUM+LEAFNODENUM];
queue<void *> q;
//portOffsetTable pot;
int portOffsetTable[65536];

extern flowRecord flowWaitExport[];
extern innerNode *srcIpRootNode;
extern innerNode *dstIpRootNode;
extern portIndexTable srcPortIndexTable;
extern portIndexTable dstPortIndexTable;
extern int outBase;
extern unsigned compressedIndexArr[];
extern int indexOffset;

char dateFileName[32];
char srcIpIndexFileName[32];
char dstIpIndexFileName[32];
char srcPortIndexFileName[32];
char dstPortIndexFileName[32];

int* exportDataAndIndex();
int exportDataFile();
int writeIpIndexFile(const char *path);
int writePortIndexFile(const char *path);
int writeIndexFile(const char *path,void *ptr,int size);
int exportIpIndexFile(innerNode *rootNode,int flag);
int exportPortIndexFile(portIndexTable *pit,int flag);
void *getFirstLeafNode(innerNode *rootNode);

extern int isFive(indexNode *ptr);
extern int isThree(indexNode *ptr);
extern int isTwo(indexNode *ptr); 
extern int isSpecial(indexNode *ptr);
extern void compress5(indexNode  *ptr);
extern void compress3(indexNode *ptr);
extern void compress2(indexNode *ptr);
extern int moveToNext(indexNode **ptr,int steps);
extern int compressIndexLink(indexNode *ptr);

#endif
