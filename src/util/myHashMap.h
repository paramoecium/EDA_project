/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap() : _numBuckets(0), _buckets(0) {}
   HashMap(size_t b) : _numBuckets(0), _buckets(0) { init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   // (_bId, _bnId) range from (0, 0) to (_numBuckets, 0)
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(vector<HashNode>* b, size_t n, size_t w, size_t l):
      _buck(b), _numBuck(n), _whB(w), _loc(l) {}
      iterator(const iterator& i): 
      _buck(i._buck), _numBuck(i._numBuck), _whB(i._whB), _loc(i._loc) {}
      ~iterator() {} 

      const HashNode& operator * () const { return (_buck + _whB)->at(_loc); }
      HashNode& operator * () { return (_buck + _whB)->at(_loc); }
      iterator& operator ++ () {
         ++_loc;
         if (_whB + 1 == _numBuck) 
            return (*this);
         while ((_whB + 1 != _numBuck) && (_loc == (_buck + _whB)->size())){
            _loc = 0;
            ++_whB;
         }
         return *(this); 
      }
      iterator operator ++ (int) { iterator temp = *(this); ++(*this); return temp; }
      iterator& operator -- () {  
         --_loc;
         while (_loc < 0)
            _loc = (_buck + --_whB)->size() - 1;
         return *(this); 
      }
      iterator operator -- (int) { iterator temp = *(this); --(*this); return temp; }
      iterator& operator = (const iterator& i) {
         _buck = i._buck;
         _numBuck = i._numBuck;
         _whB = i._whB;
         _loc = i._loc;
         return (*this); 
      }
      bool operator != (const iterator& i) const { return !(*this == i); }
      bool operator == (const iterator& i) const { 
         return ((_whB == i._whB) && (_loc == i._loc) && 
                 (_numBuck == i._numBuck) && (_buck == i._buck)); 
      }

   private:
      vector<HashNode>* _buck;
      size_t _numBuck;
      size_t _whB;    // in which bucket
      int _loc;       // _l location in the bucket

   private:
   };
   // end of class definition;

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      size_t i = 0; 
      while ((_buckets + i)->size() == 0 && i != (_numBuckets - 1))
         ++i;
      iterator temp(_buckets, _numBuckets, i, 0);
      return temp; 
   }
   // Pass the end
   iterator end() const { 
      iterator temp(_buckets, _numBuckets, _numBuckets - 1, (_buckets + _numBuckets -1)->size());
      return temp;      
   }
   // return true if no valid data
   bool empty() const { return (size() == 0); }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
      for (size_t i = 0; i < _numBuckets; ++i)
         s += (_buckets + i)->size();
      return s;
    }

   // check if d is in the hash...
   // (check whether key is the same)
   // if yes, return true;
   // else return false;
   bool check(const HashNode& d) const { 
      size_t n = bucketNum(d.first);
      assert(n < _numBuckets);
      for (size_t i = 0; i < (_buckets + n)->size(); ++i)
         if (((_buckets + n)->at(i)).first == d.first) return true;
      return false; 
   }

   // query if d's key is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(HashNode& d) const { 
      size_t n = bucketNum(d.first);
      for (size_t i = 0; i < (_buckets + n)->size(); ++i){
         if (((_buckets + n)->at(i)).first == d.first){
            d.second = ((_buckets + n)->at(i)).second;
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashNode& d) { 
      size_t n = bucketNum(d.first);
      for (size_t i = 0; i < (_buckets + n)->size(); ++i){
         if ((_buckets + n)->at(i) == d){
            (_buckets + n)->at(i) = d;
            return true;
         }
      }
      insert(d);
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const HashNode& d) {  
      if (check(d)) return false;
      size_t i = bucketNum(d.first);
      (_buckets + i)->push_back(d);
      return true;  
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


#endif // MY_HASH_H
