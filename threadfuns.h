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

//OPWS STIS DIAFANEIES GIA KYKLIKO BUFFER
class pool{
public:
  int * fds; //o buffer me tous file descriptors
  int size;
  int start;
  int end;
  int count;

  pthread_mutex_t lock; //mutex gia ton kykliko buffer
  pthread_cond_t nonempty; //condtion var gia nonempty. diafaneies ntoula producer consumer
  pthread_cond_t nonfull; //condition var gia nonfull. diafaneies ntoula producer consumer

  pool(int );//me megethos buffersize
  ~pool();
  void place(int ); //topo8ethse enan descriptor ston kykliko buffer
  int obtain(); //pare enan descriptor apo ton kykliko buffer
};


extern synchro_stdout st ;//gia xrhsh apo threads
//gia kykliko buffer
extern pool * circle;
#endif
