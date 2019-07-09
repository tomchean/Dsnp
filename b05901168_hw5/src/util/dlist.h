/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next ;return *(this); }
      iterator operator ++ (int) { iterator tmp = *this; _node = _node->_next ;return tmp; }
      iterator& operator -- () { _node = _node->_prev ;return *(this); }
      iterator operator -- (int) { iterator tmp = *this; _node = _node->_prev ;return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node;return *(this); }

      bool operator != (const iterator& i) const { if (_node == i._node)return false; else return true; }
      bool operator == (const iterator& i) const { if(i!=*(this))return false;else return true; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {  iterator ret;ret._node = _head; return ret; }
   iterator end() const { iterator ret;ret._node = (_head->_prev); return ret; } 
   bool empty() const { if(_head->_prev == _head) return true; else return false; }
   size_t size() const {  if(empty())return 0; 
                          else{
                            size_t ret=0;
                            iterator tmp = (this)->begin();
                            while(tmp != end()){
                              ret++;
                              tmp++;
                            }
                            return ret;
                          } 
                        }

   void push_back(const T& x) { if(empty()){
                                  DListNode<T>* ret = new DListNode<T>(x,_head,_head);
                                  _head->_prev = ret;
                                  _head->_next = ret;
                                  _head = ret;
                                }
                                else{
                                  DListNode<T>* ret = new DListNode<T>(x,(_head->_prev)->_prev,_head->_prev);
                                  (_head->_prev)->_prev->_next = ret;
                                  (_head->_prev)->_prev = ret;
                                }
                              }
   void pop_front(){ if (!empty()){ DListNode<T>* tmp = _head; _head = _head->_next; _head->_prev = tmp->_prev; (tmp->_prev)->_next= _head;delete tmp; }}
   void pop_back() { 
     if (!empty()){

      DListNode<T>* tmp = (_head->_prev)->_prev; 
      (tmp->_prev)->_next = tmp->_next;
      (tmp->_next)->_prev = tmp->_prev;
      if(tmp->_prev->_next == tmp->_prev) _head = tmp->_prev;
      delete tmp;
      
      } 
    }

   // return false if nothing to erase
   bool erase(iterator pos) { 
    if (!empty()) {  
      if(pos._node == _head){
        pop_front(); 
        return true;
      }
      else{
        pos._node->_prev->_next = pos._node->_next;
        pos._node->_next->_prev = pos._node->_prev;
        delete pos._node; 
        return true;
      }
    }       
    else  return false; 
  }
   bool erase(const T& x) {  if((find(x))!=end()) {erase(find(x)); return true;} else return false; }

   iterator find(const T& x) { for (iterator tmp =begin(); tmp != end(); tmp++ ){ if((tmp._node)->_data == x){return tmp ;}}return end();  }

   void clear() { if (!empty()){
                    size_t sz = size(); 
                    for(size_t i =0;i<sz;i++){ 
                      pop_back();
                    }  
                  }  // delete all nodes except for the dummy node
                }

    void sort() const { 
      bubble_sort(); 
      _isSorted =true;
    }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
  void bubble_sort() const {
    bool finish =false;
    iterator tmp = end();
    tmp--;
    while(!finish){
      finish = true;
      for(iterator i = begin(); i != tmp ; i++ ){
        if(i._node->_data > i._node->_next->_data ){
          change(i);
          finish = false;
        }
      }
      tmp--;
    }
  } 
  void change(iterator& i) const{
    T tmp = i._node->_data;
    i._node->_data = i._node->_next->_data;
    i._node->_next->_data =tmp;
  }


};

#endif // DLIST_H
