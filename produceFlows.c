/*************************************************************************
	> File Name: produceFlows.c
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月15日 星期一 15时28分01秒
 ************************************************************************/

#include "produceFlows.h"

void *receiveData(void *argv){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(555);
    int res=bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(res==-1){
        printf("bind failed!\n");
        pthread_exit(0);
    }
    int count=0;
    pthread_t tid;
    while(1){
        rDataCache[count%rDataCacheSize].length=recvfrom(sockfd,rDataCache[count%rDataCacheSize].buf,BUFSIZE,0,0,0);
        pthread_create(&tid,0,produce,(void *)&rDataCache[count%rDataCacheSize]);
        count++;
    }
}

void* produce(void *argv){
    pthread_detach(pthread_self());
    rawData *pRawData=(rawData *)argv;
    int newFlows=(pRawData->length-PKTHEADERSIZE)/FLOWSIZE;
    pthread_mutex_lock(&mutex);
    while(n+newFlows>BLOCKSIZE) pthread_cond_wait(&cond_p,&mutex);
    if(in==BLOCKSIZE) in=0;
    if((in+newFlows)>BLOCKSIZE){
        memcpy(flowBlock+in,pRawData->buf+PKTHEADERSIZE,(BLOCKSIZE-in)*FLOWSIZE);
        memcpy(flowBlock,pRawData->buf+PKTHEADERSIZE+(BLOCKSIZE-in)*FLOWSIZE,(newFlows-(BLOCKSIZE-in))*FLOWSIZE);
        in=newFlows+in-BLOCKSIZE;
    }else{
        memcpy(flowBlock+in,pRawData->buf+PKTHEADERSIZE,newFlows*FLOWSIZE);
        in+=newFlows;
    }
    n+=newFlows;
    printf("add %d flows,current n is %d\n",newFlows,n);
    if(n>=FLOWSPERPROCESS) pthread_cond_signal(&cond_c);
    pthread_mutex_unlock(&mutex);
}

void *produceFromFile(void *arg){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    pthread_detach(pthread_self());
    FILE *fp=fopen("./data","rb");
    if(fp==NULL){
        printf("fail to open file!\n");
        exit(0);
    }
    while(!feof(fp)){
        int newFlows=FLOWSPERPROCESS;
        int readNum=0;
        pthread_mutex_lock(&mutex);
        while(n+newFlows>BLOCKSIZE) pthread_cond_wait(&cond_p,&mutex);
        if(in==BLOCKSIZE) in=0;
        if((in+newFlows)>BLOCKSIZE){
            readNum=fread(flowBlock+in,FLOWSIZE,BLOCKSIZE-in,fp);
            readNum+=fread(flowBlock,FLOWSIZE,(newFlows-(BLOCKSIZE-in)),fp);
            if(readNum==0){
                printf("error in reading flows or come to file end!\n");
            }else{
                in+=readNum;
            }
        }else{
            readNum=fread(flowBlock+in,FLOWSIZE,newFlows,fp);
            if(readNum==0){
                printf("error in reading flows or come to file end!\n");
            }else{
                in+=readNum;
            }
        }
        n+=readNum;
        printf("add %d flows,current n is %d\n",readNum,n);
        if(n>=FLOWSPERPROCESS) pthread_cond_signal(&cond_c);
        pthread_mutex_unlock(&mutex);
    }
    fclose(fp);
    printf("there is no more flows in flie!\n");
    while(1);
}
