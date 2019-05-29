#ifndef STATS_H
#define STATS_H
#include "bbuff.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define MAX_FACT 1000

typedef struct{
  int producer;
  int made;
  int eaten;
  double delay;
  double minD;
  double maxD;
} stats_t;

stats_t stats[MAX_FACT];
pthread_mutex_t mstats;

// Helper functions for stats_records_consumed()
double delay(double a, double b);

// Helper functions for stats_records_consumed()
double largestDelay(double a, double b);

// Helper functions for stats_records_consumed()
double lowestDelay(double a, double b);

void stats_init(int num_producers);
void stats_record_produced(int factory_number);
void stats_record_consumed(int factory_number, double delay_in_ms);
void stats_display(int n);

#endif
