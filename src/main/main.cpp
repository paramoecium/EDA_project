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
      CirMgr* cirMgr[2];
      cirMgr[0] = new CirMgr();
      cirMgr[1] = new CirMgr();
      string fileName = "testPattern";
      for (unsigned i=0; i<2; ++i){
         cirMgr[i] -> readCircuit(argv[i+1]);
         cirMgr[i] -> linkGates();
         cirMgr[i] -> buildDfsList();
         cirMgr[i] -> printNetlist();
         if (!i)
            cirMgr[i]->genPattern(fileName, 32);
         ifstream patternFile(fileName.c_str(), ios::in);
         cirMgr->fileSim(patternFile);
      }

   }
   else {
      cerr << "Error: illegal number of argument (" << argc << ")!!\n";
   }
   return 0;
}
