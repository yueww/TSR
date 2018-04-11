/*************************************************************************
	> File Name: retrialEngine.h
	> Author:wangyue 
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年03月23日 星期五 15时16分30秒
 ************************************************************************/

#ifndef _RETRIALENGINE_H
#define _RETRIALENGINE_H
#include "common.h"
#include "myToolFunc.h"
#include "termios.h"
#include <queue>

using namespace std;

#define MASKFLAG 0xc0000000
#define UNIT5 0xc0000000
#define UNIT3 0x80000000
#define UNIT2 0x40000000
#define MASKUNIT5 0x0000003f
#define MASKUNIT3 0x000003ff
#define MASKUNIT2 0x00007fff
#define TESTU5(a) ((a&MASKFLAG)^UNIT5)
#define TESTU3(a) ((a&MASKFLAG)^UNIT3)
#define TESTU2(a) ((a&MASKFLAG)^UNIT2)

#define COMPRESSEDBUFSIZE 1000000
#define DECOMPRESSEDBUFSIZE 1000000

typedef struct queryFile{
    char *dataFileName;
    char *srcIpIndexFileName;
    char *dstIpIndexFileName;
    char *srcPortIndexFileName;
    char *dstPortIndexFileName;
    queryFile(){
        dataFileName=NULL;
        srcIpIndexFileName=NULL;
        dstIpIndexFileName=NULL;
        srcPortIndexFileName=NULL;
        dstPortIndexFileName=NULL;
    }
}queryFile,*pQueryFile;

int getDecompressedIndex(const char *path,const char *indexName,const char *indexValue);
int retrialIpCompressedIndex(const char *path,byte ip[],unsigned *compressedIndexBuf);
int retrialPortCompressedIndex(const char *path,unsigned port,unsigned *compressedIndexBuf);
int decompressIndex(unsigned *compressedIndexBuf,int compressLen,unsigned *decompressedIndexBuf);
int retrialFlows(const char *path,unsigned *decompressedIndexBuf,int decompressLen);
void formatShow(const char *path,char *opt);
int getQueryFileSet(bool flag[]);
int convertIpStrToByteArr(char *ipStr,byte ip[]);
int convertPortStrToByteArr(char *portStr);
int strToi(char *str);
unsigned long getFileSize(const char *path);
char *formatIp(byte *ip);
char *formatTime(byte *time);
unsigned short formatPort(byte *port);
unsigned formatStatis(byte *statis);
void bToStr(char str[],byte num);

#endif
