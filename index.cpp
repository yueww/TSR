/*************************************************************************
	> File Name: index.cpp
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年02月27日 星期二 13时19分13秒
 ************************************************************************/

#include "index.h"

byte index(){
    int i;
    byte ret=0;
    while(outBase<inBase){
        /*
        for(i=0;i<FLOWPERPROCESS;i++){
            leafNode *pSrcLeafNode=locateLeafNode(srcIpRootNode,flowWaitExport[outBase+i],1);
            leafNode *pDstLeafNode=locateLeafNode(dstIpRootNode,flowWaitExport[outBase+i],2);
            recordIpIndexNum(pSrcLeafNode,flowWaitExport[outBase+i],1);
            recordIpIndexNum(pDstLeafNode,flowWaitExport[outBase+i],2);
            recordPortIndexNum(flowWaitExport[outBase+i],1);
            recordPortIndexNum(flowWaitExport[outBase+i],2);
            ret=judgeExport();
            if(ret!=0) return ret;
        }
        outBase+=FLOWPERPROCESS;
        */
        leafNode *pSrcLeafNode=locateLeafNode(srcIpRootNode,flowWaitExport[outBase],1);
        leafNode *pDstLeafNode=locateLeafNode(dstIpRootNode,flowWaitExport[outBase],2);
        recordIpIndexNum(pSrcLeafNode,flowWaitExport[outBase],1);
        recordIpIndexNum(pDstLeafNode,flowWaitExport[outBase],2);
        recordPortIndexNum(flowWaitExport[outBase],1);
        recordPortIndexNum(flowWaitExport[outBase],2);
        outBase++;
        ret=judgeExport();
        if(ret!=0) return ret;
    }
    return ret;
}

leafNode* locateLeafNode(void *pTrieNode,flowRecord rec,int flag){
    if(flag==1){
        int i;
        for(i=0;i<2;i++){
            if(((innerNode *)pTrieNode)->nextLevelNodes[rec.srcIp[i]]==NULL){
                ((innerNode *)pTrieNode)->nextLevelNodes[rec.srcIp[i]]=&srcIpInnerNodeBuf[currUsedSrcIpInnerNodes++]; 
            }
            pTrieNode=((innerNode *)pTrieNode)->nextLevelNodes[rec.srcIp[i]];
        }
        if(((innerNode *)pTrieNode)->nextLevelNodes[rec.srcIp[i]]==NULL){
            ((innerNode *)pTrieNode)->nextLevelNodes[rec.srcIp[i]]=&srcIpLeafNodeBuf[currUsedSrcIpLeafNodes++]; 
        }
        pTrieNode=((innerNode *)pTrieNode)->nextLevelNodes[rec.srcIp[i]];
        return (leafNode *)pTrieNode;
    }else{
        int i;
        for(i=0;i<2;i++){
            if(((innerNode *)pTrieNode)->nextLevelNodes[rec.destIp[i]]==NULL){
                ((innerNode *)pTrieNode)->nextLevelNodes[rec.destIp[i]]=&dstIpInnerNodeBuf[currUsedDstIpInnerNodes++]; 
            }
            pTrieNode=((innerNode *)pTrieNode)->nextLevelNodes[rec.destIp[i]];
        }
        if(((innerNode *)pTrieNode)->nextLevelNodes[rec.destIp[i]]==NULL){
            ((innerNode *)pTrieNode)->nextLevelNodes[rec.destIp[i]]=&dstIpLeafNodeBuf[currUsedDstIpLeafNodes++]; 
        }
        pTrieNode=((innerNode *)pTrieNode)->nextLevelNodes[rec.destIp[i]];
        return (leafNode *)pTrieNode;
    }
}

void recordIpIndexNum(leafNode *pLeafNode,flowRecord rec,int flag){
    if(flag==1){
        if(pLeafNode->pTails[rec.srcIp[3]]==NULL){
            pLeafNode->pTails[rec.srcIp[3]]=pLeafNode->indexList[rec.srcIp[3]]=&srcIpIndexNodeBuf[currUsedSrcIpIndexNodes];
        }else{
            pLeafNode->pTails[rec.srcIp[3]]->next=&srcIpIndexNodeBuf[currUsedSrcIpIndexNodes];
            pLeafNode->pTails[rec.srcIp[3]]=pLeafNode->pTails[rec.srcIp[3]]->next;
        }
        currUsedSrcIpIndexNodes++;
    }else{
        if(pLeafNode->pTails[rec.destIp[3]]==NULL){
            pLeafNode->pTails[rec.destIp[3]]=pLeafNode->indexList[rec.destIp[3]]=&dstIpIndexNodeBuf[currUsedDstIpIndexNodes];
        }else{
            pLeafNode->pTails[rec.destIp[3]]->next=&dstIpIndexNodeBuf[currUsedDstIpIndexNodes];
            pLeafNode->pTails[rec.destIp[3]]=pLeafNode->pTails[rec.destIp[3]]->next;
        }
        currUsedDstIpIndexNodes++;
    } 
}

void recordPortIndexNum(flowRecord rec,int flag){
    if(flag==1){
        unsigned short portNum=rec.srcPort[0]*0x100+rec.srcPort[1];
        if(srcPortIndexTable.pTails[portNum]==NULL){
            srcPortIndexTable.pTails[portNum]=srcPortIndexTable.indexList[portNum]=&srcPortIndexNodeBuf[currUsedSrcPortIndexNodes];
        }else{
            srcPortIndexTable.pTails[portNum]->next=&srcPortIndexNodeBuf[currUsedSrcPortIndexNodes];
            srcPortIndexTable.pTails[portNum]=srcPortIndexTable.pTails[portNum]->next;
        }
        currUsedSrcPortIndexNodes++;
    }else{
        unsigned short portNum=rec.destPort[0]*0x100+rec.destPort[1];
        if(dstPortIndexTable.pTails[portNum]==NULL){
            dstPortIndexTable.pTails[portNum]=dstPortIndexTable.indexList[portNum]=&dstPortIndexNodeBuf[currUsedDstPortIndexNodes];
        }else{
            dstPortIndexTable.pTails[portNum]->next=&dstPortIndexNodeBuf[currUsedDstPortIndexNodes];
            dstPortIndexTable.pTails[portNum]=dstPortIndexTable.pTails[portNum]->next;
        }
        currUsedDstPortIndexNodes++;
    }
}

byte judgeExport(){
    byte ret=0;
    if(currUsedSrcIpInnerNodes>INNERNODENUM*THRESHOLD){
        ret|=0x01;
    }else if(currUsedSrcIpLeafNodes>LEAFNODENUM*THRESHOLD){
        ret|=0x02;
    }else if(currUsedDstIpInnerNodes>INNERNODENUM*THRESHOLD){
        ret|=0x04;
    }else if(currUsedDstIpLeafNodes>LEAFNODENUM*THRESHOLD){
        ret|=0x08;
    }else if(currUsedSrcIpIndexNodes>FLOWAMOUNTINMEM*THRESHOLD){
        ret|=0x10;
    }else if(currUsedDstIpIndexNodes>FLOWAMOUNTINMEM*THRESHOLD){
        ret|=0x20;
    }else if(currUsedSrcPortIndexNodes>FLOWAMOUNTINMEM*THRESHOLD){
        ret|=0x40;
    }else if(currUsedDstPortIndexNodes>FLOWAMOUNTINMEM*THRESHOLD){
        ret|=0x80;
    }
    return ret;
}
