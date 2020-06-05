#include <iostream>
#include <cstring> //strcpy gia ta pathings. genika oxi idiaitera

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

  std::cout << "i got this many works" << numWorkers << "\n";
  return 0;
}
