/*************************************************************************
	> File Name: compressIndex.h
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月04日 星期日 15时46分55秒
 ************************************************************************/

#ifndef _COMPRESSINDEX_H
#define _COMPRESSINDEX_H
#include "common.h"
#include "trieIndex.h"

#define UNIT5 0xc0000000
#define UNIT3 0x80000000
#define UNIT2 0x40000000

unsigned compressedIndexArr[FLOWAMOUNTINMEM];
int indexOffset;

int isFive(indexNode *ptr);
int isThree(indexNode *ptr);
int isTwo(indexNode *ptr); 
int isSpecial(indexNode *ptr);
void compress5(indexNode  *ptr);
void compress3(indexNode *ptr);
void compress2(indexNode *ptr);
int moveToNext(indexNode **ptr,int steps);
int compressIndexLink(indexNode *ptr);

#endif
