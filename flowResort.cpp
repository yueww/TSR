/*************************************************************************
	> File Name: flowResort.cpp
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月31日 星期三 10时54分52秒
 ************************************************************************/
#include "flowResort.h"
using namespace std;

void resort(){
    int i;
    tuple4vec vec;
    int index;
    for(i=0;i<FLOWSPERPROCESS;i++){
        extractTuple4(&vec,flows[i]);
        index=myHash(vec);
        insertFlowsToBucket(flows[i],index);
        if(hashTable[index].currNum>=BUCKETSIZE){
            printf("export resorted flows from hash bucket!\n");
            //hashTable[index].currNum-=FLOWSPERPROCESS;
            exportResortedFlows(index);
        }
        if(insertedNodeNum>=LINKNODEBUFSIZE){
            printf("export resorted flows from hash table!\n");
            int k;
            int maxk=0;
            for(k=1;k<TABLESIZE;k++){
                if(hashTable[k].currNum>hashTable[maxk].currNum){
                    maxk=k;
                }
            }
            exportResortedFlows(maxk);
        }
    }
}

/*
void extractTuple4(tuple4vec *tvec,flowRecord rec){
    memcpy(tvec->vec,rec.srcIp,4);
    memcpy(tvec->vec+4,rec.destIp,4);
    memcpy(tvec->vec+8,rec.srcPort,2);
    memcpy(tvec->vec+10,rec.destPort,2);
}
*/

void extractTuple4(tuple4vec *tvec,flowRecord rec){
    int i;
    for(i=0;i<4;i++){
        tvec->vec[i]=rec.srcIp[i];
    }
    for(;i<8;i++){
        tvec->vec[i]=rec.destIp[i-4];
    }
    tvec->vec[i++]=rec.srcPort[0]*0x100+rec.srcPort[1];
    tvec->vec[i]=rec.destPort[0]*0x100+rec.destPort[1];
}

void insertFlowsToBucket(flowRecord rec,int index){
    //memcpy(flowNodeBuf[insertedNodeNum].rec,rec,FLOWSIZE);
    flowRecordNode *tmp=unusedChainHead;
    memcpy(&(tmp->rec),&rec,FLOWSIZE);
    unusedChainHead=unusedChainHead->next;
    hashTable[index].currNum++;
    flowRecordNode *pF1,*pF2;
    pF1=pF2=&hashTable[index].head;
    while(pF1){
        if(isMatch(&(pF1->rec),&rec)){
            if(pF1==&hashTable[index].head){
                pF1=pF1->next;
                continue;
            }else{
                break;
            }
        }else{
            pF2=pF1;
            pF1=pF1->next;
        }
    }
    pF2->next=tmp;
    tmp->next=pF1;
    insertedNodeNum++;
}

void exportResortedFlows(int index){
    flowRecordNode *pF;
    pF=&hashTable[index].head;
    //unusedChainTail->next=pF->next;
    if(unusedChainHead==NULL){
        unusedChainHead=pF->next;
    }else{
        unusedChainTail->next=pF->next;
    }
    int j;
    for(j=0;j<FLOWSPERPROCESS;j++){
        pF=pF->next;
        memcpy(&flowWaitExport[inBase++],&(pF->rec),FLOWSIZE);
    }
    hashTable[index].head.next=pF->next;
    pF->next=NULL;
    unusedChainTail=pF;
    hashTable[index].currNum-=FLOWSPERPROCESS;
    insertedNodeNum-=FLOWSPERPROCESS;
}

int isMatch(flowRecord *rec1,flowRecord *rec2){
    if(!memcmp(rec1->srcIp,rec2->srcIp,4)){
        return 1;
    }else if(!memcmp(rec1->destIp,rec2->destIp,4)){
        return 1;
    }else if(!memcmp(rec1->srcPort,rec2->srcPort,2)){
        return 1;
    }else if(!memcmp(rec1->destPort,rec2->destPort,2)){
        return 1;
    }
    return 0;
}
