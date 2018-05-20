/*************************************************************************
	> File Name: exportDataAndIndex.cpp
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月06日 星期二 09时55分36秒
 ************************************************************************/

#include "exportDataAndIndex.h"

int* exportDataAndIndex(){
    char formatTime[128];
    char query[512];
    if(useDb==true){
        getNowTime(formatTime,128);
    }
    int *exportRes=(int *)malloc(sizeof(int)*5);
    exportRes[0]=exportDataFile();
    exportRes[1]=exportIpIndexFile(srcIpRootNode,1);
    exportRes[2]=exportIpIndexFile(dstIpRootNode,2);
    exportRes[3]=exportPortIndexFile(&srcPortIndexTable,1);
    exportRes[4]=exportPortIndexFile(&dstPortIndexTable,2);
    if(useDb==true){
        sprintf(query,"INSERT INTO tsr(data_file,src_ip_index_file,src_port_index_file,dst_ip_index_file,\
            dst_port_index_file,timestamp)values('%s','%s','%s','%s','%s','%s')",dateFileName,\
            srcIpIndexFileName,srcPortIndexFileName,dstIpIndexFileName,dstPortIndexFileName,formatTime);
        if(mysql_real_query(&mysql,query,(unsigned long)strlen(query))){
            printf("mysql_real_query failed!\n");
        }
    }
    return exportRes;
}

int exportDataFile(){
    int id=countFiles("./dataDir/","data");
    strcpy(dateFileName,"data");
    char strId[16];
    int res=iToStr(id,strId);
    strcat(dateFileName,strId);
    char path[128];
    strcpy(path,"./dataDir/");
    strcat(path,dateFileName);
    int fd=open(path,O_WRONLY | O_APPEND | O_CREAT,0555);
    if(fd == -1){
        printf("fail to open file %s\n",path);
        return -1;
    }
    int writedBytes=write(fd,flowWaitExport,outBase*FLOWSIZE);
    if(writedBytes==-1){
        printf("write error!\n");
        return -1;
    }
    if(writedBytes<outBase*FLOWSIZE){
        printf("did not write enough data!\n");
    }
    return writedBytes;
}

int exportIpIndexFile(innerNode *rootNode,int flag){
    void *pNode=rootNode;
    q.push(pNode);
    void *firstLeafNode=getFirstLeafNode(rootNode);
    int i;
    while((!q.empty())&&q.front()!=firstLeafNode){
        pNode=q.front();
        for(i=0;i<256;i++){
            if(((innerNode *)pNode)->nextLevelNodes[i]){
                trieNodeArr[trieNodeArrIndex].offset[i]=sizeof(trieNodeOffset)*(trieNodeNum+1);
                trieNodeNum++;
                q.push(((innerNode *)pNode)->nextLevelNodes[i]);
            }
        }
        trieNodeArrIndex++;
        q.pop();
    } 
    int base=sizeof(trieNodeOffset)*trieNodeNum;
    //printf("trie node consume %d bytes!\n",base);
    while(!q.empty()){
        pNode=q.front();
        for(i=0;i<256;i++){
            if(((leafNode *)pNode)->indexList[i]){
                trieNodeArr[trieNodeArrIndex].offset[i]=base+indexOffset*sizeof(unsigned);
                compressIndexLink(((leafNode *)pNode)->indexList[i]);
            } 
        }
        trieNodeArrIndex++;
        q.pop();
    }
    int id;
    int indexFileBytes;
    if(flag==1){
        id=countFiles("./index/","srcIpIndex");
        strcpy(srcIpIndexFileName,"srcIpIndex");
        char idStr[16];
        iToStr(id,idStr);
        strcat(srcIpIndexFileName,idStr);
        char path[128];
        strcpy(path,"./index/");
        strcat(path,srcIpIndexFileName);
        indexFileBytes=writeIpIndexFile(path);
    }
    if(flag==2){
        id=countFiles("./index/","dstIpIndex");
        strcpy(dstIpIndexFileName,"dstIpIndex");
        char idStr[16];
        iToStr(id,idStr);
        strcat(dstIpIndexFileName,idStr);
        char path[128];
        strcpy(path,"./index/");
        strcat(path,dstIpIndexFileName);
        indexFileBytes=writeIpIndexFile(path);  
    }
    trieNodeNum=0;
    trieNodeArrIndex=0;
    memset(trieNodeArr,0,sizeof(trieNodeOffset)*trieNodeNum);
    memset(compressedIndexArr,0,sizeof(unsigned)*indexOffset);
    indexOffset=0;
    return indexFileBytes;
}

int exportPortIndexFile(portIndexTable *pit,int flag){
    int base=65536*sizeof(int);
    int i;
    for(i=0;i<65536;i++){
        if(pit->indexList[i]){
            portOffsetTable[i]=base+indexOffset*sizeof(unsigned);
            compressIndexLink(pit->indexList[i]);
        }
    }
    int id;
    int indexFileBytes;
    if(flag==1){
        id=countFiles("./index/","srcPortIndex");
        strcpy(srcPortIndexFileName,"srcPortIndex");
        char idStr[16];
        iToStr(id,idStr);
        strcat(srcPortIndexFileName,idStr);
        char path[128];
        strcpy(path,"./index/");
        strcat(path,srcPortIndexFileName);
        indexFileBytes=writePortIndexFile(path);
    }
    if(flag==2){
        id=countFiles("./index/","dstPortIndex");
        strcpy(dstPortIndexFileName,"dstPortIndex");
        char idStr[16];
        iToStr(id,idStr);
        strcat(dstPortIndexFileName,idStr);
        char path[128];
        strcpy(path,"./index/");
        strcat(path,dstPortIndexFileName);
        indexFileBytes=writePortIndexFile(path); 
    }
    memset(portOffsetTable,0,sizeof(int)*65536);
    memset(compressedIndexArr,0,sizeof(unsigned)*indexOffset);
    indexOffset=0;
    return indexFileBytes;
}

int writeIpIndexFile(const char *path){
    int writedBytes=writeIndexFile(path,trieNodeArr,sizeof(trieNodeOffset)*trieNodeNum);
    if(writedBytes==-1){
        printf("suffer error when write ip index file!\n");
    }
    return writedBytes;
}

int writePortIndexFile(const char *path){
    int writedBytes=writeIndexFile(path,portOffsetTable,65536*sizeof(int));
    if(writedBytes==-1){
        printf("suffer error when write port index file!\n");
    }
    return writedBytes;
}

int writeIndexFile(const char *path,void *ptr,int size){
    int fd=open(path,O_WRONLY | O_APPEND | O_CREAT,0555);
    if(fd == -1){
        printf("fail to open file %s\n",path);
        return -1;
    }
    int writedPointerBytes=write(fd,ptr,size);
    if(writedPointerBytes==-1){
        printf("write pointer info error!\n");
        return -1;
    }
    if(writedPointerBytes<size){
        printf("did not write enough pointer info!\n");
    }
    int writedIndexNumBytes=write(fd,compressedIndexArr,sizeof(unsigned)*indexOffset);
    if(writedIndexNumBytes==-1){
        printf("write index number error!\n");
        return -1;
    }
    if(writedIndexNumBytes<sizeof(unsigned)*indexOffset){
        printf("do not write enough index number data!\n");
    }
    return writedPointerBytes+writedIndexNumBytes;
}

void *getFirstLeafNode(innerNode *rootNode){
    void *pNode=rootNode;
    int i,j;
    for(i=0;i<3;i++){
        for(j=0;j<256;j++){
            if(((innerNode *)pNode)->nextLevelNodes[j]){
                pNode=((innerNode *)pNode)->nextLevelNodes[j];
                break;
            } 
        }
    }
    return pNode;
}
