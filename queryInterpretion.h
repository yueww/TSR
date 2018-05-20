/*************************************************************************
	> File Name: queryInterpretion.h
	> Author: wangyue 
	> Mail: yuew951115@gmail.com
	> Created Time: 2018年05月02日 星期三 10时38分37秒
 ************************************************************************/

#ifndef _QUERYINTERPRETION_H
#define _QUERYINTERPRETION_H
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cctype>
#include <cstdio>
#include <cstring>
#include "retrialEngine.h"

#define QUERYLEN 4096
#define RETRLEN 8192

using namespace std;


int computeQuery(const char *queryLang,pQueryFile pqf,unsigned resIndexBuf[]);
char *checkCommand(const char *command,char *showOpt,char *sTime,char *eTime);
void clearStack(stack<string> &optrStack,stack<vector<int> > &opndStack);
vector<int> getIntersection(vector<int> arr1,vector<int> arr2);
vector<int> getUnion(vector<int> arr1,vector<int> arr2);
vector<int> logicCompute(vector<int> arr1,vector<int> arr2,string optr);
int getPos(string optr);
string getIndexName(const char *tmp);
int checkEqualSymbol(const char *tmp);
int getIndexValue(const char *tmp,char indexValue[]);
string getOptr(const char *tmp);

#endif
