#include "cirMgr.h"
#include "cirGate.h"

/************************/
/*   global variables   */
/************************/
CirMgr* cirMgr;

/*************************************/
/*   class CirMgr public functions   */
/*************************************/
CirMgr::CirMgr(): _varNum(0) {}

void
CirMgr::createGate(GateType type, const string& name,
                   const vector<string>& input){
   CirGate* gate = NULL;
   unsigned id = getIdByName(name);
   IdList faninList;
   for(unsigned i=0; i<input.size(); ++i)
      faninList.push_back(getIdByName(input[i]));
   
   switch(type){
      case GATE_PI:
         gate = new CirPiGate(id, name, faninList);
         break;
      case GATE_PO:
         gate = new CirPoGate(id, name, faninList);
         break;
      case GATE_BUF:
         gate = new CirBufGate(id, name, faninList);
         break;
      case GATE_INV:
         gate = new CirInvGate(id, name, faninList);
         break;
      case GATE_AND:
         gate = new CirAndGate(id, name, faninList);
         break;
      case GATE_NAND:
         gate = new CirNandGate(id, name, faninList);
         break;
      case GATE_OR:
         gate = new CirOrGate(id, name, faninList);
         break;
      case GATE_NOR:
         gate = new CirNorGate(id, name, faninList);
         break;
      case GATE_XOR:
         gate = new CirXorGate(id, name, faninList);
         break;
      case GATE_XNOR:
         gate = new CirXnorGate(id, name, faninList);
         break;
      default: break;
   }
   if(_gateList.size() <= id) _gateList.resize(id+1);
   _gateList[id] = gate;
}

void
CirMgr::linkGates(){
   for(unsigned i=0, n=_gateList.size(); i<n; ++i){
      CirGate* gate = _gateList[i];
      for(unsigned j=0, m=gate->getFaninNum(); j<m; ++j){
         CirGate* in = getGateById(gate->getFaninId(j));
         gate->addFanin(in);
         in->addFanout(gate);
      }
   }
}

void
CirMgr::buildDfsList(){
}

/**************************************/
/*   class CirMgr private functions   */
/**************************************/
unsigned
CirMgr::getIdByName(const string& name){
   if(_varMap.find(name) == _varMap.end())
      _varMap[name] = _varNum++;
   return _varMap[name];
}
