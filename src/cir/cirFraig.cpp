/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
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
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();
   //genProofModel(solver);
//   bool result, inv = false;
   // generate proof model (set CNF clause)
   for (unsigned i = 0, m=_dfsList.size(); i < m; ++i){
      Var v = solver.newVar();
      _dfsList[i]->setVar(v);
   }
   for (unsigned i = 0; i < _dfsList.size(); ++i)
      _dfsList[i]->genCNF(solver);

   //result = solveGateEqBySat(solver, g1, g2, inv);
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

// convert all aigGates in dfs to clauses and put in satSolver
void
CirMgr::genProofModel(SatSolver& s)
{
   //Var zero = s.newVar();
   //getGate(0)->setVar(zero);
   for (unsigned i = 0, m=_dfsList.size(); i < m; ++i){
      //if (_dfsOrder[i]->getId() == 0) continue;
      Var v = s.newVar();
      _dfsList[i]->setVar(v);
   }
   for (unsigned i = 0; i < _dfsList.size(); ++i){
      //if (_dfsOrder[i]->getType() != AIG_GATE) continue;
      _dfsList[i]->genCNF(s);
//      CirGate*& g = _dfsList[i];
//      s.addAigCNF(g->getVar(), g->getFanin(0).gate()->getVar(), g->getFanin(0).isInv(), 
//                  g->getFanin(1).gate()->getVar(), g->getFanin(1).isInv());
   }
   //zero = s.newVar();
   //s.addAigCNF(getGate(0)->getVar(), zero, false, zero, true);
}

// Solve whether two gates are fuctionally equivalent
// If they are equivalent, return true 
// else return false
bool
CirMgr::solveGateEqBySat(SatSolver& s, CirGate* g1, CirGate* g2, bool inverse){
  
   Var newV = s.newVar();
   s.addXorCNF(newV, g1->getVar(), false, g2->getVar(), inverse);
   s.assumeRelease();  // Clear assumptions
   s.assumeProperty(newV, true);
   return !s.assumpSolve();
}
