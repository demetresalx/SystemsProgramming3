#include <iostream>
#include <cstring> //strcpy gia ta pathings. genika oxi idiaitera
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "master_boss.h"
#include "worker.h"
#include "utils.h"

int main(int argc, char ** argv){

  //GIA TIS PARAMETROUS APO ARGC
  char input_path[256]; //to path tou patientRecordsFile poy tha parei apo to command line
  char serverIP[256]; // h serverIP
  int serverPort =0;
  int numWorkers=0;
  int bufferSize=0;
  for (int i = 0; i < argc; i++){
    if (strcmp("-i", argv[i]) == 0){
      strcpy(input_path, argv[i + 1]); //akoloythei to path
    }

    if (strcmp("-s", argv[i]) == 0){
      strcpy(serverIP, argv[i + 1]); //akoloythei h dieu9unsh
    }

    if (strcmp("-p", argv[i]) == 0){
      serverPort = atoi(argv[i + 1]); //akoloythei h porta
    }

    if (strcmp("-w", argv[i]) == 0){
      numWorkers = atoi(argv[i + 1]); //akoloythei h timh
    }

    if (strcmp("-b", argv[i]) == 0){
      bufferSize = atoi(argv[i + 1]); //akoloythei h timh
    }

  } //telos for command line args
  if(argc != 11) //thelw na einai akribws osa leei h ekfwnhsh
    {std::cout << "Bad command line arguments, try again...\n"; return -1;}
  if(bufferSize < 1) //prepei na einai toulaxiston 1 byte
    bufferSize = 1;
  if(numWorkers < 1) //we have AT LEAST 1 worker
    numWorkers = 1;

    //Dhmiourgia named pipes
    char pipe_names[2*numWorkers][100];
    std::string pip_names[2*numWorkers];
    for(int i=0; i<2*numWorkers; i++){
      pip_names[i] = "fifo" + std::to_string(i);
      strcpy(pipe_names[i], pip_names[i].c_str()); //onomata ths morfhs fifo1,fifo2 ktl
      mkfifo(pipe_names[i], PERMS);
    }//telos dhmiourgias pipes

    int pids[numWorkers]; //pinakas me ta pids twn paidiwn
    //dhmiourgia paidiwn-workers
    int pid;
    int child_index=0; //poio paidi eimai
    for(int i=0; i<numWorkers; i++){
      pid = fork();
      pids[i] = pid;
      if(pid == 0){ //worker, feugei ap th loypa dhmiourgias
        child_index=i;
        break;
      }
    }//for gia kathe worker

    if(pid > 0){ //parent
      //douelia sto boss.cpp
      administrate(input_path, numWorkers , bufferSize ,pip_names, pids, serverPort, serverIP);
    }
    else{ //child
      //douleia sto worker.cpp
      work(pipe_names[2*child_index +1],pipe_names[2*child_index], bufferSize, 0);
    }

    //Perimene kathe paidi kai sbhse ta pipes tou
    if(pid >0){ //parent
      for(int i=0; i<numWorkers; i++){
        kill(pids[i], SIGKILL); //Mhdeia paidoktonos!!
        int stats;
        int fpid= waitpid(pids[i], &stats, 0);
        //unlink gia sbhsimo pipes tou kathe paidiou. Meta th wait to paidi tha exei teleiwsei kai den ta xrhsimopoiei pleon
        if( unlink(pipe_names[2*i]) < 0)
              perror("Error: Cannot unlink worker");
          if( unlink(pipe_names[2*i +1]) < 0)
              perror("Error: Cannor unlink jobExecutor");
      }//telos for gia kathe worker
    }//telos telikhs leitourgias goniou

  return 0;
}
