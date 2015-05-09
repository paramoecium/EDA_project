#include <map>
#include <string>

#include "CirDef.h"

using namespace std;

class CirMgr
{
public:
   CirGate* getGateById(unsigned id) const {
      return _gateList[id];
   }

   // create gates
   void createGate(GateType, const string&, 
                   const vector<string>&);
   
   // create netlist
   void linkGates();
   void buildDfsList();

private:
   unsigned getIdByName(const string& name);

   GateList                _gateList;
   GateList                _dfsList;
   // variable map
   map<string, unsigned>   _varMap;
   unsigned                _varNum;

};
