#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
/************************/
/*   global variables   */
/************************/
CirMgr* cirMgr;

/*************************************/
/*   class CirMgr public functions   */
/*************************************/
CirMgr::CirMgr(): _varNum(0) {}

bool
CirMgr::readCircuit(const string& filename) {
   ifstream fin(filename.c_str());
   if (!fin) {
      cout<<"Cannot open design \""<<filename<<"\"!!"<<endl;
      return false;
   }
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
         while (pos!=std::string::npos) {
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
         while (pos!=std::string::npos) {
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
   return true;
}
void
CirMgr::createGate(GateType type, const string& name,
                   const vector<string>& input){
   CirGate* gate = NULL;
   unsigned id = getIdByName(name);
   IdList faninList;
   for(unsigned i=0; i<input.size(); ++i)
      faninList.push_back(getIdByName(input[i]));
   
   switch(type){
      case GATE_PI:
         gate = new CirPiGate(id, name, faninList);
         break;
      case GATE_PO:
         gate = new CirPoGate(id, name, faninList);
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
