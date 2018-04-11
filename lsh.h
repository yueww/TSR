/*************************************************************************
	> File Name: lsh.h
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月31日 星期三 13时45分50秒
 ************************************************************************/

#ifndef _LSH_H
#define _LSH_H
#include "common.h"
#include <algorithm>
#include <vector>
#include <iomanip>
#include <limits.h>
#include <map>
#include <random>

using namespace std;

#define HASHFUNCNUM 10
#define W 5
#define TABLESIZE 1024

vector<double> genAPara(int dim);
double genBPara(double w);
void genAllParas(int dim,double w);
int myHash(tuple4vec tvec);

#endif
