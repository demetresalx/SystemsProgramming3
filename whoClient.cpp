#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> //perror
#include <sys/socket.h> //socket programming
#include <netinet/in.h> //socket programming
#include <arpa/inet.h> //to idio
#include "utils.h"
#include "client_threads.h"


//gia na kseroun poy na sunde8oun ta threads
char servIP[256];
int servPort =0;
int numThreads=0;

//gia na perimenei to main thread na diabastei h grammh apo to thread
pthread_cond_t got_line_cnd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t got_line_mtx = PTHREAD_MUTEX_INITIALIZER;
bool got_line = false;
//gia na ksekinhsoun ola ta threads mazi afou diabasoun tis entoles tous
pthread_cond_t at_once_cnd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t at_once_mtx = PTHREAD_MUTEX_INITIALIZER;
bool at_once = false;
//gia sygxronismo sto stdout
synchro_stdot sto;


void * threadcl(void * arln){
  std::string comm = *((std::string *) arln); //phra th grammh moy
  std::string requ[12];
  int params = sanitize_command(comm, requ); //apomonwse ta orismata
  if(params <1)
    {sto.cs_start();std::cout << "Badly defined query. Corresponding thread will terminate.\n";sto.cs_end();pthread_exit(NULL);}
  //Paw na ftiaksw socket gia server
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  //inet_pton(AF_INET, serverIP, &(serv_addr.sin_addr)); //pare vale th dieu9unsh tou server
  serv_addr.sin_addr.s_addr = inet_addr(servIP);
  serv_addr.sin_port = htons(servPort); //Vazw to port tou orismatos Servport
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(serv_sock < 0)
    {perror("socket error\n");pthread_exit(NULL);}
  //eidopoiw main thread na sunexisei diabasma arxeiou
  got_line = true;
  pthread_cond_signal(&got_line_cnd);
  //twra perimenw mexri o main na ftiaksei ola ta threads kai na mas dwsei to ok na fugoume ola mazi!
  pthread_mutex_lock(&at_once_mtx);
  while(!at_once)
    pthread_cond_wait(&at_once_cnd, &at_once_mtx);
  pthread_mutex_unlock(&at_once_mtx);
  //ksekinhsamee!!! stelnw query
  if(connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
      sto.cs_start();
      perror("\nConnection error, Corresponding thread will terminate :");
      sto.cs_end();
      pthread_exit(NULL);
  }
  if(send_command(serv_sock, requ, params, comm) < 0)//steilto!
    {sto.cs_start();std::cout<<"Kako command. thread will temrminate\n";sto.cs_end();pthread_exit(NULL);}
  std::string answer_to_present ="";
  receive_string(serv_sock, &answer_to_present, IO_PRM);
  //ektypwnw thread-safe sto stdout thn apanthsh poy phra ISWS KAI THN ERWTHSH
  sto.cs_start();
  std::cout << comm <<"\n";
  std::cout << answer_to_present << "\n";
  sto.cs_end();
  close(serv_sock); //kleinw sundesh

  //termatismos thread
  pthread_exit(NULL);
}


int main(int argc, char ** argv){
  //GIA TIS PARAMETROUS APO ARGC
  char queryFile[256];
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
  int maxlines = get_lines_ofile(queryFile); //to xreiazomai gia thn akraia periptwsh
  //NULLpthread_t * tids = new pthread_t[maxlines]; //krataw ta pthreads
  pthread_t * tids = new pthread_t[numThreads];
  bool valid_tid[numThreads] = {false}; //gia na kserw se mia join an einai valid to id tou thread
  int threads_pack =0; //molis ftasw numthreads, perimenw na teleiwsoun gia na steilw ta numthreads epomena
  while (std::getline(infile, line)){ //read file
    //std::cout << line;
    //de thelw na exw panw apo numThreads ongoing. An sumvei auto perimene ta zwntana na teleiwsoun prin ftiakseis alla numThreads
    if((threads_pack == numThreads)){ //prepei na perimenw na teleiwsei h trexousa fournia apo numThreads prin ftiaksw nea numThreads
      if(threads_pack > 0){
        //TA KSEKINAW OLA MAZI
        at_once = true;
        pthread_cond_broadcast(&at_once_cnd);
        //perimenw na teleiwsoun auta poy einai twra k trexoun
        for(int i=0; i<threads_pack; i++)
          {pthread_join(tids[i], NULL);}
        //ean to arxeio prokeitai na teleiwsei kai einai mh pollaplasio tou numthreads
        if(numThreads > maxlines-lines){
          delete[] tids;
          tids = new pthread_t[numThreads];
          for(int i=maxlines-lines; i<numThreads; i++) //oi extra theseis tou pinaka den einai valid. gia na mhn kanw kako join
            valid_tid[i] = false;
        }
        else
          {delete[] tids; tids = new pthread_t[numThreads];} //pame gia thn epomenh numthreads-ada
        threads_pack=0; //arxizei h nea numthreads-ada
      }
    }//telos if gia an teleiwse h prwth fournia
    //stelnw th grammh sto thread poy ftiaxnw kai kanei ekei to sanitizing/elegxo
    pthread_create( &(tids[threads_pack]), NULL, threadcl, &line) ; //ta ftiaxnw kai ta bazw na pane sth vasikh tous sunarthsh
    valid_tid[threads_pack] = true; //pragmatiko thread
    lines++;
    threads_pack++;
    //perimenw to thread na parei th grammh tou
    while(!got_line)
      pthread_cond_wait(&got_line_cnd, &got_line_mtx);
    got_line = false;
  }//telos while read file
  //Ksekinaw ta threads gia tis grammes mh pollaplasia tou numthreads poy einai h teleutaia loypa
  at_once = true;
  pthread_cond_broadcast(&at_once_cnd);
  //wait for threads to finish
  for(int i=0; i<numThreads; i++){
    if(valid_tid[i]) //na mhn riskarw na kanw join se kati poy den einai thread
      pthread_join(tids[i], NULL);
  }

  //free(tids);
  delete[] tids;
  return 0;
}
