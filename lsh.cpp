/*************************************************************************
	> File Name: lsh.cpp
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月31日 星期三 14时11分10秒
 ************************************************************************/
#include "lsh.h"

vector<double> aVecParas[HASHFUNCNUM];
double bParas[HASHFUNCNUM];

/*
int main(){
    genAllParas(12,W);
    int i,j;
    for(i=0;i<HASHFUNCNUM;i++){
        printf("a:");
        for(j=0;j<12;j++){
            printf("%f ",aVecParas[i][j]);
        }
        printf("b:%f\n",bParas[i]);
    }
    return 0;
}
*/

vector<double> genAPara(int dim){
    vector<double> aPara;
    int i;
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> d(0,1);
    for(i=0;i<dim;i++){
        aPara.push_back(d(gen));
    }
    return aPara;
}

double genBPara(double w){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> d(0,w);
    return d(gen);
}

void genAllParas(int dim,double w){
    int i;
    for(i=0;i<HASHFUNCNUM;i++){
        aVecParas[i]=genAPara(dim);
        bParas[i]=genBPara(w);
    }
}

int myHash(tuple4vec tvec){
    int i,j;
    int index=0;
    double tmp;
    for(i=0;i<HASHFUNCNUM;i++){
        tmp=bParas[i];
        for(j=0;j<VECTORDIM;j++){
            tmp+=aVecParas[i][j]*tvec.vec[j];
        }
        index+=(int)(tmp/W);
    }
    index=index%TABLESIZE;
    if(index<0) return index+TABLESIZE;
    return index;
}
