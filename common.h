/*************************************************************************
	> File Name: common.h
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月15日 星期一 14时46分01秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define BUFSIZE 4096
#define FLOWSIZE 48
#define BLOCKSIZE 400000
#define PKTHEADERSIZE 24
#define FLOWSPERPROCESS 4000
#define VECTORDIM 10
#define FLOWAMOUNTINMEM 10000000
#define SHM_SIZE 4096

typedef unsigned char byte;

/*
typedef struct tuple4vec{
    byte vec[12];
}tuple4vec;
*/

typedef struct tuple4vec{
    unsigned short vec[10];
}tuple4vec;

typedef struct flowRecord{
    byte srcIp[4];
    byte destIp[4];
    byte nextHop[4];
    byte inSnmpIndex[2];
    byte outSnmpIndex[2];
    byte dPkts[4];
    byte dOcts[4];
    byte firstTime[4];
    byte lastTime[4];
    byte srcPort[2];
    byte destPort[2];
    byte pad1;
    byte tcp_flags;
    byte proto;
    byte tos;
    byte src_as[2];
    byte dst_as[2];
    byte src_mask;
    byte dst_mask;
    byte pad2[2];
}flowRecord;

typedef struct flowRecordNode{
    flowRecordNode *next;
    flowRecord rec;
}flowRecordNode;

typedef struct hashBucket{
    int currNum;
    flowRecordNode head;
}hashBucket;

/*
typedef struct flowRecord{
    byte srcIp[4];
    byte destIp[4];
    byte nextHop[4];
    unsigned short inSnmpIndex;
    unsigned short outSnmpIndex;
    unsigned dPkts;
    unsigned dOcts;
    unsigned firstTime;
    unsigned lastTime;
    unsigned short srcPort;
    unsigned short destPort;
    byte pad1;
    unsigned short proto;
    byte tos;
    unsigned short flags;
    byte pad2;
    byte pad3;
    unsigned reserved;
}flowRecord;
*/


#endif
