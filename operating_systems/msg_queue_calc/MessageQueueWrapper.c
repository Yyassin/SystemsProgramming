/**
 * Message Queue Wrapper Header
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include "MessageQueueWrapper.h"

int message_queue_create(key_t key)
{
    return msgget(key, IPC_CREAT | 0666);
}

int message_queue_send(int qid, Message* msg)
{
    return msgsnd(qid, (void *)msg, MAX_TEXT, 0);
}

int message_queue_receive(int qid, Message* msg, long type)
{
    return msgrcv(qid, (void *)msg, MAX_TEXT, type, 0);
}

int message_queue_delete(int qid)
{
    return msgctl(qid, IPC_RMID, 0);
}
