#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

/************************/
/*   global variables   */
/************************/
CirMgr* cirMgr;
unsigned dfsFlag = 0;

/**************************************/
/*   static variables and functions   */
/**************************************/
static GateType getGateTypeByName(const string& name){
   if(name == "buf" ) return GATE_BUF;
   if(name == "not" ) return GATE_NOT;
   if(name == "and" ) return GATE_AND;
   if(name == "nand") return GATE_NAND;
   if(name == "or"  ) return GATE_OR;
   if(name == "nor" ) return GATE_NOR;
   if(name == "xor" ) return GATE_XOR;
   if(name == "xnor") return GATE_XNOR;
   return GATE_END;
}

static bool isValidGateTypeName(const string& name){
   return getGateTypeByName(name) != GATE_END;
}

static string getGateNameByType(GateType type){
   if(type == GATE_BUF ) return "buf";
   if(type == GATE_NOT ) return "not";
   if(type == GATE_AND ) return "and";
   if(type == GATE_NAND) return "nand";
   if(type == GATE_OR  ) return "or";
   if(type == GATE_NOR ) return "nor";
   if(type == GATE_XOR ) return "xor";
   if(type == GATE_XNOR) return "xnor";
   return "";
}

static bool isConstant(const string& name){
   return (name == "1'b0" || name == "1'b1" ||
           name == "1'd0" || name == "1'd1" ||
           name == "1'h0" || name == "1'h1" );
}

static string toConstant(const string& name){
   assert(isConstant(name));
   if(name[3] == '0') return "1'b0";
   return "1'b1";
}

/*************************************/
/*   class CirMgr public functions   */
/*************************************/
CirMgr::CirMgr(): _varNum(0) {
   StrList faninList;
   unsigned id;
   createIdByName("1'b0");
   createGate(GATE_CONST, "1'b0", faninList);
   faninList.push_back("1'b0");
   createIdByName("1'b1");
   createGate(GATE_NOT, "1'b1", faninList);

   id = getIdByName("1'b0");
   getGateById(id)->setPi();
   id = getIdByName("1'b1");
   getGateById(id)->setPi();
}  

bool
CirMgr::readCircuit(const string& filename, bool design) {
   // design = 0 -> design A
   // design = 1 -> design B (another circuit)
   ifstream fin(filename.c_str());
   if (!fin) {
      cout<<"Cannot open design \""<<filename<<"\"!!"<<endl;
      return false;
   }
   
   vector<char> sep, stop; // seperating chars / stop chars
   vector<string> tokens;  // parsed string tokens
   string prefixName = (design? "design_A_" : "design_B_");
   unsigned id;
   
   // initialize seperating chars
   sep.push_back(' ');
   sep.push_back('\t');
   sep.push_back('\n');
   sep.push_back('(');
   sep.push_back(')');
   sep.push_back(',');
   // initialize stop chars
   stop.push_back(';');

   while(!fin.eof()){
      parseStr(fin, sep, stop, tokens);
      /*      
      for(unsigned i=0, n=tokens.size(); i<n; ++i)
         cout << tokens[i] << " ";
      cout << endl;
      */
      unsigned n = tokens.size();
      if(n == 0) continue;

      // "module" <module name> <pin1 name> <pin2 name> ...
      if(tokens[0] == "module"){
         _ioNameList[design].clear();
         for(unsigned i=2; i<n; ++i)
            _ioNameList[design].push_back(tokens[i]);
      }
      // "input" <in1 name> <in2 name> ...
      else if(tokens[0] == "input"){
         for(unsigned i=1; i<n; ++i){
            // One Pi connect to two Buf
            // Buf are input connect to design_A&B respectively
            if (design){
               createIdByName(tokens[i]);
               createPI(tokens[i]);
               id = getIdByName(tokens[i]);
               getGateById(id)->setPi();
            }
            vector<string> bufFanin;
            bufFanin.push_back(tokens[i]);
            createIdByName(prefixName + tokens[i]);
            createGate(GATE_BUF, prefixName + tokens[i], bufFanin);
            id = getIdByName(prefixName + tokens[i]);
            getGateById(id)->setPi();
         }
      }
      // "output" <out1 name> <out2 name> ...
      else if(tokens[0] == "output"){
         for(unsigned i=1; i<n; ++i){
            createIdByName(prefixName + tokens[i]);
            createPO(prefixName + tokens[i]);
         }
      }
      // "wire" <w1 name> <w2 name> ...
      else if(tokens[0] == "wire"){
         for(unsigned i=1; i<n; ++i)
            createIdByName(prefixName + tokens[i]);
      }
      // <gate type> <out name> <in1 name> <in2 name> ...
      else if(isValidGateTypeName(tokens[0])){
         GateType type = getGateTypeByName(tokens[0]);
         vector<string> faninList;
         for (unsigned i=2, m=tokens.size(); i<m; ++i){
            if(isConstant(tokens[i]))
               faninList.push_back(toConstant(tokens[i]));
            else
               faninList.push_back(prefixName + tokens[i]);
         }
         createGate(type, prefixName + tokens[1], faninList);
      }
   }
   fin.close();

   // PO gate is created after "output" declaration,
   // so setPo can only be done here
   for(unsigned i=0, n=_poList.size(); i<n; ++i)
      getGateById(_poList[i])->setPo();

   return true;
}

bool 
CirMgr::writeCircuit(const string& filename, bool design) const {
   ofstream fout(filename.c_str());
   if (!fout) {
      cout<<"Cannot open design \""<<filename<<"\"!!"<<endl;
      return false;
   }

   string prefixName = (design? "design_A_" : "design_B_");
   unsigned prefixSize = prefixName.size();
   CirGate* gate;
   string gatePrefix, gateName;
   bool tmp; // handle output comma
   
   // module(a, b, ...);
   fout << "module ( ";
   for(unsigned i=0, n=_ioNameList[design].size(); i<n; ++i){
      if(i > 0) fout << " , ";
      fout << _ioNameList[design][i];
   }
   fout << ");" << endl;

   // input
   tmp = false;
   fout << "input ";
   for(unsigned i=0, n=_piList.size(); i<n; ++i){
      gate = getGateById(_piList[i]);
      gateName = gate->getName();
      if(tmp) fout << " , ";
      else tmp = true;
      fout << gateName;
   }
   fout << ";" << endl;

   // output
   tmp = false;
   fout << "output ";
   for(unsigned i=0, n=_poList.size(); i<n; ++i){
      gate = getGateById(_poList[i]);
      gatePrefix = gate->getName().substr(0, prefixSize);
      gateName   = gate->getName().substr(prefixSize);
      if(gatePrefix  == prefixName){
         if(tmp) fout << " , ";
         else tmp = true;
         fout << gateName;
      }
   }
   fout << ";" << endl;

   // wire
   tmp = false;
   fout << "wire ";
   for(unsigned i=0, n=_gateList.size(); i<n; ++i){
      gate = _gateList[i];
      if(gate == NULL || gate->isPi() || gate->isPo()) continue;
      gatePrefix = gate->getName().substr(0, prefixSize);
      gateName   = gate->getName().substr(prefixSize);
      if(gatePrefix == prefixName){
         if(tmp) fout << " , ";
         else tmp = true;
         fout << gateName;
      }
   }
   fout << ";" << endl;

   // gates
   for(unsigned i=0, n=_gateList.size(); i<n; ++i){
      gate = _gateList[i];
      if(gate == NULL || gate->isPi()) continue;
      gatePrefix = gate->getName().substr(0, prefixSize);
      gateName   = gate->getName().substr(prefixSize);
      if(gatePrefix == prefixName){
         fout << gate->getGateType() << " ( " << gateName;
         for(unsigned i=0, n=gate->getFaninSize(); i<n; ++i){
            CirGate* in = gate->getFaninGate(i);
            if(isConstant(in->getName()))
               fout << " , " << in->getName();
            else
               fout << " , " << in->getName().substr(prefixSize);
         }
         fout << " );" << endl;
      }
   }

   // endmodule
   fout << "endmodule" << endl;
   
   fout.close();
   return true;
}

void
CirMgr::createPI(const string& name){
   assert(checkNameDeclared(name));
   unsigned id = getIdByName(name);
   CirGate* gate = new CirPiGate(id, name, IdList());
   if(_gateList.size() <= id) _gateList.resize(id + 1);
   _gateList[id] = gate;
   _piList.push_back(id);
}

void
CirMgr::createPO(const string& name){
   assert(checkNameDeclared(name));
   unsigned id = getIdByName(name);
   _poList.push_back(id);
}

void
CirMgr::createGate(GateType type, const string& name, const vector<string>& input){
   assert(checkNameDeclared(name));
   unsigned id = getIdByName(name);
   CirGate* gate = NULL;
   IdList faninList;
   
   for(unsigned i=0; i<input.size(); ++i)
      faninList.push_back(getIdByName(input[i]));
   
   switch(type){
      case GATE_CONST:
         gate = new CirConstGate(id, name, faninList);
         break;
      case GATE_PI:
         gate = new CirPiGate(id, name, faninList);
         break;
      case GATE_BUF:
         gate = new CirBufGate(id, name, faninList);
         break;
      case GATE_NOT:
         gate = new CirNotGate(id, name, faninList);
         break;
      case GATE_AND:
         gate = new CirAndGate(id, name, faninList);
         break;
      case GATE_NAND:
         gate = new CirNandGate(id, name, faninList);
         break;
      case GATE_OR:
         gate = new CirOrGate(id, name, faninList);
         break;
      case GATE_NOR:
         gate = new CirNorGate(id, name, faninList);
         break;
      case GATE_XOR:
         gate = new CirXorGate(id, name, faninList);
         break;
      case GATE_XNOR:
         gate = new CirXnorGate(id, name, faninList);
         break;
      default: break;
   }
   assert(gate != 0);
   assert(_gateList.size() > id);
   _gateList[id] = gate;
}

void
CirMgr::linkGates(){
   for(unsigned i=0, n=_gateList.size(); i<n; ++i){
      CirGate* gate = _gateList[i];
      if(gate == 0) continue; // declared but not used
      for(unsigned j=0, m=gate->getFaninSize(); j<m; ++j){
         CirGate* in = getGateById(gate->getFaninId(j));
         assert(in != 0); // floating input
         gate->addFanin(in);
         in->addFanout(gate);
      }
   }
}

void
CirMgr::buildDfsList(){
   ++dfsFlag;
   for(unsigned i=0, n=_poList.size(); i<n; ++i)
      dfs(getGateById(_poList[i]));
}

void 
CirMgr::printNetlist() const{
   for(unsigned i=0, n=_dfsList.size(); i<n; ++i){
      cout << "[" << i << "] " << left << setw(5) << _dfsList[i]->getId();
      _dfsList[i]->printGate();
   }
}

void
CirMgr::printFECPairs() const
{
   for (unsigned i=0, n=_fecGrps.size(); i<n; ++i){
      cout << "[" << i << "] ";
      for (unsigned j=0, m=_fecGrps[i]->size(); j<m; ++j){
         if (_fecGrps[i]->at(j)%2 != _fecGrps[i]->at(0)%2) cout << "!";
         cout << _fecGrps[i]->at(j)/2 << " ";
      }
      cout << endl;
   }
}

/**************************************/
/*   class CirMgr private functions   */
/**************************************/
bool
CirMgr::checkNameDeclared(const string& name) const {
   return _varMap.find(name) != _varMap.end();
}

void
CirMgr::createIdByName(const string& name){
   if(checkNameDeclared(name)){
      cerr << "gate \"" << name << "\" is already declared!" << endl;
      return;
   }
   _varMap[name] = _varNum++;
   _gateList.push_back(NULL);
   assert(_gateList.size() == _varNum);
}

unsigned
CirMgr::getIdByName(const string& name){
   if(!checkNameDeclared(name)){
      cerr << "gate \"" << name << "\" is not declared!" << endl;
      return -1;
   }
   return _varMap[name];
}

// used in buildDfsList
void
CirMgr::dfs(CirGate* gate){
   if(gate->isMark()) return;
   gate->mark();
   for(unsigned i=0, n=gate->getFaninSize(); i<n; ++i)
      dfs(gate->getFaninGate(i));
   _dfsList.push_back(gate);
}
