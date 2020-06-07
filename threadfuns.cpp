#include <iostream>
#include "threadfuns.h"

//sunarthseis twn threads!
//dokimastikh
void hello(){
  std::cout << "i am thread " << pthread_self() << "\n";
}
