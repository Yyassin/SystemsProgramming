/**
 * Chrono Header : Timer Module
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#pragma once
#ifndef _CHRONO_H_
#define _CHRONO_H_

#include <sys/time.h>
// Seconds to micro seconds conversion factor
#define MICRO_SEC_IN_SEC 1000000

// Chrono Struct
typedef struct {
    struct timeval initial; // Initial start time
    struct timeval final;   // Final end time
} Chrono;

/**
 * @brief Allocates and initializes a Chrono timer.
 * 
 * @return Chrono*, the initialized Chrono timer.
 */
Chrono* chrono_init();

/**
 * @brief Starts the timer on the Chrono timer.
 * 
 * @param[inout] c Chrono*, the timer to start.
 */
void chrono_start(Chrono* c);

/**
 * @brief Ends the timer on the Chrono timer.
 * 
 * @param[inout] c Chrono*, the timer to end.
 */
void chrono_end(Chrono* c);

/**
 * @brief Fetches the time elapsed (in micro seconds)
 * as registered in the specified timer.
 * 
 * @param[in] c Chrono*, the timer to get elapsed time for.
 * @return long, the time elapsed in micro seconds.
 */
long chrono_elapsed(Chrono* c);

/**
 * @brief Destroys and cleans up the specified
 * Chrono timer.
 * 
 * @param[in] c Chrono*, the timer to destroy. 
 */
void chrono_destroy(Chrono* c);

#endif
