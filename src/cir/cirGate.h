#include <vector>
#include <string>
#include "cirDef.h"
#include "cirCut.h"
#include "bddMgr.h"
#include "sat.h"

using namespace std;

/************************/
/*   extern variables   */
/************************/
extern unsigned dfsFlag;

/*********************/
/*   class CirGate   */
/*********************/
class CirGate
{
public:
   CirGate(unsigned id, const string& name, const IdList& faninIdList, bool inv); 

   // basic gate information
   unsigned getId() const { return _id; }
   string getName() const { return _name; }
   void setPi(){ _isPi = true; }
   void setPo(){ _isPo = true; }
   bool isPi()  const { return _isPi; }
   bool isPo()  const { return _isPo; }
   bool isInv() const { return _inv;  }
   unsigned getFaninSize()             const { return _faninIdList.size(); }
   unsigned getFaninId  (unsigned idx) const { return _faninIdList[idx]; }
   CirGate* getFaninGate(unsigned idx) const { return _faninGateList[idx]; }
   
   virtual string getGateType() const = 0;

   //dfs
   bool isMark() const { return (_dfsFlag == dfsFlag); }
   void mark() { _dfsFlag = dfsFlag; }

   // print function
   virtual void printGate() const;

   // set gate information
   void addFanin(CirGate* g) { _faninGateList.push_back(g); }
   void addFanout(CirGate* g) { _fanoutGateList.push_back(g); }

   // simulation
   virtual void     simulate() = 0;
   virtual unsigned getSimOutput() const { return _simVal; }
   void             setFecGrp(IdList* p) { _fecGrp=p; }

   // about solve SAT 
   virtual void genCNF(SatSolver&) = 0;
   void         setVar(Var v) { _var=v; }
   Var          getVar() const { return _var; }
   void         setEqGate(CirGate* g) { _eqGate = g; }
   CirGate*     getEqGate() const { return _eqGate; }

   // Methods about cut
	virtual void genCutList();
	CirCutList&  getCutList(){ return _cutList; }
   void         setGateFunc(BddNode);
	virtual void genGateFunc();
	BddNode      getGateFunc() const { return _tmpFunc; }

protected:
   // bosic information
   unsigned          _id;
   string            _name;
   bool              _isPi, _isPo;
   bool              _inv;
   
   // fanin and fanout
   IdList            _faninIdList;
   GateList          _faninGateList;
   GateList          _fanoutGateList;
   
   // simulation value
   unsigned          _simVal;
   IdList*           _fecGrp;
   CirGate*          _eqGate;
   
   // SAT solver
   Var               _var;

   // cut
   CirCutList        _cutList;
   BddNode           _tmpFunc;

   // dfs
   mutable unsigned  _dfsFlag;
};

/******************************************/
/*   inherited class from class CirGate   */
/******************************************/
class CirConst0Gate: public CirGate
{
public:
   CirConst0Gate(unsigned, const string&, const IdList&, bool inv);
   string getGateType() const { return isInv()? "const1" : "const0"; }
   void simulate();
   void genCNF(SatSolver&);
	// Methods about cut
	void genCutList();
	void genGateFunc();
};

class CirPiGate: public CirGate
{
public: 
   CirPiGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "pi"; }
   void simulate() {}
   void simulate(unsigned);
   void genCNF(SatSolver&);
	// Methods about cut
	// void genGateFunc();
};

class CirBufGate: public CirGate
{
public: 
   CirBufGate(unsigned, const string&, const IdList&, bool inv); 
   string getGateType() const { return isInv()? "not" : "buf"; }
   void simulate();
   void genCNF(SatSolver&);
	// Methods about cut
	// void genGateFunc();
};

class CirAndGate: public CirGate
{
public: 
   CirAndGate(unsigned, const string&, const IdList&, bool inv); 
   string getGateType() const { return isInv()? "nand" : "and"; }
   void simulate();
   void genCNF(SatSolver&);
	// Methods about cut
	// void genGateFunc();
};

class CirOrGate: public CirGate
{
public: 
   CirOrGate(unsigned, const string&, const IdList&, bool inv); 
   string getGateType() const { return isInv()? "nor" : "or"; }
   void simulate();
   void genCNF(SatSolver&);
	// Methods about cut
	// void genGateFunc();
};

class CirXorGate: public CirGate
{
public: 
   CirXorGate(unsigned, const string&, const IdList&, bool inv); 
   string getGateType() const { return isInv()? "xnor" : "xor"; }
   void simulate();
   void genCNF(SatSolver&);
	// Methods about cut
	// void genGateFunc();
};

