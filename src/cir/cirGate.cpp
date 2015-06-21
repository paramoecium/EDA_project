#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "cirGate.h"
#include "cirMgr.h"

using namespace std;
/************************/
/*   extern variables   */
/************************/
extern CirMgr* cirMgr;
extern BddMgr* bddMgr;

/************************/
/*   static variables   */
/************************/
static unsigned dfsFlag = 0;

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

void
CirGate::incDfsFlag(){ ++dfsFlag; }

bool 
CirGate::isMark() const { return (_dfsFlag == dfsFlag); }

void 
CirGate::mark() { _dfsFlag = dfsFlag; }

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
   bool addRoot = (_fecGrp != 0 && _fanoutGateList.size() <= 1);
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

void
CirBufGate::genGateFunc(){
   if(isMark()) return;
   mark();
   _faninGateList[0]->genGateFunc();
   _tmpFunc = _faninGateList[0]->getGateFunc();
   if(isInv())
      _tmpFunc = ~_tmpFunc;
}

void
CirOrGate::genGateFunc(){
   if(isMark()) return;
   mark();
   _tmpFunc = ~bddMgr->getSupport(0);
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      _faninGateList[i]->genGateFunc();
      _tmpFunc |= _faninGateList[i]->getGateFunc();
   }
   if(isInv())
      _tmpFunc = ~_tmpFunc;
}

void
CirAndGate::genGateFunc(){
   if(isMark()) return;
   mark();
   _tmpFunc = bddMgr->getSupport(0);
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      _faninGateList[i]->genGateFunc();
      _tmpFunc &= _faninGateList[i]->getGateFunc();
   }
   if(isInv())
      _tmpFunc = ~_tmpFunc;
}

void
CirXorGate::genGateFunc(){
   if(isMark()) return;
   mark();
   _tmpFunc = ~bddMgr->getSupport(0);
   for(unsigned i=0, n=_faninGateList.size(); i<n; ++i){
      _faninGateList[i]->genGateFunc();
      _tmpFunc ^= _faninGateList[i]->getGateFunc();
   }
   if(isInv())
      _tmpFunc = ~_tmpFunc;
}

bool 
CirGate::getMatchCut(const CirCutList& cutList2, unsigned root2, CirCut*& retCut1, CirCut*& retCut2)
{
   CirGate* gate2 = cirMgr->getGateById(root2);
   CirCut* cut1;
   const CirCut* cut2;
   unsigned cutList1Size = _cutList.size();
   unsigned cut1Size;
   bool matchLeaf;
   bool ret = false;
   
   assert(cirMgr->inSameFecGroup(this, gate2));
   
   for(unsigned i=0; i<cutList1Size && !ret; ++i){
      cut1 = _cutList[i];
      cut1Size = cut1->size();
      // size of 1 is not allowed
      if(cut1Size == 1) continue;
      unsigned* leaf = new unsigned[cut1Size];
      unsigned* perm = new unsigned[cut1Size];
      cut1->genCutFunc();
      for(unsigned j=0; j<cut1Size && !ret; ++j){
         cut2 = cutList2[j];
         /*
         cout << "-------------" << endl;
         cout << "checking cut" << endl;
         cout << *cut1 << endl;
         cout << *cut2 << endl;
         cout << "-------------" << endl;
         */
         if(cut2->size() != cut1Size) continue;
         for(unsigned p=0; p<cut1Size; ++p) perm[p] = p;
         do{
            matchLeaf = true;
            for(unsigned k=0; k<cut1Size; ++k){
               if(!cirMgr->inSameFecGroup(cut1->getLeaf(k), cut2->getLeaf(perm[k]))){
                  matchLeaf = false;
                  break;
               }
               leaf[k] = cut2->getLeaf(perm[k]);
            }
            if(matchLeaf){
               CirCut* cut = new CirCut(leaf, cut1Size);
               cut->setRoot(root2);
               cut->genCutFunc();
               if(cut1->getFunc() == cut->getFunc()){
                  retCut1 = new CirCut(*cut1);
                  retCut2 = new CirCut(*cut);
                  ret = true;
               }
               delete cut;
            }
         } while(next_permutation(perm, perm+cut1Size) && !ret);
      }
      delete [] leaf;
      delete [] perm;
   }
   return ret;
}
