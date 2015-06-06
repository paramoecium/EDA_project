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

   // read input file
   bool readCircuit (const string& filename, bool design);
   bool writeCircuit(const string& filename, bool design) const ;

   // create gates
   void createPI(const string& name);
   void createPO(const string& name);
   void createGate(GateType type, const string& name, const vector<string>& input);
   
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
   
   CirGate* getGateById(unsigned id) const { return _gateList[id]; }

private:
   // gate id and name mapping
   bool     checkNameDeclared(const string& name) const;
   void     createIdByName(const string& name);
   unsigned getIdByName(const string& name);
   
   // used in buildDfsList
   void dfs(CirGate* gate);

   // Member function about simulation
   void initFec(const unsigned*, const unsigned&);
   bool checkFec(const unsigned*, const unsigned&);
   void sortFecGrp(IdList*, vector<IdList*>&);
   void simPi(const unsigned*);
   void doSimBySAT(const SatSolver&);
   bool getPiSimFromFile(ifstream&, unsigned*, unsigned&);
   //void outputSimValueToFile(unsigned);
   
   void genProofModel(SatSolver&);
   bool solveGateEqBySat(SatSolver&, CirGate*, CirGate*,bool);
   
   vector<string>          _ioNameList[2];
   GateList                _gateList;
   GateList                _dfsList;
   IdList                  _piList;
   IdList                  _poList;
   // variable map
   map<string, unsigned>   _varMap;
   unsigned                _varNum;
   vector<IdList*>         _fecGrps; 

};

#endif
