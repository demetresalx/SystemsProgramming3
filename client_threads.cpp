#include "client_threads.h"
#include <iostream>

//SYNCHRO_STDOUT
synchro_stdot::synchro_stdot(){
  user = false;
  lock = PTHREAD_MUTEX_INITIALIZER;
  in_use = PTHREAD_COND_INITIALIZER;
}

synchro_stdot::~synchro_stdot(){
  user = false;
  pthread_cond_destroy(&in_use);
  pthread_mutex_destroy(&lock);
}

//arxh critical section gia printing sto stdout
void synchro_stdot::cs_start(){
  pthread_mutex_lock(&lock);
  while(user)
    pthread_cond_wait(&in_use, &lock);
  user =true;
}

//telos critical secton gia priting sto stdout
void synchro_stdot::cs_end(){
  user=false;
  pthread_mutex_unlock(&lock);
  pthread_cond_broadcast(&in_use);
}
