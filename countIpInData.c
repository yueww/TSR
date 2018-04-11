/*************************************************************************
	> File Name: countIpInData.c
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月26日 星期一 14时24分20秒
 ************************************************************************/

#include "common.h"

flowRecord recBuf[400];

int main(){
    int fd=open("./data",O_RDONLY);
    read(fd,recBuf,360*FLOWSIZE);
    close(fd);
    byte srcIp[4],dstIp[4];
    srcIp[0]=dstIp[0]=192;
    srcIp[1]=dstIp[1]=168;
    srcIp[2]=dstIp[2]=1;
    srcIp[3]=dstIp[3]=200;
    int i=0;
    int srcMatch=0,dstMatch=0;
    for(;i<360;i++){
        if(!memcmp(srcIp,recBuf[i].srcIp,4)){
            srcMatch++;
        }
        if(!memcmp(dstIp,recBuf[i].destIp,4)){
            dstMatch++;
        }
    }
    printf("srcIp match %d flows,dstIp match %d flows!\n",srcMatch,dstMatch);
    return 0;
}
