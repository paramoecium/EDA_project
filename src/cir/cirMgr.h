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
   void genPattern(const string& pattern);

   // combinational equivelance checking
   void cec();

   // circuit reporting
   bool inSameFecGroup(unsigned id1, unsigned id2) const ;
   bool inSameFecGroup(const CirGate* gate1, const CirGate* gate2) const ;
   void printNetlist() const ;
   void printFECPairs() const ;
   void writeFECPairs(const string& filename) const ;

   // Member functions about cut mapping
   void genAllCutList(unsigned k);
   void printAllCutList() const ;
   void writeAllCutList(const string& filename) const ;
   void mapCut();
   void nonEqAddCut();
   
   CirGate* getGateById(unsigned id) const { return _gateList[id]; }
   CirGate* getGateByName(const string& name) const {
      return getGateById(getIdByName(name)); 
   }

private:
   // gate id and name mapping
   bool     checkNameDeclared(const string& name) const ;
   void     createIdByName   (const string& name);
   unsigned getIdByName      (const string& name) const ;
   
   // used in buildDfsList
   void dfs(CirGate* gate);

   // Member function about simulation
   bool cmpFecGrp(const IdList* &fecgrp1, const IdList* &fecGrp2) const ;
   void initFec(const unsigned* v);
   bool checkFec(const unsigned* v);
   void sortFecGrp(IdList*, vector<IdList*>&);
   void simPi(const unsigned*);
   void doSimBySAT(const SatSolver& s);
   bool getPiSimFromFile(ifstream& patternFile, unsigned* v);
   //void outputSimValueToFile(unsigned);
   
   // Member function about cec
   void genProofModel(SatSolver& s);
   bool solveGateEqBySat(SatSolver& s, CirGate* g1, CirGate* g2, bool inverse);
   void solveBuf();

   // Member functions about cut mapping
   void updatePairList(CutPair cp, vector<CutPair>& pairList);
   void dfsAddCut(CirGate*);

   string                  _moduleName[2];
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
