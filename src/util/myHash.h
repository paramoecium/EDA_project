/****************************************************************************
  FileName     [ myHash.h ]
  PackageName  [ util ]
  Synopsis     [ Define Hash and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-2013 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_H
#define MY_HASH_H

#include <vector>

using namespace std;

//--------------------
// Define Hash classes
//--------------------
// To use Hash ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) { return true; }
// 
// private:
// };
//
/*
class Key  
{
public:
   Key() {
      _netS = ptr->_inPinLeft->_net;
      _invS = ptr->_invLeft;
      _netL = ptr->_inPinRight->_net;
      _invL = ptr->_invRight;
      if (_netS<_netL) {
      
      }
   }
   size_t operator() () const { 
      return 0; 
   }
   size_t operator() (AndGate* ptr) const { 
      size_t net2 = (ptr->_inPinRight->_net)+((ptr->_invRight)?(sizeof(size_t)/4):0);
      return 0; 
   }
   bool operator == (const Key& k) { return true; }
private:
   _netS;   //small
   _netL;   //large
   _invS;
   _invL;
};
*/
template <class HashKey, class HashData>
class Hash
{
typedef pair<HashKey, HashData> HashNode;
typedef typename vector<HashNode>::iterator BucketIter;

public:
   Hash() : _numBuckets(0), _buckets(0) {}
   Hash(size_t b) : _numBuckets(0), _buckets(0) { init(b); }
   ~Hash() { reset(); delete [] _buckets; }

   // TODO: implement the Hash<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the Hash
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
      friend class Hash<HashKey, HashData>;
   public:
      iterator(vector<HashNode>* theBucket, BucketIter iter, Hash* hashPtr)
         : _theBucket(theBucket), _iter(iter), _hashPtr(hashPtr) {}
      iterator(const iterator& i): _theBucket(i._theBucket), _iter(i._iter) {}
      ~iterator() {} // Should NOT delete _node

      const HashNode& operator * () const { return (*_iter); }
      HashNode& operator * () { return (*_iter); }
      iterator& operator ++ () {  //++/--iterator
         if (this != _hashPtr.end()) {
            _iter++;
            if (_iter == _theBucket->end()) toNextBucket;
         }
         return (*this); 
      }
      iterator operator ++ (int) { 
         vector<HashNode>* temp1 = _theBucket;
         BucketIter temp2 = _iter; 
         if (this != _hashPtr.end()) {
            _iter++;
            if (_iter == _theBucket->end()) toNextBucket;
         }
         return iterator(temp1,temp2,_hashPtr); 
      }
      iterator& operator -- () {  //++/--iterator
         if (this != _hashPtr.begin()) {
            if (_iter == _theBucket->begin()) toPrevBucket;
            else _iter--;
         }
         return (*this); 
      }
      iterator operator -- (int) { 
         vector<HashNode>* temp1 = _theBucket;
         BucketIter temp2 = _iter; 
         if (this != _hashPtr.begin()) {
            if (_iter == _theBucket->begin()) toPrevBucket;
            else _iter--;
         }
         return iterator(temp1,temp2,_hashPtr); 
      }
      iterator& operator = (const iterator& i) { 
         _theBucket = i._theBucket; 
         _iter = i._iter;
         return (*this); 
      }
      bool operator != (const iterator& i) const { return (_iter!=i._iter); }
      bool operator == (const iterator& i) const { return (_iter==i._iter); }

   private:
      void toNextBucket() {_theBucket++; _iter = _theBucket->begin();}
      void toPrevBucket() {_theBucket--; _iter = _theBucket->end()-1;}

      vector<HashNode>*    _theBucket;
      BucketIter           _iter;
      Hash*                _hashPtr;
   };

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      for (size_t i=0; i < _numBuckets; i++) {
         if (_buckets[i].empty() == false)
            return iterator(_buckets+i, _buckets[i].begin(), this);
      }
      return iterator(_buckets+_numBuckets, _buckets[_numBuckets].end(), this);
   }
   // Pass the end
   iterator end() const {
      int i = (int)_numBuckets-1;
      for ( ; i >= 0; i--) {
         if (_buckets[i].empty() == false) break;
      }
      return iterator(_buckets+i, _buckets[i].end(),this);
   }
   // return true if no valid data
   bool empty() const { return (begin()==end()); }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
       for (size_t i=0; i < _numBuckets; i++) s+=_buckets[i].size();
      return s; 
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   void init(size_t b) { 
      _numBuckets = b;
      _buckets = new vector<HashNode>[b];
   }
   void reset() { 
      for (size_t i=0; i < _numBuckets; i++) _buckets[i].clear();
   }

   // check if k is in the hash...
   // if yes, update n and return true;
   // else return false;
   bool check(const HashKey& k, HashData& n) { 
      BucketIter it = _buckets[bucketNum(k)].begin();
      for (; it != _buckets[bucketNum(k)].end(); ++it) {
         if (k == (*it).first) {
            n = (*it).second;
            return true;
         }
      }
      return false; 
   }

   // return true if inserted successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) { 
      BucketIter it = _buckets[bucketNum(k)].begin();
      for (; it != _buckets[bucketNum(k)].end(); ++it) {
         if (k == (*it).first) return false;
      }
      _buckets[bucketNum(k)].insert(it, HashNode(k,d) );
      return true; 
   }

   // return true if inserted successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> still do the insertion
   bool replaceInsert(const HashKey& k, const HashData& d) { 
      bool alreadyIn = false;
      BucketIter it = _buckets[bucketNum(k)].begin();
      for (; it != _buckets[bucketNum(k)].end(); ++it) {
         if (k == (*it).first) alreadyIn = true;
      }
      _buckets[bucketNum(k)].insert(it, HashNode(k,d) );
      return alreadyIn; 
   }

   // Need to be sure that k is not in the hash
   void forceInsert(const HashKey& k, const HashData& d) {
      _buckets[bucketNum(k)].push_back( HashNode(k,d) );
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { }
   void reset() { }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const { return false; }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) { }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
