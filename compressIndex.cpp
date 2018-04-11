/*************************************************************************
	> File Name: compressIndex.cpp
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月04日 星期日 16时17分58秒
 ************************************************************************/

#include "compressIndex.h"

int compressIndexLink(indexNode *ptr){
    int base=indexOffset;
    compressedIndexArr[indexOffset++]=0;
    compressedIndexArr[indexOffset++]=ptr->indexNum;
    int testRes;
    while(ptr){
        testRes=isFive(ptr);
        if(testRes==5){
            int specialNum=isSpecial(ptr);
            if(specialNum>=10){
                compressedIndexArr[indexOffset++]=ptr->indexNum;
                compressedIndexArr[indexOffset++]=specialNum;
                moveToNext(&ptr,specialNum);
            }else{
                compress5(ptr);
                moveToNext(&ptr,5);
            }
        }else if(testRes>=3){
            compress3(ptr);
            moveToNext(&ptr,3);
        }else{
            testRes=isThree(ptr);
            if(testRes==3){
                compress3(ptr);
                moveToNext(&ptr,3);
            }else if(testRes==2){
                compress2(ptr);
                moveToNext(&ptr,2);
            }else{
                testRes=isTwo(ptr);
                if(testRes==2){
                    compress2(ptr);
                    moveToNext(&ptr,2);
                }else if(testRes==1){
                    compressedIndexArr[indexOffset]=UNIT2;
                    compressedIndexArr[indexOffset++]|=(ptr->next->indexNum-ptr->indexNum);
                    ptr=ptr->next->next;
                    if(!ptr) break;
                    compressedIndexArr[indexOffset++]=ptr->indexNum;
                }else{
                    ptr=ptr->next;
                    if(!ptr) break;
                    compressedIndexArr[indexOffset++]=ptr->indexNum;
                }
            }
        }
    }
    compressedIndexArr[base]=indexOffset-base-1;
    return indexOffset-base;
}

int moveToNext(indexNode **ptr,int steps){
    int moveSteps=0;
    while((*ptr)!=NULL&&(moveSteps<steps)){
        *ptr=(*ptr)->next;
        moveSteps++;
    }
    return moveSteps;
}

int isFive(indexNode *ptr){
    indexNode *nPtr=ptr->next;
    if(!nPtr) return 0;
    int i;
    for(i=0;i<5;i++){
        if(nPtr->indexNum-ptr->indexNum>63){
            return i;
        }
        ptr=nPtr;
        nPtr=nPtr->next;
        if(!nPtr) return i+1;
    }
    return 5;
}

int isThree(indexNode *ptr){
    indexNode *nPtr=ptr->next;
    if(!nPtr) return 0;
    int i;
    for(i=0;i<3;i++){
        if(nPtr->indexNum-ptr->indexNum>1023){
            return i;
        }
        ptr=nPtr;
        nPtr=nPtr->next;
        if(!nPtr) return i+1;
    }
    return 3;
}

int isTwo(indexNode *ptr){
    indexNode *nPtr=ptr->next;
    if(!nPtr) return 0;
    int i;
    for(i=0;i<2;i++){
        if(nPtr->indexNum-ptr->indexNum>32767){
            return i;
        }
        ptr=nPtr;
        nPtr=nPtr->next;
        if(!nPtr) return i+1;
    }
    return 2; 
}

int isSpecial(indexNode *ptr){
    indexNode *nPtr=ptr->next;
    int specialNum=0;
    while(nPtr){
        if(nPtr->indexNum-ptr->indexNum>1){
            break;
        }
        specialNum++;
        ptr=nPtr;
        nPtr=nPtr->next;
    }
    return specialNum; 
}

void compress5(indexNode *ptr){
    compressedIndexArr[indexOffset]=UNIT5;
    indexNode *nPtr=ptr->next;
    if(!nPtr) return;
    int i;
    for(i=0;i<5;i++){
        compressedIndexArr[indexOffset]|=((nPtr->indexNum-ptr->indexNum)<<6*i);
        ptr=nPtr;
        nPtr=nPtr->next;
        if(!nPtr){
            indexOffset++;
            return;
        }
    }
    indexOffset++;
}

void compress3(indexNode *ptr){
    compressedIndexArr[indexOffset]=UNIT3;
    indexNode *nPtr=ptr->next;
    if(!nPtr) return;
    int i;
    for(i=0;i<3;i++){
        compressedIndexArr[indexOffset]|=((nPtr->indexNum-ptr->indexNum)<<10*i);
        ptr=nPtr;
        nPtr=nPtr->next;
        if(!nPtr){
            indexOffset++;
            return;
        }
    }
    indexOffset++;
}

void compress2(indexNode *ptr){
    compressedIndexArr[indexOffset]=UNIT2;
    indexNode *nPtr=ptr->next;
    if(!nPtr) return;
    int i;
    for(i=0;i<2;i++){
        compressedIndexArr[indexOffset]|=((nPtr->indexNum-ptr->indexNum)<<15*i);
        ptr=nPtr;
        nPtr=nPtr->next;
        if(!nPtr){
            indexOffset++;
            return;
        }
    }
    indexOffset++;
}

