/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashMap.h"
//#include <bitset>

using namespace std;

/**********************************/
/*********    HashKey    **********/
/**********************************/
class SimPValue
{
public:
   SimPValue(unsigned v = 0):_value(v) {}
   unsigned operator () () const { 
      unsigned k = _value;
      for (unsigned i = 0; i < (_value % 10); ++i)
         k ^= (_value << (3*i));
      return k; 
   } 
   bool operator == (const SimPValue& a) const { return (a._value == _value); }
   friend ostream& operator << (ostream& os , const SimPValue& s) { os << s._value; return os; }
   unsigned getValue(){ return _value; }
private:
   unsigned _value;
};

typedef pair<SimPValue, IdList*> FecNode;
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
// If there is no fecGroup in fecGroupLIst,
// whether has been simulated before, simulated again
// Else, simulated by original fecGroup
   /*
void
CirMgr::randomSim()
{
   unsigned* iv = new unsigned[_pis.size()];
   unsigned failTime = 0, numPattern = 0;
   for (unsigned i = 0; i < _pis.size(); ++i)
      iv[i] = (rand() << 16) ^ rand();
   if (_fecGrps.size() == 0) 
      initFec(iv, 32);
   else checkFec(iv, 32);
   numPattern += 32;
   unsigned stopTime;
   if (_pis.size() < 10) stopTime = _pis.size();
   else if (_pis.size() < 100) stopTime = _pis.size() * _pis.size() / 100 + 10;
   else stopTime = 181;

   while (failTime < stopTime && _fecGrps.size()){
      for (unsigned i = 0; i < _pis.size(); ++i)
         iv[i] = (rand() << 16) ^ rand();
      if (!checkFec(iv, 32)) ++failTime;
      numPattern += 32;
   }
   cout << "MAX_FAILS = " << stopTime << endl
        << numPattern << " patterns simulated." << endl;
   delete[] iv;
}
   */

void 
CirMgr::genPattern(const string& fileName, const unsigned& k)
{
   ofstream patternFile;
   patternFile.open(fileName.c_str(),ios::out);
   if(!patternFile){
      cout << "Could not construct file " << fileName << endl;
      return;
   }

   for (unsigned i=0; i<k; ++i){
      for (unsigned j=0, m=_piList.size(); j<m; ++j)
         patternFile << rand()%2;
      patternFile << endl;
   }
   patternFile.close();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   
   unsigned* v = new unsigned[_piList.size()];
   unsigned numPattern = 0;
   for (unsigned i = 0; numPattern == 32 * i; ++i){
      if (!getPiSimFromFile(patternFile, v, numPattern)){
         numPattern = 32 * i;
         break;
      }
      if (_fecGrps.size() == 0 && i == 0)
         initFec(v, numPattern);
      else checkFec(v, numPattern);
   }
   cout << numPattern << " patterns simulated." << endl;
   patternFile.close();
   delete[] v;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
// circuit first time simulation 
// (called when no fecGroup in fecGroupsList)
// should not be called if there are fec groups
void
CirMgr::initFec(const unsigned* v, const unsigned& outputBit)
{
   IdList fec;
   // get pi value from v
   // simulation all gate first
   for (unsigned i=0, m=_piList.size(); i < m; ++i){
      //CirPiGate* gate = (CirPiGate*)(getGateById(_piList[i]));
      //gate->simulate(v[i]);
      ((CirPiGate*)getGateById(_piList[i]))->simulate(v[i]);
   }
   for (unsigned i=0, m=_dfsList.size(); i < m; ++i)
      _dfsList[i]->simulate();
   fec.push_back(0);
   for (unsigned i = 0, m=_dfsList.size(); i < m; ++i){
      fec.push_back(2*_dfsList[i]->getId());
   }
   sortFecGrp(&fec, _fecGrps);
}

// called when there are groups in fecGroupsList
// If newFecGroups == old one, retrun false
// else return true;
bool 
CirMgr::checkFec(const unsigned *v, const unsigned& outputBit)
{
   bool different = false;
   vector <IdList*> newGrps;
   // get pi value from v
   // simulation all gate
   for (unsigned i=0, m=_piList.size(); i < m; ++i)
      ((CirPiGate*)getGateById(_piList[i]))->simulate(v[i]);
   for (unsigned i=0, m=_dfsList.size(); i < m; ++i)
      _dfsList[i]->simulate();
   for (unsigned i = 0; i < _fecGrps.size(); ++i)
      sortFecGrp(_fecGrps[i], newGrps);
   // replace old fecGroupsList by new fecGroupsList
   // check whether old = new
   _fecGrps.swap(newGrps);
   if (newGrps.size() != _fecGrps.size()) different = true;
   else{
      for (unsigned i = 0; i < newGrps.size(); ++i)
         if (*newGrps[i] != *_fecGrps[i]) different = true;
   }
   // delete old one (release memory)
   for (unsigned i = 0; i < newGrps.size(); ++i)
      delete newGrps[i];
   return different;
}


// 1.Get all gates in the same fecGroup (oldFecGrp)
// 2.Query whether there is fecGroup which g belongs to in HashMap
// 3.If not, creat a new one, and insert
//   else set g's fec
// 4.If simulation is inverse the same, store 2*id+1 in fecgrp
//   else store 2*id
// 5.Collect all valid fecGroups(size > 1) into newGrps
void
CirMgr::sortFecGrp(IdList* oldFecGrp, vector <IdList*>& newGrps)
{
   HashMap<SimPValue, IdList*> fecHashMap(getHashSize(oldFecGrp->size()));
   //HashMap<SimPValue, IdList*> fecHashMap((oldFecGrp->size())/2);
   for (unsigned j = 0; j < oldFecGrp->size(); ++j){
      CirGate* g = getGateById(oldFecGrp->at(j)/2);
      if (g == NULL) continue;
      IdList* fecGrp;
      unsigned phase = 0;
      SimPValue temp(g->getSimOutput());
      SimPValue inv(~g->getSimOutput());
      FecNode gNode(temp, NULL), invNode(inv, NULL);

      if (fecHashMap.query(gNode))
         fecGrp = gNode.second;
      else if (fecHashMap.query(invNode)){
         fecGrp = invNode.second;
         phase = 1;
      }
      else{
         fecGrp = new IdList;
         assert(fecHashMap.insert(FecNode(temp, fecGrp)));
      }
      g->setFecGrp(fecGrp);
      fecGrp->push_back(2 * g->getId() + phase);
   }
   for (HashMap<SimPValue, IdList*>::iterator it = fecHashMap.begin(); it != fecHashMap.end(); ++it){
      if ((*it).second->size() > 1)
         newGrps.push_back((*it).second);
      else if ((*it).second->size() == 1){
         getGateById( ((*it).second->at(0))/2 )->setFecGrp(NULL);
         delete (*it).second;
      }
   }
}

// When encounting space, see the following as another pattern
// Use flag to detect space, true = there is space
bool
CirMgr::getPiSimFromFile(ifstream& patternFile, unsigned* v, unsigned& count)
{
   string temp;
   bool flag = false;
   for (unsigned i = 0; i < 32;){
      if (flag) flag = false;
      else { if (!getline(patternFile, temp)) return true; }
      if (temp.empty()) continue;
      unsigned n = 0;
      for (; n < temp.length(); ++n){
         if (n > _piList.size()){
            cerr << "Error: Pattern(" << temp << ") length(" << temp.length() << ")" 
                 << "does not match the number of inpust(" << _piList.size() << ") in a circuit!!" << endl;
            return false;
         }
         if (i == 0) v[n] = 0; //initialize;
         if (temp[n] == '1')
            v[n] += (1 << (i + 1));
         else if (temp[n] == ' ') { flag = true; break; }
         else if (temp[n] != '0'){
            cerr << "Error: Pattern(" << temp << ") contains a non-0/1 character('" 
                 << temp[n] << "')." << endl;
            return false;
         }
      }
      if (n != _piList.size() && n != 0){
         cerr << "Error: Pattern(" << temp.substr(0,n) << ") length(" << n << ") " 
              << "does not match the number of inpust(" << _piList.size() << ") in a circuit!!" << endl;
         return false;
      }
      if (n != 0) { ++count; ++i; }
      if (flag) temp = temp.substr(n + 1);
   }
   return true;
}

/*
void 
CirMgr::outputSimValueToFile(unsigned outputBit)
{
   while (outputBit > 32) outputBit -= 32;
   for (unsigned n = 1; n < 33 && n <= outputBit; ++n){
      for (unsigned i = 0; i < _pis.size(); ++i){
         bitset<32> simV(_pis[i]->getSimOutput());
         *_simLog << simV[n];
      }
      *_simLog << " ";
      for (unsigned i = 0; i < _pos.size(); ++i){
         bitset<32> simV(_pos[i]->getSimOutput());
         *_simLog << simV[n];
      }
      *_simLog << endl;
   }
}
*/
