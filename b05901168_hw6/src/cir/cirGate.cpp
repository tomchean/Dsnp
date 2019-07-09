/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
  cout<<"==================================================\n"
          <<"= UNDEF(("<<ID<<")"<<", line 0\n"
          <<"==================================================\n";
}

void
CirGate::reportFanin(int level) 
{
  assert (level >= 0);
  visit=true;
  cout<<getTypeStr()<<" "<<ID<<endl;
  const vector<fan>* tmp;
  if( getfanin(tmp)) {
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        report_fanin_recur((*tmp)[i].gate,1,level,(*tmp)[i].invert);  
      }
      else {
        cout<<(*tmp)[i].gate->getTypeStr()<<" "<<(*tmp)[i].gate->getid()<<" (*)"<<endl;
      }
    }
  }
  reset_fanin(level);
}

void
CirGate::reset_fanin(int level){
  visit=false;
  if(level ==0) return;
  const vector<fan>* tmp;
  if( getfanin(tmp)) {
    for(unsigned i =0;i<(*tmp).size();i++){
      (*tmp)[i].gate->reset_fanin(level-1);       
    }
  }
}

void
CirGate::reset_fanout(int level){
  visit=false;
  if(level ==0) return;
  const vector<fan>* tmp;
  if( getfanout(tmp)) {
    for(unsigned i =0;i<(*tmp).size();i++){
      (*tmp)[i].gate->reset_fanout(level-1);       
    }
  }
}

void 
CirGate::report_fanin_recur(CirGate* gate,unsigned space,int level,bool inv) {
  for(unsigned i=0;i<space;i++){
    cout<<"  ";
  }
  if(inv) cout<<"!";
  cout<<gate->getTypeStr()<<" "<<gate->getid()<<endl;
  if(level==1) return;
  gate->visit=true;
  const vector<fan>* tmp;
  const vector<fan>* tmp1;
  if( gate->getfanin(tmp)) {
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        report_fanin_recur((*tmp)[i].gate,space+1,level-1,(*tmp)[i].invert);  
      }
      else {
        for(unsigned i=0;i<=space;i++){
          cout<<"  ";
        }
        if((*tmp)[i].invert){cout<<"!";}
        cout<<(*tmp)[i].gate->getTypeStr()<<" "<<(*tmp)[i].gate->getid();
        if((*tmp)[i].gate->getfanin(tmp1) && level != 2) {cout <<" (*)";}
        cout<<endl;
      }
    }
  }
}

void
CirGate::reportFanout(int level) {
  assert (level >= 0);
  visit=true;
  cout<<getTypeStr()<<" "<<ID<<endl;
  const vector<fan>* tmp;
  if( getfanout(tmp)) {
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        report_fanout_recur((*tmp)[i].gate,1,level,(*tmp)[i].invert);  
      }
      else {
        cout<<(*tmp)[i].gate->getTypeStr()<<" "<<(*tmp)[i].gate->getid()<<" (*)"<<endl;
      }
    }
  }
  reset_fanout(level); 
}

void 
CirGate::report_fanout_recur(CirGate* gate,unsigned space,int level,bool inv) {

  for(unsigned i=0;i<space;i++){
    cout<<"  ";
  }
  if(inv) cout<<"!";
  cout<<gate->getTypeStr()<<" "<<gate->getid()<<endl;
  if(level==1) return;
  gate->visit=true;
  const vector<fan>* tmp;
  const vector<fan>* tmp1;
  if( gate->getfanout(tmp)) {
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        report_fanout_recur((*tmp)[i].gate,space+1,level-1,(*tmp)[i].invert);  
      }
      else {
        for(unsigned i=0;i<=space;i++){
          cout<<"  ";
        }
        if((*tmp)[i].invert){cout<<"!";}
        cout<<(*tmp)[i].gate->getTypeStr()<<" "<<(*tmp)[i].gate->getid();
        if((*tmp)[i].gate->getfanout(tmp1) && level != 2) {cout <<" (*)";}
        cout<<endl;
      }
    }
  }
}





void
CirGate::DFS(unsigned& time) {
  const vector<fan>* tmp;
  if( getfanin(tmp)){
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        dfsviset((*tmp)[i].gate,time);
      }
    }
    cout<<"["<<time<<"] ";
    printGate();
    ++time; 
  }
  
}

void CirGate::dfsviset(CirGate* gate,unsigned& time){
  gate->visit=true;
  const vector<fan>* tmp;
  if( gate->getfanin(tmp)){
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        dfsviset((*tmp)[i].gate,time);
      }
    }
    cout<<"["<<time<<"] ";
    gate->printGate();
    ++time;
  }
  else{
    if(gate->getTypeStr() != "UNDEF"){
      cout<<"["<<time<<"] ";
      gate->printGate();
      ++time;
    }
    
  }
}



void
CirGate::Wirte_AIG( GateList& ret) {
  
  const vector<fan>* tmp;
  if( getfanin(tmp)){
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        wirte_AIG_REC((*tmp)[i].gate,ret);
      }
    }
  }
}

void CirGate::wirte_AIG_REC(CirGate* gate,GateList& ret){
  gate->visit=true;
  const vector<fan>* tmp;
  if( gate->getfanin(tmp)){
    for(unsigned i =0;i<(*tmp).size();i++){
      if((*tmp)[i].gate->visit == false){
        wirte_AIG_REC((*tmp)[i].gate,ret);
      }
    }
    if(gate->getTypeStr() =="AIG"){
      ret.push_back(gate);
    }
  }
  else{
    if(gate->getTypeStr() == "AIG"){
      ret.push_back(gate);
    }
    
  }
}

