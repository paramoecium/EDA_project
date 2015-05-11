#include <vector>
#include <string>
#include "cirDef.h"

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
   unsigned getFaninId(unsigned& idx) const { 
      return _faninIdList[idx]; 
   }
   CirGate* getFaninGate(unsigned& idx) const { 
      return _faninGateList[idx]; 
   }
   unsigned getFaninNum() const { return _faninIdList.size(); }
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

   // cut
   void resetCut(){ _cutIdx = 0; }
   void setCut(unsigned idx){ _cutIdx = idx; }
   bool getCutIdx() const { return _cutIdx; }

protected:
   unsigned          _id;
   string            _name;
   // fanin and fanout
   IdList            _faninIdList;
   GateList          _faninGateList;
   GateList          _fanoutGateList;
   // simulation value
   unsigned          _simVal;
   // cut
   unsigned          _cutIdx;
   mutable unsigned  _dfsFlag;
};

/******************************************/
/*   inherited class from class CirGate   */
/******************************************/
class CirPiGate: public CirGate
{
public: 
   CirPiGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Pi"; }
   void simulate();
};

class CirPoGate: public CirGate
{
public: 
   CirPoGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Po"; }
   void simulate();
};

class CirBufGate: public CirGate
{
public: 
   CirBufGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Buf"; }
   void simulate();
};

class CirInvGate: public CirGate
{
public: 
   CirInvGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Inv"; }
   void simulate();
};

class CirAndGate: public CirGate
{
public: 
   CirAndGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "And"; }
   void simulate();
};

class CirNandGate: public CirGate
{
public: 
   CirNandGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Nand"; }
   void simulate();
};

class CirOrGate: public CirGate
{
public: 
   CirOrGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Or"; }
   void simulate();
};

class CirNorGate: public CirGate
{
public: 
   CirNorGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Nor"; }
   void simulate();
};

class CirXorGate: public CirGate
{
public: 
   CirXorGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Xor"; }
   void simulate();
};

class CirXnorGate: public CirGate
{
public: 
   CirXnorGate(unsigned, const string&, const IdList&); 
   string getGateType() const { return "Xnor"; }
   void simulate();
};

