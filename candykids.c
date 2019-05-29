#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "bbuff.h"
#include "stats.h"

// Declare global variable candies: buffer
extern bbuff_t *candies;

extern stats_t stats[MAX_FACT];

extern pthread_mutex_t mstats;

 // Boolean to signal threads to stop
_Bool stop_thread = false;

// Thread functions:

// Factory thread func
void* fThread_func(void* arg){
  int fact = *(int*)arg;
  //int item = 0;
  int wtime = 1;
  printf("Spawnning Factory%d\n",fact);
  while(!stop_thread){
    wtime = wtime % 3;    // chooses time to wait: [0,3], increases with each iteration and resets after reaching 3
    printf("Factory%d produces Candy & waits %ds\n",fact, wtime);
    candy_t *candy = malloc(sizeof(candy_t));
    candy->factory_number = fact;
    candy->creation_ts_ms = current_time_in_ms();
    sem_wait(&candies->empty);             // if no empty slots, wait
    pthread_mutex_lock(&candies->mutex);   // if other thread is using candies [buffer] then wait
    bbuff_blocking_insert(candy);
    pthread_mutex_unlock(&candies->mutex); // unlock buffer
    sem_post(&candies->full);              // increment # of slots full
    for(int i = 0; i < wtime; i++){
      sleep(1);
    }
    wtime++;
  }
  printf("Candy-factory%d: done\n",fact);
  
  return NULL;
}

// Kid thread func
void* kThread_func(void* arg){
  int k = *(int*)arg;
  int t = 1;

  while(1){
    sem_wait(&candies->full); 
    pthread_mutex_lock(&candies->mutex);
    bbuff_blocking_extract(k);
    pthread_mutex_unlock(&candies->mutex);
    sem_post(&candies->empty);
    
    if( t % 2 == 1){
      t = 1;
    }
    else{
      t = 0;
    }
    while(t >= 0){
      sleep(1);
      t--;
    }
    t++;
  }
  return NULL;
} 

int main(int argc, char *argv[]) {
  // Extract arguments

  int nFactories = atoi(argv[1]);
  int nKids = atoi(argv[2]);
  int nSeconds = atoi(argv[3]);
  if(nFactories <= 0 || nKids <= 0 || nSeconds <= 0){
    printf("Please Enter Valid Argument");
    return 0;
  } 

  // Initialize modules

  // buffer module
  bbuff_init();
  // stats module
  stats_init(nFactories);
  // facotry number arrary to be passed to each factory thread
  int Nfactory[nFactories];
  for(int i =0; i < nFactories;i++){
    Nfactory[i] = i;
  }
  
  // Kid number array to be passed to each kid thread
  int Nkid[nKids];
  for(int i = 0; i < nKids; i++){
    Nkid[i] = i;
  }

  // Launch candy-factory threads
  
  pthread_t fid[nFactories]; // array of factory ThreadIDs
 
  for (int i=0; i < nFactories; i++) { // create actory threads
    pthread_create( &fid[i], NULL, fThread_func, (void*)&Nfactory[i]);
    
  }

  // Launch kid threads
  
  pthread_t kid[nKids]; // array of kid ThreadIDs

  for(int i=0; i < nKids; i++){ // create kid threads
    pthread_create( &kid[i], NULL, kThread_func, (void*)&Nkid[i]);
  }
  
  // Wait for requested time

  for(int i = nSeconds; i >= 0; i--){
    printf("Time: %ds\n",i);
    sleep(1);
  }

  // Stop candy-factory threads

  stop_thread = true; // signal threads to stop
  for(int i = 0; i < nFactories; i++){
    pthread_join(fid[i],NULL);
  }

  // wait until no more candy

  while(!bbuff_is_empty()){
    printf("Waiting for candy to be consumed\n");
    sleep(1);
  }

  // Stop kid threads

  for(int i = 0; i < nKids; i++){
    pthread_cancel(kid[i]);
    pthread_join(kid[i],NULL);
    printf("Kid%d terminated\n",i);
  }

  // Print statistics

  stats_display(nFactories);
  for(int i=0; i < nFactories; i++){
    if(stats[i].made == stats[i].eaten){
      continue;
    }
    else{
      printf("ERROR: Mistmatch! #Made != #Eaten\n");
      break;
    }
  }

	return 0;
}

