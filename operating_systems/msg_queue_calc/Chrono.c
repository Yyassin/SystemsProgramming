/**
 * Chrono : Timer Module
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Chrono.h"

Chrono* chrono_init() {
    Chrono* c = (Chrono *)malloc(sizeof(Chrono));
    assert(c != NULL);
    // Initialize timers to 0
    c->initial = (struct timeval){ 0 };
    c->final = (struct timeval){ 0 };
    return c;
}

void chrono_start(Chrono* c) {
    assert(c != NULL);
    gettimeofday(&c->initial, NULL);
}

void chrono_end(Chrono* c) {
    assert(c != NULL);
    gettimeofday(&c->final, NULL);
}

long chrono_elapsed(Chrono* c) {
    assert(c != NULL);
    // Return elapsed time in micro seconds.
    return ((c->final.tv_sec * MICRO_SEC_IN_SEC + c->final.tv_usec) - 
        (c->initial.tv_sec * MICRO_SEC_IN_SEC + c->initial.tv_usec));
}

void chrono_destroy(Chrono* c) {
    assert(c != NULL);
    //printf("Clean chrono.\n");
    free(c);
}
