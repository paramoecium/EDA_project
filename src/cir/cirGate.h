#include <vector>
#include <string>
#include "cirDef.h"
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
   CirGate(unsigned, const string&, const IdList&); 

   // basic gate information
   unsigned getId() const { return _id; }
   string getName() const { return _name; }
   void setPi(){ _isPi = true; }
   void setPo(){ _isPo = true; }
   bool isPi() const { return _isPi; }
   bool isPo() const { return _isPo; }
   unsigned getFaninSize() const { return _faninIdList.size(); }
   unsigned getFaninId(unsigned idx) const { return _faninIdList[idx]; }
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
   virtual void simulate() = 0;
   virtual unsigned getSimOutput() const { return _simVal; }
   void setFecGrp(IdList* p) { _fecGrp=p; }

   // SAT solver
   virtual void genCNF(SatSolver&) = 0;
   void setVar(const Var& v) { _var=v; }
   Var getVar() const { return _var; }

   // cut
   void resetCut(){ _cutIdx = 0; }
   void setCut(unsigned idx){ _cutIdx = idx; }
   bool getCutIdx() const { return _cutIdx; }

protected:
   // bosic information
   unsigned          _id;
   string            _name;
   bool              _isPi, _isPo;
   
   // fanin and fanout
   IdList            _faninIdList;
   GateList          _faninGateList;
   GateList          _fanoutGateList;
   
   // simulation value
   unsigned          _simVal;
   IdList*           _fecGrp;
   
   // SAT solver
   Var               _var;
   
   // cut
   unsigned          _cutIdx;
   mutable unsigned  _dfsFlag;
};

/******************************************/
/*   inherited class from class CirGate   */
/******************************************/
class CirConstGate: public CirGate
{
public:
   CirConstGate(unsigned, const string&, const IdList&);
   string getGateType() const { return "const"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirPiGate: public CirGate
{
public: 
   CirPiGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "pi"; }
   void simulate() {}
   void simulate(unsigned);
   void genCNF(SatSolver&);
};

class CirBufGate: public CirGate
{
public: 
   CirBufGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "buf"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirNotGate: public CirGate
{
public: 
   CirNotGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "not"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirAndGate: public CirGate
{
public: 
   CirAndGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "and"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirNandGate: public CirGate
{
public: 
   CirNandGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "nand"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirOrGate: public CirGate
{
public: 
   CirOrGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "or"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirNorGate: public CirGate
{
public: 
   CirNorGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "nor"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirXorGate: public CirGate
{
public: 
   CirXorGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "xor"; }
   void simulate();
   void genCNF(SatSolver&);
};

class CirXnorGate: public CirGate
{
public: 
   CirXnorGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "xnor"; }
   void simulate();
   void genCNF(SatSolver&);
};

