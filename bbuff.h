#ifndef BBUFF_H
#define BBUFF_H
#define BUFFER_SIZE 10

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "stats.h"

typedef struct  {
    int factory_number;
    double creation_ts_ms;
    int eaten_by_kid;
} candy_t;

typedef struct {
 candy_t buf[BUFFER_SIZE]; // shared variable
 int in;                   // buf[in%BUFFER_SIZE]: first empty slot
 int out;                  // buf[out%BUFFER_SIZE]: first full slot
 sem_t full;               // semaphore: tracks # of full slots
 sem_t empty;              // semaphore: tracks # of empty slots
 pthread_mutex_t mutex;    // mutex: allows access to only one thread at a time
}bbuff_t;

double current_time_in_ms(void);

bbuff_t *candies;
void bbuff_init(void);
void bbuff_blocking_insert(void* item);
void bbuff_blocking_extract(int nk);
_Bool bbuff_is_empty(void);

#endif