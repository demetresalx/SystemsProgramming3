#include <iostream>
#include <cstring>
#include <sys/socket.h> //socket programming
#include <netinet/in.h> //socket programming
#include <arpa/inet.h> //to idio
#include <signal.h> //sigaction
#include <string>
#include <errno.h>
#include <unistd.h>
#include "threadfuns.h" //nhmata
#include "utils.h"

//gia dikh moy omalh leitourgia exit sto server me CTRL-C. Den einai aparaithto kata ekfwnhsh
int quitflag =0; //gia na kserw an tha grapsw log kai kleinw
void quit_hdl(int signo){
  quitflag=1; //gia na kserei sth megalh while ti tha kanei to paidi
}
//KOINOXRHSTES METAVLHTES GIA THREADS TOU SERVER, MUTEXES TOUS K CONDVARS! polla einai extern gia eukolia sto grapsimo sunarthsewn, des threadfuns.h
pool * circle; //H DOMH POOL TWN DIAFANEIWN, apoteleitai apo to array poy eiai o kyklikos buffer kai ta start, end, count. Arxikopoieitai apo to main thread
//external klash gia sugxronismo tou stdout metaksu threads. Des threadfuns.h gia to ti kanei
synchro_stdout  st; //des threadfuns.h & .cpp

void * thread_basis(void * ar){
  pthread_exit(NULL);
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
  //Dhmiourgw 2 sockets "autia". To ena akouei sundeseis sto port gia statistika kai to allo sto port gia queries
  //Mesw poll() tha mporw na vlepw poy dexomai sundeseis kai na tis prow8w analoga
  char buffer1[256], buffer2[256];
  int listen_stats = socket(AF_INET, SOCK_STREAM, 0);
  int listen_queries = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in my_addr, peer_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port = htons(statisticsPortNum);
  //antistoixizw to socket sthn porta gia statistics
  bind(listen_stats, (struct sockaddr*) &my_addr, sizeof(my_addr));
  my_addr.sin_port = htons(queryPortNum);
  //antistoixizw to allo socket sthn porta gia queries
  bind(listen_queries, (struct sockaddr*) &my_addr, sizeof(my_addr));
  //listen
  listen(listen_stats, 10);
  listen(listen_queries, 10);
  socklen_t addr_size;
  addr_size = sizeof(struct sockaddr_in);
  std::string tool ="";
  struct pollfd listenfds[2]; //ta read fds twn pipes poy tha mpoyn kai sthn poll
  listenfds[0].fd = listen_stats;
  listenfds[1].fd = listen_queries;
  reset_poll_parameters(listenfds, 2);
  //arxizei h leitourgia tou server poy anazhta sundeseis

  int accepted_fd;
  while(1){
    if(quitflag >0){ //fagame sigint/quit telos
      break;
    }
    //poll???
    int rc = poll(listenfds, 2, 2000);
    if(rc == 0) //timeout
      {;;}
    else{ //tsekarw poio auti exei sundesh
        for(int i=0; i<2; i++){
          if(listenfds[i].revents == POLLIN){ //exoume sundesh edw
            //elegxw poio apo ta 2 einai gia na to xeiristw analoga
            if(listenfds[i].fd == listen_stats){ //yparxei sundesh gia statistics. proceed with reading/printing tous
              //pame na piasoume twra ola ta incoming connections gia statistics poy einai pending edw
              do{
                accepted_fd = accept(listen_stats, (struct sockaddr*) &peer_addr, &addr_size);
                std::cout << "New statistics connection!!\n";
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); //pare address tou worker
                std::cout << "sto " << ip << "\n";
                //pame na paroume ta summary statistics apo edw
                int ndirs=0;
                receive_integer(accepted_fd, &ndirs);
                for(int j=0; j<ndirs; j++){
                  int nfls =0;
                  receive_integer(accepted_fd, &nfls);
                  //std::cout << "tha diabasw size " << ndirs << " " << nfls << "\n";
                  for(int k=0; k<nfls; k++)
                    receive_and_print_file_summary(accepted_fd, IO_PRM); //ektupwse to summary
                }
                if(check_if_will_block(listen_stats)) //an tis phrame oles tis sundeseis
                  {break;}
              }while(accepted_fd > 0);
            }
            else{ //einai o query listener
              ;;
            }
          } //telos elegxou diathesimothtas fd
        }//telos for gia ta 2 autia
    } //telos else gia timeout ths poll

  }//telos while sundesewn


  //wait for threads to terminate
  for(int i=0; i<numThreads; i++)
    pthread_join(tids[i], NULL);
  delete[] tids; //svhse axrhsto pleon pinaka
  delete circle; //destroy kykliko buffer epishs
  //destroy all mutexes and condition vars
  return 0;
}
