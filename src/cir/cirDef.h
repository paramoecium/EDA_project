#ifndef _CIRDEF_H_
#define _CIRDEF_H_

#include <vector>
#include <string>

class CirGate;

typedef std::vector<CirGate*>    GateList;
typedef std::vector<unsigned>    IdList;
typedef std::vector<std::string> StrList;

enum GateType{
   GATE_CONST0,
   GATE_CONST1,
   GATE_PI,
   GATE_BUF, 
   GATE_NOT,
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
