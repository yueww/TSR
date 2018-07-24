/*************************************************************************
	> File Name: test.c
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月16日 星期二 11时11分22秒
 ************************************************************************/

#include "test.h"

/*
int main(){
    pid_t pid;
    pid = fork();
    if(pid>0){
        int exeRes=system("fprobe -i eth0 -f \"tcp&&port 80\" localhost:555");
        if(exeRes==127){
            printf("exec failed!\n");
        }else if(exeRes==-1){
            printf("fork failed or waitpid return error!\n");
        }else{
            char flag;
            int status;
            //pid_t exitPid;
            while(1){
                printf("input q to stop emitting flows!\n");
                scanf("%c",&flag);
                if(flag=='q'){*/
                    /*
                    int killRes=kill(emitPid,SIGABRT);
                    printf("the killRes is %d\n",killRes);
                    exitPid=waitpid(emitPid,&status,0);
                    if(exitPid!=emitPid){
                        printf("receive and process module terminated unexpectedly!\n");
                        printf("input q to stop emitting flows!\n");
                        continue;
                    }
                    if(WIFSIGNALED(status)){
                        printf("emitting stopped normally\n");
                    }else{
                        printf("emitting stopped abnormally\n");
                    }
                    */
                    /*
                    int res=system("ps aux | grep fprobe | grep -v grep | cut -c 9-15 | xargs kill -9");
                    while(res==-1||res==127){
                        res=system("ps aux | grep fprobe | grep -v grep | cut -c 9-15 | xargs kill -9");
                    }
                    break;
                }else{
                    continue;
                }
            }
            printf("press ctrl C to end entire process!\n");
            //while(1);
        }
    }else if(pid==0){
        pthread_t pro,con;
        pthread_mutex_init(&mutex,0);
        pthread_cond_init(&cond_p,0);
        pthread_cond_init(&cond_c,0);
        pthread_create(&pro,0,receiveData,0);
        pthread_create(&con,0,outputFlows,0);
        pthread_join(pro,0);
        pthread_join(con,0);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_p);
        pthread_cond_destroy(&cond_c);
    }else{
        printf("fail to launch an other process!\n");
        exit(1);
    }
    return 0;
}
*/

int main(int argc,char *argv[]){
    if(argc<3){
        printf("not enough arguments!\n");
        exit(0);
    }
    if(argc==4){
        if(strcmp(argv[3],"-t")==0){
            printf("please input host:");
            scanf("%s",host);
            printf("please input user:");
            scanf("%s",user);
            printf("please input password:");
            scanf("%s",password);
            printf("please input database:");
            scanf("%s",database);
            printf("please input port:");
            scanf("%u",&port);
            fflush(stdin);
            mysql_init(&mysql);
            if(!mysql_real_connect(&mysql,host,user,password,database,port,NULL,0)){
                printf("mysql_real_connect failed!\n");
                return 0;
            }
            useDb=true;
        }else{
            printf("unknown option:%s\n",argv[3]);
            return 0;
        }
    }
    if(argc>4){
        printf("too much arguments!\n");
        return 0;
    }
    /*
    set a share memory,use it to check whether export process is running. 
    */
    if((shmid=shmget(IPC_PRIVATE,SHM_SIZE,IPC_CREAT|0666))==-1){
        printf("shmget error!\n");
        exit(0);
    }
    if((shmptr=shmat(shmid,(const void *)0,0))==(void *)-1){
        printf("shmat error!\n");
        exit(0);
    }
    *(int *)shmptr=0;
    /*
    initiate correlative data structure. 
    */
    unusedChainHead=&flowNodeBuf[0];
    unusedChainTail=&flowNodeBuf[LINKNODEBUFSIZE-1];
    int i;
    for(i=0;i<LINKNODEBUFSIZE-1;i++){
        flowNodeBuf[i].next=&flowNodeBuf[i+1];
    }
    flowNodeBuf[i].next=NULL;
    srcIpRootNode=&srcIpInnerNodeBuf[0];
    currUsedSrcIpInnerNodes++;
    dstIpRootNode=&dstIpInnerNodeBuf[0];
    currUsedDstIpInnerNodes++;
    for(i=0;i<FLOWAMOUNTINMEM;i++){
        srcIpIndexNodeBuf[i].indexNum=i;
        srcPortIndexNodeBuf[i].indexNum=i;
        dstIpIndexNodeBuf[i].indexNum=i;
        dstPortIndexNodeBuf[i].indexNum=i;
    }
    genAllParas(VECTORDIM,W);
    pthread_t proc;
    int createRes=pthread_create(&proc,0,processFunc,0);
    if(createRes!=0){
        printf("fail to create storage process,exit system!\n");
        exit(0);
    }
    char *command=processCommand(argv[1],argv[2]);
    if(!command){
        exit(0);
    }
    int exeRes=system(command);
    free(command);
    if(exeRes==127){
        printf("fail to launch probe because of exec error!\n");
        exit(0);
    }else if(exeRes==-1){
        printf("fail to launch probe because of fork error!\n");
        exit(0);
    }else{
        char flag;
        while(1){
            printf("input q to stop emitting flows!\n");
            scanf("%c",&flag);
            if(flag=='q'){
                int res=system("ps aux | grep fprobe | grep -v grep | cut -c 9-15 | xargs kill -9");
                while(res==-1||res==127){
                    res=system("ps aux | grep fprobe | grep -v grep | cut -c 9-15 | xargs kill -9");
                }
                break;
            }else{
                continue;
            }
        }
        printf("emit process terminated!\n");
        sleep(1);
        while(*(int *)shmptr){
            
        }
        int canRes=pthread_cancel(pro);
        if(canRes!=0){
            printf("fail to cancel process which receive flows!\n");
        }
        threadpool_destroy(pThreadPool);
        canRes=pthread_cancel(con);
        if(canRes!=0){
            printf("fail to cancel process which index flows!\n");
        }
        /*
         resort left flowRecord in cache(flowBlock).left flowRecords are fewer than 40, so
         we can not resort them by call resort().
        */
        int k;
        tuple4vec vec;
        int key;
        for(k=0;k<n;k++){
            extractTuple4(&vec,flowBlock[out+k]);
            key=myHash(vec);
            insertFlowsToBucket(flowBlock[out+k],key);
        }
        /*
        flush the hashTable which is used in resort,because the left flowRecord can not satisfy the 
        old rule,they can not be exported automatically.
        */
        int i;
        flowRecordNode *pF;
        for(i=0;i<TABLESIZE;i++){
            if(hashTable[i].currNum>0){
                pF=&hashTable[i].head;
                int j;
                for(j=0;j<hashTable[i].currNum;j++){
                    pF=pF->next;
                    memcpy(&flowWaitExport[inBase++],&(pF->rec),FLOWSIZE);
                }
            }
        }
        /*
        index();
        int *exportRes=exportDataAndIndex();
        FILE *fp=fopen("./exportRecord.log","a+");
        */
        FILE *fp=fopen("./exportRecord.log","a+");
        while(outBase<inBase){
            int exportFlag=index();
            if(exportFlag){
                int *exportRes=exportDataAndIndex();
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
        int *exportRes=exportDataAndIndex();
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
    }
    mysql_close(&mysql);
    return 0;
}


void *processFunc(void *arg){
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYCHRONOUS,NULL);
    //pthread_t pro,con;
    pthread_mutex_init(&mutex,0);
    pthread_cond_init(&cond_p,0);
    pthread_cond_init(&cond_c,0);
    pthread_create(&pro,0,receiveData,0);
    pthread_create(&con,0,outputFlows,0);
    pthread_join(pro,0);
    pthread_join(con,0);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_p);
    pthread_cond_destroy(&cond_c);
}



char *processCommand(const char *opt,const char *name){
    char *command=(char *)malloc(sizeof(char)*COMMANDSIZE);
    if(!strcmp(opt,"-i")){
       strcpy(command,"fprobe -i ");
       strcat(command,name);
       strcat(command," -B 8192 -r 2 -q 10000 localhost:555");
    }else if(!strcmp(opt,"-f")){
       strcpy(command,"cat < ");
       strcat(command,name);
       strcat(command," | fprobe -i - -B 8192 -r 2 -q 10000 -t 10000:10000000 localhost:555");
    }else{
        printf("unknown options!\n");
        free(command);
        command=NULL;
    }
    return command;
}






