//
// Created by kewen on 2019-08-16.
//

#ifndef ADI_LOOPER_H
#define ADI_LOOPER_H

#include <pthread.h>

#define MAX_MESSAGE_SIZE  1014
#define MESSAGE_EXIT_LOOP  -1
#define LOOPER_IS_NULL  -10
#define LOOPER_START_SUCCESS  0
#define LOOPER_START_REPEAT_ERROR  -1
#define LOOPER_START_THREAD_ERROR  -2


typedef struct message_t {
    int what;
    struct message_t *next;
    size_t data_size;
    void *data;
} message_t;

typedef struct {
    message_t *head;
    int size;
} message_queue_t;

typedef void (*CALLBACK_FUNC)(message_t *msg);

typedef struct {
    volatile int is_looping;  //退出线程循环标志
    pthread_t looper_thread;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;
    message_queue_t queue;
    CALLBACK_FUNC handler; //消息处理函数
} message_looper_t;

message_looper_t *looperCreate(CALLBACK_FUNC func);

int looperStart(message_looper_t *looper);

int looperStop(message_looper_t *looper);

int looperDestroy(message_looper_t **looper);

int looperPost(message_looper_t *looper, int what, const void *data, size_t size);

#endif //ADI_LOOPER_H