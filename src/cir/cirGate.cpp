#include <cstdlib>
#include "cirGate.h"
#include "cirMgr.h"

/************************/
/*   extern variables   */
/************************/
extern CirMgr* cirMgr;

/**************************************/
/*   Class CirGate public functions   */
/**************************************/
CirGate::CirGate(unsigned id, const string& name, 
                 const IdList& faninIdList): 
   _id(id), _name(name), _faninIdList(faninIdList) {}

/****************************************/
/*   inherited class public functions   */
/****************************************/
// simulation
void
CirPiGate::simulate(){
   _simVal = rand();
}

void
CirPoGate::simulate(){
   CirGate* gate = _faninGateList[0];
   _simVal = gate->getSimOutput();
}
void 
CirBufGate::simulate(){ 
   CirGate* gate = _faninGateList[0];
   _simVal = gate->getSimOutput(); 
}

void 
CirInvGate::simulate(){ 
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
