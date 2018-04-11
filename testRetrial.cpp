/*************************************************************************
	> File Name: testRetrial.cpp
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月26日 星期一 10时27分27秒
 ************************************************************************/


#include "retrialEngine.h"

extern unsigned tmpCompressedIndexBuf[COMPRESSEDBUFSIZE];
extern unsigned tmpDecompressedIndexBuf[DECOMPRESSEDBUFSIZE];
extern char resFileName[48];
extern queue<pQueryFile> queryFileSet;

//unsigned compressedIndexBuf[1000];
//unsigned decompressedIndexBuf[1000];

/*
int main(){
   byte ip[4];
   ip[0]=192;
   ip[1]=168;
   ip[2]=1;
   ip[3]=200;
   int compressLen=0;
   if((compressLen=retrialIpCompressedIndex("./index/srcIpIndex0",ip,compressedIndexBuf))==-1){
       printf("fail to retrial!\n");
       return 0;
   }
   */
   /*
    int compressLen=0;
    if((compressLen=retrialPortCompressedIndex("./index/srcPortIndex0",80,compressedIndexBuf))==-1){
       printf("fail to retrial!\n");
       return 0;
    }
    printf("compressLen is %d!\n",compressLen);
    int decompressLen=0;
    if((decompressLen=decompressIndex(compressedIndexBuf,compressLen,decompressedIndexBuf))==-1){
       printf("fail to retrial!\n");
       return 0;
    }
    printf("decompressLen is %d!\n",decompressLen);
    FILE *fp=fopen("./gotSrcIpIndex","w+");
    for(int k=0;k<decompressLen;k++){
        fprintf(fp,"srcIp index:%d\n",decompressedIndexBuf[k]);
    }
    fclose(fp);
   int retrialNum=0;
   if((retrialNum=retrialFlows("./dataDir/data0",decompressedIndexBuf,decompressLen))==-1){
       printf("fail to retrial!\n");
       return 0;
   }
   printf("retrial %d flows!\n",retrialNum);
   return 0;
}
*/

int main(int argc,char *argv[]){
    char indexName[48],indexValue[48];
    int flowsAmount=0;
    if(argc<3){
        printf("not enough parameter!\n");
        return 0;
    }else if(argc==3){
        strcpy(indexName,argv[1]);
        strcpy(indexValue,argv[2]);
    }else if(argc==4){
        strcpy(indexName,argv[2]);
        strcpy(indexValue,argv[3]);
    }else{
        printf("too much parameter!\n");
        return 0;
    }
    bool flag[4]={0,0,0,0};
    if(!strcmp(indexName,"srcIp")) flag[0]=1;
    else if(!strcmp(indexName,"srcPort")) flag[1]=1;
    else if(!strcmp(indexName,"dstIp")) flag[2]=1;
    else if(!strcmp(indexName,"dstPort")) flag[3]=1;
    else{
        printf("indexName which is not supported!\n");
        return 0;
    }
    int fileNum=getQueryFileSet(flag);
    if(fileNum==0){
        printf("index file does not exist or some file has been removed incorrectly!\n");
        return -1;
    }
    while(!queryFileSet.empty()){
        pQueryFile pqf=queryFileSet.front();
        char indexPath[128];
        strcpy(indexPath,"./index/");
        if(!strcmp(indexName,"srcIp")) strcat(indexPath,pqf->srcIpIndexFileName);
        else if(!strcmp(indexName,"srcPort")) strcat(indexPath,pqf->srcPortIndexFileName);
        else if(!strcmp(indexName,"dstIp")) strcat(indexPath,pqf->dstIpIndexFileName);
        else if(!strcmp(indexName,"dstPort")) strcat(indexPath,pqf->dstPortIndexFileName);
        else{
            printf("indexName which is not supported!\n");
            return 0;
        }
        int decompressLen=getDecompressedIndex(indexPath,indexName,indexValue);
        if(decompressLen==-1){
            return 0;
        }
        if(decompressLen==0){
            printf("there is no record whose %s equal to %s\n",indexName,indexValue);
            return 0;
        }
        char dataPath[128];
        strcpy(dataPath,"./dataDir/");
        strcat(dataPath,pqf->dataFileName);
        int flowsNum;
        if((flowsNum=retrialFlows(dataPath,tmpDecompressedIndexBuf,decompressLen))==-1){
            printf("error happened when retrial flows from data file!\n");
            return 0;
        }
        flowsAmount+=flowsNum;
        queryFileSet.pop();
    }
    printf("retrialed %d flows!\n",flowsAmount);
    if(argc==4){
        char resPath[128];
        strcpy(resPath,"./result/");
        strcat(resPath,resFileName);
        formatShow(resPath,argv[1]);
    }
    return 0;
}
