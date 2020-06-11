#include <iostream>
#include <cstring>
#include <sys/socket.h> //socket programming
#include <netinet/in.h> //socket programming
#include <arpa/inet.h> //to idio
#include <signal.h> //sigaction
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
//external klash gia na krataw metadata gia tous workers kai na kserw ws thread ti na prow8hsw se poion kai se poia porta ktl...
worker_db * work_db; //tha akoloy8h8ei readers-writers politikh gia thn prospelash-enhmerwsh ths klashs apo ta threads

//sunarthsh POY KANEI TH DOULEIA TWN THREADS TOU SERVER
void * thread_basis(void * ar){
  //pthread_exit(NULL);
  while(1){
    tuple got = circle->obtain();
    pthread_cond_signal(&(circle->nonfull));
    if(got.type == "statistics"){ //phrame sundesh gia worker kai statistics
      //paw na enhmerwshs work_db gia metadata twn workers
      work_db->cs_writer_start();
      work_db->extract_worker(got);
      work_db->cs_writer_end();
      //diabazw & ektypwnw ta statistics!
      //xrhsimopoiw to synchro_stdout poy eftiaksa gia na mh mpleketai to stdout outpu
      int ndirs=0;
      receive_integer(got.fd, &ndirs);
      for(int j=0; j<ndirs; j++){
        int nfls =0;
        receive_integer(got.fd, &nfls);
        //std::cout << "tha diabasw size " << ndirs << " " << nfls << "\n";
        for(int k=0; k<nfls; k++)
          {st.cs_start();receive_and_print_file_summary(got.fd, IO_PRM);st.cs_end();} //ektupwse to summary
      }
    }//telos if statistics sundesh
    else if(got.type == "query"){ //phrame sundesh apo client kai periexei erwthma
      //read and forward query to the Corresponding worker(s???)
      std::string quest = ""; receive_string(got.fd, &quest ,IO_PRM); //pare onoma entolhs
      std::string answer = "";
      if(must_ask_all(quest)){ //prepei na rwthsw olous tous workers
        int * asked_workers = NULL;
        int indx = 0;
        ask_them_all(got.fd, quest, &asked_workers, &indx);
        get_and_compose_answer_from_all(quest, asked_workers, indx, &answer);
        //stelnw apanthsh ston client
        send_string(got.fd, &answer, IO_PRM);
      }
      else{ //paw mono sto swsto worker
        ;;
      }
    }//telos if query sundesh
  }//telos while atermonhs??
}//telos thread_basis


int main(int argc, char ** argv){
  //de xreiazetai, aplws gia tis dokimes thelw na kanw omalo exit me SIGINT/QUIT
  struct sigaction actquit;
  actquit.sa_handler = quit_hdl;
  sigaction(SIGINT, &actquit, NULL); //to orisame!
  sigaction(SIGQUIT, &actquit, NULL); //to orisame!
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
  work_db = new worker_db; //to arxikopoiei
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
  const int opt = 1;
  setsockopt(listen_stats, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  setsockopt(listen_stats, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
  int listen_queries = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(listen_queries, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  setsockopt(listen_queries, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
  struct sockaddr_in my_addr, peer_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port = htons(statisticsPortNum);
  //antistoixizw to socket sthn porta gia statistics
  if(bind(listen_stats, (struct sockaddr*) &my_addr, sizeof(my_addr)) < 0)
    {perror("Bind 1:"); return -1;}
  my_addr.sin_port = htons(queryPortNum);
  //antistoixizw to allo socket sthn porta gia queries
  if(bind(listen_queries, (struct sockaddr*) &my_addr, sizeof(my_addr)) <0)
    {perror("Bind 2:"); return -1;}
  //listen
  if(listen(listen_stats, 15) < 0)
    {perror("Listen 1:"); return -1;}
  if(listen(listen_queries, 15) <0)
    {perror("Listen 1:"); return -1;}
  socklen_t addr_size;
  addr_size = sizeof(struct sockaddr_in);
  std::string tool ="";
  struct pollfd listenfds[2]; //ta read fds twn pipes poy tha mpoyn kai sthn poll
  listenfds[0].fd = listen_stats;
  listenfds[1].fd = listen_queries;
  reset_poll_parameters(listenfds, 2);
  //arxizei h leitourgia tou server poy anazhta sundeseis

  int accepted_fd;
  std::cout << "Listening!!\n";
  while(1){
    if(quitflag >0){ //fagame sigint/quit telos
      break;
    }

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
                //pare address tou worker apo 2o orisma accept
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); //pare address tou worker
                tuple newfd; newfd.fd = accepted_fd; newfd.type = "statistics"; newfd.address= std::string(ip);
                //topo8ethse ton file descriptor ston kykliko buffer gia na asxolh8oun ta threads
                circle->place(newfd);
                pthread_cond_broadcast(&(circle->nonempty));
                //char ip[INET_ADDRSTRLEN];
                //inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); //pare address tou worker
                //std::cout << "sto " << ip << "\n"; //ISWS THELEI NA TOU STELNEI TO IP TOU TO WORKER
                //work_db->extract_worker(accepted_fd);
                //pame na paroume ta summary statistics apo edw
                /*int ndirs=0;
                receive_integer(accepted_fd, &ndirs);
                for(int j=0; j<ndirs; j++){
                  int nfls =0;
                  receive_integer(accepted_fd, &nfls);
                  //std::cout << "tha diabasw size " << ndirs << " " << nfls << "\n";
                  for(int k=0; k<nfls; k++)
                    receive_and_print_file_summary(accepted_fd, IO_PRM); //ektupwse to summary
                }*/
                if(check_if_will_block(listen_stats)) //an tis phrame oles tis sundeseis
                  {break;}
              }while(accepted_fd > 0);
            }
            else if(listenfds[i].fd == listen_queries){ //einai o query listener
              do{
                accepted_fd = accept(listen_queries, (struct sockaddr*) &peer_addr, &addr_size);
                st.cs_start();std::cout << "New queries connection!!\n";st.cs_end();
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN); //pare address tou client
                tuple newfd; newfd.fd = accepted_fd; newfd.type = "query"; newfd.address= std::string(ip);
                //topo8ethse ton file descriptor ston kykliko buffer gia na asxolh8oun ta threads
                circle->place(newfd);
                pthread_cond_broadcast(&(circle->nonempty));

                if(check_if_will_block(listen_queries)) //an tis phrame oles tis sundeseis
                  {break;}
              }while(accepted_fd > 0);
            }//telos if query
          } //telos elegxou diathesimothtas fd
        }//telos for gia ta 2 autia
    } //telos else gia timeout ths poll

  }//telos while sundesewn

  for(int i=0; i< work_db->n_workers; i++){
    std::cout << "eimai o " <<  work_db->workers[i].address << ntohs( work_db->workers[i].port) << "\n";
    for(int j=0; j<  work_db->workers[i].n_countries; j++)
      std::cout <<  work_db->workers[i].countries[j] << "\n";
  }


  //wait for threads to terminate
  //for(int i=0; i<numThreads; i++)
    //pthread_join(tids[i], NULL);
  delete[] tids; //svhse axrhsto pleon pinaka
  delete circle; //destroy kykliko buffer epishs
  delete work_db; //destroy metadata workers
  //destroy all mutexes and condition vars
  return 0;
}
