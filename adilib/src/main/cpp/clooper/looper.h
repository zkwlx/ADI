//
// Created by kewen on 2019-08-16.
//

#ifndef ADI_LOOPER_H
#define ADI_LOOPER_H

#include <pthread.h>

#define MAX_MESSAGE_SIZE  1014
#define MESSAGE_EXIT_LOOP  -1

typedef struct message_t {
    int what;
    struct message_t *next;
    int data_size;
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
    CALLBACK_FUNC handle_msg; //消息处理函数
} message_looper_t;


int create_looper(message_looper_t **looper, CALLBACK_FUNC func);

int start_loop(message_looper_t *looper);

int stop_loop(message_looper_t *looper);

int destroy_looper(message_looper_t *looper);

int push_message(message_looper_t *looper, int what, const void *data, int size);

#endif //ADI_LOOPER_H