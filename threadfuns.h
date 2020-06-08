#ifndef _THREADFUNS_H_
#define _THREADFUNS_H_

#include <pthread.h>

//sunarthseis poy tha xrhsimopoioyn ta threads
void hello(); //dokimastikh

//KLASH GIA TO SUGXRONISMO TOU STDOUT poy zhteitai
class synchro_stdout{
public:
  pthread_mutex_t lock;
  pthread_cond_t in_use;
  bool user;

  synchro_stdout();
  ~synchro_stdout();
  void cs_start(); //arxh critical section sto stdout
  void cs_end(); //telos critical section sto stdout
};

extern synchro_stdout st ;//gia xrhsh apo threads

#endif
