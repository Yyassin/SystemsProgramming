#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/msg.h>
#include "MessageQueueWrapper.h"
#include "MedianHeap.h"
/**
 * Calculator Module
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include "Message.h"
#include "Chrono.h"

#define INITIAL_CAPACITY 10 // Initial buffer capacity
// All other msg packet indexing definitions can be found in Message.h

/**
 * @brief Processes the command in the specified 
 * message and modifies the message to store
 * the result.
 * 
 * @param[inout] msg, the message recieved.
 */
void command_controller(Message* msg) 
{
    static bool initialized = false;    // Single init for static structures.
    static MedianHeap* dataset;         // Stores all numbers
    static Chrono* chrono;              // Used as timer
    static int medians[2];              // median buffer
    static long total_elapsed[6];       // Tracks total proc. time for each command.
    static int total_commands[6];       // Tracks total commands received for each command.

    if (!initialized) { 
        initialized = true; dataset = medianheap_create(10); chrono = chrono_init(chrono); 
    }

    if (msg->operation < 6 && msg->operation >= 0) total_commands[msg->operation]++;


    chrono_start(chrono);               // Start timer


    // If our set is empty, the only viable command is insert.
    // *Could return 0 as result too
    if (medianheap_is_empty(dataset) && !(msg->operation == INSERT || msg->operation == QUIT)) { 
        printf("Received command on empty set, return error!\n\n");
        msg->operation = ERROR;
        chrono_end(chrono);                     // Stop timer
        msg->elapsed = chrono_elapsed(chrono); 
        return; 
    }
    
    switch(msg->operation) {
        case INSERT: {
            printf("Received command Insert with argument %d.\n\n", (int)msg->operands[ARGUMENT]);
            medianheap_insert(dataset, msg->operands[ARGUMENT]);
            break;
        }

        case DELETE: {
            printf("Received command Delete with argument %d.\n\n", (int)msg->operands[ARGUMENT]);
            medianheap_delete_all(dataset, msg->operands[ARGUMENT]);
            break;
        }

        case AVERAGE: {
            printf("Received command Average.\n");
            msg->operands[RESULT] = medianheap_get_average(dataset);
            break;
        }

        case SUM: {
            printf("Received command Sum.\n");
            msg->operands[RESULT] = medianheap_get_sum(dataset);
            break;
        }

        case MINIMUM: {
            printf("Received command Minimum.\n");
            msg->operands[RESULT] = medianheap_get_min(dataset);
            break;
        }

        case MEDIAN: {
            printf("Received command Median.\n");
            // Third index indicates if two or 1 median.
            if (medianheap_get_median2(dataset, medians)) {
                msg->operands[MEDIAN1] = medians[0];
                msg->operands[MEDIAN2] = medians[1];
                msg->operands[FLAG_TWO_MEDIAN] = TWO_MEDIANS;
            } else {
                msg->operands[MEDIAN1] = medians[0];
                msg->operands[FLAG_TWO_MEDIAN] = ONE_MEDIAN;
            }
            break;
        }

        case QUIT: {
            // Cleanup
            medianheap_destroy(dataset);
            chrono_destroy(chrono);
            printf("Received command Quit. Exiting.\n");
            return;
        }

        default: {
            return; 
        }
    }

    // Print status info on server
    if (!(msg->operation == INSERT || msg->operation == DELETE)) {
        if (msg->operation == AVERAGE) {
            printf("Returned result %0.3f\n\n", msg->operands[RESULT]);
        }
        else if (msg->operation == MEDIAN && (msg->operands[FLAG_TWO_MEDIAN] == TWO_MEDIANS)) {
            printf("Returned result two medians %d %d\n\n", (int)msg->operands[MEDIAN1], (int)msg->operands[MEDIAN2]);
        }
        else {
            printf("Returned result %d\n\n", (int)msg->operands[RESULT]);
        }
    }
    
    // Update average processing time info.
    chrono_end(chrono); // Stop timer
    if (msg->operation < 6 && msg->operation >= 0) {
        total_elapsed[msg->operation] += chrono_elapsed(chrono);
        msg->elapsed = (double)total_elapsed[msg->operation] /  (double)total_commands[msg->operation]; // Add elapsed
    }
}

int main(void) 
{
    // Message Queue Initializers
    char    *client_path = "user.c",
            *server_path = "calculator.c";
    int id = 'C';

    // Keys for message queues
    key_t   client_to_server_key = ftok(client_path, id), 
            server_to_client_key = ftok(server_path, id);

    int client_to_server, server_to_client;     // Message queue IDS
    Message msg_packet;                         // Stores the message to send/receive
    msg_packet.my_msg_type = 1;
    long int msg_to_receive = 0;

    // Set up the message queue
    assert((client_to_server = message_queue_create(client_to_server_key)) != -1);
    assert((server_to_client = message_queue_create(server_to_client_key)) != -1);

    printf("Calculator started successfully.\n");

    while(true)
    {
        assert(message_queue_receive(client_to_server, (void *)&msg_packet, msg_to_receive) != -1);

        command_controller(&msg_packet);
        if (msg_packet.operation == QUIT) break; // Need to quit after processing to cleanup first

        assert(message_queue_send(server_to_client, (void *)&msg_packet) != -1);
    }

    printf("Calculator shutting down.\n");

    // Cleanup message queues.
    assert(message_queue_delete(server_to_client) != -1);  
    assert(message_queue_delete(client_to_server) != -1);
    exit(EXIT_SUCCESS);
}
