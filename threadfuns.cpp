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

//GIA KYKLIKO BUFFER DIAFANEIWN KOU. NTOULA
//constructor p kanei initialize
pool::pool(int sz){
  fds = new int[sz]; //oso to bufferSize tha einai auto
  //memset(fds, 0, sizeof(fds)); //gemise to me 0 arxika
  start = 0;
  end = -1;
  count = 0;
  size = sz;
  lock = PTHREAD_MUTEX_INITIALIZER;
  nonempty = PTHREAD_COND_INITIALIZER;
  nonfull = PTHREAD_COND_INITIALIZER;
}
//akoloy8ei logikh diafaneiwn kou. Ntoula se producers consumers
void pool::place(int fd){
  pthread_mutex_lock(&lock) ;
  while(count >= size){
    printf(">> Found Buffer Full \n");
    pthread_cond_wait (&nonfull , &lock) ;
  }
  end = (end + 1) % size ;
  fds[end] = fd ;
  count++;
  pthread_mutex_unlock(&lock) ;
}//telos sunarthshs

int pool::obtain(){
  int data = 0;
  pthread_mutex_lock(&lock) ;
  while(count <= 0){
    printf (">> Found Buffer Empty\n");
    pthread_cond_wait(&nonempty , &lock) ;
  }
  data = fds[start];
  printf("Eimai o %d kai tsimphsa to fd %d\n", pthread_self(), data);
  start = (start + 1) % size ;
  count--;
  pthread_mutex_unlock(&lock) ;
  return data ;
}

//destructor
pool::~pool(){
  delete[] fds;
  pthread_cond_destroy(&nonempty);
  pthread_cond_destroy(&nonfull);
  pthread_mutex_destroy(&lock);
}



//SYNCHRO_STDOUT
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
}

//telos critical secton gia priting sto stdout
void synchro_stdout::cs_end(){
  user=false;
  pthread_mutex_unlock(&lock);
  pthread_cond_broadcast(&in_use);
}
