/*************************************************************************
	> File Name: testRetrial.cpp
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月26日 星期一 10时27分27秒
 ************************************************************************/


#include "retrialEngine.h"
#include "queryInterpretion.h"

extern unsigned tmpCompressedIndexBuf[COMPRESSEDBUFSIZE];
extern unsigned tmpDecompressedIndexBuf[DECOMPRESSEDBUFSIZE];
extern char resFileName[48];
extern queue<pQueryFile> queryFileSet;
unsigned resIndexBuf[2*DECOMPRESSEDBUFSIZE];

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

/*
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
    if(fileNum==-1){
        printf("error happened when access database!\n");
        return 0;
    }
    if(fileNum==0){
        printf("index file does not exist or some file has been removed incorrectly!\n");
        return 0;
    }
    while(!queryFileSet.empty()){
        pQueryFile pqf=queryFileSet.front();
        char indexPath[128];
        strcpy(indexPath,"./index/");
        if(!strcmp(indexName,"srcIp")){
            strcat(indexPath,pqf->srcIpIndexFileName);   
            delete pqf->srcPortIndexFileName;
        }
        else if(!strcmp(indexName,"srcPort")){
            strcat(indexPath,pqf->srcPortIndexFileName);
            delete pqf->srcPortIndexFileName;
        }
        else if(!strcmp(indexName,"dstIp")){
            strcat(indexPath,pqf->dstIpIndexFileName);
            delete pqf->dstIpIndexFileName;
        }
        else if(!strcmp(indexName,"dstPort")){
            strcat(indexPath,pqf->dstPortIndexFileName);
            delete pqf->dstPortIndexFileName;
        }
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
        delete pqf->dateFileName;
        int flowsNum;
        if((flowsNum=retrialFlows(dataPath,tmpDecompressedIndexBuf,decompressLen))==-1){
            printf("error happened when retrial flows from data file!\n");
            return 0;
        }
        flowsAmount+=flowsNum;
        delete pqf;
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
*/

bool withTimestamp;

int main(int argc,char *argv[]){
    MYSQL mysql;
    if(argc>2){
        printf("too many arguments!\n");
        return 0;
    }
    if(argc==2){
        if(strcmp(argv[1],"-t")==0){
            char host[256];
            char user[128];
            char password[128];
            char dbName[128];
            unsigned port;
            printf("please input host:");
            scanf("%s",host);
            printf("please input port:");
            scanf("%u",&port);
            printf("please input user:");
            scanf("%s",user);
            printf("please input password:");
            scanf("%s",password);
            printf("please input database:");
            scanf("%s",dbName);
            mysql_init(&mysql);
            if(!mysql_real_connect(&mysql,host,user,password,dbName,port,NULL,0)){
                printf("fail to connect database!\n");
                return 0;
            }
            withTimestamp=true;
        }else{
            printf("unknown option:%s\n",argv[1]);
            return 0;
        }
    } 
    char command[RETRLEN];
    if(withTimestamp==true){
        fgets(command,RETRLEN,stdin);
    }
    while(1){
        printf("please input retrieve command or exit to exit:\n");
        fgets(command,RETRLEN,stdin);
        int commLen=strlen(command);
        if(commLen==RETRLEN-1){
            fflush(stdin);
            continue;
        }
        if(command[commLen-1]=='\n') command[commLen-1]=0;
        if(strcmp(command,"exit")==0) break;
        char sTime[64];
        char eTime[64];
        char showOpt[16];
        sTime[0]=0;
        eTime[0]=0;
        showOpt[0]=0;
        char *queryLang=checkCommand(command,showOpt,sTime,eTime);
        if(!queryLang) continue;
        bool flag[4]={false,false,false,false};
        if(strstr(queryLang,"srcIp")) flag[0]=true;
        if(strstr(queryLang,"srcPort")) flag[1]=true;
        if(strstr(queryLang,"dstIp")) flag[2]=true;
        if(strstr(queryLang,"dstPort")) flag[3]=true;
        if(withTimestamp==true){
            char fSTime[64];
            char fETime[64];
            if(sTime[0]==0) strcpy(sTime,"1970-01-01 00:00:00");
            if(eTime[0]==0) strcpy(eTime,"2100-12-31 23:59:59");
            int timeLen=getQueryTime(sTime,fSTime);
            if(timeLen==-1){
                printf("incorrect time format:%s\n",sTime);
                continue;
            }
            timeLen=getQueryTime(eTime,fETime);
            if(timeLen==-1){
                printf("incorrect time format:%s\n",eTime);
                continue;
            }
            int queueLen=getQueryFileSet(&mysql,flag,fSTime,fETime);
            if(queueLen==-1) continue;
        }else{
            int fileNum=getQueryFileSet(flag);
            if(fileNum==0){
                printf("index file does not exist or some file has been removed incorrectly!\n");
                return 0;
            }
        }
        int recAmount=0;
        bool isError=false;
        while(!queryFileSet.empty()){
            pQueryFile pqf=queryFileSet.front();
            int num=computeQuery(queryLang,pqf,resIndexBuf);
            if(num==-1){
                clearQueue();
                isError=true;
                break;
            }
            char dataPath[128];
            strcpy(dataPath,"./dataDir/");
            strcat(dataPath,pqf->dataFileName);
            int flowsNum;
            if((flowsNum=retrialFlows(dataPath,resIndexBuf,num))==-1){
                printf("error happened when retrial flows from data file!\n");
                return 0;
            }
            recAmount+=flowsNum;
            delete pqf->dataFileName;
            if(flag[0]==true) delete pqf->srcIpIndexFileName;
            if(flag[1]==true) delete pqf->srcPortIndexFileName;
            if(flag[2]==true) delete pqf->dstIpIndexFileName;
            if(flag[3]==true) delete pqf->dstPortIndexFileName;
            delete pqf;
            queryFileSet.pop();
        }
        if(isError==true) continue;
        if(showOpt[0]!=0){
            char resPath[128];
            strcpy(resPath,"./result/");
            strcat(resPath,resFileName);
            formatShow(resPath,showOpt,command);
        }
        resFileName[0]=0;
    }
    mysql_close(&mysql);
    return 0;
}
