/*************************************************************************
	> File Name: myToolFunc.c
	> Author: wangyue
	> Mail:yuew951115@gmail.com 
	> Created Time: 2018年01月19日 星期五 10时17分16秒
 ************************************************************************/
#include "myToolFunc.h"

/*
unsigned long mySystem(const char * cmdStr){
    pid_t pid;
    int status;
    unsigned long waitRes;

    if(cmdStr == NULL){
        return(0);
    }
    if((pid=fork())<0){
        return 1;
    }else if(pid==0){
        char *cmd[MAXARGSNUM+1];
        int argNum=splitStr(cmdStr,&cmd);
        if(!argNum) _exit(1);
        */
        /*
        char *args[argNum];
        int i;
        for(i=1;i<=argNum;i++){
            args[i-1]=cmd[i];
        }
        */
        /*
        if(execvp(cmd[0],cmd)==-1){
            printf("launch the fprobe failed!\n");
        };
        printf("launch the fprobe failed!\n");
        _exit(2);
    }else{ 
        while((waitRes=waitpid(pid,&status,0))<0){
            if(errno != EINTR){
                return 1;
            }
        }
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status)){
                return WEXITSTATUS(status);
            }
        }
        */
        /*
        waitRes=wait(&status);
        if(WIFEXITED(status)){
            printf("fprobe(pid:%d) exited\n",waitRes);
        }
        */
/*
    }
    return (unsigned long)pid;
}

int splitStr(const char *str,char *(*res)[]){
    char *pStr,*buf;
    int resNum=0;
    int len1,len2;
    len2=strlen(str);
    if(len2==0) return 0;
    buf=(char *)malloc(sizeof(char)*(len2+1));
    memcpy(buf,str,len2);
    buf[len2]=0;
    char *deli="#";
    while(buf){
        pStr=strsep(&buf,deli);
        len1=strlen(pStr);
        (*res)[resNum]=(char *)malloc(sizeof(char)*(len1+1));
        memcpy((*res)[resNum],pStr,len1);
        (*res)[resNum][len1]=0;
        resNum++;
    }
    (*res)[resNum]=NULL;
    return resNum;
}
*/
int countFiles(const char *path,const char *filter){
    DIR *dp;
    struct dirent *dirp;
    int fileNum=0;
    if((dp=opendir(path))==NULL){
        int mkRes=mkdir(path,0777);
        if(mkRes==-1){
            printf("fail to create dir %s\n",path);
        }
        return 0;
    }
    while((dirp=readdir(dp))!=NULL){
        if(strstr(dirp->d_name,filter)){
            fileNum++;
        }
    }
    return fileNum;
}

int iToStr(int num,char str[]){
    if(num==0){
        str[1]=0;
        str[0]='0';
        return 0;
    }
    int len=0;
    int tmpNum=num;
    while(tmpNum){
        tmpNum/=10;
        len++;
    }
    str[len--]=0;
    while(num){
        str[len--]=(num%10)+'0';
        num/=10;
    }
    if(len!=-1){
        return -1;
    }
    return 0;
}
