/*************************************************************************
	> File Name: yyThreadPool.c
	> Author: 
	> Mail: 
	> Created Time: 2018年07月18日 星期三 15时49分21秒
 ************************************************************************/

#include "yyThreadPool.h"

void *thread_work(void *args);
int bsem_init(bsem *pBsem,int sem);
void bsem_reset(bsem *pBsem);
void bsem_wait(bsem *pBsem);
void bsem_post(bsem *pBsem);

volatile int keepAlive;




threadpool_t *threadpool_init(int threadNum,int queueMaxLen){
    threadpool_t *pThreadPool=(threadpool_t *)malloc(sizeof(threadpool_t));
    if(pThreadPool==NULL) return NULL;
    (pThreadPool->tasks).front=NULL;
    (pThreadPool->tasks).rear=NULL;
    pthread_mutex_init(&((pThreadPool->tasks).qmutex),NULL);
    pthread_cond_init(&((pThreadPool->tasks).not_full),NULL);
    (pThreadPool->tasks).max_len=queueMaxLen;
    (pThreadPool->tasks).qlen=0;
    (pThreadPool->tasks).has_tasks=(bsem *)malloc(sizeof(bsem));
    if((pThreadPool->tasks).has_tasks==NULL) return NULL;
    if(bsem_init((pThreadPool->tasks).has_tasks,0)!=0) return NULL;
    pThreadPool->work_threads=(pthread_t *)malloc(threadNum*sizeof(pthread_t));
    pThreadPool->num_alive_threads=threadNum;
    pThreadPool->num_running_threads=0;
    pthread_mutex_init(&(pThreadPool->tmutex),NULL);
    pthread_cond_init(&(pThreadPool->has_idle),NULL);
    keepAlive=1;
    for(int i=0;i<threadNum;i++){
        pthread_create(&((pThreadPool->work_threads)[i]),0,thread_work,(void *)pThreadPool);
    }
    return pThreadPool;
}

void *thread_work(void *args){
    pthread_detach(pthread_self());
    threadpool_t *pThreadPool=(threadpool_t *)args;
    while(keepAlive){
        bsem_wait((pThreadPool->tasks).has_tasks);
        if(keepAlive){
            pthread_mutex_lock(&(pThreadPool->tmutex));
            pThreadPool->num_running_threads++;
            pthread_mutex_unlock(&(pThreadPool->tmutex));
            task *t;
            pthread_mutex_lock(&((pThreadPool->tasks).qmutex));
            if((pThreadPool->tasks).qlen==0){
                pthread_mutex_unlock(&((pThreadPool->tasks).qmutex));
                continue;
            }else if((pThreadPool->tasks).qlen==1){
                t=(pThreadPool->tasks).front;
                (pThreadPool->tasks).front=NULL;
                (pThreadPool->tasks).rear=NULL;
            }else{
                t=(pThreadPool->tasks).front;
                (pThreadPool->tasks).front=((pThreadPool->tasks).front)->next;
                bsem_post((pThreadPool->tasks).has_tasks);
            }
            (pThreadPool->tasks).qlen--;
            if((pThreadPool->tasks).qlen==((pThreadPool->tasks).max_len-1)){
                pthread_cond_signal(&((pThreadPool->tasks).not_full));
            }
            pthread_mutex_unlock(&((pThreadPool->tasks).qmutex));
            void *(*func)(void *)=t->func;
            func(t->args);
            free(t);
            pthread_mutex_lock(&(pThreadPool->tmutex));
            pThreadPool->num_running_threads--;
#ifdef LIMITTASK
            pthread_cond_signal(&(pThreadPool->has_idle));
#endif
            pthread_mutex_unlock(&(pThreadPool->tmutex));
        }
    }
    pthread_mutex_lock(&(pThreadPool->tmutex));
    pThreadPool->num_alive_threads--;
    pthread_mutex_unlock(&(pThreadPool->tmutex));
}

int threadpool_add_task(threadpool_t *pThreadPool,void* (*func)(void *),void *args){
#ifdef LIMITTASK
    pthread_mutex_lock(&(pThreadPool->tmutex));
    while((pThreadPool->num_running_threads==pThreadPool->num_alive_threads)&&(pThreadPool->num_alive_threads!=0)) pthread_cond_wait(&(pThreadPool->has_idle),&(pThreadPool->tmutex));
    pthread_mutex_unlock(&(pThreadPool->tmutex));
#endif
    task *t=(task *)malloc(sizeof(task));
    if(t==NULL) return -1;
    t->func=func;
    t->args=args;
    t->next=NULL;
    pthread_mutex_lock(&((pThreadPool->tasks).qmutex));
    while((pThreadPool->tasks).qlen==(pThreadPool->tasks).max_len){
        pthread_cond_wait(&((pThreadPool->tasks).not_full),&((pThreadPool->tasks).qmutex));
    }
    if((pThreadPool->tasks).qlen==0){
        (pThreadPool->tasks).front=(pThreadPool->tasks).rear=t;
    }else{
        ((pThreadPool->tasks).rear)->next=t;
        (pThreadPool->tasks).rear=((pThreadPool->tasks).rear)->next;
    }
    (pThreadPool->tasks).qlen++;
    bsem_post((pThreadPool->tasks).has_tasks);
    pthread_mutex_unlock(&((pThreadPool->tasks).qmutex));
    return 0;
}

void threadpool_destroy(threadpool_t *pThreadPool){
    keepAlive=0;
    while(pThreadPool->num_alive_threads!=0){
        bsem_post((pThreadPool->tasks).has_tasks);
        sleep(0.1);
    }
    task *t=(pThreadPool->tasks).front;
    while(t!=NULL){
        task *tmp=t;
        t=t->next;
        free(tmp);
    }
    free((pThreadPool->tasks).has_tasks);
    free(pThreadPool->work_threads);
    free(pThreadPool);
}

int bsem_init(bsem *pBsem,int sem){
    if(sem<0||sem>1){
        printf("the binary sem only can be 1 or 0!\n");
        return -1;
    }
    pthread_mutex_init(&(pBsem->mut),NULL);
    pthread_cond_init(&(pBsem->con),NULL);
    return 0;
}

void bsem_reset(bsem *pBsem){
    bsem_init(pBsem,0);
}

void bsem_wait(bsem *pBsem){
    pthread_mutex_lock(&(pBsem->mut));
    while(pBsem->sem==0) pthread_cond_wait(&(pBsem->con),&(pBsem->mut));
    pBsem->sem=0;
    pthread_mutex_unlock(&(pBsem->mut));
}

void bsem_post(bsem *pBsem){
    pthread_mutex_lock(&(pBsem->mut));
    pBsem->sem=1;
    pthread_cond_signal(&(pBsem->con));
    pthread_mutex_unlock(&(pBsem->mut));
}
