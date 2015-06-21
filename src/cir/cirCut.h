#ifndef CIR_CUT_H
#define CIR_CUT_H

#include <iostream>
#include <vector>
#include "cirDef.h"
#include "bddMgr.h"
#include "myHashSet.h"

/************************/
/*   Class definition   */
/************************/
class CirCut;
class CirCutV;
class CirCutList;

/********************/
/*   Class CirCut   */
/********************/
class CirCut
{
public:
   CirCut();
	CirCut(unsigned leaf);
	CirCut(const CirCut& cut);
   ~CirCut();

	// unsigned size() const { return _leaf.size(); }
   unsigned size() const { return _sz; }
   unsigned rootSize() const { return _root.size(); }
   void addRoot(unsigned root) { _root.push_back(root); }
	unsigned getRoot(int idx) const { return _root[idx]; }
	unsigned getSign() const { return _sign; }
	bool     operator == (const CirCut&) const ;
	unsigned        getLeaf     (int idx) const { return _leaf[idx]; }
	unsigned&       operator [] (int idx)       { return _leaf[idx]; }
	const unsigned& operator [] (int idx) const { return _leaf[idx]; }

	CirCut* merge(const CirCut* cut) const ;
	bool containGateId(unsigned gid) const ;
	bool dominateCut(const CirCut* cut) const ;
   void setBoss(CirCut* cut){ _boss = cut; }
   CirCut* getBoss(){ return _boss==this? _boss : _boss->getBoss(); }

	void genCutFunc();
	BddNode getFuncByIdx(int idx) const { return _func[idx]; }
   BddNode getFuncByRoot(unsigned root) const ;
   void setVisit(){ _visited = true; }
   bool isVisit() const { return _visited; }

	// void resetGateNum(){ _nGate = 0; }
	// void incGateNum(){ _nGate++; }
	// unsigned getGateNum() const { return _nGate; }
   void resetGateNum() {}
   void incGateNum() {}
   unsigned getGateNum() const { return 1; }

	static void setMaxCutSize(int s){ _maxCutSize = s; }

	friend ostream& operator << (ostream&, const CirCut&);

private:
	bool addLeafForce(unsigned leaf);
	bool addLeaf     (unsigned leaf);
	double costFunc(int) const ;
	
	IdList		      _root;
	// IdList		    _leaf;        // must be sorted
   unsigned          *_leaf, _sz;   // _leaf must be sorted
	size_t            _sign;         // signature
   CirCut*           _boss;         // cut that dominates this cut
	vector<BddNode>   _func;         // functions of each root
   bool              _visited;      // true if _func is generated
	// unsigned        _nGate;       // # of gates in the cut

	static unsigned   _maxCutSize;

	// TODO: use array instead of vector for _leaf?
};

/************************/
/*   Class CirCutList   */
/************************/
class CirCutList
{
public:
	CirCutList();
	~CirCutList();

	inline unsigned size() const { return _cuts.size(); }
	CirCut* operator [] (int idx) { return _cuts[idx]; }
	const CirCut* operator [] (int idx) const { return _cuts[idx]; }

	void clear(){ _cuts.clear(); }
	bool addCutForce(CirCut* cut, unsigned root);
	bool addCut     (CirCut* cut, unsigned root);
   void addUnitCut(unsigned root);
   void removeRedundant();
   void removeUnitCut();
   void replaceByHash(unsigned root);
	void deleteCutById(unsigned id);
   void genCutList(unsigned root, bool addRoot);
	void genCutList(CirCutList& cutList, unsigned root, bool addRoot);
	void genCutList(const CirCutList& cutList0, const CirCutList& cutList1, unsigned root, bool addRoot);

   friend ostream& operator << (ostream&, const CirCutList&);

private:	
	vector<CirCut*> _cuts;
};

#endif // CIR_CUT_H
