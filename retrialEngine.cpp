/*************************************************************************
	> File Name: retrialEngine.cpp
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月23日 星期五 16时10分59秒
 ************************************************************************/

#include "retrialEngine.h"

unsigned tmpCompressedIndexBuf[COMPRESSEDBUFSIZE];
unsigned tmpDecompressedIndexBuf[DECOMPRESSEDBUFSIZE];
char resFileName[48];
queue<pQueryFile> queryFileSet;


int getDecompressedIndex(const char *path,const char *indexName,const char *indexValue){
    int compressLen=0;
    if(!strcmp(indexName,"srcIp")||!strcmp(indexName,"dstIp")){
        byte ip[4];
        char ipStr[48];
        strcpy(ipStr,indexValue);
        if(convertIpStrToByteArr(ipStr,ip)==-1){
            return -1;
        }
        if((compressLen=retrialIpCompressedIndex(path,ip,tmpCompressedIndexBuf))==-1){
            printf("fail to retrial,error in %s\n",path);
            return -1;
        }
        if(compressLen==0) return 0;
    }else if(!strcmp(indexName,"srcPort")||!strcmp(indexName,"dstPort")){
        int portNum;
        char portStr[48];
        strcpy(portStr,indexValue);
        if((portNum=convertPortStrToByteArr(portStr))==-1){
            return -1;
        }
        if((compressLen=retrialPortCompressedIndex(path,portNum,tmpCompressedIndexBuf))==-1){
            printf("fail to retrial,error in %s\n",path);
            return -1;
        }
        if(compressLen==0) return 0;
    }else{
        printf("wrong index name!\n");
        return -1;
    }
    int decompressLen=0;
    if((decompressLen=decompressIndex(tmpCompressedIndexBuf,compressLen,tmpDecompressedIndexBuf))==-1){
        printf("fail to retrial,error in %s\n",path);
        return -1;
    }
    return decompressLen;
}

int retrialIpCompressedIndex(const char *path,byte ip[],unsigned *compressedIndexBuf){
    int fd=open(path,O_RDONLY);
    if(fd==-1){
        printf("fail to open file %s!\n",path);
        return -1;
    }
    unsigned offset[256];
    unsigned readRes=read(fd,offset,256*sizeof(unsigned));
    if(readRes==-1){
        printf("error in reading index file!\n");
        return -1;
    }
    int i;
    int seekRes;
    for(i=0;i<3;i++){
        if(offset[ip[i]]){
            seekRes=lseek(fd,offset[ip[i]],SEEK_SET);
            if(seekRes==-1){
                printf("error in reading index file!\n");
                return -1;
            }
            readRes=read(fd,offset,256*sizeof(unsigned));
            if(readRes==-1){
                printf("error in reading index file!\n");
                return -1;
            }
        }else{
            return 0;
        }
    }
    seekRes=lseek(fd,offset[ip[i]],SEEK_SET);
    if(seekRes==-1){
        printf("error in reading index file!\n");
        return -1;
    }
    int len;
    readRes=read(fd,&len,sizeof(int));
    if(readRes==-1){
        printf("error in reading index file!\n");
        return -1;
    }
    readRes=read(fd,compressedIndexBuf,len*sizeof(unsigned));
    if(readRes==-1){
        printf("error in reading index file!\n");
        return -1;
    }
    return len;
}

int retrialPortCompressedIndex(const char *path,unsigned port,unsigned *compressedIndexBuf){
    int fd=open(path,O_RDONLY);
    if(fd==-1){
        printf("fail to open index file %s!\n",path);
        return -1;
    }
    unsigned offset;
    int seekRes=lseek(fd,port*sizeof(int),SEEK_SET);
    if(seekRes==-1){
        printf("error in reading index file %s!\n",path);
        return -1;
    }
    int readRes=read(fd,&offset,sizeof(unsigned));
    if(readRes==-1){
        printf("error in reading index file %s!\n",path);
        return -1;
    }
    if(offset==0) return 0;
    seekRes=lseek(fd,offset,SEEK_SET);
    if(seekRes==-1){
        printf("error in reading index file %s!\n",path);
        return -1;
    }
    int len;
    readRes=read(fd,&len,sizeof(int));
    if(readRes==-1){
        printf("error in reading index file %s!\n",path);
        return -1;
    }
    readRes=read(fd,compressedIndexBuf,len*sizeof(unsigned));
    if(readRes==-1){
        printf("error in reading index file %s!\n",path);
        return -1;
    }
    return len;
}

int decompressIndex(unsigned *compressedIndexBuf,int compressLen,unsigned *decompressedIndexBuf){
    int len=0;
    decompressedIndexBuf[len++]=compressedIndexBuf[0];
    int currBase=compressedIndexBuf[0];
    int i;
    for(i=1;i<compressLen;i++){
        if(!TESTU5(compressedIndexBuf[i])){
            for(int j=0;j<5;j++){
                currBase+=((((MASKUNIT5<<6*j)&compressedIndexBuf[i]))>>6*j);
                decompressedIndexBuf[len++]=currBase;
            }
        }else if(!TESTU3(compressedIndexBuf[i])){
            for(int j=0;j<3;j++){
                currBase+=((((MASKUNIT3<<10*j)&compressedIndexBuf[i]))>>10*j);
                decompressedIndexBuf[len++]=currBase;
            }
        }else if(!TESTU2(compressedIndexBuf[i])){
            currBase+=(MASKUNIT2&compressedIndexBuf[i]);
            decompressedIndexBuf[len++]=currBase;
            if(((MASKUNIT2<<15)&compressedIndexBuf[i])!=0){
                currBase+=((((MASKUNIT2<<15)&compressedIndexBuf[i]))>>15);
                decompressedIndexBuf[len++]=currBase;
            }
        }else{
            if(compressedIndexBuf[i]==currBase){
                i++;
                int k;
                for(k=0;k<compressedIndexBuf[i];k++){
                    currBase++;
                    decompressedIndexBuf[len++]=currBase;
                }
            }else{
                currBase=compressedIndexBuf[i];
                decompressedIndexBuf[len++]=compressedIndexBuf[i];
            }
        }
    }
    return len;
}

int retrialFlows(const char *path,unsigned *decompressedIndexBuf,int decompressLen){
    int rfd,wfd;
    rfd=open(path,O_RDONLY);
    if(rfd==-1){
        printf("fail to open file %s",path);
        return -1;
    }
    if(resFileName[0]==0){
        int id=countFiles("./result/","retrialRes");
        strcpy(resFileName,"retrialRes");
        int nameLen=strlen(resFileName);
        resFileName[nameLen]='0'+id;
        resFileName[nameLen+1]=0;
    }
    char filePath[128];
    strcpy(filePath,"./result/");
    strcat(filePath,resFileName);
    wfd=open(filePath,O_RDWR | O_CREAT | O_APPEND,0666);
    if(wfd==-1){
        printf("fail to open file %s",path);
        return -1;
    }
    int i=0;
    int seekRes,readRes;
    flowRecord *recBuf=(flowRecord *)malloc(decompressLen*FLOWSIZE);
    for(;i<decompressLen;i++){
        seekRes=lseek(rfd,decompressedIndexBuf[i]*FLOWSIZE,SEEK_SET);
        if(seekRes==-1){
            printf("error in reading index file!\n");
            return -1;
        }
        readRes=read(rfd,recBuf+i,FLOWSIZE);
        if(readRes==-1){
            printf("error in reading index file!\n");
            return -1;
        }
    }
    int writeRes;
    writeRes=write(wfd,recBuf,decompressLen*FLOWSIZE);
    if(writeRes==-1){
        printf("error in writing result!\n");
        return -1;
    }
    close(rfd);
    close(wfd);
    return writeRes/FLOWSIZE;
}


void formatShow(const char *path,char *opt,const char *command){
    int rfd=open(path,O_RDONLY);
    if(rfd==-1){
        printf("fail to open raw retrialRes file!\n");
        return;
    }
    if(!strcmp(opt,"-q")){
        FILE *fp=fopen("./formatRetrialRes","a+");
        if(fp==NULL){
            printf("fail to open format result file!\n");
            return;
        }
        fprintf(fp,"retrial command:%s\n",command);
        fprintf(fp,"          srcIp srcPort           dstIp dstPort proto       pkts       octs        firstTime         lastTime\n");
        unsigned long size=getFileSize(path);
        if(size==-1){
            printf("fail to get raw retrialRes file size!\n");
            return;
        }
        unsigned flowNum=size/FLOWSIZE;
        flowRecord *recBuf=(flowRecord *)malloc(FLOWSIZE*flowNum);
        int readRes=read(rfd,recBuf,FLOWSIZE*flowNum);
        if(readRes!=FLOWSIZE*flowNum){
            printf("fail to read whole raw retrialRes file!\n");
            return;
        }
        close(rfd);
        for(int i=0;i<flowNum;i++){
            char *srcIp=formatIp(recBuf[i].srcIp);
            char *dstIp=formatIp(recBuf[i].destIp);
            char *firstTime=formatTime(recBuf[i].firstTime);
            char *lastTime=formatTime(recBuf[i].lastTime);
            unsigned short srcPort=formatPort(recBuf[i].srcPort);
            unsigned short dstPort=formatPort(recBuf[i].destPort);
            unsigned pkts=formatStatis(recBuf[i].dPkts);
            unsigned octs=formatStatis(recBuf[i].dOcts);
            fprintf(fp,"%15s %7u %15s %7u %5u %10u %10u %16s %16s\n",srcIp,srcPort,dstIp,dstPort,recBuf[i].proto,pkts,octs,firstTime,lastTime);
        }
        fclose(fp);
    }else if(!strcmp(opt,"-p")){
        printf("input c to continue,q to exit!\n");
        fprintf(stdout,"          srcIp srcPort           dstIp dstPort proto       pkts       octs        firstTime         lastTime\n");
        flowRecord *recBuf=(flowRecord *)malloc(FLOWSIZE*10);
        /*
        int readRes=read(fd,recBuf,FLOWSIZE*10);
        if(readRes==-1){
            printf("error when reading raw retrialRes file!\n");
            return;
        }
        */
        int flowNum;
        char flag='c';
        while(1){
            if(flag=='c'){
                int readRes=read(rfd,recBuf,FLOWSIZE*10);
                if(readRes==-1){
                    printf("error when reading raw retrialRes file!\n");
                    return;
                }
                flowNum=readRes/FLOWSIZE;
                for(int i=0;i<flowNum;i++){
                    char *srcIp=formatIp(recBuf[i].srcIp);
                    char *dstIp=formatIp(recBuf[i].destIp);
                    char *firstTime=formatTime(recBuf[i].firstTime);
                    char *lastTime=formatTime(recBuf[i].lastTime);
                    unsigned short srcPort=formatPort(recBuf[i].srcPort);
                    unsigned short dstPort=formatPort(recBuf[i].destPort);
                    unsigned pkts=formatStatis(recBuf[i].dPkts);
                    unsigned octs=formatStatis(recBuf[i].dOcts);
                    fprintf(stdout,"%15s %7u %15s %7u %5u %10u %10u %16s %16s\n",srcIp,srcPort,dstIp,dstPort,recBuf[i].proto,pkts,octs,firstTime,lastTime);
                }
                if(readRes<FLOWSIZE*10){
                    break;
                }
                struct termios oldAttr,newAttr;
                tcgetattr(STDIN_FILENO,&oldAttr);
                newAttr=oldAttr;
                newAttr.c_lflag &= ~(ECHO | ICANON);
                tcsetattr(STDIN_FILENO,TCSANOW,&newAttr);
                flag=getchar();
                tcsetattr(STDIN_FILENO,TCSANOW,&oldAttr);
            }else if(flag=='q'){
                break;
            }else{
                printf("wrong option!input c to continue,q to exit!\n");
                struct termios oldAttr,newAttr;
                tcgetattr(STDIN_FILENO,&oldAttr);
                newAttr=oldAttr;
                newAttr.c_lflag &= ~(ECHO | ICANON);
                tcsetattr(STDIN_FILENO,TCSANOW,&newAttr);
                flag=getchar();
                tcsetattr(STDIN_FILENO,TCSANOW,&oldAttr);
            }
        }
        close(rfd);
    }else{
        printf("unknown option!\n");
        return;
    }
}

unsigned long getFileSize(const char *path){
    struct stat statBuf;
    if(stat(path,&statBuf)<0){
        return -1;
    }else{
        return statBuf.st_size;
    }
}


char *formatIp(byte *ip){
    char *res=(char *)malloc(sizeof(char)*48);
    res[0]=0;
    char subIp1[4],subIp2[4],subIp3[4],subIp4[4];
    bToStr(subIp1,ip[0]);
    bToStr(subIp2,ip[1]);
    bToStr(subIp3,ip[2]);
    bToStr(subIp4,ip[3]);
    strcat(res,subIp1);
    int len=strlen(res);
    res[len]='.';
    res[len+1]=0;
    strcat(res,subIp2);
    len=strlen(res);
    res[len]='.';
    res[len+1]=0;
    strcat(res,subIp3);
    len=strlen(res);
    res[len]='.';
    res[len+1]=0;
    strcat(res,subIp4);
    return res;
}

char *formatTime(byte *time){
    char *res=(char *)malloc(sizeof(char)*48);
    time_t t=time[0]*0x1000000+time[1]*0x10000+time[2]*0x100+time[3];
    struct tm *pt=localtime(&t);
    sprintf(res,"%02d-%02d %02d:%02d:%02d",pt->tm_mon+1,pt->tm_mday,pt->tm_hour,pt->tm_min,pt->tm_sec);
    return res;
}

unsigned short formatPort(byte *port){
    unsigned short portNum;
    portNum=port[0]*0x100+port[1];
    return portNum;
}

unsigned formatStatis(byte *statis){
    unsigned size;
    size=statis[0]*0x1000000+statis[1]*0x10000+statis[2]*0x100+statis[3];
    return size;
}

void bToStr(char str[],byte num){
    int i=0;
    if(num>=100){
        str[3]=0;
        while(num&&(i<3)){
            str[2-i]=(num%10)+'0';
            num/=10;
            i++;
        }
    }else if(num>=10){
        str[2]=0;
        while(num&&(i<2)){
            str[1-i]=(num%10)+'0';
            num/=10;
            i++;
        }
    }else{
        str[0]=num+'0';
        str[1]=0;
    }
}

int convertIpStrToByteArr(char *ipStr,byte ip[]){
    int i=0;
    char ipStrBuf[128];
    memcpy(ipStrBuf,ipStr,strlen(ipStr));
    ipStrBuf[strlen(ipStr)]=0;
    char *deli=".";
    char *numStr=strtok(ipStrBuf,deli);
    while(numStr&&(i<4)){
        int ipNum=strToi(numStr);
        if(ipNum==-1){
            printf("invalid ip!\n");
            return -1;
        }else if(ipNum>255){
            printf("ip out of range!\n");
            return -1;
        }else{
            ip[i]=ipNum;
        }
        numStr=strtok(NULL,deli);
        i++;
    }
    if(i<4){
        printf("incomplete ip address!\n");
        return -1;
    }
    return 0;
}

int convertPortStrToByteArr(char *portStr){
    int portNum=strToi(portStr);
    if(portNum==-1){
        printf("invalid port number!\n");
        return -1;
    }else if(portNum>65535){
        printf("port number out of range!\n");
        return -1;
    }else{
        return portNum;
    }
}

int strToi(char *str){
    int res=0;
    while(*str){
        int num=*str-'0';
        if(num>=0&&num<=9){
            res=res*10+num;
        }else{
            printf("can not convert str to integer!\n");
            return -1;
        }
        str++;
    }
    return res;
}

int getQueryFileSet(bool flag[]){
    int num=countFiles("./index/","srcIpIndex");
    for(int i=0;i<num;i++){
        pQueryFile pqf=new queryFile();
        pqf->dataFileName=new char[48];
        strcpy(pqf->dataFileName,"data");
        (pqf->dataFileName)[4]=i+'0';
        (pqf->dataFileName)[5]=0;
        if(flag[0]){
            pqf->srcIpIndexFileName=new char[48];
            strcpy(pqf->srcIpIndexFileName,"srcIpIndex");
            (pqf->srcIpIndexFileName)[10]=i+'0';
            (pqf->srcIpIndexFileName)[11]=0;
        }
        if(flag[1]){
            pqf->srcPortIndexFileName=new char[48];
            strcpy(pqf->srcPortIndexFileName,"srcPortIndex");
            (pqf->srcPortIndexFileName)[12]=i+'0';
            (pqf->srcPortIndexFileName)[13]=0;
        }
        if(flag[2]){
            pqf->dstIpIndexFileName=new char[48];
            strcpy(pqf->dstIpIndexFileName,"dstIpIndex");
            (pqf->dstIpIndexFileName)[10]=i+'0';
            (pqf->dstIpIndexFileName)[11]=0;
        }
        if(flag[3]){
            pqf->dstPortIndexFileName=new char[48];
            strcpy(pqf->dstPortIndexFileName,"dstPortIndex");
            (pqf->dstPortIndexFileName)[12]=i+'0';
            (pqf->dstPortIndexFileName)[13]=0;
        }
        queryFileSet.push(pqf);
    }
    return num;
}

int getQueryFileSet(MYSQL *mysql,bool flag[],const char *startTime,const char *endTime){
    int num=0;    
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query1[512];
    sprintf(query1,"SELECT data_file,src_ip_index_file,src_port_index_file,dst_ip_index_file,\
            dst_port_index_file FROM tsr WHERE timestamp > '%s' AND timestamp <= '%s'",\
            startTime,endTime);
    if(mysql_real_query(mysql,query1,(unsigned long)strlen(query1))){
        printf("mysql_real_query failed!\n");
        return -1;
    }
    res=mysql_store_result(mysql);
    if(res==NULL){
        printf("mysql_store_result failed!\n");
        return -1;
    }
    while(row=mysql_fetch_row(res)){
        pQueryFile pqf=new queryFile();
        pqf->dataFileName=new char[48];
        strcpy(pqf->dataFileName,row[0]);
        if(flag[0]){
            pqf->srcIpIndexFileName=new char[48];
            strcpy(pqf->srcIpIndexFileName,row[1]);
        }
        if(flag[1]){
            pqf->srcPortIndexFileName=new char[48];
            strcpy(pqf->srcPortIndexFileName,row[2]);
        }
        if(flag[2]){
            pqf->dstIpIndexFileName=new char[48];
            strcpy(pqf->dstIpIndexFileName,row[3]);
        }
        if(flag[3]){
            pqf->dstPortIndexFileName=new char[48];
            strcpy(pqf->dstPortIndexFileName,row[4]);
        }
        queryFileSet.push(pqf);
        num++;
    }
    mysql_free_result(res);
    if(num==0){
        char query2[512];
        sprintf(query2,"SELECT data_file,src_ip_index_file,src_port_index_file,dst_ip_index_file,\
            dst_port_index_file FROM orders WHERE timestamp > '%s' ORDER BY timestamp LIMIT 1",\
            endTime);
        if(mysql_real_query(mysql,query2,(unsigned long)strlen(query2))){
            printf("mysql_real_query failed!\n");
            return -1;
        }
        res=mysql_store_result(mysql);
        if(res==NULL){
            printf("mysql_store_result failed!\n");
            return -1;
        }
        if(row=mysql_fetch_row(res)){
            pQueryFile pqf=new queryFile();
            pqf->dataFileName=new char[48];
            strcpy(pqf->dataFileName,row[0]);
            if(flag[0]){
                pqf->srcIpIndexFileName=new char[48];
                strcpy(pqf->srcIpIndexFileName,row[1]);
            }
            if(flag[1]){
                pqf->srcPortIndexFileName=new char[48];
                strcpy(pqf->srcPortIndexFileName,row[2]);
            }
            if(flag[2]){
                pqf->dstIpIndexFileName=new char[48];
                strcpy(pqf->dstIpIndexFileName,row[3]);
            }
            if(flag[3]){
                pqf->dstPortIndexFileName=new char[48];
                strcpy(pqf->dstPortIndexFileName,row[4]);
            }
            queryFileSet.push(pqf);
            num++;
        }
        mysql_free_result(res);
    }
    //mysql_close(&mysql);
    return num;
}

void clearQueue(){
    while(!queryFileSet.empty()){
        pQueryFile pqf=queryFileSet.front();
        if(pqf->dataFileName) delete pqf->dataFileName;
        if(pqf->srcIpIndexFileName) delete pqf->srcIpIndexFileName;
        if(pqf->srcPortIndexFileName) delete pqf->srcPortIndexFileName;
        if(pqf->dstIpIndexFileName) delete pqf->dstIpIndexFileName;
        if(pqf->dstPortIndexFileName) delete pqf->dstPortIndexFileName;
        delete pqf;
        queryFileSet.pop();
    }    
}

int getQueryTime(const char *raw,char *formated){
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now=localtime(&now);
    int len=strlen(raw);
    if(len==19){
        memcpy(formated,raw,19);
    }else if(len==14){
        strftime(formated,48,"%Y-",tm_now);
        memcpy(formated+5,raw,14);
    }else if(len==8){
        strftime(formated,48,"%Y-%m-%d ",tm_now);
        memcpy(formated+11,raw,8);
    }else if(len==10){
        if(!strchr(raw,':')){
            memcpy(formated,raw,10);
            memcpy(formated+10," 00:00:00",9);
        }else{
            formated[0]=0;
            return -1;
        }
    }else if(len==5){
        if(!strchr(raw,':')){
            strftime(formated,48,"%Y-",tm_now);
            memcpy(formated+5,raw,5);
            memcpy(formated+10," 00:00:00",9);
        }else{
            formated[0]=0;
            return -1;
        }
    }else{
        formated[0]=-1;
        return -1;
    }
    formated[19]='\0';
    return len;
}
