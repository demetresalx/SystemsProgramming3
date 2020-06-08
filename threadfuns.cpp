#include <iostream>
#include <string>
#include "threadfuns.h"

//sunarthseis twn threads!
//dokimastikh
void hello(){
  st.cs_start();
  std::cout << "Quanmiong\n i am thread " << std::to_string(pthread_self()) << "\n";
  //st.print_safe(tprnt.c_str());
  st.cs_end();
  //std::cout << tprnt;
}

synchro_stdout::synchro_stdout(){
  lock = PTHREAD_MUTEX_INITIALIZER;
  in_use = PTHREAD_COND_INITIALIZER;
}

synchro_stdout::~synchro_stdout(){
  user = false;
  pthread_cond_destroy(&in_use);
  pthread_mutex_destroy(&lock);
}

//arxh critical section gia printing sto stdout
void synchro_stdout::cs_start(){
  pthread_mutex_lock(&lock);
  while(user)
    pthread_cond_wait(&in_use, &lock);
  user =true;
  //std::cout << "Hump me fuck me t clean" << pthread_self() << "\n";
  //user=false;
  //pthread_mutex_unlock(&lock);
  //pthread_cond_broadcast(&in_use);
}

//telos critical secton gia priting sto stdout
void synchro_stdout::cs_end(){
  //pthread_mutex_lock(&lock);
  //while(user)
    //pthread_cond_wait(&in_use, &lock);
  //user =true;
  //std::cout << "Hump me fuck me t clean" << pthread_self() << "\n";
  user=false;
  pthread_mutex_unlock(&lock);
  pthread_cond_broadcast(&in_use);
}
