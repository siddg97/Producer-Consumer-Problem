#include "stats.h"
#include "bbuff.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define MAX_FACT 1000

// typedef struct{
//   int producer;
//   int made;
//   int eaten;
//   double delay;
//   double minD;
//   double maxD;
// } stats_t;



// Helper functions for stats_records_consumed()
double delay(double a, double b){
  return (b-a);
}

// Helper functions for stats_records_consumed()
double largestDelay(double a, double b){
  if(a == 0.0 || a <= b){
    return b;
  }
  else{
    return a;
  }
}

// Helper functions for stats_records_consumed()
double lowestDelay(double a, double b){
  if(a == 0.0 || a >= b){
    return b;
  }
  else{
    return a;
  }
}

// stats.h implementation:
void stats_init(int num_producers) {
  for(int i = 0; i < num_producers; i++){
    stats[i].made = 0;
    stats[i].eaten = 0;
    stats[i].delay = 0.0;
    stats[i].maxD = 0.0;
    stats[i].minD = 0.0;
  }
  pthread_mutex_init(&mstats,NULL);
  
}

// to be called in fThreadfunc()
void stats_record_produced(int factory_number){
  pthread_mutex_lock(&mstats);
  stats[factory_number].producer = factory_number;
  stats[factory_number].made++;
  pthread_mutex_unlock(&mstats);
}

// To be called in kThreadfunc()
void stats_record_consumed(int factory_number, double delay_in_ms){
  pthread_mutex_lock(&mstats);
  stats[factory_number].eaten++;
  stats[factory_number].delay += delay_in_ms;
  stats[factory_number].maxD = largestDelay(stats[factory_number].maxD, delay_in_ms);
  stats[factory_number].minD = lowestDelay(stats[factory_number].minD, delay_in_ms);
  pthread_mutex_unlock(&mstats);
}

void stats_display(int n){
  pthread_mutex_lock(&mstats);
  printf("Statistics:\n");
  printf("%8s%10s%10s%15s%15s%15s\n","Factory#","#Made","#Eaten","Min Delay[ms]","Avg Delay[ms]","Max Delay[ms]");
  for(int i=0; i < n; i++){
    printf("%8d%10d%10d%5s%10.5f%5s%10.5f%5s%10.5f\n",i,stats[i].made,stats[i].eaten," ", stats[i].minD," ", (stats[i].delay)/(stats[i].made)," ",stats[i].maxD);
  }
  pthread_mutex_unlock(&mstats);
}