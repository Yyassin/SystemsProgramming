#pragma once
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

// Operand buffer indices for message components
#define RESULT 0
#define ARGUMENT 0
#define MEDIAN1 0
#define MEDIAN2 1
#define FLAG_TWO_MEDIAN 2
#define TWO_MEDIANS 1
#define ONE_MEDIAN 0

// Legal operations enum
typedef enum {
    INSERT,
    DELETE,
    AVERAGE,
    SUM,
    MINIMUM,
    MEDIAN,
    QUIT, 
    ERROR
} operation_type;

/** Message format struct 
 * A message sent by the client will simply be modified
 * with the reply information and sent back rather than defining req and res types.
 */
typedef struct {
    long int my_msg_type;           // Message type
    operation_type operation;       // Operation Type
    float operands[3];              // Operand Buffer (Stores arguements and commands, more below)
                                    // Float since we can still store ints, but can store float for the average.
    float elapsed;                   // Average elapsed time in micro seconds.
} Message;

/**
 * When sending, argument is in operands[0]
 * 
 * When receiving, result is in operands[0]
 * Median 1 is in operands[0], 2 in [1]. If two medians, operands[2] is flagged with a 1.
 * Elapsed is -1 if error
 * 
 */

#endif
