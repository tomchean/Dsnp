/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   char flag;
   int value;
   string key;
   is >> flag;
   while(true)
   {
    is >> key >>flag  >> value >> flag;
    if(key == "}" )break;
    key.erase (0,1);
    key.pop_back();
    DBJsonElem _element( key , value );
    j.add(_element);
    if(flag == '}')break;
   }
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os <<"{\n";
   for (size_t i = 0; i < j.size()-1; i++ ){
     os << j[i];
     os <<" ,";
   }
   os << j[j.size()-1] << "\n}";
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
  setifopen(false);
  _obj.clear();

}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for(size_t i = 0 ; i < size() ; i++){
     if(_obj[i].key() == elm.key()){
       return false ;
     }
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if(!empty()){
    float ave = 0 ;
    for(size_t i = 0 ; i < size() ; i++){
      ave += _obj[i].value();
    }
    ave = ave/size();
    return ave;
   }
   else return NAN;
   
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   if(!empty()){
   int maxN = INT_MIN;
   for(size_t i = 0 ; i < size() ; i++){
      if (_obj[i].value() > maxN) {
        maxN = _obj[i].value();
        idx = i;
      } 
    }
   return  maxN;
   }
   else{ 
    idx = size();
    return INT_MIN;
   }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   if(!empty()){
   int minN = INT_MAX;
   for(size_t i = 0 ; i < size() ; i++){
      if (_obj[i].value() < minN ) {
        minN  = _obj[i].value();
        idx = i;
      } 
    }
   return  minN ;
   }
   else{ 
    idx = size();
    return INT_MAX;
   }
   
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(!empty()){
   int s = 0;
   for(size_t i = 0 ; i < size() ; i++){
      s += _obj[i].value() ;
    }
   return s;
   }
   else  return 0;
}

