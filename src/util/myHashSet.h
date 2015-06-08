/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet() : _numBuckets(0), _buckets(0) {}
   HashSet(size_t b) : _numBuckets(0), _buckets(0) { init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   // [Note] Comment out for TODO
   // (_bId, _bnId) range from (0, 0) to (_numBuckets, 0)
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      // define constructor and its arguments on your own
      iterator(HashSet<Data>* h = 0, size_t b = 0, size_t bn = 0)
      : _hash(h), _bId(b), _bnId(bn) {}
      iterator(const iterator& i)
      : _hash(i._hash), _bId(i._bId), _bnId(i._bnId) {}
      ~iterator() {} // Should NOT delete Data

      const Data& operator * () const { return (*_hash)[_bId][_bnId]; }
      Data& operator * () { return (*_hash)[_bId][_bnId]; }
      iterator& operator ++ () {
         if (_hash == 0) return (*this);
         if (_bId >= _hash->_numBuckets) return (*this);
         if (++_bnId >= (*_hash)[_bId].size()) {
            while ((++_bId < _hash->_numBuckets) && (*_hash)[_bId].empty());
            _bnId = 0;
         }
         return (*this);
      }
      iterator& operator -- () {
         if (_hash == 0) return (*this);
         if (_bnId == 0) {
            if (_bId == 0) return (*this);
            while ((*_hash)[--_bId].empty())
               if (_bId == 0) return (*this);
            _bnId = (*_hash)[_bId].size() - 1;
         }
         else
            --_bnId;
         return (*this);
      }
      iterator operator ++ (int) { iterator li=(*this); ++(*this); return li; }
      iterator operator -- (int) { iterator li=(*this); --(*this); return li; }

      iterator& operator = (const iterator& i) {
         _hash = i._hash; _bId = i._bId; _bnId = i._bnId; return (*this); }

      bool operator != (const iterator& i) const { return !(*this == i); }
      bool operator == (const iterator& i) const {
         return (_hash == i._hash && _bId == i._bId && _bnId == i._bnId); }

   private:
      HashSet<Data>*    _hash;
      size_t            _bId;
      size_t            _bnId;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      if (_buckets == 0) return end();
      for (size_t i = 0; i < _numBuckets; ++i)
         if (!_buckets[i].empty())
            return iterator(const_cast<HashSet<Data>*>(this), i, 0);
      return end();
   }
   // Pass the end
   iterator end() const {
      return iterator(const_cast<HashSet<Data>*>(this), _numBuckets, 0);
   }
   // return true if no valid data
   bool empty() const {
      for (size_t i = 0; i < _numBuckets; ++i)
         if (_buckets[i].size() != 0) return false;
      return true;
   }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for (size_t i = 0; i < _numBuckets; ++i) s += _buckets[i].size();
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t b = bucketNum(d);
      for (size_t i = 0, bn = _buckets[b].size(); i < bn; ++i)
         if (_buckets[b][i] == d)
            return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t b = bucketNum(d);
      for (size_t i = 0, bn = _buckets[b].size(); i < bn; ++i)
         if (_buckets[b][i] == d) {
            d = _buckets[b][i];
            return true;
         }
      return false;
   }

   // update the entry in hash that is equal to d
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t b = bucketNum(d);
      for (size_t i = 0, bn = _buckets[b].size(); i < bn; ++i)
         if (_buckets[b][i] == d) {
            _buckets[b][i] = d;
            return true;
         }
      _buckets[b].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t b = bucketNum(d);
      for (size_t i = 0, bn = _buckets[b].size(); i < bn; ++i)
         if (_buckets[b][i] == d) return false;
      _buckets[b].push_back(d);
      return true;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
//cout << "key = " << d() << "; #B = " << d() % _numBuckets << endl;
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
