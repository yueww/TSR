/*************************************************************************
	> File Name: processFlows.c
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月16日 星期二 10时40分25秒
 ************************************************************************/

#include "processFlows.h"

void *outputFlows(void *ptr){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    /*
    FILE *fp=fopen("./data","wb");
    if(fp==NULL){
        printf("fail to create file\n");
        pthread_exit(0);
    }
    */
    /*
    int fd;
    fd=open("./data",O_RDWR | O_CREAT | O_APPEND,S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
    if(fd==-1){
        printf("fail to create file\n");
        pthread_exit(0);
    }
    
    
    int writeNum=0;
    while(1){
        pthread_mutex_lock(&mutex);
        while(n<FLOWSPERPROCESS) pthread_cond_wait(&cond_c,&mutex);
        if(out==BLOCKSIZE) out=0;
        writeNum=write(fd,flowBlock+out,FLOWSPERPROCESS*FLOWSIZE);
        if(writeNum==-1){
            printf("write error!\n");
            exit(0);
        }
        out+=FLOWSPERPROCESS;
        n-=FLOWSPERPROCESS;
        if(n<BLOCKSIZE) pthread_cond_signal(&cond_p);
        pthread_mutex_unlock(&mutex);
    }
    */
    //printf("file descriptor is %d\n",fd);
    //int writeNum;
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    
    while(1){
        pthread_mutex_lock(&mutex);
        while(n<FLOWSPERPROCESS) pthread_cond_wait(&cond_c,&mutex);
        if(out==BLOCKSIZE) out=0;
        memcpy(flows,flowBlock+out,FLOWSPERPROCESS*FLOWSIZE);
        out+=FLOWSPERPROCESS;
        n-=FLOWSPERPROCESS;
        resort();
        byte exportFlag=index();
        if(exportFlag){
            pid_t pid;
            if((pid=fork())<0){
                printf("fail to create export process,program will exit!\n");
                exit(0);
            }else if(pid==0){
                *(int *)shmptr=1;
                int *exportRes=exportDataAndIndex();
                FILE *fp=fopen("./exportRecord.log","a+");
                int writeRes[6];
                writeRes[0]=fprintf(fp,"export %d flows.\n",exportRes[0]/48);
                writeRes[1]=fprintf(fp,"data take up %d bytes\n",exportRes[0]);
                writeRes[2]=fprintf(fp,"srcIp take up %d bytes\n",exportRes[1]);
                writeRes[3]=fprintf(fp,"dstIp take up %d bytes\n",exportRes[2]);
                writeRes[4]=fprintf(fp,"srcPort take up %d bytes\n",exportRes[3]);
                writeRes[5]=fprintf(fp,"dstPort take up %d bytes\n",exportRes[4]);
                if(writeRes[0]<0 || writeRes[1]<0 || writeRes[2]<0 || writeRes[3]<0 ||writeRes[4]<0 ||writeRes[5]<0){
                    printf("error in write log!\n");
                } 
                free(exportRes);
                fclose(fp);
                *(int *)shmptr=0;
                exit(0);
            }else{
                memcpy(flowWaitExport,flowWaitExport+outBase,(inBase-outBase)*FLOWSIZE);
                int i;
                for(i=0;i<outBase;i++){
                    srcIpIndexNodeBuf[i].next=NULL;
                    dstIpIndexNodeBuf[i].next=NULL;
                    srcPortIndexNodeBuf[i].next=NULL;
                    dstPortIndexNodeBuf[i].next=NULL;
                }
                memset(srcIpInnerNodeBuf,0,sizeof(innerNode)*currUsedSrcIpInnerNodes);
                memset(dstIpInnerNodeBuf,0,sizeof(innerNode)*currUsedDstIpInnerNodes);
                memset(srcIpLeafNodeBuf,0,sizeof(leafNode)*currUsedSrcIpLeafNodes);
                memset(dstIpLeafNodeBuf,0,sizeof(leafNode)*currUsedDstIpLeafNodes);
                memset(&srcPortIndexTable,0,sizeof(portIndexTable));
                memset(&dstPortIndexTable,0,sizeof(portIndexTable));
                srcIpRootNode=&srcIpInnerNodeBuf[0];
                dstIpRootNode=&dstIpInnerNodeBuf[0];
                inBase-=outBase;
                outBase=0;
                currUsedSrcIpInnerNodes=1;
                currUsedDstIpInnerNodes=1;
                currUsedSrcIpLeafNodes=0;
                currUsedDstIpLeafNodes=0;
                currUsedSrcIpIndexNodes=0;
                currUsedDstIpIndexNodes=0;
                currUsedSrcPortIndexNodes=0;
                currUsedDstPortIndexNodes=0;
            }
        }
        
        /*
        if((writeNum=write(fd,flows,FLOWSIZE*FLOWSPERPROCESS))!=FLOWSIZE*FLOWSPERPROCESS){
            printf("only write %d bytes,write error!\n",writeNum);
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
        printf("write 40 flows to file\n");
        */
    
        if(n<BLOCKSIZE) pthread_cond_signal(&cond_p);
        pthread_mutex_unlock(&mutex);
    }
    
}
