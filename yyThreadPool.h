/*************************************************************************
	> File Name: yyThreadPool.h
	> Author: 
	> Mail: 
	> Created Time: 2018年07月18日 星期三 15时09分37秒
 ************************************************************************/

#ifndef _YYTHREADPOOL_H
#define _YYTHREADPOOL_H
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct bsem{
    pthread_mutex_t mut;
    pthread_cond_t con;
    int sem;
}bsem;

typedef struct task{
    void* (*func)(void *);
    void *args;
    struct task *next;
}task;

typedef struct taskqueue{
    task *front;
    task *rear;
    pthread_mutex_t qmutex;
    pthread_cond_t not_full;
    int max_len;
    int qlen;
    bsem *has_tasks;
}taskqueue;

typedef struct threadpool_t{
    taskqueue tasks;
    pthread_t *work_threads;
    int num_alive_threads;
    int num_running_threads;
    pthread_mutex_t tmutex;
    pthread_cond_t has_idle;
}threadpool_t;

threadpool_t *threadpool_init(int threadNum,int queueMaxLen);
int threadpool_add_task(threadpool_t *pThreadPool,void* (*func)(void *),void *args);
void threadpool_destroy(threadpool_t *pThreadPool);

#ifdef __cplusplus
}
#endif

#endif
