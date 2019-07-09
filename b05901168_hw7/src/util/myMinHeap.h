/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data.front(); }
   void insert(const Data& d) {
    size_t s = _data.size();
    _data.push_back(d);
    while(s>0){
      int parent = (s-1)/2;
      if( _data[parent] < d)
        break;
      _data[s] = _data[parent];
      s = parent;
    }
    _data[s] = d;
  }
   void delMin() {
      delData(0); 
    }
   void delData(size_t i) {
    size_t s = _data.size();
    size_t rec =2*i+1;
    while( rec< s){
      if(rec != s-1){
        if(_data[rec+1] < _data[rec]) ++rec;
      } 
      if( _data[s-1] < _data[rec]) break;
      _data[i] = _data[rec];
      i = rec;
      rec = 2*rec+1;
    }
    _data[i] = _data[s-1];
    _data.pop_back();
  }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
