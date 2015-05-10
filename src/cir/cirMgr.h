#ifndef _CIRMGR_H_
#define _CIRMGR_H_

#include <map>
#include <string>
#include "cirDef.h"

using namespace std;

/********************/
/*   class CirMgr   */
/********************/
class CirMgr
{
public:
   CirMgr();

   CirGate* getGateById(unsigned id) const {
      return _gateList[id];
   }
   // read input file
   bool readCircuit(const string&);

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

#endif
