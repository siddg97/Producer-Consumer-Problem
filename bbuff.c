#include "bbuff.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "stats.h"


double current_time_in_ms(void)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void bbuff_init(void){
  candies = malloc(sizeof(bbuff_t));
  for(int i = 0; i < BUFFER_SIZE; i++){
    candies->buf[i].eaten_by_kid = -1;
  }
  // candies.in = 0;
  // candies.out = 0;
  sem_init(&candies->full,0,0);
  sem_init(&candies->empty,0,BUFFER_SIZE);
  pthread_mutex_init(&candies->mutex,NULL);
}
void bbuff_blocking_insert(void* item){
  candy_t candy = *(candy_t*)item;
  candies->buf[candies->in] = candy;
  stats_record_produced(candy.factory_number);
  candies->in = (candies->in + 1) % BUFFER_SIZE;
  printf("[F%d] Producing candy\n",candy.factory_number);
}
void bbuff_blocking_extract(int nk){
  candies->buf[candies->out].eaten_by_kid = nk;
  double teat = current_time_in_ms();
  double tprod = candies->buf[candies->out].creation_ts_ms;
  double delt = delay(tprod,teat);
  int f = candies->buf[candies->out].factory_number;
  stats_record_consumed(f,delt); 
  candies->out = (candies->out+1) % BUFFER_SIZE;
  printf("[K%d] Consuming candy\n",nk);

}
_Bool bbuff_is_empty(void){
  for(int i =0; i < BUFFER_SIZE; i++){
    if(candies->buf[i].eaten_by_kid != -1){
      continue;
    }
    else if(candies->buf[i].eaten_by_kid >=0){
      return false;
    }
  }
  return true;
}
