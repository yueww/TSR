/*************************************************************************
	> File Name: trieIndex.h
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年02月05日 星期一 15时20分27秒
 ************************************************************************/

#ifndef _TRIEINDEX_H
#define _TRIEINDEX_H

#define INNERNODENUM 20000
#define LEAFNODENUM 30000

typedef struct indexNode{
    unsigned indexNum;
    struct indexNode *next;
}indexNode;

typedef struct innerNode{
    void *nextLevelNodes[256];
}innerNode;

typedef struct leafNode{
    indexNode *indexList[256];
    indexNode *pTails[256];
}leafNode;

typedef struct portIndexTable{
    indexNode *indexList[65536];
    indexNode *pTails[65536];
}portIndexTable;

#endif
