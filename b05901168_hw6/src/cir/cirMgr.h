/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() { 
     if(gate_all !=0){
      for(unsigned i=0;i<cir_para[0]+cir_para[3]+1;i++){
        if(gate_all[i]!= 0){
          delete[] gate_all[i];
        }
      }
      delete [] gate_all;
     }
     
     /*
     ID_IP.clear();
     ID_OP.clear();
     ID_UNDEF.clear();
     ID_AIG.clear();
     */
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const ;

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void resetvisit() const;
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   class AIG_REC{
    friend CirMgr;
    public:
      AIG_REC(unsigned i1,unsigned i2,unsigned i){  ip[0] = i1; ip[1] =i2 ; id =i; };
    private:
      unsigned ip[2];
      unsigned id;
  };
   bool isPrintable(const string& str);
private:
  unsigned cir_para[5];
  vector<unsigned> ID_IP,ID_OP,ID_UNDEF;
  vector<AIG_REC> ID_AIG;
  CirGate** gate_all;
};

#endif // CIR_MGR_H
