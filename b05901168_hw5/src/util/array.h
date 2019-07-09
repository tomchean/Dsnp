/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node = _node+1;return *(this); }
      iterator operator ++ (int) { iterator ret; ret._node =_node ;_node = _node+1; return ret; }
      iterator& operator -- () { _node = _node-1 ;return (*this); }
      iterator operator -- (int) { iterator ret; ret._node =_node ;_node = _node-1; return ret; }

      iterator operator + (int i) const { iterator ret; ret._node =_node + i; return ret; }
      iterator& operator += (int i) { _node = _node +i;return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node;return (*this); }

      bool operator != (const iterator& i) const { if (i._node == _node)return false; else return true; }
      bool operator == (const iterator& i) const { if (*(this) != i ) return false; else return true; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { if(_capacity ==0) return 0; iterator ret; ret._node = _data; return ret; }
   iterator end() const {  if(_capacity ==0) return 0; iterator ret;ret._node = _data; ret += (_size) ; return ret; }
   bool empty() const { if (_size != 0)return false; else return true; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) { if(_size == _capacity) expand();
                                *(_data+_size) =x;
                                 _size+=1;
                              }
   void pop_front() { if (!empty()) {_data[0] = _data[_size-1]; _size -=1;}}
   void pop_back() {if (!empty()) _size -=1; }
  
   bool erase(iterator pos) { if (!empty()){*(pos._node) = _data[_size-1]; _size-=1; return true;} else return false; }
   bool erase(const T& x) {  iterator tmp = find(x); if (tmp!=end()) {erase(tmp);return true; } else return false;}

   iterator find(const T& x){ for (iterator tmp = begin(); tmp != end(); tmp++ ){
                                if(*(tmp._node) == x) return tmp;
                              }
                              return end(); 
                            }

   void clear() { _size =0; }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void expand(){
      T* add;
      if(_capacity==0) {
        add = new T[1];
        _capacity =1;
        _data = add;
      }
      else{
        add = new T[_capacity*2];
        _capacity =_capacity*2;
        for(size_t i=0; i< _size ; i++){
          add[i] = _data[i];
        }
        delete[] _data;
        _data = add; 
      }
   }
};

#endif // ARRAY_H
