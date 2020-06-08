#ifndef _THREADFUNS_H_
#define _THREADFUNS_H_

#include <pthread.h>
#include <string>

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

//kratame pragmata apo ta statistics poy pairnoyme opws ip, porta, xwres
class worker{
public:
  int n_countries; //poses xwres exei = trexon megethos pinaka
  std::string * countries; //o pinakas me ta onomata xwrwn
  uint16_t port;
  std::string address;
  //operator overloading gia eukoles pros8hkes me resize
  void operator = (worker const &obj) {
    n_countries = obj.n_countries;
    countries = obj.countries;
    port = obj.port;
    address = obj.address;
  }
  worker(){n_countries =0; countries=NULL;};
  //~worker();
  bool has_country(std::string );
  void add_country(std::string ); //prosthetei xwra kanontas resize to antistoixo buffer
};
//klash poy sugkentrwnei antikeimena san to parapanw. tha einai xrhsimh gia na kseroume poy prepei na phgainoyn ta erwthmata
class worker_db{
public:
  int n_workers;
  worker * workers;

  worker_db(){n_workers =0; workers=NULL;};
  ~worker_db();
  void add_worker(worker ); //prosthetei worker kanontas to swsto resize
};

extern synchro_stdout st ;//gia xrhsh apo threads
//gia kykliko buffer
extern pool * circle;
#endif
