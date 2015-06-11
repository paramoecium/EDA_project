/****************************************************************************
  FileName     [ sat.h ]
  PackageName  [ sat ]
  Synopsis     [ Define miniSat solver interface functions ]
  Author       [ Chung-Yang (Ric) Huang, Cheng-Yin Wu ]
  Copyright    [ Copyleft(c) 2010-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef SAT_H
#define SAT_H

#include <cassert>
#include <iostream>
#include "Solver.h"

using namespace std;

/********** MiniSAT_Solver **********/
class SatSolver
{
   public : 
      SatSolver():_solver(0) { }
      ~SatSolver() { }

      // Solver initialization and reset
      void initialize() {
         reset();
         if (_curVar == 0) { _solver->newVar(); ++_curVar; }
      }
      void reset() {
         if (_solver) delete _solver;
         _solver = new Solver();
         _assump.clear(); _curVar = 0;
      }

      // Constructing proof model
      // Return the Var ID of the new Var
      inline Var newVar() { _solver->newVar(); return _curVar++; }
      // fa/fb = true if it is inverted
      /*
      void addAigCNF(Var vf, Var va, bool fa, Var vb, bool fb) {
         vec<Lit> lits;
         Lit lf = Lit(vf);
         Lit la = fa? ~Lit(va): Lit(va);
         Lit lb = fb? ~Lit(vb): Lit(vb);
         lits.push(la); lits.push(~lf);
         _solver->addClause(lits); lits.clear();
         lits.push(lb); lits.push(~lf);
         _solver->addClause(lits); lits.clear();
         lits.push(~la); lits.push(~lb); lits.push(lf);
         _solver->addClause(lits); lits.clear();
      }
      */
      /**************************************/
      /************   my AddCNF   ***********/
      /**************************************/
      void addConst0CNF(Var vf, bool Const1) {
         vec<Lit> lits;
         Lit lf = Const1? Lit(vf) : ~Lit(vf);
         lits.push(lf);
         _solver->addClause(lits); lits.clear();
      }
     
      void addBufCNF(Var vf, Var va, bool Inv) {
         vec<Lit> lits;
         Lit lf = Inv? ~Lit(vf) : Lit(vf);
         Lit la = Lit(va);
         lits.push(~lf); lits.push(la);
         _solver->addClause(lits); lits.clear();
         lits.push(lf); lits.push(~la);
         _solver->addClause(lits); lits.clear();
      }
      
      // And & Nand CNF
      // vf = va & vb
      // if Nand is true, this is NandGate
      void addAndCNF(Var vf, const vector<Var>& faninVar, bool Nand) {
         vec<Lit> lits, lastLits;
         Lit lf = Nand? ~Lit(vf): Lit(vf);
         for (unsigned i=0, m=faninVar.size(); i<m; ++i){
            Lit la = Lit(faninVar[i]);
            lits.push(la); lits.push(~lf); // (~vf+va)
            _solver->addClause(lits); lits.clear();
            lastLits.push(~la);
         }
         lastLits.push(lf); //(~va+~vb+vf)
         _solver->addClause(lastLits); lastLits.clear();
      }
     
      // Or & Nor CNF
      // vf = va + vb
      // if Nand is true, this is NorGate
      void addOrCNF(Var vf, const vector<Var>& faninVar, bool Nor) {
         vec<Lit> lits, lastLits;
         Lit lf = Nor? ~Lit(vf): Lit(vf);
         for (unsigned i=0, m=faninVar.size(); i<m; ++i){
            Lit la = Lit(faninVar[i]);
            lits.push(~la); lits.push(lf); // (vf+~va)
            _solver->addClause(lits); lits.clear();
            lastLits.push(la);
         }
         lastLits.push(~lf); //(va+vb+~vf)
         _solver->addClause(lastLits); lastLits.clear();
      }
    
      // fa/fb = true if it is inverted
      // XOr & Xnor CNF
      // vf = va xor vb
      // if Xor is true, this is XnorGate
      void addXorCNF(Var vf, const vector<Var>& faninVar, bool Xnor) {
         int n = faninVar.size();
         vec<Lit> lits, tmpLit;
         Lit      la, lb, lf; // lf = la ^ lb

         assert(n > 0);
         // just in case
         if(n == 1){
            addBufCNF(vf, faninVar[0], Xnor);
            return;
         }
         tmpLit.push(Lit(faninVar[0]));
         for(int i=1; i<n-1; ++i) tmpLit.push(Lit(newVar()));
         tmpLit.push(Xnor? ~Lit(vf) : Lit(vf));
         assert(tmpLit.size() == n);
         for(int i=1; i<n; ++i){
            la = tmpLit[i-1];
            lb = Lit(faninVar[i]);
            lf = tmpLit[i];
            lits.push(~la); lits.push(lb); lits.push(lf);
            _solver->addClause(lits); lits.clear();
            lits.push(la); lits.push(~lb); lits.push(lf);
            _solver->addClause(lits); lits.clear();
            lits.push(la); lits.push(lb); lits.push(~lf);
            _solver->addClause(lits); lits.clear();
            lits.push(~la); lits.push(~lb); lits.push(~lf);
            _solver->addClause(lits); lits.clear();
         }
      }
     
      // For incremental proof, use "assumeSolve()"
      void assumeRelease() { _assump.clear(); }
      void assumeProperty(Var prop, bool val) {
         _assump.push(val? Lit(prop): ~Lit(prop));
      }
      bool assumpSolve() { return _solver->solve(_assump); }

      // For one time proof, use "solve"
      void assertProperty(Var prop, bool val) {
         _solver->addUnit(val? Lit(prop): ~Lit(prop));
      }
      bool solve() { _solver->solve(); return _solver->okay(); }

      // Functions about Reporting
      // Return 1/0/-1; -1 means unknown value
      int getValue(Var v) const {
         return (_solver->modelValue(v)==l_True?1:
                (_solver->modelValue(v)==l_False?0:-1)); }
      void printStats() const { const_cast<Solver*>(_solver)->printStats(); }

   private : 
      Solver           *_solver;    // Pointer to a Minisat solver
      Var               _curVar;    // Variable currently
      vec<Lit>          _assump;    // Assumption List for assumption solve
};

#endif  // SAT_H

