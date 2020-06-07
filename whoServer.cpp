#include <iostream>
#include <cstring>
#include <sys/socket.h> //socket programming
#include <netinet/in.h> //socket programming
#include <arpa/inet.h> //to idio
#include <signal.h> //sigaction
#include <string>
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
//mutex, cond var kai condition gia thn ektupwsh sto stdout apo threads
pthread_mutex_t stdout_lock = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t stdout_in_use = PTHREAD_COND_INITIALIZER;
int stdout_user =0;

void * thread_basis(void * ar){
  pthread_exit(NULL);
  pthread_mutex_lock(&stdout_lock);
  while(stdout_user >0)
    pthread_cond_wait(&stdout_in_use, &stdout_lock);
  stdout_user =1;
  hello();
  stdout_user=0;
  pthread_mutex_unlock(&stdout_lock);
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
    pthread_create( &tids[i], NULL, thread_basis, NULL) ; //ta ftiaxnw kai ta bazw na pane sth vasikh tous sunarthsh
  }
  //pame na paroume ta statistika apo tous workers
  char buffer1[256], buffer2[256];
  int server = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in my_addr, peer_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port = htons(statisticsPortNum);
  bind(server, (struct sockaddr*) &my_addr, sizeof(my_addr));
  listen(server, 10);
  socklen_t addr_size;
  addr_size = sizeof(struct sockaddr_in);
  std::string tool ="";
  while(1){
    if(quitflag >0){ //fagame sigint/quit telos
      break;
    }
    //poll???
    int acc = accept(server, (struct sockaddr*) &peer_addr, &addr_size);
    printf("Connection Established\n");
    // "ntohs(peer_addr.sin_port)" function is
    // for finding port number of client
    //printf("connection established with IP : %s and PORT : %d\n", buffer1, htons(peer_addr.sin_port));
    receive_string(acc, &tool, 12);
    std::cout << "mageireuw: " << tool << "\n";
  }//telos while sundesewn

  //wait for threads to terminate
  for(int i=0; i<numThreads; i++)
    pthread_join(tids[i], NULL);
  delete[] tids; //svhse axrhsto pleon pinaka
  delete circle; //destroy kykliko buffer epishs
  //destroy all mutexes and condition vars
  pthread_cond_destroy(&stdout_in_use); pthread_mutex_destroy(&stdout_lock);
  return 0;
}
