/*************************************************************************
	> File Name: myToolFunc.h
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月18日 星期四 16时20分02秒
 ************************************************************************/

#ifndef _MYTOOLFUNC_H
#define _MYTOOLFUNC_H

#include<stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "dirent.h"
#include "sys/stat.h"
#include <time.h>

#define MAXARGSNUM 16

//unsigned long mySystem(const char *cmd);
//int splitStr(const char *str,char *(*res)[]);
int countFiles(const char *path,const char *filter);
int iToStr(int num,char str[]);
void getNowTime(char formatTime[],int len);

#endif
