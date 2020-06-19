#include "client_threads.h"
#include <fstream>
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

//gia na xeirizomai kapoia edge cases https://stackoverflow.com/questions/19140148/c-get-total-file-line-number
int get_lines_ofile(char * filename ){
  std::ifstream f(filename);
  std::string line;
  int i;
  for (i = 0; std::getline(f, line); ++i)
      ;
  return i;
}
