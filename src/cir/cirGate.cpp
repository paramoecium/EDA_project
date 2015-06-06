#include <cstdlib>
#include <iostream>
#include "cirGate.h"
#include "cirMgr.h"

using namespace std;
/************************/
/*   extern variables   */
/************************/
extern CirMgr* cirMgr;

/**************************************/
/*   class CirGate public functions   */
/**************************************/
// constructor
CirGate::CirGate(unsigned id, const string& name, const IdList& faninIdList): 
   _id(id), _name(name), _isPi(false), _isPo(false),_faninIdList(faninIdList) {}

CirConstGate::CirConstGate(unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirPiGate::CirPiGate      (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirBufGate::CirBufGate    (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirNotGate::CirNotGate    (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirAndGate::CirAndGate    (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirNandGate::CirNandGate  (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirOrGate::CirOrGate      (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirNorGate::CirNorGate    (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirXorGate::CirXorGate    (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

CirXnorGate::CirXnorGate  (unsigned id, const string& name, const IdList& faninIdList): 
   CirGate(id, name, faninIdList) {}

// simulation
void
CirConstGate::simulate(){
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
}

void 
CirNotGate::simulate(){ 
   CirGate* gate = _faninGateList[0];
   _simVal = ~gate->getSimOutput(); 
}

void 
CirAndGate::simulate(){ 
   _simVal = ~0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal &= gate->getSimOutput(); 
   }
}
void 
CirNandGate::simulate(){ 
   _simVal = ~0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal &= gate->getSimOutput();
   }
   _simVal = ~_simVal;
}

void 
CirOrGate::simulate(){ 
   _simVal = 0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal |= gate->getSimOutput(); 
   }
}

void 
CirNorGate::simulate(){ 
   _simVal = 0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal |= gate->getSimOutput(); 
   }
   _simVal = ~_simVal;
}

void 
CirXorGate::simulate(){ 
   _simVal = 0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal ^= gate->getSimOutput();
   }
}

void 
CirXnorGate::simulate(){ 
   _simVal = 0u;
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      CirGate* gate = _faninGateList[i];
      _simVal ^= gate->getSimOutput(); 
   }
   _simVal = ~_simVal;
}

// generate CNF clause
void 
CirConstGate::genCNF(SatSolver& s){}
void 
CirPiGate::genCNF(SatSolver& s){}
void 
CirBufGate::genCNF(SatSolver& s){}
void 
CirNotGate::genCNF(SatSolver& s){}
void 
CirAndGate::genCNF(SatSolver& s){
   //for (unsigned i=0, m=_faninGateList.size(); i<m; ++i)
   s.addAigCNF(_var, _faninGateList[0]->getVar(), true, 
                     _faninGateList[1]->getVar(), true);
}
void 
CirNandGate::genCNF(SatSolver& s){}
void 
CirOrGate::genCNF(SatSolver& s){}
void 
CirNorGate::genCNF(SatSolver& s){}
void 
CirXorGate::genCNF(SatSolver& s){}
void 
CirXnorGate::genCNF(SatSolver& s){}

// print function
void
CirGate::printGate() const {
   cout << getGateType() << " ";
   for(unsigned i=0, n=_faninIdList.size(); i<n; ++i)
      cout << _faninIdList[i] << " ";
   cout << endl;
}
