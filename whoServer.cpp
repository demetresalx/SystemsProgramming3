#include <iostream>
#include <cstring>
#include <signal.h> //sigaction
#include "threadfuns.h" //nhmata
#include "utils.h"

//gia dikh moy omalh leitourgia exit sto server me CTRL-C. Den einai aparaithto kata ekfwnhsh
int quitflag =0; //gia na kserw an tha grapsw log kai kleinw
void quit_hdl(int signo){
  quitflag=1; //gia na kserei sth megalh while ti tha kanei to paidi
}
//KOINOXRHSTES METAVLHTES GIA THREADS TOU SERVER, MUTEXES TOUS K CONDVARS!
pool * circle; //H DOMH POOL TWN DIAFANEIWN, exei mesa to array poy eiai o kyklikos buffer kai ta start, end, count
pthread_mutex_t circle_lock = PTHREAD_MUTEX_INITIALIZER ;

pthread_cond_t stdout_in_use = PTHREAD_COND_INITIALIZER;
int stdout_user =0;

void * godlo(void * ar){
  pthread_mutex_lock(&circle_lock);
  while(stdout_user >0)
    pthread_cond_wait(&stdout_in_use, &circle_lock);
  stdout_user =1;
  hello();
  stdout_user=0;
  pthread_mutex_unlock(&circle_lock);
  pthread_cond_broadcast(&stdout_in_use);

}


int main(int argc, char ** argv){
  //de xreiazetai, aplws gia tis dokimes thelw na kanw omalo exit me SIGINT/QUIT
  struct sigaction actquit;
  actquit.sa_handler = quit_hdl;
  //sigaction(SIGINT, &actquit, NULL); //to orisame!
  //sigaction(SIGQUIT, &actquit, NULL); //to orisame!
  //GIA TIS PARAMETROUS APO ARGC
  int queryPortNum =0;
  int statisticsPortNum =0;
  int numThreads=0;
  int bufferSize=0; //megethos kyklikou buffer
  for (int i = 0; i < argc; i++){
    if (strcmp("-s", argv[i]) == 0){
      statisticsPortNum = atoi(argv[i + 1]); //akoloythei h porta
    }

    if (strcmp("-q", argv[i]) == 0){
      queryPortNum = atoi(argv[i + 1]); //akoloythei h porta
    }

    if (strcmp("-w", argv[i]) == 0){
      numThreads = atoi(argv[i + 1]); //akoloythei h timh
    }

    if (strcmp("-b", argv[i]) == 0){
      bufferSize = atoi(argv[i + 1]); //akoloythei h timh
    }

  } //telos for command line args
  if(argc != 9) //thelw na einai akribws osa leei h ekfwnhsh
    {std::cout << "Bad command line arguments, try again...\n"; return -1;}
  if(bufferSize < 1) //prepei na einai toulaxiston 1 job
    bufferSize = 1;
  if(numThreads < 1) //we have AT LEAST 1 thread
    numThreads = 1;
  //ftiaxnw ton kykliko buffer. EINAI TO POOL TWN DIAFANEIWN TOU NTOULA
  circle = new pool(bufferSize); //to arxikopoiei me to dothen megethos
  //ftiaxnw numThreads nhmata
  pthread_t * tids; //krataw ta pthreads
  tids = new pthread_t[numThreads];
  for(int i=0; i<numThreads; i++){
    pthread_create( &tids[i], NULL, godlo, NULL) ;
  }

  //wait for threads to terminate
  for(int i=0; i<numThreads; i++)
    pthread_join(tids[i], NULL);
  delete[] tids; //svhse axrhsto pleon pinaka
  delete circle; //destroy kykliko buffer epishs
  return 0;
}
