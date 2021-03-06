#include <iostream>
#include <fstream>
#include "cirMgr.h"

using namespace std;

extern CirMgr* cirMgr;

int main(int argc, char** argv){
   if (argc == 5) {  // <program> <in_1.v> <in_2.v> <out1.v> <out2.v>
      cirMgr = new CirMgr();
      cirMgr -> readCircuit(argv[1], true);
      cirMgr -> readCircuit(argv[2], false);
      cirMgr -> linkGates();
      cirMgr -> buildDfsList();
      // cirMgr -> printNetlist();
      // cirMgr -> genPattern(fileName);
      // ifstream patternFile(fileName.c_str(), ios::in);
      // cirMgr -> fileSim(patternFile);
      
      cirMgr -> randomSim();
      // cout << "====================" << endl;
      // cout << "FEC pairs before CEC" << endl;
      // cirMgr -> printFECPairs();
      // cout << "====================" << endl;
      cirMgr -> cec();
      // cout << "====================" << endl;
      // cout << "FEC pairs after CEC" << endl;
      cirMgr -> writeFECPairs("out.fec");
      // cout << "====================" << endl;
      
      cirMgr -> genAllCutList(6);
      cirMgr -> writeAllCutList("out.cut");
      cirMgr -> mapCut();
      cirMgr -> writeCircuit(argv[3], true);
      cirMgr -> writeCircuit(argv[4], false);
   }
   else {
      cerr << "Error: illegal number of argument (" << argc << ")!!\n";
   }
   return 0;
}
