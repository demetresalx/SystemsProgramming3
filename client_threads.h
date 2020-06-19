#ifndef _CLIENT_THREADS_H_
#define _CLIENT_THREADS_H_

#include <pthread.h>
#include <string>

//KLASH GIA TO SUGXRONISMO TOU STDOUT poy zhteitai
class synchro_stdot{
public:
  pthread_mutex_t lock;
  pthread_cond_t in_use;
  bool user;

  synchro_stdot();
  ~synchro_stdot();
  void cs_start(); //arxh critical section sto stdout
  void cs_end(); //telos critical section sto stdout
};

int get_lines_ofile(char * ); //gia na xeirizomai kapoia edge cases

#endif
