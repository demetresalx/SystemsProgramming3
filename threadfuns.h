#ifndef _THREADFUNS_H_
#define _THREADFUNS_H_

#include <pthread.h>
#include <string>

//sunarthseis poy tha xrhsimopoioyn ta threads

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
class tuple{
public:
  int fd; //o file descriptor
  std::string type; //ti eidous fd einai
  std::string address; //h ip poy prokyptei apo thn accept
  void operator = (tuple const &obj){ //operator overloading gia eukolia
    fd = obj.fd;
    type = obj.type;
    address = obj.address;
  }
};

class pool{
public:
  tuple * fds; //o buffer me tous file descriptors
  int size;
  int start;
  int end;
  int count;

  pthread_mutex_t lock; //mutex gia ton kykliko buffer
  pthread_cond_t nonempty; //condtion var gia nonempty. diafaneies ntoula producer consumer
  pthread_cond_t nonfull; //condition var gia nonfull. diafaneies ntoula producer consumer

  pool(int );//me megethos buffersize
  ~pool();
  void place(tuple ); //topo8ethse enan descriptor ston kykliko buffer
  tuple obtain(); //pare enan descriptor apo ton kykliko buffer
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
//tha akoloy8h8ei readers-writers politikh gia thn prospelash-enhmerwsh ths klashs
class worker_db{
public:
  int n_workers;
  worker * workers;

  pthread_mutex_t lock; //mutex gia ton kykliko buffer
  pthread_cond_t readcond; //condtion var gia diabasma ths domhs. diafaneies ntoula readers writers
  pthread_cond_t writecond; //condition var gia writer. diafaneies ntoula readers writers
  int readers;
  bool writer;

  worker_db();
  ~worker_db();
  void add_worker(worker ); //prosthetei worker kanontas to swsto resize
  void extract_worker(tuple ); //diabazei apo fd stoixeia enos worker
  //gia eisodo-eksodo apo critical section:
  void cs_writer_start();
  void cs_writer_end();
  void cs_reader_start();
  void cs_reader_end();
  //gia anazhthsh poiou worker prepei na psaksw
  worker * search_worker_by_country(std::string );
};

extern synchro_stdout st ;//gia xrhsh apo threads
//gia kykliko buffer
extern pool * circle;
//gia metadata workers
extern worker_db * work_db;
//prepei na tous rwthsw olous h enan ?
int must_ask_all(std::string );
//gia prow8hsh erwthmatos sto worker apo server
void ask_the_right_one(int , std::string , int *, std::string *);
void ask_them_all(int , std::string , int **, int *, std::string *);
//gia lhpsh apanthsewn apo worker se erwthma
void get_answer_from_right_one(std::string, int , std::string *);
void get_and_compose_answer_from_all(std::string , int *, int, std::string *);
#endif
