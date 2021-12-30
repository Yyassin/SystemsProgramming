/**
 * User Module
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "Message.h"
#include "MessageQueueWrapper.h"

// All msg packet indexing definitions can be found in Message.h

/**
 * @brief Returns the operation type
 * associated with the specified command.
 * 
 * @param[in] command, the command to cast.
 * @return operation_type, the operation type.
 */
operation_type get_op_type(char command) {
    command = (char)tolower(command);
    switch (command)
    {
        case 'i': return INSERT;
        case 'd': return DELETE;
        case 'a': return AVERAGE;
        case 's': return SUM;
        case 'm': return MINIMUM;
        case 'u': return MEDIAN;
        case 'q': return QUIT;
        default:  return ERROR;
    }
}

/**
 * @brief Gets the argument to go along with
 * the specified command.
 * 
 * @param[in] op, the specified operation command.
 * @return int, the argument.
 */
int get_arg(const operation_type op) {
    // Only insert and delete need an argument, otherwise set it to 0.
    if (!(op == INSERT || op == DELETE)) return 0;
    printf("Selected %s(). Insert an *integer* argument: ", (op == INSERT) ? "Insert" : "Delete");

    int command;
    scanf(" %d", &command);
    return command;
}

/**
 * @brief Formats and prepares the message to send
 * based on the specified command.
 * 
 * @param[out] msg, stores the message to send.
 * @param[in] command, the specified command.
 */
bool format_msg(Message* msg, const char command) {
    msg->operation = get_op_type(command);                // Encode command to operation type.
    if (msg->operation == ERROR) return false;

    msg->operands[ARGUMENT] = get_arg(msg->operation);    // Get the argument for insert/delete.
    msg->elapsed = 0;   // Some default elapsed time
    return true;
}

/**
 * @brief Processes the replied message and 
 * prints the result.
 * 
 * @param[in] msg, the reply message.
 */
void process_msg(const Message* msg) {
    // Server signalled error
    if (msg->operation == ERROR) {
        printf("[av.elapsed=%0.3fus] Server encountered an error processing the request! Retry.\n", msg->elapsed);
        return;
    }
    if (msg->operation == MEDIAN) {
        // medians[2] = 1 for 2 medians, 0 for 1 median.
        if (msg->operands[FLAG_TWO_MEDIAN]) {
            printf("[av.elapsed=%0.3fus] Server> medians= %d %d.\n", msg->elapsed, (int)msg->operands[MEDIAN1], (int)msg->operands[MEDIAN2]);
        } else {
            printf("[av.elapsed=%0.3fus] Server> median= %d.\n", msg->elapsed, (int)msg->operands[MEDIAN1]);
        }
    }
    else if (msg->operation == AVERAGE) {
         printf("[av.elapsed=%0.3fus] Server> average= %0.3f.\n", msg->elapsed, msg->operands[RESULT]);
    }
    else if (msg->operation == SUM || msg->operation == MINIMUM) {
        char* command = (msg->operation == SUM) ? "sum" : "minimum";

        printf("[av.elapsed=%0.3fus] Server> %s= %d.\n", msg->elapsed, command, (int)msg->operands[RESULT]);
    }
    else {
        printf("[av.elapsed=%0.3fus] Server %s %d successfully. \n", msg->elapsed, msg->operation == INSERT ? "inserted" : "removed all instances of", (int)msg->operands[ARGUMENT]);
    }
}

void opening_prompt() {
    printf("Welcome to the user interface.\n" 
        "Please begin by entering a command:\n"
        "(I)nsert (N)\n(D)elete (N)\n(U)Median\n(M)inimum\n(S)um\n(A)verage\n"
    );
}

/**
 * @brief Prompts the user to enter 
 * a command (and command if applicable)
 * and formats it into a message.
 * 
 * @param[out] msg, the message that stores
 * the command information to send.
 */
void prompt_user(Message* msg) {
    char command;

    printf("\nEnter a command: ");
    scanf(" %c", &command);
    while (!format_msg(msg, command)) {
        printf("That's an invalid command, try again!\n\nEnter a command: ");
        scanf(" %c", &command);
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


    int client_to_server, server_to_client; // Message queue IDS
    Message msg_packet;                     // Stores the message to send/receive
    msg_packet.my_msg_type = 1;             
    long int msg_to_receive = 0;

    printf("Message Size: %ld\n", sizeof(msg_packet));

    // Set up the message queue
    assert((client_to_server = message_queue_create(client_to_server_key)) != -1);
    assert((server_to_client = message_queue_create(server_to_client_key)) != -1);

    opening_prompt();
    while (true) {
        prompt_user(&msg_packet);
        
        assert(message_queue_send(client_to_server, (void *)&msg_packet) != -1);

        if (msg_packet.operation == QUIT) break;

        assert(message_queue_receive(server_to_client, (void *)&msg_packet, msg_to_receive) != -1);
        process_msg(&msg_packet);
    }

    printf("Client shutting down.\n");
    // Server cleans up the message queues
    exit(EXIT_SUCCESS);
}
