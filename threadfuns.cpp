#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include "threadfuns.h"
#include "utils.h"

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
  fds = new tuple[sz]; //oso to bufferSize tha einai auto
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
void pool::place(tuple fd){
  pthread_mutex_lock(&lock) ;
  while(count >= size){
    //printf(">> Found Buffer Full \n");
    pthread_cond_wait (&nonfull , &lock) ;
  }
  end = (end + 1) % size ;
  fds[end] = fd ;
  count++;
  pthread_mutex_unlock(&lock) ;
}//telos sunarthshs

tuple pool::obtain(){
  tuple data;
  pthread_mutex_lock(&lock) ;
  while(count <= 0){
    //printf (">> Found Buffer Empty\n");
    pthread_cond_wait(&nonempty , &lock) ;
  }
  data = fds[start];
  //printf("Eimai o %d kai tsimphsa to fd %d\n", pthread_self(), data);
  start = (start + 1) % size ;
  count--;
  //std::cout << "eimai o " << pthread_self() << " phra" << data.fd << data.type << "\n";
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

//metadata poy krataei o server gia kathe worker
//tsekarw an exw auth th xwra
bool worker::has_country(std::string cntry ){
  for(int i=0; i<n_countries; i++)
    if(countries[i] == cntry)
      return true;
  return false;
}

void worker::add_country(std::string cntry ){
  if(has_country(cntry)) //h xwra yparxei, den thn ksanabazw
    return;
  std::string * newcountries = new std::string[n_countries+1];
  for(int i=0; i< n_countries; i++)
    newcountries[i] = countries[i];
  newcountries[n_countries] = cntry;
  n_countries++ ;
  delete[] countries;
  countries = newcountries;
}

//gia thn klash poy krataei workers kai th vlepoyn ta threads
worker_db::worker_db(){
  n_workers =0;
  workers = NULL;
  readers =0;
  writer = false;
  lock = PTHREAD_MUTEX_INITIALIZER;
  readcond = PTHREAD_COND_INITIALIZER;
  writecond = PTHREAD_COND_INITIALIZER;
}

worker_db::~worker_db(){
  for(int i=0; i< n_workers; i++)
    delete[] workers[i].countries;
  delete[] workers;
  pthread_cond_destroy(&readcond);
  pthread_cond_destroy(&writecond);
  pthread_mutex_destroy(&lock);
}

void worker_db::add_worker(worker wrkr){
  worker * newworkers = new worker[n_workers+1];
  for(int i=0; i< n_workers; i++)
    newworkers[i] = workers[i]; //thanks to operator overloading!
  newworkers[n_workers] = wrkr;
  n_workers++;
  delete[] workers;
  workers = newworkers;
}

//sthn arxh diabasmatos apo statistics port enhmerwnei th domh metadata gia workers
void worker_db::extract_worker(tuple sfd){
  uint16_t worker_port =0;
  read(sfd.fd, &worker_port, sizeof(worker_port));
  char ip[256];
  receive_string(sfd.fd, ip, IO_PRM);
  //std::cout << "Phra to " << ntohs(worker_port) << " " << ip <<"\n";
  int cntrs =0;
  receive_integer(sfd.fd, &cntrs);
  worker thisone;
  thisone.port = worker_port;
  thisone.address = sfd.address;
  std::string cntr;
  for(int j=0; j<cntrs; j++){
    receive_string(sfd.fd, &cntr, IO_PRM ); //pare xwra
    thisone.add_country(cntr);
  }
  add_worker(thisone);
}

//OI PARAKATW 4 METHODOI GIA DIABASMA K enhmerwsh THS WORKER_DB EINAI BASISMENA STO READERS/WRITER TWN DIAFANEIWN
//thelw na grapsw kati sth domh worker_db, gia na mpw se critical section
void worker_db::cs_writer_start(){
  pthread_mutex_lock(&lock);
  while(readers >0 || writer)
    pthread_cond_wait(&writecond, &lock);
  writer = true;
  pthread_mutex_unlock(&lock);
  //arxise to critcal section gia enhmerwsh ths classhs
}
//eksodos apo CS writer
void worker_db::cs_writer_end(){
  pthread_mutex_lock(&lock);
  writer = false;
  pthread_cond_broadcast(&readcond);
  pthread_cond_signal(&writecond);
  pthread_mutex_unlock(&lock);
}
//eisodos se CS gia reader apo thn klash
void worker_db::cs_reader_start(){
  pthread_mutex_lock(&lock);
  while(writer)
    pthread_cond_wait(&readcond, &lock);
  readers++ ;
  pthread_mutex_unlock(&lock);
  //arxise critical section gia diabasma apo thn classh
}
//eksodos apo CS gia reader
void worker_db::cs_reader_end(){
  pthread_mutex_lock(&lock);
  readers-- ;
  if(readers == 0)
    pthread_cond_signal(&writecond); //broadcast xwris if??
  pthread_mutex_unlock(&lock);
}
