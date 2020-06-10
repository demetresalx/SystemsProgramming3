#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

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
  while (std::getline(infile, line)){ //read file
    //stelnw th grammh sto thread poy ftiaxnw kai kanei ekei to sanitizing/elegxo

  }//telos while read file

  return 0;
}
