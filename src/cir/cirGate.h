#include "cirDef.h"

#include <vector>
#include <string>

using namespace std;

class CirGate
{
public:
   CirGate(unsigned, const string&, const IdList&); 

   // basic gate information
   unsigned getId() const { return _id; }
   string getName() const { return _name; }
   virtual string getGateType() const = 0;

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
};

class CirPiGate: public CirGate
{
public: 
   string getGateType() const { return "Pi"; }
   void simulate();
};

class CirPoGate: public CirGate
{
public: 
   string getGateType() const { return "Po"; }
   void simulate();
};

class CirBufGate: public CirGate
{
public: 
   string getGateType() const { return "Buf"; }
   void simulate();
};

class CirInvGate: public CirGate
{
public: 
   string getGateType() const { return "Inv"; }
   void simulate();
};

class CirAndGate: public CirGate
{
public: 
   string getGateType() const { return "And"; }
   void simulate();
};

class CirNandGate: public CirGate
{
public: 
   string getGateType() const { return "Nand"; }
   void simulate();
};

class CirOrGate: public CirGate
{
public: 
   string getGateType() const { return "Or"; }
   void simulate();
};

class CirNorGate: public CirGate
{
public: 
   string getGateType() const { return "Nor"; }
   void simulate();
};

class CirXorGate: public CirGate
{
public: 
   string getGateType() const { return "Xor"; }
   void simulate();
};

class CirXnorGate: public CirGate
{
public: 
   string getGateType() const { return "Xnor"; }
   void simulate();
};

