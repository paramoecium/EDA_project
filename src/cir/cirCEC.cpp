/****************************************************************************
  FileName     [ cirCEC.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir CEC functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "myHashMap.h"
#include "util.h"


using namespace std;

void
CirMgr::cec()
{
   solveBuf();
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
   for (int i=0, m=_fecGrps.size(); i<m; ++i){
      for (unsigned j=1, n=_fecGrps[i]->size(); j<n; ++j){
         unsigned g1Num = _fecGrps[i]->at(0);
         unsigned g2Num = _fecGrps[i]->at(j);
         CirGate* g1 = getGateById(g1Num/2);
         CirGate* g2 = getGateById(g2Num/2);
         assert(g1 != 0 && g2 != 0);
         // already solve g1 == g2;
         if (g2->getEqGate() == g1)
            continue;
         bool inv = ((g1Num%2) ^ (g2Num%2));
         cout << "solving " << g1->getId() << " with " << g2->getId() << "......";
         if (solveGateEqBySat(solver, g1, g2, inv)){
            cout << "Equivalence!!" << endl;
            g2->setEqGate(g1);
         }
         else{
            cout << "UnEquivalence!!" << endl;
            doSimBySAT(solver);
            i = -1;
            break;
         }
      }
      // finish solving one group
   }
}

/************************************************/
/*   Private member functions about solEqGate   */
/************************************************/

// convert all aigGates in dfs to clauses and put in satSolver
void
CirMgr::genProofModel(SatSolver& s)
{
   for (unsigned i=0, m=_dfsList.size(); i<m; ++i){
      Var v = s.newVar();
      _dfsList[i]->setVar(v);
   }
   for (unsigned i=0, m=_dfsList.size(); i<m; ++i)
      _dfsList[i]->genCNF(s);
}

// Solve whether two gates are fuctionally equivalent
// If they are equivalent, return true 
// else return false
bool
CirMgr::solveGateEqBySat(SatSolver& s, CirGate* g1, CirGate* g2, bool inverse){
   Var newV = s.newVar();
   vector<Var> vars;
   vars.push_back(g1->getVar());
   vars.push_back(g2->getVar());
   s.addXorCNF(newV, vars, inverse);
   s.assumeRelease();  // Clear assumptions
   s.assumeProperty(newV, true);
   return !s.assumpSolve();
}

void 
CirMgr::doSimBySAT(const SatSolver& s)
{
   unsigned* simValue = new unsigned[_piList.size()];
   for (unsigned i = 0, m=_piList.size(); i<m; ++i){
      int temp = s.getValue(getGateById(_piList[i])->getVar());
      if (temp == -1) temp = 0;
      simValue[i] = temp + (rand() & ~1u);
   }
   checkFec(simValue, 32);
   delete[] simValue;
}

void
CirMgr::solveBuf()
{
   for (unsigned i=0, m=_dfsList.size(); i<m; ++i){
      string type = _dfsList[i]->getGateType();
      if (type=="buf" || type=="not"){
         CirGate* fanin = _dfsList[i]->getFaninGate(0);
         _dfsList[i]->setEqGate(fanin);
      }
   }
}
