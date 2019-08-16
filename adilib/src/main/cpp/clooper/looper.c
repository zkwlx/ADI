//
// Created by kewen on 2019-08-16.
//

#include "looper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <android/log.h>

#define LOG_TAG "adi"
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static message_t *fetch_message(message_queue_t *queue) {
    if (queue->size < 1) {
        return NULL;
    }
    message_t *message = queue->head;
    queue->head = message->next;    //取出队列头的消息后，消息队列指向下一个
    (queue->size)--;
    return message;
}

static int delete_message(message_t *message) {
    if (message == NULL) {
        ALOGI("%s", "[message_looper] delete_message message is null.\n");
        return -1;
    }
    if (message->data != NULL) {
        free(message->data);
        message->data = NULL;
    }
    free(message);
    return 0;
}

static int clear_message_queue(message_queue_t *queue) {
    if (queue == NULL) {
        return -1;
    }
    message_t *message;
    while ((queue->size) > 0) {
        message = queue->head;
        queue->head = message->next;    //取出队列头的消息后，消息队列指向下一个
        (queue->size)--;
        delete_message(message);
    }
    return 0;
}

static void *message_loop(void *arg) {
    message_looper_t *looper = (message_looper_t *) arg;
    while (looper->is_loop) {
        pthread_mutex_lock(&(looper->queue_mutex));        //获取锁
        if ((looper->queue).size < 1) {
            pthread_cond_wait(&(looper->queue_cond), &(looper->queue_mutex));  //等待这个信号量
            pthread_mutex_unlock(&(looper->queue_mutex));  //下次进入判断队列是否有消息
            continue;
        }
        message_t *msg = fetch_message(&(looper->queue));
        if (msg == NULL) {
            pthread_mutex_unlock(&(looper->queue_mutex));  //释放锁
            continue;
        }
        pthread_mutex_unlock(&(looper->queue_mutex));      //释放锁

        /*
         * 如果handle_msg() 可能会导致死锁，比如异步消息需要push到这个线程中
         * 刚好这个线程正在处理消息，然后出现消息又需要用到异步发送消息的函数
         * 异步发送消息的函数中又刚好需要锁，这时候会出现死锁
         */
        looper->handle_msg(msg);  //处理消息
        delete_message(msg);    //删除消息

    }
    clear_message_queue(&(looper->queue));  //最后清空链表
    return NULL;
}

////////////////////////////////////////////////////////////////////////

int create_looper(message_looper_t **looper, CALLBACK_FUNC func) {
    if (looper == NULL || func == NULL) {
        ALOGI("[message_looper] create_looper looper or func is null.\n");
        return -1;
    }

    *looper = (message_looper_t *) malloc(sizeof(message_looper_t));
    if (*looper == NULL) {
        ALOGI("[message_looper] create_looper malloc fail.\n");
        return -1;
    }
    (*looper)->is_loop = 0;
    pthread_mutex_init(&((*looper)->queue_mutex), NULL);
    pthread_cond_init(&((*looper)->queue_cond), NULL);
    (*looper)->queue.head = NULL;
    (*looper)->queue.size = 0;
    (*looper)->handle_msg = func;
    return 0;
}

int start_loop(message_looper_t *looper) {
    if (looper == NULL) {
        ALOGI("[message_looper] start_loop looper is null.");
        return -1;
    }

    pthread_mutex_lock(&(looper->queue_mutex));   //加锁保护
    if (looper->is_loop) {
        ALOGI("[message_loop] start_loop message_loop had start.");
        pthread_mutex_unlock(&(looper->queue_mutex));   //释放锁
        return -1;
    }
    looper->is_loop = 1;   //标志创建了 looper线程
    if (pthread_create(&(looper->looper_thread), NULL, message_loop, looper)) {
        ALOGI("[message_looper] pthread_create message_loop fail.");
        looper->is_loop = 0;  //线程创建失败，loop标志为false
        pthread_mutex_unlock(&(looper->queue_mutex));   //释放锁
        return -1;
    }
    pthread_mutex_unlock(&(looper->queue_mutex));   //释放锁
    return 0;
}

int stop_loop(message_looper_t *looper) {
    if (looper == NULL) {
        ALOGI("[message_looper] stop_loop loop is null.");
        return -1;
    }
    pthread_mutex_lock(&(looper->queue_mutex));   //加锁保护
    looper->is_loop = 0; //标志线程结束
    pthread_mutex_unlock(&(looper->queue_mutex));   //释放锁
    push_message(looper, MESSAGE_EXIT_LOOP, NULL, 0);  //线程可能还在等待消息，push 一个消息，让它结束运行
    pthread_join(looper->looper_thread, NULL);   //等待线程运行结束
    return 0;
}

int destroy_looper(message_looper_t *looper) {
    if (looper == NULL) {
        ALOGI("[message_looper] destroy_loop looper is null.");
        return -1;
    }

    if (looper->is_loop) {
        ALOGI("[message_looper] destroy_loop looper is start.");
        stop_loop(looper); //结束线程
    }

    pthread_cond_destroy(&(looper->queue_cond));
    pthread_mutex_destroy(&(looper->queue_mutex));
    free(looper);

    return 0;
}

int push_message(message_looper_t *looper, int what, const void *data, int size) {
    if (looper == NULL) {
        ALOGI("[message_looper] push_message looper is null.");
        return -1;
    }
    pthread_mutex_lock(&(looper->queue_mutex));
    if ((looper->queue).size > MAX_MESSAGE_SIZE) {
        //当前队列中的消息太多了，还没来得及处理
        ALOGI("[message_looper] queue.size(%d) more than %d.", (looper->queue).size, MAX_MESSAGE_SIZE);
//        pthread_mutex_unlock(&(looper->queue_mutex));
//        return -1;
    }
    message_t *message = (message_t *) malloc(sizeof(message_t));
    message->what = what;
    message->next = NULL;

    if (data != NULL && size > 0) {
        message->data = malloc(size);
        memcpy(message->data, data, size);
        message->data_size = size;
    } else {
        message->data = NULL;
        message->data_size = 0;
    }

    if ((looper->queue).head == NULL) {
        //头部为空时，直接指向新的消息
        (looper->queue).head = message;
        ((looper->queue).size)++;
        pthread_cond_signal(&(looper->queue_cond));
        pthread_mutex_unlock(&(looper->queue_mutex));
        return 0;
    }
    message_t *tmp = (looper->queue).head;
    while (tmp->next != NULL) {
        //新的消息发到链表尾部
        tmp = tmp->next;
    }
    tmp->next = message;
    ((looper->queue).size)++;
    pthread_cond_signal(&(looper->queue_cond));
    pthread_mutex_unlock(&(looper->queue_mutex));
    return 0;
}