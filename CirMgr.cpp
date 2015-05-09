#include "CirMgr.h"

CirMgr* cirMgr;

unsigned
CirMgr::getIdByName(const string& name){
   if(_varMap.find(name) == _varMap.end())
      _varMap[name] = _varNum++;
   return _varMap[name];
}

void
CirMgr::createGate(GateType type, const string& output,
                   const vector<string>& input){
   
}

void
CirMgr::linkGates(){
}

void
CirMgr::buildDfsList(){
}
