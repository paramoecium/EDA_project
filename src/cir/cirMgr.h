#ifndef _CIRMGR_H_
#define _CIRMGR_H_

#include <map>
#include <string>
#include <vector>
#include "cirDef.h"

using namespace std;

class SatSolver;

/********************/
/*   class CirMgr   */
/********************/
class CirMgr
{
public:
   CirMgr();

   CirGate* getGateById(unsigned id) const {
      return _gateList[id];
   }
   // read input file
   bool readCircuit(const string&, bool);

   // create gates
   void createPI(const string&);
   void createPO(const string&);
   void createGate(GateType, const string&, const vector<string>&);
   // create netlist
   void linkGates();
   void buildDfsList();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void genPattern(const string& pattern, const unsigned&);
   void fraig();

   // circuit reporting
   void printNetlist() const;
   void printFECPairs() const;
   

private:
   void dfs(CirGate*);
   unsigned getIdByName(const string& name);

   GateList                _gateList;
   GateList                _dfsList;
   IdList                  _piList;
   IdList                  _poList;
   // variable map
   map<string, unsigned>   _varMap;
   unsigned                _varNum;
   vector<IdList*>         _fecGrps; 

   void genProofModel(SatSolver&);
   bool solveGateEqBySat(SatSolver&, CirGate*, CirGate*,bool);

   // Member function about simulation
   void initFec(const unsigned*, const unsigned&);
   bool checkFec(const unsigned*, const unsigned&);
   void sortFecGrp(IdList*, vector<IdList*>&);
   void simPi(const unsigned*);
   bool getPiSimFromFile(ifstream&, unsigned*, unsigned&);
   void doSimBySAT(const SatSolver&);
   //void outputSimValueToFile(unsigned);

};

#endif
