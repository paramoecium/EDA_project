#include <cstdlib>
#include <iostream>
#include "cirGate.h"
#include "cirMgr.h"

using namespace std;
/************************/
/*   extern variables   */
/************************/
extern CirMgr* cirMgr;
extern BddMgr* bddMgr;

/**************************************/
/*   class CirGate public functions   */
/**************************************/
// constructor
CirGate::CirGate(unsigned id, const string& name, const IdList& faninIdList, bool inv): 
   _id(id), _name(name), _isPi(false), _isPo(false), _inv(inv),
   _faninIdList(faninIdList), _fecGrp(NULL), _eqGate(NULL), _dfsFlag(0) {}

CirConst0Gate::CirConst0Gate(unsigned id, const string& name, const IdList& faninIdList, bool inv): 
   CirGate(id, name, faninIdList, inv) {}

CirPiGate::CirPiGate(unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList, false) {}

CirBufGate::CirBufGate(unsigned id, const string& name, const IdList& faninIdList, bool inv): 
   CirGate(id, name, faninIdList, inv) {}

CirAndGate::CirAndGate(unsigned id, const string& name, const IdList& faninIdList, bool inv): 
   CirGate(id, name, faninIdList, inv) {}

CirOrGate::CirOrGate(unsigned id, const string& name, const IdList& faninIdList, bool inv): 
   CirGate(id, name, faninIdList, inv) {}

CirXorGate::CirXorGate(unsigned id, const string& name, const IdList& faninIdList, bool inv): 
   CirGate(id, name, faninIdList, inv) {}

// print function
void
CirGate::printGate() const {
   cout << getGateType() << " ";
   for(unsigned i=0, n=_faninIdList.size(); i<n; ++i)
      cout << _faninIdList[i] << " ";
   cout << endl;
}

// simulation
void
CirConst0Gate::simulate(){
   _simVal = 0u;
}

void
CirPiGate::simulate(unsigned val){
   _simVal = val;
}

void 
CirBufGate::simulate(){ 
   CirGate* gate = _faninGateList[0];
   _simVal = gate->getSimOutput(); 
   if(isInv()) _simVal = ~_simVal;
}

void 
CirAndGate::simulate(){ 
   _simVal = ~0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal &= gate->getSimOutput(); 
   }
   if(isInv()) _simVal = ~_simVal;
}

void 
CirOrGate::simulate(){ 
   _simVal = 0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal |= gate->getSimOutput(); 
   }
   if(isInv()) _simVal = ~_simVal;
}

void 
CirXorGate::simulate(){ 
   _simVal = 0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal ^= gate->getSimOutput();
   }
   if(isInv()) _simVal = ~_simVal;
}

// generate CNF clause
void 
CirConst0Gate::genCNF(SatSolver& s){
   s.addConst0CNF(getVar(), isInv());
}

void
CirPiGate::genCNF(SatSolver& s){
}

void 
CirBufGate::genCNF(SatSolver& s){
   s.addBufCNF(getVar(), _faninGateList[0]->getVar(), isInv());
}

void 
CirAndGate::genCNF(SatSolver& s){
   vector<Var> faninVar;
   for (unsigned i=0, m=_faninGateList.size(); i<m; ++i)
      faninVar.push_back(_faninGateList[i]->getVar());
   s.addAndCNF(getVar(), faninVar, isInv());
}

void 
CirOrGate::genCNF(SatSolver& s){
   vector<Var> faninVar;
   for (unsigned i=0, m=_faninGateList.size(); i<m; ++i)
      faninVar.push_back(_faninGateList[i]->getVar());
   s.addOrCNF(getVar(), faninVar, isInv());
}

void 
CirXorGate::genCNF(SatSolver& s){
   vector<Var> faninVar;
   for (unsigned i=0, m=_faninGateList.size(); i<m; ++i)
      faninVar.push_back(_faninGateList[i]->getVar());
   s.addXorCNF(getVar(), faninVar, isInv());
}

// genCutList
void
CirGate::genCutList(){
   bool addRoot = (_fecGrp != 0);
   CirCutList tmpCutList;
   tmpCutList.genCutList(_faninGateList[0]->getCutList(), _id, false);
   for(unsigned i=1, n=_faninGateList.size(); i<n; ++i){
      _cutList = tmpCutList;
      tmpCutList.genCutList(_cutList, _faninGateList[i]->getCutList(), _id, false);
   }
   _cutList.genCutList(tmpCutList, _id, addRoot);
}

void
CirConst0Gate::genCutList(){
   _cutList.genCutList(_id, false);
}

void
CirPiGate::genCutList(){
   _cutList.genCutList(_id, false);
}

void
CirGate::setGateFunc(BddNode func){
   mark();
   _tmpFunc = func;
}

// genGateFunc
void
CirGate::genGateFunc(){
   if(isMark()) return;
   mark();
   _tmpFunc = bddMgr->getSupport(0);
}

void
CirConst0Gate::genGateFunc(){
   if(isMark()) return;
   mark();
   _tmpFunc = bddMgr->getSupport(0); 
}

void
CirPiGate::genGateFunc(){
   return;
}

bool 
CirGate::getMatchCut(const CirCutList& cutList2, unsigned root2, CirCut*& cut1, CirCut*& cut2)
{

   return false;
}
