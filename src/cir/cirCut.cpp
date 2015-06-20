#include <algorithm>
#include "cirCut.h"
#include "cirMgr.h"
#include "cirGate.h"
#include "myHashSet.h"

// #define NO_HASH

extern CirMgr* cirMgr;
extern BddMgr* bddMgr;

/************************/
/*   Static functions   */
/************************/

// count # of 1's in binary
static unsigned numOfOne(unsigned n) {
	unsigned ret = 0;
	while(n > 0){
		n &= (n-1);
		ret++;
	}
	return ret;
}

static bool cutCmp(const CirCut* cut1, const CirCut* cut2) {
   unsigned n = cut1->size(), m = cut2->size();
   if(n != m) return n < m;
   for(unsigned i=0; i<n; ++i){
      if(cut1->getLeaf(i) != cut2->getLeaf(i))
         return cut1->getLeaf(i) < cut2->getLeaf(i);
   }
   return false;
}

/********************************************/
/*   Class CirCut static member variables   */
/********************************************/
unsigned CirCut::_maxCutSize = 6;

/********************************************/
/*   Class CirCut public member functions   */
/********************************************/
CirCut::CirCut(): _sign(0), _boss(this), _visited(false){
   _leaf = new unsigned [_maxCutSize*2+2];
   _sz = 0;
}

CirCut::CirCut(unsigned leaf): _sign(0), _boss(this), _visited(false){
   // _leaf.reserve(_maxCutSize*2);
   _leaf = new unsigned[_maxCutSize*2+2];
   _sz = 0;
	addLeafForce(leaf);
}

CirCut::CirCut(const CirCut& cut): _sign(0), _boss(this), _visited(false){
   _leaf = new unsigned[_maxCutSize*2+2];
   _sz = 0;
	for(unsigned i=0, n=cut.size(); i<n; ++i)
		addLeafForce(cut[i]);
}

CirCut::~CirCut(){
   delete[] _leaf;
}

bool
CirCut::operator == (const CirCut& cut) const {
   if(getSign() != cut.getSign()) return false;
   if(size() != cut.size()) return false;
   for(unsigned i=0, n=size(); i<n; ++i){
      if(getLeaf(i) != cut.getLeaf(i)) return false;
   }
   return true;
}

CirCut*
CirCut::merge(const CirCut* cut) const {
	// check if signature implies that the merged cut is too big
	if(numOfOne(_sign | cut->getSign()) > _maxCutSize) return 0;

	CirCut* retCut = new CirCut;

   unsigned n1 = size(), n2 = cut->size();
   unsigned p1 = 0, p2 = 0;
   while(p1 < n1 || p2 < n2){
      if(p2 == n2) 
         retCut->addLeafForce( getLeaf(p1++) );
      else if(p1 == n1) 
         retCut->addLeafForce( cut->getLeaf(p2++) );
      else if(getLeaf(p1) < cut->getLeaf(p2))
         retCut->addLeafForce( getLeaf(p1++) );
      else if(getLeaf(p1) > cut->getLeaf(p2))
         retCut->addLeafForce( cut->getLeaf(p2++) );
      else
         p2++;
   }

	if(retCut->size() > _maxCutSize){
		delete retCut;
		retCut = 0;
	}
	return retCut;
}

bool
CirCut::containGateId(unsigned gid) const {
	// can use binary search as well
	// cut size is small so linear search is OK
	for(unsigned i=0, n=size(); i<n; ++i)
		if(getLeaf(i) == gid) return true;
	return false;
}

bool
CirCut::dominateCut(const CirCut* cut) const {
	if((_sign & cut->getSign()) != _sign) return false;
   unsigned n1 = size(), n2 = cut->size();
	for(unsigned i1=0, i2=0; i1<n1 && i2<n2; ++i1){
      while(i2 < n2 && cut->getLeaf(i2) < getLeaf(i1)) i2++;
      if(i2 == n2 || cut->getLeaf(i2) != getLeaf(i1)) return false;
   }
	return true;
}

void
CirCut::genCutFunc()
{
   for(unsigned i=0, n=size(); i<n; ++i){
      CirGate* gate = cirMgr->getGateById(getLeaf(i));
      gate->setGateFunc(bddMgr->getSupport(i+1));
   }
   _func.resize(rootSize());
   sort(_root.begin(), _root.end());
   for(unsigned i=0, n=rootSize(); i<n; ++i){
      CirGate* gate = cirMgr->getGateById(getRoot(i));
      gate->genGateFunc();
      _func[i] = gate->getGateFunc();
   }
}

// TODO: change to binary search
BddNode
CirCut::getFuncByRoot(unsigned root) const {
   for(int idx=0, n=_root.size(); idx<n; ++idx){
      if(_root[idx] == root)
         return getFuncByIdx(idx);
   }
   cerr << "cannot find root " << root << endl;
   return bddMgr->getSupport(0);
}

ostream& operator << (ostream& os, const CirCut& cut){
	os << "{ " ;
	for(unsigned i=0, n=cut.size(); i<n; ++i)
		os << cut[i] << (i==n-1? "" : ",") << " ";
	os << "}" ;
	return os;
}

/*********************************************/
/*   Class CirCut private member functions   */
/*********************************************/
bool
CirCut::addLeafForce(unsigned leaf){
	// _leaf.push_back(leaf);
   _leaf[_sz++] = leaf;
	// insertion sort, faster than sort()
	for(unsigned i=size()-1; i>0; --i){
		if(_leaf[i] < _leaf[i-1]) swap(_leaf[i], _leaf[i-1]);
		else break;
	}
	// _sign += 2^(leaf mod 32 or 64), bitwise addition
	_sign |= (1u << (leaf & (sizeof(size_t)*8-1) ) );  
	return true;
}

bool
CirCut::addLeaf(unsigned leaf){
	if(containGateId(leaf)) return false;
	return addLeafForce(leaf);
}

/*********************************************/
/*   Class CirCutV public member functions   */
/*********************************************/
int
CirCutV::operator () () const {
   int ret = 0;
   for(unsigned i=0, n=_cut->size(); i<n; ++i)
      ret = (ret*1237 + _cut->getLeaf(i)) % 1000001;
   return ret;
}

bool 
CirCutV::operator == (const CirCutV& cutV) const {
   return *getCut() == *(cutV.getCut());
}

/************************************************/
/*   Class CirCutList static member variables   */
/************************************************/
HashSet<CirCutV>* CirCutList::_hashSet = 0;

/************************************************/
/*   Class CirCutList public member functions   */
/************************************************/
CirCutList::CirCutList(){
   // _cuts.reserve(10);
}

CirCutList::~CirCutList(){
	// for(unsigned i=0, n=_cuts.size(); i<n; ++i)
	// 	delete _cuts[i];
}

bool
CirCutList::addCutForce(CirCut* cut, unsigned root){
   bool ret = true;
#ifndef NO_HASH
   CirCutV cutV(cut);
   if(_hashSet->query(cutV)){
      if(cutV.getCut()->getBoss() != cutV.getCut()) return false;
      cut = cutV.getCut()->getBoss();
      ret = false;
   }
   else{
      _hashSet->insert(cutV);
   }
#endif
   cut->addRoot(root);
   _cuts.push_back(cut);
   return ret;
}

bool
CirCutList::addCut(CirCut* cut, unsigned root){
	for(unsigned i=0, n=_cuts.size(); i<n; ++i){
		if(_cuts[i]->dominateCut(cut)) return false;
	}
	for(unsigned i=0, n=_cuts.size(); i<n; ++i){
		while(i<n && cut->dominateCut(_cuts[i])){
			deleteCutById(i);
			--n;
		}
	}
	return addCutForce(cut, root);
}

void
CirCutList::addUnitCut(unsigned root){
#ifndef NO_HASH
   CirCut* cut = new CirCut(root);
   if(!addCutForce(cut, root)) delete cut;
#else
   addCutForce(new CirCut(root), root);
#endif
}

void
CirCutList::removeRedundant(){
   unsigned n = _cuts.size(), i, j, k;
   for(i=0, j=0; i<n; ++i){
      for(k=0; k<j; ++k){
         if(_cuts[k]->dominateCut(_cuts[i])){
            _cuts[i]->setBoss(_cuts[k]);
            // cout << "deleted: " << *_cuts[i] << endl;
            break;
         }
      }
      if(k == j) _cuts[j++] = _cuts[i];
   }
   _cuts.resize(j);
}

void
CirCutList::removeUnitCut(){
   unsigned n = _cuts.size(), i, j;
   for(i=0, j=0; i<n; ++i){
      if(_cuts[i]->size() > 1) _cuts[j++] = _cuts[i];
   }
   _cuts.resize(j);
}

void
CirCutList::replaceByHash(unsigned root){
#ifndef NO_HASH
   for(unsigned i=0, n=_cuts.size(); i<n; ++i){
      CirCutV cutV(_cuts[i]);
      if(_hashSet->query(cutV)){
         delete _cuts[i];
         _cuts[i] = cutV.getCut();
         _cuts[i]->addRoot(root);
      }
      else{
         _hashSet->insert(cutV);
      }
   }
#endif
}

void
CirCutList::deleteCutById(unsigned id){
   unsigned n = _cuts.size();
   _cuts[id] = _cuts[n-1];
   _cuts.resize(n-1);
}

void
CirCutList::genCutList(unsigned root, bool addRoot){
   _cuts.clear();
   if(addRoot) addUnitCut(root);
}

void
CirCutList::genCutList(CirCutList& cutList, unsigned root, bool addRoot){
   _cuts.clear();
#ifndef NO_HASH
	for(unsigned i=0, n=cutList.size(); i<n; ++i)
      addCutForce(cutList[i], root);
#else
	for(unsigned i=0, n=cutList.size(); i<n; ++i)
      addCutForce(new CirCut(*cutList[i]), root);
#endif
   if(addRoot){
      removeUnitCut();
      addUnitCut(root);
   }
   sort(_cuts.begin(), _cuts.end(), cutCmp);
}

// First generate new cuts
// Then remove redundant cuts
// Finally, replace cuts which are already in hash
//    or insert the cut into hash
// 
void
CirCutList::genCutList(const CirCutList& cutList0, 
		                 const CirCutList& cutList1, 
							  unsigned root, bool addRoot){
   CirCut* cut;
	_cuts.clear();
	for(unsigned i=0, n0=cutList0.size(); i<n0; ++i){
		for(unsigned j=0, n1=cutList1.size(); j<n1; ++j){
			cut = cutList0[i]->merge(cutList1[j]);
			if(cut != 0 && !addCutForce(cut, root)) delete cut;
		}
	}
   if(addRoot){ 
      removeUnitCut();
      addUnitCut(root);
   }
   sort(_cuts.begin(), _cuts.end(), cutCmp);
   removeRedundant();
}

void
CirCutList::initHash(unsigned k)
{
#ifndef NO_HASH
   deleteHash();
   _hashSet = new HashSet<CirCutV>(k);
#endif
}

void
CirCutList::deleteHash()
{
#ifndef NO_HASH
   if(_hashSet != 0){
      HashSet<CirCutV>::iterator it;
      for(it=_hashSet->begin(); it!=_hashSet->end(); ++it)
         delete (*it).getCut();
      
      delete _hashSet;
      _hashSet = 0;
   }
#endif
}

void
CirCutList::printAllCut()
{
#ifndef NO_HASH
   cout << "all cuts:" << endl;
   if(_hashSet != 0){
      HashSet<CirCutV>::iterator it;
      for(it=_hashSet->begin(); it!=_hashSet->end(); ++it){
         CirCut* cut = (*it).getCut();
         cout << *cut;
         cout << "[ ";
         for(unsigned i=0, n=cut->rootSize(); i<n; ++i)
            cout << cut->getRoot(i) << (i==n-1? "" : ",") << " ";
         cout << "]" << endl;
      }
   }
#endif
}

ostream& operator << (ostream& os, const CirCutList& cutList){
	os << "{" << endl;
	for(unsigned i=0, n=cutList._cuts.size(); i<n; ++i){
      // os << "   [" << cutList._cuts[i] << "]";
      os << "   ";
		os << *(cutList._cuts[i]) << (i==n-1? "" : ",") << endl;
   }
	os << "}";
	return os;
}
