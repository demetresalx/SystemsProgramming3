#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

pthread_cond_t got_line_cnd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t got_line_mtx = PTHREAD_MUTEX_INITIALIZER;
bool got_line = false;

pthread_cond_t at_once_cnd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t at_once_mtx = PTHREAD_MUTEX_INITIALIZER;
bool at_once = false;

void * threadcl(void * arln){
  std::string ma = *((std::string *) arln); //phra th grammh moy
  std::cout << ma << "\n";
  //eidopoiw main thread na sunexisei diabasma arxeiou
  got_line = true;
  pthread_cond_signal(&got_line_cnd);
  //twra perimenw mexri o main na ftiaksei ola ta threads kai na mas dwsei to ok na fugoume ola mazi!
  pthread_mutex_lock(&at_once_mtx);
  while(!at_once)
    pthread_cond_wait(&at_once_cnd, &at_once_mtx);
  pthread_mutex_unlock(&at_once_mtx);

  pthread_exit(NULL);
}

int main(int argc, char ** argv){
  //GIA TIS PARAMETROUS APO ARGC
  char queryFile[256];
  char servIP[256];
  int servPort =0;
  int numThreads=0;
  for (int i = 0; i < argc; i++){
    if (strcmp("-q", argv[i]) == 0){
      strcpy(queryFile, argv[i+1]); //akoloythei to onoma/monopati tou query file
    }

    if (strcmp("-sip", argv[i]) == 0){
      strcpy(servIP, argv[i+1]); //akoloythei h porta
    }

    if (strcmp("-w", argv[i]) == 0){
      numThreads = atoi(argv[i + 1]); //akoloythei h timh
    }

    if (strcmp("-sp", argv[i]) == 0){
      servPort = atoi(argv[i + 1]); //akoloythei h porta
    }

  } //telos for command line args
  if(argc != 9) //thelw na einai akribws osa leei h ekfwnhsh
    {std::cout << "Bad command line arguments, try again...\n"; return -1;}
  if(numThreads < 1) //we have AT LEAST 1 thread
    numThreads = 1;
  //pame diabasma arxeiou
  std::ifstream infile(queryFile); //diabasma apo tis grammes tou arxeiou
  std::string line; //EPITREPETAI H STRING EIPAN STO PIAZZA
  int lines = 0;
  pthread_t * tids = NULL; //krataw ta pthreads
  while (std::getline(infile, line)){ //read file
    //std::cout << line;
    //stelnw th grammh sto thread poy ftiaxnw kai kanei ekei to sanitizing/elegxo
    tids = (pthread_t *) realloc(tids, (lines+1)*sizeof(pthread_t)); //gia neo thread proekteinw ton pinaka
    pthread_create( &(tids[lines]), NULL, threadcl, &line) ; //ta ftiaxnw kai ta bazw na pane sth vasikh tous sunarthsh
    lines++;
    while(!got_line)
      pthread_cond_wait(&got_line_cnd, &got_line_mtx);
    got_line = false;
  }//telos while read file
  //TA KSEKINAW OLA MAZI
  at_once = true;
  pthread_cond_broadcast(&at_once_cnd);

  //wait for threads to finish
  for(int i=0; i<lines; i++)
    pthread_join(tids[i], NULL);

  free(tids);
  return 0;
}
