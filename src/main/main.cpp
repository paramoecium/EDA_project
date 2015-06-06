#include <iostream>
#include <fstream>
#include "cirMgr.h"

using namespace std;

int
main(int argc, char** argv){
   /*
    string file;
    cin >> file;
    CirMgr* cirMgr = new CirMgr();
    cirMgr->readCircuit(file);
    cirMgr->linkGates();
    cirMgr->buildDfsList();
    cirMgr->printNetlist();
    */
   if (argc == 3) {  // <program> <in_1.v> <in_2.v>
      CirMgr* cirMgr = new CirMgr();
      string fileName = "testPattern";
      cirMgr -> readCircuit(argv[1], true);
      cirMgr -> readCircuit(argv[2], false);
      cirMgr -> linkGates();
      cirMgr -> buildDfsList();
      cirMgr -> printNetlist();
      cirMgr -> genPattern(fileName, 32);
      ifstream patternFile(fileName.c_str(), ios::in);
      cirMgr -> fileSim(patternFile);
      cirMgr -> printFECPairs();
      cirMgr -> fraig();
      cirMgr -> printFECPairs();
   }
   else {
      cerr << "Error: illegal number of argument (" << argc << ")!!\n";
   }
   return 0;
}
