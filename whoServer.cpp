#include <iostream>
#include <cstring>

int main(int argc, char ** argv){
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

  std::cout << queryPortNum << "\n";

  return 0;
}
