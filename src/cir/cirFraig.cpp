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
#include "sat.h"
#include "myHashMap.h"
#include "util.h"


using namespace std;

// 1.Create a new dfslist save id in dfs order.
//   Note that const 0 is the first one of the list.
// 2.Optimize circuit usually to solve less SAT problem.
// 3.Remember to check whether the gate of that id is exit or not.
// 4.When every time get SAT, update dfs list (otherwise simulation will crash)
//   do optimization and swepp again, and simultaion by the pattern from SatSolver.
// 5.Whem get UNSAT, merge the gate.
// 6.Final, remember to update all information about circuit.
void
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
//   bool result, inv = false;

   //result = solveGateEqBySat(solver, g1, g2, inv);
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

// convert all aigGates in dfs to clauses and put in satSolver
void
CirMgr::genProofModel(SatSolver& s)
{
   /*
   Var zero = s.newVar();
   getGate(0)->setVar(zero);
   for (size_t i = 0; i < _dfsOrder.size(); ++i){
      if (_dfsOrder[i]->getId() == 0) continue;
      Var v = s.newVar();
      _dfsOrder[i]->setVar(v);
   }
   for (size_t i = 0; i < _dfsOrder.size(); ++i){
      if (_dfsOrder[i]->getType() != AIG_GATE) continue;
      CirGate*& g = _dfsOrder[i];
      s.addAigCNF(g->getVar(), g->getFanin(0).gate()->getVar(), g->getFanin(0).isInv(), 
                  g->getFanin(1).gate()->getVar(), g->getFanin(1).isInv());
   }
   zero = s.newVar();
   s.addAigCNF(getGate(0)->getVar(), zero, false, zero, true);
   */
}

// Solve whether two gates are fuctionally equivalent
// If they are equivalent, return true 
// else return false
bool
CirMgr::solveGateEqBySat(SatSolver& s, CirGate* g1, CirGate* g2, bool inverse){
  
   Var newV = s.newVar();
   /*
   s.addXorCNF(newV, g1->getVar(), false, g2->getVar(), inverse);
   s.assumeRelease();  // Clear assumptions
   s.assumeProperty(newV, true);
   */
   return !s.assumpSolve();
}
