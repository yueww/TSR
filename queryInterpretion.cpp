/*************************************************************************
	> File Name: queryInterpretion.cpp
	> Author: wangyue
	> Mail: yuew951115@gmail.com
	> Created Time: 2018年05月02日 星期三 10时52分01秒
 ************************************************************************/

#include "queryInterpretion.h"
#include "retrialEngine.h"

extern int tmpDecompressedIndexBuf[];

char priority[5][5]={
    {'>','<','<','>','>'},
    {'>','>','<','>','>'},
    {'<','<','<','=','x'},
    {'>','>','x','>','>'},
    {'<','<','<','x','='}
};

vector<int> getIntersection(vector<int> arr1,vector<int> arr2){
    vector<int> res;
    int i=0,j=0;
    int len1=arr1.size();
    int len2=arr2.size();
    while((i<len1)&&(j<len2)){
        if(arr1[i]>arr2[j]){
            j++;
        }else if(arr1[i]<arr2[j]){
            i++;
        }else{
            res.push_back(arr1[i]);
            i++;
            j++;
        }
    }
    return res;
}

vector<int> getUnion(vector<int> arr1,vector<int> arr2){
    vector<int> res;
    int i=0,j=0;
    int len1=arr1.size();
    int len2=arr2.size();
    while((i<len1)&&(j<len2)){
        while(arr1[i]<arr2[j]) res.push_back(arr1[i++]);
        if(arr1[i]==arr2[j]) i++;
        while(arr1[i]>arr2[j]) res.push_back(arr2[j++]);
        if(arr1[i]==arr2[j]) j++;
    }
    while(i<len1) res.push_back(arr1[i++]);
    while(j<len2) res.push_back(arr2[j++]);
    return res;
}

vector<int> logicCompute(vector<int> arr1,vector<int> arr2,string optr){
    if(optr.compare("&&")==0){
        return getIntersection(arr1,arr2);
    }else if(optr.compare("||")==0){
        return getUnion(arr1,arr2);
    }else{
        printf("unknown opreator!\n");
        return vector<int>();
    }
}

int getPos(string optr){
    if(optr.compare("||")==0){
        return 0;
    }else if(optr.compare("&&")==0){
        return 1;
    }else if(optr.compare("(")==0){
        return 2;
    }else if(optr.compare(")")==0){
        return 3;
    }else if(optr.compare("#")==0){
        return 4;
    }else{
        return -1;
    }
}

int computeQuery(const char *queryLang,pQueryFile pqf,unsigned resIndexBuf[]){
    int resLen=0;
    stack<string> optrStack;
    stack<vector<int> > opndStack;
    optrStack.push(string("#"));
    char query[QUERYLEN];
    strcpy(query,queryLang);
    int qLen=strlen(queryLang);
    query[qLen]='#';
    query[qLen+1]=0;
    char *tmp=query;
    while((*tmp)!='#'||(optrStack.top()).compare("#")!=0){
        if(isalpha(*tmp)){
            string indexName=getIndexName(tmp);
            char indexPath[128];
            strcpy(indexPath,"./index/");
            if(indexName.compare("srcIp")==0){
                strcat(indexPath,pqf->srcIpIndexFileName);
            }else if(indexName.compare("srcPort")==0){
                strcat(indexPath,pqf->srcPortIndexFileName);
            }else if(indexName.compare("dstIp")==0){
                strcat(indexPath,pqf->dstIpIndexFileName);
            }else if(indexName.compare("dstPort")==0){
                strcat(indexPath,pqf->dstPortIndexFileName);
            }else{
                printf("indexName which is not supported!\n");
                clearStack(optrStack,opndStack);
                return -1;
            }
            tmp+=indexName.length();
            int fowardSteps=checkEqualSymbol(tmp);
            if(fowardSteps==-1) {
                clearStack(optrStack,opndStack);
                return -1;               
            }
            tmp+=fowardSteps;
            char indexValue[48];
            int valueLen=getIndexValue(tmp,indexValue);
            if(fowardSteps==-1) {
                clearStack(optrStack,opndStack);
                return -1;               
            }
            tmp+=valueLen;
            while(*tmp==' ') tmp++;
            int decompressLen=getDecompressedIndex(indexPath,indexName.c_str(),indexValue);
            if(decompressLen==-1){
                printf("error happened when get decompressed index!\n");
                clearStack(optrStack,opndStack);
                return -1;
            }
            vector<int> opnd;
            for(int i=0;i<decompressLen;i++){
                opnd.push_back(tmpDecompressedIndexBuf[i]);
            }
            opndStack.push(opnd);
        }else{
            string optr=getOptr(tmp);
            int pos=getPos(optr);
            if(pos==-1){
                printf("incorrect opreator:%s,only support && and ||",optr.c_str());
                clearStack(optrStack,opndStack);
                return -1;
            }
            switch(priority[getPos(optrStack.top())][pos]){
                case '<':
                    optrStack.push(optr);
                    tmp+=optr.length();
                    while(*tmp==' ') tmp++;
                    break;
                case '=':
                    optrStack.pop();
                    tmp+=optr.length();
                    while(*tmp==' ') tmp++;
                    break;
                case '>':
                {
                    string opr=optrStack.top();
                    optrStack.pop();
                    vector<int> opn1=opndStack.top();
                    opndStack.pop();
                    vector<int> opn2=opndStack.top();
                    opndStack.pop();
                    opndStack.push(logicCompute(opn1,opn2,opr));
                    break;
                }
                default:
                    printf("syntax error!\n");
                    clearStack(optrStack,opndStack);
                    return -1;
            }
        }
    }
    vector<int> resVec=opndStack.top();
    opndStack.pop();
    resLen=resVec.size();
    for(int i=0;i<resLen;i++){
        resIndexBuf[i]=resVec[i];
    }
    return resLen;
}

void clearStack(stack<string> &optrStack,stack<vector<int> > &opndStack){
    while(!optrStack.empty()){
        optrStack.pop();
    }    
    while(!opndStack.empty()){
        opndStack.pop();
    }
}

string getIndexName(const char *tmp){
    string res;
    while(isalpha(*tmp)){
        res+=*tmp;
        tmp++;
    }
    return res;
}

int checkEqualSymbol(const char *tmp){
    int fowardNum=0;
    while(*tmp==' '){
        tmp++;
        fowardNum++;
    }
    if((*tmp!='=')||(*(tmp+1)!='=')){
        printf("expect == before %s\n",tmp);
        return -1;
    }
    fowardNum+=2;
    tmp+=2;
    while(*tmp==' '){
        tmp++;
        fowardNum++;
    }
    return fowardNum;
}

int getIndexValue(const char *tmp,char indexValue[]){
    int i=0;
    while(isdigit(*tmp)||(*tmp=='.')){
        indexValue[i++]=*(tmp++);
    }
    indexValue[i]=0;
    if((indexValue[0]=='.')||(indexValue[strlen(indexValue)-1]=='.')){
        printf("incorrect ip format:%s!\n",indexValue);
        return -1;
    }
    return i;
}

string getOptr(const char *tmp){
    string res;
    res+=*tmp;
    if((*tmp=='&')||(*tmp=='|')){
        res+=*(tmp+1);
    }
    return res;
}

char *checkCommand(const char *command,char *showOpt,char *sTime,char *eTime){
    const char *tmp=command;
    while(*tmp==' ') tmp++;
    if(memcmp(tmp,"retrieve",8)!=0){
        printf("incorrect command type!only support retrieve!\n");
        return NULL;
    }
    tmp+=8;
    while(*tmp==' ') tmp++;
    while(*tmp=='-'){
        if((*(tmp+1)=='p')||(*(tmp+1)=='q')){
            showOpt[0]='-';
            showOpt[1]=*(tmp+1);
            showOpt[2]=0;
            tmp+=2;
            while(*tmp==' ') tmp++;
        }else if(*(tmp+1)=='s'){
            tmp+=2;
            while(*tmp==' ') tmp++;
            int i=0;
            while(isdigit(*tmp)||*tmp=='-'||*tmp==':'){
                sTime[i++]=*(tmp++);
            }
            if(*tmp==' '&&isdigit(*(tmp+1))){
                while(isdigit(*tmp)||*tmp=='-'||*tmp==':'){
                    sTime[i++]=*(tmp++);
                }
            }
            sTime[i]=0;
            while(*tmp==' ') tmp++;
        }else if(*(tmp+1)=='e'){
            tmp+=2;
            while(*tmp==' ') tmp++;
            int i=0;
            while(isdigit(*tmp)||*tmp=='-'||*tmp==':'){
                eTime[i++]=*(tmp++);
            }
            if(*tmp==' '&&isdigit(*(tmp+1))){
                while(isdigit(*tmp)||*tmp=='-'||*tmp==':'){
                    eTime[i++]=*(tmp++);
                }
            }
            eTime[i]=0;
            while(*tmp==' ') tmp++;
        }else{
            printf("unknown option:%c\n",*(tmp+1));
            return NULL;
        }
    }
    return (char *)tmp;
}
