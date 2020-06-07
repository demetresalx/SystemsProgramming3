#include <iostream>
#include "threadfuns.h"

//sunarthseis twn threads!
//dokimastikh
void * hello(void * ar){
  std::cout << "i am thread " << pthread_self() << "\n";
  return NULL;
}
