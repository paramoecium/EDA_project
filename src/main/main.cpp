#include <iostream>
#include "cirMgr.h"

using namespace std;

int
main(int argc, char** argv){
   if (argc == 2) {  // <program> <in_1.v>
      CirMgr* cirMgr = new CirMgr();
      cirMgr->readCircuit(argv[1]);
   }
   else {
      cerr << "Error: illegal number of argument (" << argc << ")!!\n";
   }
   return 0;
}
