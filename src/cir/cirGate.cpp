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
CirGate::CirGate(unsigned id, const string& name, 
                 const IdList& faninIdList): 
   _id(id), _name(name), _faninIdList(faninIdList) {}

CirPiGate::CirPiGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirBufGate::CirBufGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirInvGate::CirInvGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirAndGate::CirAndGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirNandGate::CirNandGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirOrGate::CirOrGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirNorGate::CirNorGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirXorGate::CirXorGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

CirXnorGate::CirXnorGate(unsigned id, const string& name, 
                         const IdList& faninIdList): 
   CirGate(id, name, faninIdList){}

// simulation
void
CirPiGate::simulate(){
   _simVal = rand();
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

// print function
void
CirGate::printGate() const{
   cout << getGateType() << " ";
   for(unsigned i=0, m=_faninIdList.size(); i<m; ++i)
      cout << _faninIdList[i] << " ";
   cout << endl;
}
