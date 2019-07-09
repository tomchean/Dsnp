/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

    BSTreeNode(const T& d,BSTreeNode<T>* p = 0 , BSTreeNode<T>* l = 0, BSTreeNode<T>* r=0):
     _data(d),_left(l),_right(r),_parent(p) { }

    T _data;
    BSTreeNode<T>* _left;
    BSTreeNode<T>* _right;
    BSTreeNode<T>* _parent;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   public:
   BSTree() { _root = 0;  _dummy = new BSTreeNode<T>(T()); _min = _dummy;  }
   ~BSTree(){ clear(); delete _dummy; };
     
   enum leaftype {
      leaf_both,leaf_left,leaf_right,leaf_none
   };
   class iterator
   {
      friend class BSTree;
      
      public:
         iterator(BSTreeNode<T>* n= 0): _node(n) {}
         iterator(const iterator& i) : _node(i._node) {}
         ~iterator() {} // Should NOT delete _node

         const T& operator * () const { return _node->_data; }
         T& operator * () { return _node->_data; }
         iterator& operator ++ () 
         {  
            iterator tmp = *this;
            if(_node->_right !=0){  // has right child 
              _node = _node->_right;
              while(_node->_left !=0){
                _node = _node->_left;
              }
              return *(this);
            }
            else {
              if( _node->_data >= _node->_parent->_data){
                while( _node->_parent != 0){
                  if( _node->_data >= _node->_parent->_data){
                    _node = _node->_parent;
                  }
                  else {
                    _node = _node->_parent;
                    return *(this);
                  }
                }
                *this = tmp;
                return *this;
              }
              else {  // is left child
                _node = _node->_parent;
                return *(this);
              }
            }
          }
         iterator operator ++ (int) 
         { 
            iterator ret = *(this);
            ++(*this);
            return ret;
          }
         iterator& operator -- () 
         {
            iterator ret = *(this);
            if(_node->_left !=0){  // has left child 
              _node = _node->_left;
              while(_node->_right !=0){
                _node = _node->_right;
              }
              return *(this);
            }
            else {
              if( _node->_data < _node->_parent->_data){ //left child
                while( _node->_parent != 0){
                  if( _node->_data < _node->_parent->_data){
                    _node = _node->_parent;
                  }
                  else {
                    _node = _node->_parent;
                    return *(this);
                  }
                }
                *(this) = ret;
                return *this;
              }
              else {  // is right child
                _node = _node->_parent;
                return *(this);
              }
            }
          }
         iterator operator -- (int) 
          {
            iterator ret = *(this);
            --(*this);
            return ret;
          }
         iterator& operator = (const iterator& i) {this->_node = i._node; return *this; }

         bool operator != (const iterator& i) const { if (_node == i._node)return false; else return true;  }
         bool operator == (const iterator& i) const { if(i!=*(this))return false;else return true; }

      private:
         BSTreeNode<T>* _node;
   };
   iterator begin() const { return iterator(_min);} 
   iterator end() const  { return iterator(_dummy);} 
   iterator find(const T& x) const 
   {
      if(_root == 0) return end();
      iterator index ;
      index._node =_root;
      while( index != end() ){
        if( x > *index){
          if(index._node->_right == 0) { return end(); }
          index._node = index._node->_right;
        }
        else if(x < *index){
          if(index._node->_left == 0) {return  end(); }
          index._node = index._node->_left;
        }
        else return index;
      }
      return end();
   }
   size_t size() const 
    {
      if(_root == 0) return 0;
      iterator tmp ;
      tmp._node = _min;
      size_t ret =0;
      while(tmp != end()){
        ret ++;
        tmp++;
      }
      return ret;
    }
   leaftype leaf(iterator& pos) const  // 查看note 型態
   {
      if( pos._node->_left==0){
         if( pos._node->_right==0) return leaf_none;
         else return leaf_right;
      }
      else {
         if( pos._node->_right==0) return leaf_left;
         else return leaf_both;
      }
    }
   bool empty() const { return (_root == 0); }
   iterator succesor( iterator i){
      i._node = i._node->_right; 
      while(i._node->_left != 0 ){
        i._node = i._node->_left;
      }
      return i;
   }
   bool erase(iterator pos) 
    { 
      if(empty()) return false;
      if(pos._node == _min) {pop_front(); return true;}
      if(pos._node == _dummy->_parent) { pop_back(); return true;}      
      leaftype type = leaf(pos);
      switch(type){
        case leaf_both :
          {
            iterator tmp = succesor(pos);
            if(tmp._node->_parent != pos._node){ 
              if(tmp._node->_right != 0){ //succesor has  right child
                tmp._node->_parent->_left = tmp._node->_right;
                tmp._node->_right->_parent = tmp._node->_parent;
              }
              else {  //succesor is leaf
                tmp._node->_parent->_left =0; 
              }
              tmp._node->_left = pos._node->_left;
              pos._node->_left->_parent = tmp._node;
              tmp._node->_right = pos._node->_right;
              pos._node->_right->_parent = tmp._node;
            }
            else{
              tmp._node->_left = pos._node->_left;
              pos._node->_left->_parent = tmp._node;
            }        
            if(pos._node != _root ){
              tmp._node->_parent = pos._node->_parent;
              if(pos._node->_data >= pos._node->_parent->_data){
                pos._node->_parent->_right = tmp._node;
              }
              else{
                pos._node->_parent->_left = tmp._node;
              }
            }
            else{
              _root = tmp._node;
              tmp._node->_parent =0;
            }
            delete pos._node;
            break;
          }
        case leaf_none: {
          if(pos._node->_data >= pos._node->_parent->_data){
            pos._node->_parent->_right = 0;
          }
          else{
            pos._node->_parent->_left = 0;
          }
          delete pos._node;
          break;
          }
        case leaf_left:{
          pos._node->_left->_parent = pos._node->_parent;
          if(pos._node->_data >= pos._node->_parent->_data){
            pos._node->_parent->_right = pos._node->_left;
          }
          else{
            pos._node->_parent->_left = pos._node->_left;
          }
          delete pos._node;
          break;
        }
        case leaf_right :{
          pos._node->_right->_parent = pos._node->_parent;
          if(pos._node->_data >= pos._node->_parent->_data) { // is right child
            pos._node->_parent->_right = pos._node->_right;
          }
          else { // is left child
            pos._node->_parent->_left = pos._node->_right;           
          }
          delete pos._node;
          break;
        }  
      }      
      return true;
    }

   bool erase(const T& x) { iterator tmp = find(x); if(tmp != end()) {erase(tmp); return true;} else return false; }
   void pop_back() 
   { 
     if(!empty()){
       if(_dummy->_parent == _root){
        BSTreeNode<T>* tmp = _root;
        if(_root->_left != 0){
          _dummy->_parent = _root->_left;
          _root = _dummy->_parent;
          while(_dummy->_parent->_right != 0){
            _dummy->_parent = _dummy->_parent->_right;
          }
          _dummy->_parent->_right = _dummy;
          _dummy->_data = _dummy->_parent->_data;
        }
        else{
          _root =0;
          _min = _dummy;
        }
        delete tmp;
       }
      else{
        BSTreeNode<T>* tmp = _dummy->_parent;
        if(_dummy->_parent->_left != 0){
          _dummy->_parent->_parent->_right = _dummy->_parent->_left;
          _dummy->_parent->_left->_parent = _dummy->_parent->_parent;
          _dummy->_parent = _dummy->_parent->_left;
          while(_dummy->_parent->_right !=0) {
            _dummy->_parent = _dummy->_parent->_right;
          }
          _dummy->_parent->_right = _dummy;
          _dummy->_data = _dummy->_parent->_data;
        }
        else{
          _dummy->_parent->_parent->_right = _dummy ;
          _dummy->_parent = _dummy->_parent->_parent;
          _dummy->_data = _dummy->_parent->_data;
        }
        delete tmp;
      }
    }
   } 
   void pop_front() 
   { 
    if(!empty()){
      if( _root == _min){
        BSTreeNode<T>* tmp = _min;
        if(_min->_right == _dummy){
          _root = 0;
           _min = _dummy;
        }
        else{
          _root = _min->_right;
          _min = _root;
	  _root->_parent =0;
          while( _min->_left !=0){
            _min = _min->_left;
          }
        }
        delete tmp;
      }
      else{
        BSTreeNode<T>* tmp = _min;
        if(_min->_right !=0){
          _min->_parent->_left = _min->_right;
          _min->_right->_parent = _min->_parent;
          _min = _min->_right;
          while(_min->_left !=0) {
            _min = _min->_left;
          }
        }
        else{
          _min->_parent->_left =0;
          _min = _min->_parent;
        }        
        delete tmp;
      }
    }
  }
   void insert(const T& x) { push_back(x);}
   void push_back(const T& x) 
   {  
      bool finish = false;
      bool min = true;

      BSTreeNode<T>* _apend = new BSTreeNode<T>(x);
      if(_root == 0){
        _root = _apend;
        _root->_right = _dummy;
        _dummy->_parent = _root;
        _min = _root;
        _dummy->_data = _dummy->_parent->_data;
      }
      else {
          iterator index ;
          index._node = _root;
          while( !finish ){
            if( x >= *index){
              min = false;
              if(index._node->_right == 0 ) {
                index._node->_right = _apend;
                _apend->_parent = index._node;
                finish = true;
              }
              if(index._node->_right == _dummy){
                index._node->_right = _apend;
                _apend->_parent = index._node;
                _apend->_right = _dummy;
                _dummy->_parent = _apend;
                _dummy->_data = _dummy->_parent->_data;
                finish = true;
              }
              index._node = index._node->_right;
            }
            else {
              if(index._node->_left == 0) {
                index._node->_left = _apend;
                _apend->_parent = index._node;
                finish = true;
              }
              index._node = index._node->_left;
            }
          }
          if(min) _min = _apend;
      }
   } 
   void clear()
    { 
      if (!empty()){
        size_t sz = size(); 
        for(size_t i =0;i<sz;i++){ 
          pop_front();
        }
        _min = _dummy;
      } 
    }
   void sort() const{ ;} //do nothing
   void print() const { ;} 

   private:
      BSTreeNode<T>* _root;
      BSTreeNode<T>* _min;
      BSTreeNode<T>* _dummy;
};

#endif // BST_H
