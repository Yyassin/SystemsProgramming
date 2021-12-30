/**
 * Message Queue Wrapper Header
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#pragma once
#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include "errno.h"

#include "Message.h"
#define MAX_TEXT 35

/**
 * @brief Creates (or gets if created) a message
 * queue with the specified key and returns
 * the associated identifier.
 * 
 * @param[in] key, the queue key.
 * @return int, -1 on failure, else queue id.
 */
int message_queue_create(key_t key);

/**
 * @brief Deletes the message queue
 * assocaited with the specified id.
 * 
 * @param[in] qid, the id of the queue to be deleted. 
 * @return int, -1 on failure, else 0.
 */
int message_queue_delete(int qid);

/**
 * @brief Sends the specified message
 * in the message queue specified by qid.
 * 
 * @param[in] qid, the id of the queue to send the message in.
 * @param[in] msg, the message to send.
 * @return int, -1 on failure, else 0.
 */
int message_queue_send(int qid, Message *msg);

/**
 * @brief Blocking receive for a message 
 * of the specified type on the the 
 * message queue specified by qid. The
 * received message is stored into msg.
 * 
 * @param[in] qid, the id of the queue to receive on.
 * @param[in] msg, the location where the received message is stored.
 * @param[in] type, the type of message to wait for. 
 * @return int, -1 on failure, else 0. 
 */
int message_queue_receive(int qid, Message *msg, long type);

#endif
