/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
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
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
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
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* _buckets,size_t _numBuckets): _buckets(_buckets), _numBuckets(_numBuckets),_index1(0),_index2(0) {
        while (_index1 < _numBuckets){
          if(_buckets[_index1].size() != 0){ 
            return;
          }
	  _index1++;
        }
        _index1 = _numBuckets;
      }
      ~iterator() {}
      iterator& gote_end(){_index1 = _numBuckets;_index2=0; return(*this); }
      const Data& operator * () const { return _buckets[_index1][_index2]; }
      Data& operator * () { return _buckets[_index1][_index2]; }
      iterator& operator ++ () { 
        if(_index2 < _buckets[_index1].size()-1) {
          _index2++; 
          return(*this);
          }
        while (_index1 < (_numBuckets-1)){
          _index1++;
          if(_buckets[_index1].size() != 0){ 
            _index2 =0 ;
            return(*this);
          }
        } 
        _index1 = _numBuckets;
        _index2=0;
        return (*this); 
      }
      iterator& operator -- () { 
        if(_index2 > 0) {
          _index2--; 
          return(*this);
          }
        size_t tmp = _index1;  
        while(_index1 > 0){
          _index1--;
          if(_buckets[_index1].size() != 0) {
            _index2 = _buckets[_index1].size()-1;
            return (*this);
          }
        } 
        _index1 = tmp;
        return (*this); 
      }
      iterator operator ++ (int) { iterator ret = (*this); ++(*this); return ret; }
      iterator operator -- (int) { iterator ret = (*this); --(*this); return ret; }
      bool operator != (const iterator& i) const { if(i._buckets == _buckets && i._numBuckets == _numBuckets && i._index1 == _index1 && i._index2 == _index2) return false; else return true; }
      bool operator == (const iterator& i) const { if((*this) != i) return false; else return true; }
      iterator& operator = (const iterator& i){ _buckets = i._buckets; _numBuckets =i._numBuckets; _index1 = i._index1; _index2 = i._index2;}

   private:
      vector<Data>* _buckets;
      size_t _numBuckets,_index1,_index2;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { return iterator(_buckets,_numBuckets); }
   // Pass the end
   iterator end() const { return iterator(_buckets,_numBuckets).gote_end(); }
   // return true if no valid data
   bool empty() const { return (begin() == end()); }
   // number of valid data
   size_t size() const { 
    size_t s = 0;
    iterator i = iterator(_buckets,_numBuckets);
    while(i != end()){
      i++;
      s++;
    }
    return s; 
    }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
    for(size_t i=0; i <_buckets[bucketNum(d)].size();i++){
      if(_buckets[bucketNum(d)][i] ==  d){
        return true;
      }      
    }
    return false; }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
    for(size_t i=0; i <_buckets[bucketNum(d)].size();i++){
      if(_buckets[bucketNum(d)][i] ==  d){
        d = _buckets[bucketNum(d)][i];
        return true;
      }      
    }
    return false; }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
    for(size_t i=0; i <_buckets[bucketNum(d)].size();i++){
      if(_buckets[bucketNum(d)][i] ==  d){
        _buckets[bucketNum(d)][i] = d;
        return true;
      }      
    }
    _buckets[bucketNum(d)].push_back(d);
    return false; 
    }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
    for(size_t i=0; i <_buckets[bucketNum(d)].size();i++){
      if(_buckets[bucketNum(d)][i] ==  d){
        return false;
      }      
    }
    _buckets[bucketNum(d)].push_back(d);
    return true; 
  }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
    for(size_t i=0; i <_buckets[bucketNum(d)].size();i++){
      if(_buckets[bucketNum(d)][i] ==  d){
        _buckets[bucketNum(d)].erase(_buckets[bucketNum(d)].begin()+i);
        return true;
      }      
    }
    return false; 
    }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
