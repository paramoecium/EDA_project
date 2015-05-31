#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

/************************/
/*   global variables   */
/************************/
CirMgr* cirMgr;
unsigned dfsFlag = 0;

/************************/
/*   static functions   */
/************************/
static GateType getGateTypeByName(const string& name){
   if(name == "buf" ) return GATE_BUF;
   if(name == "inv" ) return GATE_INV;
   if(name == "and" ) return GATE_AND;
   if(name == "nand") return GATE_NAND;
   if(name == "or"  ) return GATE_OR;
   if(name == "nor" ) return GATE_NOR;
   if(name == "xor" ) return GATE_XOR;
   if(name == "xnor") return GATE_XNOR;
   return GATE_END;
}

/*************************************/
/*   class CirMgr public functions   */
/*************************************/
CirMgr::CirMgr(): _varNum(0) {}

bool
CirMgr::readCircuit(const string& filename, bool design) {
   // design = 0 -> design A
   // design = 1 -> design B (another circuit)
   ifstream fin(filename.c_str());
   vector<char> sep, stop;
   vector<string> tokens;
   string extraName ="";
   if (design) extraName = "_design_A";
   else extraName = "_design_B";
   
   // initialize seperate char
   sep.push_back(' ');
   sep.push_back('\t');
   sep.push_back('\n');
   sep.push_back('(');
   sep.push_back(')');
   sep.push_back(',');
   // initialize stop char
   stop.push_back(';');

   if (!fin) {
      cout<<"Cannot open design \""<<filename<<"\"!!"<<endl;
      return false;
   }

   while(!fin.eof()){
      parseStr(fin, sep, stop, tokens);
      /*      
      for(unsigned i=0, n=tokens.size(); i<n; ++i)
         cout << tokens[i] << " ";
      cout << endl;
      */
      unsigned n = tokens.size();
      if(n == 0) continue;
      if(tokens[0] == "module"){

      }
      else if(tokens[0] == "input"){
         for(unsigned i=1; i<n; ++i){
            // One Pi connect to two Buf
            // Buf are input connect to design_A&B respectively
            if (design)
               createPI(tokens[i]);
            vector<string> BufFanin;
            BufFanin.push_back(tokens[i]);
            tokens[i] += extraName;
            createGate(GATE_BUF, tokens[i], BufFanin);
         }
      }
      else if(tokens[0] == "output"){
         for(unsigned i=1; i<n; ++i){
            tokens[i] += extraName;
            createPO(tokens[i]);
         }
      }
      else if(tokens[0] == "wire"){
         // neglect
      }
      else if(tokens[0] == "buf" || tokens[0] == "inv"  ||
              tokens[0] == "and" || tokens[0] == "nand" ||
              tokens[0] == "or"  || tokens[0] == "nor"  ||
              tokens[0] == "xor" || tokens[0] == "xnor" ){
         GateType type = getGateTypeByName(tokens[0]);
         vector<string>::iterator st, ed;
         st = tokens.begin(); ++st; ++st;
         ed = tokens.end();
         for (unsigned i=1, m=tokens.size(); i<m; ++i)
            tokens[i] += extraName;
         createGate(type, tokens[1], vector<string>(st, ed));
      }
   }

   /*
   string line, par;
   vector<string> params;
   //neglect the "module top" part
   //neglect the "module top" part
   while (line.substr(0,5) != "input") {
      getline(fin, line);
      cout<<"//"<<line<<"\n";
   }
   //parse "output" part
   while (line.substr(0,6) != "output")  {
      getline(fin, line);
      cout<<"//"<<line<<"\n";
   }
   //parse "wire" part
   while (line.substr(0,4) != "wire")  {
      do {
         getline(fin, line);
         cout<<"//"<<line<<"\n";
         size_t pos = 0;
         while (pos!=string::npos) {
            pos = myStrGetTok(line, par, pos," (),;\n");
         }
      } while (line.find(";")==string::npos);//may go wrong
   }
   //parse gate descriptions
   params.clear();
   par = "";
   while (true) {
      do {
         getline(fin, line);
         cout<<"\n//"<<line<<"\n";
         if (line.substr(0,9) == "endmodule") break;
         size_t pos = 0;
         while (pos!=string::npos) {
            pos = myStrGetTok(line, par, pos," (),;\n");
            if(par.length()>0) cout<<par<<"_";
            if(par.length()>0) params.push_back(par);
         }
      } while (line.find(";")==string::npos);//may go wrong
      if (line.substr(0,9) == "endmodule") break;
      params.pop_back();// ";"
      GateType gatetype = GATE_END;
      if(params[0].compare("buf")==0)
         gatetype = GATE_BUF;
      else if(params[0].compare("inv")==0)
         gatetype = GATE_INV;
      else if(params[0].compare("and")==0)
         gatetype = GATE_AND;
      else if(params[0].compare("nand")==0)
         gatetype = GATE_NAND;
      else if(params[0].compare("or")==0)
         gatetype = GATE_OR;
      else if(params[0].compare("nor")==0)
         gatetype = GATE_NOR;
      else if(params[0].compare("xor")==0)
         gatetype = GATE_XOR;
      else if(params[0].compare("xnor")==0)
         gatetype = GATE_XNOR;
      createGate(gatetype, params[1], vector<string>(++++(params.begin()), params.end()));
      params.clear();
      par = "";
   }
   */
   return true;
}

void
CirMgr::createPI(const string& name){
   unsigned id = getIdByName(name);
   CirGate* gate = new CirPiGate(id, name, IdList());
   if(_gateList.size() <= id) _gateList.resize(id + 1);
   _gateList[id] = gate;
   _piList.push_back(id);
}

void
CirMgr::createPO(const string& name){
   unsigned id = getIdByName(name);
   _poList.push_back(id);
}

void
CirMgr::createGate(GateType type, const string& name, const vector<string>& input){
   CirGate* gate = NULL;
   unsigned id = getIdByName(name);
   IdList faninList;
   for(unsigned i=0; i<input.size(); ++i)
      faninList.push_back(getIdByName(input[i]));
   
   switch(type){
      case GATE_PI:
         gate = new CirPiGate(id, name, faninList);
         break;
      case GATE_BUF:
         gate = new CirBufGate(id, name, faninList);
         break;
      case GATE_INV:
         gate = new CirInvGate(id, name, faninList);
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
   if(_gateList.size() <= id) _gateList.resize(id+1);
   _gateList[id] = gate;
}

void
CirMgr::linkGates(){
   for(unsigned i=0, n=_gateList.size(); i<n; ++i){
      CirGate* gate = _gateList[i];
      for(unsigned j=0, m=gate->getFaninNum(); j<m; ++j){
         CirGate* in = getGateById(gate->getFaninId(j));
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
   for (size_t i = 0; i < _fecGrps.size(); ++i){
      cout << "[" << i << "] ";
      for (size_t j = 0; j < _fecGrps[i]->size(); ++j){
         if (_fecGrps[i]->at(j)%2 != _fecGrps[i]->at(0)%2) cout << "!";
         cout << _fecGrps[i]->at(j)/2 << " ";
      }
      cout << endl;
   }
}

/**************************************/
/*   class CirMgr private functions   */
/**************************************/
unsigned
CirMgr::getIdByName(const string& name){
   if(_varMap.find(name) == _varMap.end())
      _varMap[name] = _varNum++;
   return _varMap[name];
}

// used in buildDfsList
void
CirMgr::dfs(CirGate* gate){
   if(gate->isMark()) return;
   gate->mark();
   for(unsigned i=0, n=gate->getFaninNum(); i<n; ++i)
      dfs(gate->getFaninGate(i));
   _dfsList.push_back(gate);
}
