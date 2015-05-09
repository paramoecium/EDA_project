#ifndef _CIRDEF_H_
#define _CIRDEF_H_

#include <vector>

class CirGate;

typedef std::vector<CirGate*> GateList;
typedef std::vector<unsigned> IdList;

enum GateType{
   GATE_PI,
   GATE_PO,
   GATE_BUF, 
   GATE_INV,
   GATE_AND,
   GATE_NAND,
   GATE_OR,
   GATE_NOR,
   GATE_XOR,
   GATE_XNOR,

   // dummy end
   GATE_END
};

#endif
