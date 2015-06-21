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

static bool inSameFecGroup(const CirGate* gate1, const CirGate* gate2){
   return (gate1->getFecGrp() != NULL &&
           gate1->getFecGrp() == gate2->getFecGrp());
}

/*************************************/
/*   class CirMgr public functions   */
/*************************************/
CirMgr::CirMgr(): _varNum(0) {
   createIdByName("1'b0");
   createGate(GATE_CONST0, "1'b0", StrList());
   createIdByName("1'b1");
   createGate(GATE_CONST1, "1'b1", StrList());

   getGateByName("1'b0")->setPi();
   getGateByName("1'b1")->setPi();
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
         _moduleName[design] = tokens[1];
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
               getGateByName(tokens[i])->setPi();
            }
            vector<string> bufFanin;
            bufFanin.push_back(tokens[i]);
            createIdByName(prefixName + tokens[i]);
            createGate(GATE_BUF, prefixName + tokens[i], bufFanin);
            getGateByName(prefixName + tokens[i])->setPi();
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
   
   // module <module name> (a, b, ...);
   fout << "module " << _moduleName[design] << " ( ";
   for(unsigned i=0, n=_ioNameList[design].size(); i<n; ++i){
      if(i > 0) fout << " , ";
      fout << _ioNameList[design][i];
   }
   fout << " );" << endl;

   // input
   tmp = false;
   fout << "input ";
   for(unsigned i=0, n=_piList.size(); i<n; ++i){
      // _piList is actually not the pi of the two circuits
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
      case GATE_CONST0:
         gate = new CirConst0Gate(id, name, faninList, false);
         break;
      case GATE_CONST1:
         gate = new CirConst0Gate(id, name, faninList, true);
         break;
      case GATE_PI:
         gate = new CirPiGate(id, name, faninList);
         break;
      case GATE_BUF:
         gate = new CirBufGate(id, name, faninList, false);
         break;
      case GATE_NOT:
         gate = new CirBufGate(id, name, faninList, true);
         break;
      case GATE_AND:
         gate = new CirAndGate(id, name, faninList, false);
         break;
      case GATE_NAND:
         gate = new CirAndGate(id, name, faninList, true);
         break;
      case GATE_OR:
         gate = new CirOrGate(id, name, faninList, false);
         break;
      case GATE_NOR:
         gate = new CirOrGate(id, name, faninList, true);
         break;
      case GATE_XOR:
         gate = new CirXorGate(id, name, faninList, false);
         break;
      case GATE_XNOR:
         gate = new CirXorGate(id, name, faninList, true);
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
CirMgr::printNetlist() const
{
   for(unsigned i=0, n=_dfsList.size(); i<n; ++i){
      cout << "[" << i << "] " << left << setw(5) << _dfsList[i]->getId();
      _dfsList[i]->printGate();
   }
}

void
CirMgr::printFECPairs() const
{
   unsigned head, current;
   for (unsigned i=0, n=_fecGrps.size(); i<n; ++i){
      cout << "[" << i << "] ";
      head = _fecGrps[i]->at(0);
      for (unsigned j=0, m=_fecGrps[i]->size(); j<m; ++j){
         current = _fecGrps[i]->at(j);
         if (current%2 != head%2) cout << "!";
         cout << current/2 << "(" << getGateById(current/2)->getName() << ") ";
      }
      cout << endl;
   }
}

void
CirMgr::writeFECPairs(const string& filename) const
{
   ofstream fout(filename.c_str());
   unsigned head, current;
   for (unsigned i=0, n=_fecGrps.size(); i<n; ++i){
      head = _fecGrps[i]->at(0);
      for (unsigned j=0, m=_fecGrps[i]->size(); j<m; ++j){
         current = _fecGrps[i]->at(j);
         if (current%2 != head%2) fout << "!";
         if (j) fout << " ";
         fout << getGateById(current/2)->getName();
      }
      fout << endl;
   }
   fout.close();
}

// generate cut list for all gates
void
CirMgr::genAllCutList(unsigned k)
{
	unsigned size = 0;
	CirCut::setMaxCutSize(k);
	for(unsigned i=0, n=_dfsList.size(); i<n; ++i){
      // cout << "generating gate " << _dfsList[i]->getGid() << endl;
      _dfsList[i]->genCutList();
      // cout << "size = " << _dfsList[i]->getCutList().size() << endl;
      size += _dfsList[i]->getCutList().size();
   }    
	cout << "average number of cuts: " << (double)size/_dfsList.size() << endl;
}

void
CirMgr::printAllCutList() const
{
   for(unsigned i=0, n=_dfsList.size(); i<n; ++i){
      CirGate* gate = _dfsList[i];
      cout << "gate " << gate->getId() 
           << " (" << gate->getName() << ") :" << endl;
      cout << gate->getCutList() << endl;
   }
   cout << endl;
}

void
CirMgr::writeAllCutList(const string& filename) const
{
   ofstream fout(filename.c_str());
   for(unsigned i=0, n=_dfsList.size(); i<n; ++i){
      CirGate* gate = _dfsList[i];
      fout << "gate " << gate->getId() 
           << " (" << gate->getName() << ") :" << endl;
      fout << gate->getCutList() << endl;
   }
   fout << endl;
   fout.close();
}

void
CirMgr::mapCut()
{
   vector<CutPair> pairList;
   CirGate *gateA, *gateB;
   string nameA;

   // put all the PO into pairList
   for(unsigned i=0, n=_poList.size(); i<n; ++i){
      gateA = getGateById(_poList[i]);
      nameA = gateA->getName();
      if(nameA.substr(0, 9) == "design_A_"){
         gateB = getGateByName("design_B_" + nameA.substr(9));
         assert(gateB != NULL);
         pairList.push_back(make_pair(gateA->getId(), gateB->getId()));
      }
   }

   for(unsigned i=0, n=pairList.size(); i<n; ++i)
      cout << pairList[i].first << ", " << pairList[i].second << endl;

   while(pairList.size()){
      CutPair cp = pairList.back();
      pairList.pop_back();
      updatePairList(cp, pairList);
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
CirMgr::getIdByName(const string& name) const {
   if(!checkNameDeclared(name)){
      cerr << "gate \"" << name << "\" is not declared!" << endl;
      return UINT_MAX;
   }
   return _varMap.at(name);
}

// used in buildDfsList
void
CirMgr::dfs(CirGate* gate){
   if(gate->isMark()) return;
   gate->mark();
   for(unsigned i=0, n=gate->getFaninSize(); i<n; ++i)
      dfs(gate->getFaninGate(i));
   _dfsList.push_back(gate);
   gate->setDfsOrder(_dfsList.size());
}

// cut mapping
void
CirMgr::updatePairList(CutPair cp, vector<CutPair>& pairList)
{
   CirGate* gate1 = getGateById(cp.first);
   CirGate* gate2 = getGateById(cp.second);
   CirCut *cut1, *cut2;
   assert(gate1 != NULL && gate2 != NULL);
   if(inSameFecGroup(gate1, gate2)){
      if(gate1->getMatchCut(gate2->getCutList(), gate2->getId(), cut1, cut2)){
         assert(cut1->size() == cut2->size());
         for(unsigned i=0, n=cut1->size(); i<n; ++i)
            pairList.push_back(make_pair(cut1->getLeaf(i), cut2->getLeaf(i)));
      }
   }
   else{ // only happen when gate1/gate2 are POs
      
   }
}


// 1.Called when two design are UNEquivalence
// 2.DFS from PO, if find equivalence then cut and stop
//   else run till reach PI
void 
CirMgr::nonEqAddCut()
{
   ++dfsFlag;
   for (unsigned i=0, n=_poList.size(); i<n; ++i)
      dfsAddCut(getGateById(_poList[i]));
}

// Called only in nonEqAddCut
// Check whether gate1 is in any fecgroup
// DO NOT CUT If 
//   1. Other gate in fecgroup is in the same design with gate1
//   2. gate1 is pi
// else cut and ignore gate1's fanin
void 
CirMgr::dfsAddCut(CirGate* gate1)
{
   if(gate1->isMark()|| gate1->isCut()) return;
   gate1->mark();
   // if find eq then cut and stop
   static unsigned number = 0;
   IdList* fecGrp= gate1->getFecGrp();
   bool cut = false;
   if (fecGrp != NULL && !gate1->isPi()){
      unsigned gatePhase = 0;
      for (unsigned i=0, n=fecGrp->size(); i<n; ++i){
         if (fecGrp->at(i)/2 != gate1->getId())
            continue;
         gatePhase = fecGrp->at(i)%2;
      }
      string design = gate1->getName().substr(0,9);

      for(unsigned i=0, n=fecGrp->size(); i<n; ++i){
         unsigned phase = fecGrp->at(i)%2 ^ gatePhase;
         CirGate* gate2 = getGateById(fecGrp->at(i)/2);
         if (gate2->getName().substr(0,9) == design ||
             gate2->getId() == gate1->getId() || 
             gate2->isPi())
            continue;
         cut = true;
         gate2->setIsCut(true);
         gate2->setCutId(2*number + phase);
         //cout << gate2->getName() << endl;
      }   
      if (cut){
         gate1->setIsCut(cut);
         gate1->setCutId(2*number);
         //cout << gate1->getName() << endl;
         //cout << "=============" << endl;
         ++number;
         return;
      }
   }
   // else continue
   for(unsigned i=0, n=gate1->getFaninSize(); i<n; ++i)
      dfsAddCut(gate1->getFaninGate(i));
}
