/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class fan{
  public:
  fan(){}
  fan(CirGate* g,bool b): gate(g),invert(b){}
  CirGate* gate;
  bool invert;
};

class CirGate
{

public:

   CirGate() :visit(false) {}
   CirGate(unsigned ID,unsigned line) :line(line),ID(ID),visit(false) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return line; }
   
   // Printing functions
   virtual void printGate() const = 0;

   virtual void reportGate() const;
   virtual void reportFanin(int level)  ;
   virtual void reportFanout(int level) ;
  
   void report_fanin_recur(CirGate* gate,unsigned space,int level,bool inv) ;  
   void report_fanout_recur(CirGate* gate,unsigned space,int level,bool inv);
   void reset_fanin(int level);
   void reset_fanout(int level);
   //self define
   virtual bool getfanin(const vector<fan>*& vec)  const { return false;}
   virtual bool getfanout(const vector<fan>*& vec) const {return false;}
   void setline(unsigned i) {line =i;}
   unsigned getid(){return ID;}
   void DFS(unsigned& time);
   void reset(){ visit=false;}
   void dfsviset(CirGate* gate,unsigned& time);
   virtual void push_back_fi(CirGate* i,bool invert) {}
   virtual void push_back_fo(CirGate* i,bool invert) {}
   virtual void setsymbol(string s){}
   virtual string getsymbol(){ return "";}
   virtual unsigned getfanin_size(){ return 0;}
   virtual unsigned getfanout_size(){ return 0;}
   void Wirte_AIG( GateList& ret);
   void wirte_AIG_REC(CirGate* gate,GateList& ret);

private:

protected:
  unsigned line;
  unsigned ID;
  bool visit;//visit and report
};

class CirGate_PI : public CirGate{
  public:
    CirGate_PI(){};
    CirGate_PI(unsigned ID,unsigned line) :CirGate(ID,line){}
    string getTypeStr() const { return "PI"; }
    unsigned getfanout_size(){ return fan_out.size();}
    void printGate() const{ 
      cout<<"PI  "<<ID;
      if(symbol.size()!=0) cout<<" ("<<symbol<<")";
      cout<<endl;
    }
    void push_back_fo(CirGate* i,bool invert) {
      fan a = fan(i,invert);
      fan_out.push_back(a);
    }
    void setsymbol(string s){symbol =s;}
    void reportGate() const{
      std::stringstream rep;
      string tmp;
      cout<<"==================================================\n";
      rep<<"= PI("<<ID<<")";
      if(symbol.size()!=0) {rep <<"\""<<symbol<<"\"";}
      rep<<", line "<<line;
      tmp=rep.str();
      tmp.resize(49,' ');
      cout<<tmp;               
      cout<<"=\n==================================================\n";
    }
    void reportFanout(int level) const;
   
    bool getfanout(const vector<fan>* &vec) const { vec = &fan_out; return true;}
  
    string getsymbol(){ return symbol;}

  protected:
    string symbol;
    vector<fan> fan_out;
};

class CirGate_PO : public CirGate{
  public:
    CirGate_PO(){}
    CirGate_PO(unsigned ID,unsigned line) :CirGate(ID,line){}
    string getTypeStr() const { return "PO"; }
    unsigned getfanin_size(){ return fan_in.size();}
    void printGate() const {
      cout<<"PO  "<<ID;
      cout<<" ";
      for (unsigned i =0;i<fan_in.size();i++){
        if(fan_in[i].gate->getTypeStr() == "UNDEF") cout<<"*";
        if(fan_in[i].invert) cout<<"!";
        cout<<fan_in[i].gate->getid();
      }
      if(symbol.size()!=0) cout<<" ("<<symbol<<")";
      cout<<endl;
    }

    void push_back_fi(CirGate* i,bool invert) {
      fan a = fan(i,invert);
      fan_in.push_back(a);
    }

    void reportGate() const{
      std::stringstream rep;
      string tmp;
      cout<<"==================================================\n";
      rep<<"= PO("<<ID<<")";
      if(symbol.size()!=0) {rep <<"\""<<symbol<<"\"";}
      rep<<", line "<<line;
      tmp=rep.str();
      tmp.resize(49,' ');
      cout<<tmp; 
      cout<<"=\n==================================================\n";
    }

    void setsymbol(string s){symbol =s;}
    bool getfanin(const vector<fan>* &vec)  const { vec = &fan_in; return true;}
    string getsymbol(){ return symbol;}
  protected:
    string symbol;
    vector<fan> fan_in;
};
class CirGate_AIG : public CirGate{
  public:
    CirGate_AIG(){}
    CirGate_AIG(unsigned ID,unsigned line) :CirGate(ID,line){}
    string getTypeStr() const { return "AIG"; }
    unsigned getfanout_size(){ return fan_out.size();}
    unsigned getfanin_size(){ return fan_in.size();}
    void printGate() const {
      cout<<"AIG "<<ID<<" ";
      for (unsigned i =0;i<fan_in.size()-1;i++){
        if(fan_in[i].gate->getTypeStr() == "UNDEF") cout<<"*";
        if(fan_in[i].invert) cout<<"!";
        cout<<fan_in[i].gate->getid()<<" ";
      }
      if(fan_in[fan_in.size()-1].gate->getTypeStr() == "UNDEF") cout<<"*";
      if(fan_in[fan_in.size()-1].invert) cout<<"!";
        cout<<fan_in[fan_in.size()-1].gate->getid();
      cout<<endl;
    }

    void push_back_fi(CirGate* i,bool invert) { fan_in.push_back(fan(i,invert));
    }
    void push_back_fo(CirGate* i,bool invert) {fan_out.push_back(fan(i,invert));}
    void reportGate() const{
      std::stringstream rep;
      string tmp;
      cout<<"==================================================\n";
      rep<<"= AIG("<<ID<<")"<<", line "<<line;
      tmp=rep.str();
      tmp.resize(49,' ');
      cout<<tmp;   
      cout<<"=\n==================================================\n";
    }

    bool getfanin(const vector<fan>* &vec)  const { vec = &fan_in; return true;}
    bool getfanout(const vector<fan>* &vec) const { vec = &fan_out; return true;}
  protected:
    vector<fan> fan_out , fan_in;
};


class CirGate_CONS : public CirGate{
  public:
    CirGate_CONS(){}
    CirGate_CONS(unsigned ID,unsigned line) :CirGate(ID,line){}

    string getTypeStr() const { return "CONST"; }
    unsigned getfanout_size(){ return fan_out.size();}
    void printGate() const{ 
      cout<<"CONST"<<ID;
      cout<<endl;
    }
    void push_back_fo(CirGate* i,bool invert) {
      fan a = fan(i,invert);
      fan_out.push_back(a);
    }

    void reportGate() const{
      std::stringstream rep;
      string tmp;
      cout<<"==================================================\n";
      rep<<"= CONST(0)"<<", line 0";
      tmp=rep.str();
      tmp.resize(49,' ');
      cout<<tmp;
      cout<<"=\n==================================================\n";
    }

    bool getfanout(const vector<fan>* &vec) const { vec = &fan_out; return true;}

  protected:
    vector<fan> fan_out;
};

class CirGate_UNDEF : public CirGate{
  public:
    CirGate_UNDEF(){}
    CirGate_UNDEF(unsigned ID,unsigned line) :CirGate(ID,line){}

    string getTypeStr() const { return "UNDEF"; }
    unsigned getfanout_size(){ return fan_out.size();}
    void printGate() const{ 
      cout<<"UNDEF "<<ID;
      cout<<endl;
    }
    void push_back_fo(CirGate* i,bool invert) {
      fan a = fan(i,invert);
      fan_out.push_back(a);
    }

    void reportGate() const{
      std::stringstream rep;
      string tmp;
      cout<<"==================================================\n";
      rep<<"= UNDEF("<<ID<<"), line 0";
      tmp=rep.str();
      tmp.resize(49,' ');
      cout<<tmp;
      cout<<"=\n==================================================\n";
    }
    
    bool getfanout(const vector<fan>* &vec) const { vec = &fan_out; return true;}

  protected:
    vector<fan> fan_out;
};

#endif // CIR_GATE_H
