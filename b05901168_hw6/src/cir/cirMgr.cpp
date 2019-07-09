/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <algorithm>
using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
  lineNo=0;
  int i =0;
  int tmp[3];
  fstream ifile;
  char cbuf;
  CirGate* gate;
  gate_all=0;
  colNo  = 0;  
  errMsg ="";
  errInt =0;
  errGate =0;
  ifile.open(fileName,ios::in);
  if(!ifile){
    cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
    return false;
  }
  ifile.get(buf,1024,' '); // get header
  if( (string)buf != "aag") {
    errMsg = string(buf);
    parseError(ILLEGAL_IDENTIFIER);
    return false;
  }
  while(i<4){  // get para
    ifile.get(cbuf);
    if(cbuf != ' '){
      parseError(ILLEGAL_WSPACE);
      return false;
    }
    ifile.get(buf,1024,' ');
    if(!myStr2Int((string)buf,tmp[0])){
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    }
    if(tmp[0]<0){
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    } 
    cir_para[i] = tmp[0];
    i++;
  }
  ifile.get(cbuf);
  if(cbuf != ' '){
      parseError(ILLEGAL_WSPACE);
      return false;
  }
  ifile.getline(buf,1024);
  if(!myStr2Int((string)buf,tmp[0])){
    errMsg=buf;
    parseError(ILLEGAL_NUM);
    return false;
  }
  if(tmp[0]<0){
    errMsg=buf;
    parseError(ILLEGAL_NUM);
    return false;
  } 
  cir_para[4] = tmp[0];  
  lineNo+=1;
  gate_all =new CirGate*[cir_para[0]+1+cir_para[3]];
  for(unsigned i=0;i<cir_para[0]+1+cir_para[3];i++){
    gate_all[i]=0;
  }
  gate_all[0] = new CirGate_CONS(0,0);

  for(unsigned i =0; i<cir_para[1];i++,lineNo++){ // get input id
    ifile.getline(buf,1024);
    if(!myStr2Int((string)buf,tmp[0])){
      colNo=i;
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    }
    if((unsigned)tmp[0]/2>cir_para[0]){
      colNo=i;
      errInt=tmp[0];
      parseError(MAX_LIT_ID);
      return false;
    }
    if(tmp[0]<0){
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    } 
    ID_IP.push_back((unsigned)tmp[0]/2);
    gate = new CirGate_PI(ID_IP[i],lineNo+1);
    if((gate_all)[tmp[0]/2] !=0){
      errGate =(gate_all)[tmp[0]/2];
      errInt=tmp[0];
      lineNo++;
      parseError(REDEF_GATE);
      return false;
    } 
    else (gate_all)[tmp[0]/2] = gate;
  }
  for(unsigned i =0; i<cir_para[3];i++,lineNo++){ //get output id
    ifile.getline(buf,1024);
    if(!myStr2Int((string)buf,tmp[0])){
      colNo=i;
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    }
    if((unsigned)tmp[0]/2>cir_para[0]){
      colNo=i;
      errInt=tmp[0];
      parseError(MAX_LIT_ID);
      return false;
    }
    if(tmp[0]<0){
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    } 
    ID_OP.push_back(tmp[0]);
  }  
  for(unsigned i =0; i<cir_para[4];i++,lineNo++){ // get aig gate
    for(unsigned j =0; j<2;j++){  
      ifile.get(buf,1024,' ');
      if(!myStr2Int((string)buf,tmp[j])){
        colNo=j;
        errMsg=buf;
        parseError(ILLEGAL_NUM);
        return false;
      }
      ifile.get(cbuf);
      if(cbuf != ' '){
        parseError(ILLEGAL_WSPACE);
        return false;
      }
      if((unsigned)tmp[j]/2>cir_para[0]){
        colNo=i;
        errInt=tmp[0];
        parseError(MAX_LIT_ID);
        return false;
      }
      if(tmp[j]<0){
        errMsg=buf;
        parseError(ILLEGAL_NUM);
        return false;
      } 
    }   
    ifile.getline(buf,1024);
    if(!myStr2Int((string)buf,tmp[2])){
      colNo=2;
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    }
    if((unsigned)tmp[2]/2>cir_para[0]){
      colNo=2;
      errInt=tmp[2];
      parseError(MAX_LIT_ID);
      return false;
    }
    if(tmp[2]<0){
      errMsg=buf;
      parseError(ILLEGAL_NUM);
      return false;
    } 
    AIG_REC a = AIG_REC((unsigned)tmp[1],(unsigned)tmp[2],(unsigned)tmp[0]/2); 
    ID_AIG.push_back(a);
    gate = new CirGate_AIG(ID_AIG[i].id,lineNo+1);
    if((gate_all)[tmp[0]/2] !=0){
      errGate =(gate_all)[tmp[0]/2];
      errInt=tmp[0];
      lineNo++;
      parseError(REDEF_GATE);
      return false;
    } 
    else (gate_all)[tmp[0]/2] = gate;
  }

  
  for(unsigned i =0; i<cir_para[4];i++){ // connect node;
    for(int k =0;k<2;k++){
      if(gate_all[ID_AIG[i].ip[k]/2] != 0){
        gate_all[ID_AIG[i].id]->push_back_fi(gate_all[ID_AIG[i].ip[k]/2],ID_AIG[i].ip[k]%2);
        gate_all[ID_AIG[i].ip[k]/2]->push_back_fo(gate_all[ID_AIG[i].id],ID_AIG[i].ip[k]%2);
      }
      else{
        gate = new CirGate_UNDEF(ID_AIG[i].ip[k]/2,0);
        ID_UNDEF.push_back(ID_AIG[i].ip[k]/2);
        gate_all[ID_AIG[i].id]->push_back_fi( gate,ID_AIG[i].ip[k]%2);
        gate->push_back_fo(gate_all[ID_AIG[i].id],ID_AIG[i].ip[k]%2);
        gate_all[ID_AIG[i].ip[k]/2] = gate;
      }      
    }     
  }
  for(unsigned i =0; i<cir_para[3];i++){ //connect output
    gate = new CirGate_PO(cir_para[0]+1+i,cir_para[1]+i+2);
    gate_all[cir_para[0]+1+i] = gate;
    if(gate_all[ID_OP[i]/2] != 0){
      gate->push_back_fi(gate_all[ID_OP[i]/2],ID_OP[i]%2);
      gate_all[ID_OP[i]/2]->push_back_fo(gate,ID_OP[i]%2);
    }
    else{
      gate = new CirGate_UNDEF(ID_OP[i]/2,0);
      ID_UNDEF.push_back(ID_OP[i]/2);
      gate_all[cir_para[0]+1+i]->push_back_fi( gate,ID_OP[i]%2);
      gate->push_back_fo(gate_all[cir_para[0]+1+i],ID_OP[i]%2);
      gate_all[ID_OP[i]/2] = gate;
    }       
  }
  while(ifile.peek() != 'c' && ifile.peek() != EOF){
    ifile.get(cbuf);
    ifile.get(buf,1024,' ');
    if(cbuf =='i'){
      if(!myStr2Int((string)buf,tmp[0])){
        colNo=0;
        errMsg=buf;
        parseError(ILLEGAL_NUM);
        return false;
      }
      if((unsigned)tmp[0]>cir_para[1]){
        colNo=0;
        errInt=tmp[0];
        parseError(MAX_LIT_ID);
        return false;
      }
      if(tmp[0]<0){
        errMsg=buf;
        parseError(ILLEGAL_NUM);
        return false;
      } 
      ifile.get(cbuf);
      ifile.getline(buf,1024);
      if((string)buf==""){
        errMsg="symbolic name";
        parseError(MISSING_IDENTIFIER);
        return false;
      }
      if(!isPrintable((string)buf)){
        colNo=1;
        parseError(ILLEGAL_SYMBOL_NAME);
        return false;
      }
      if(gate_all[ID_IP[tmp[0]]]->getsymbol() !=""){
        errMsg = 'i';
        errInt =tmp[0];
        parseError(REDEF_SYMBOLIC_NAME);
        return false;
      }
      else{
        gate_all[ID_IP[tmp[0]]]->setsymbol((string)buf);
      }      
    }
    else if(cbuf == 'o'){
      if(!myStr2Int((string)buf,tmp[0])){
        colNo=0;
        errMsg=buf;
        parseError(ILLEGAL_NUM);
        return false;
      }
      if((unsigned)tmp[0]>cir_para[3]){
        colNo=0;
        errInt=tmp[0];
        parseError(MAX_LIT_ID);
        return false;
      }
      if(tmp[0]<0){
        errMsg=buf;
        parseError(ILLEGAL_NUM);
        return false;
      } 
      ifile.get(cbuf);
      ifile.getline(buf,1024);
      if((string)buf==""){
        errMsg="symbolic name";
        parseError(MISSING_IDENTIFIER);
        return false;
      }
      if(!isPrintable((string)buf)){
        colNo=1;
        parseError(ILLEGAL_SYMBOL_NAME);
        return false;
      }
      if(gate_all[cir_para[0]+tmp[0]+1]->getsymbol() !=""){
        errMsg = 'o';
        errInt =tmp[0];
        parseError(REDEF_SYMBOLIC_NAME);
        return false;
      }
      else gate_all[cir_para[0]+tmp[0]+1]->setsymbol((string)buf);
    }
    else{
      colNo=0;
      errMsg=cbuf;
      parseError(ILLEGAL_SYMBOL_TYPE);
      return false;
    }
    lineNo++;
  }
  if(ifile.peek() =='c'){
    ifile.getline(buf,1024);
    if((string)buf != "c"){
      colNo=1;
      parseError(MISSING_NEWLINE);
      return false;
    }
  }

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    cout<<"Circuit Statistics\n"
        <<"==================\n"
        <<"  PI  "<<setw(10)<<cir_para[1]<<"\n"
        <<"  PO  "<<setw(10)<<cir_para[3]<<"\n"
        <<"  AIG "<<setw(10)<<cir_para[4]<<"\n"
        <<"------------------\n"
        <<"  Total"<<setw(9)<<cir_para[1]+cir_para[3]+cir_para[4]<<"\n";

}

void
CirMgr::printNetlist() const
{
  unsigned time=0;
  for(unsigned i=0;i<cir_para[3];i++){
    gate_all[cir_para[0]+1+i]->DFS(time);
  }  
  resetvisit();
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned i=0;i<cir_para[1];i++){
     cout<<" "<<ID_IP[i];
   }   
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i=0; i<cir_para[3]; i++){
     cout<<" "<<cir_para[0]+1+i;
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{ 
  vector<unsigned> rec;
  const vector<fan>* tmp;
  for(unsigned i=0;i<ID_UNDEF.size();i++){
    if( gate_all[ID_UNDEF[i]]->getfanout(tmp)){
      for(unsigned j =0;j<(*tmp).size();j++){
        rec.push_back((*tmp)[j].gate->getid());
      }
    }
  }
  std::sort(rec.begin(),rec.end());
  vector<unsigned>::iterator pos;
  rec.erase(unique(rec.begin(),rec.end()),rec.end());
  if(!rec.empty()){
    cout<<"Gates with floating fanin(s):";
    for(unsigned i=0;i<rec.size();i++){
      cout<<" "<<rec[i]; 
    }
    cout<<endl;
    rec.clear();
  }
  for(unsigned i=1; i<cir_para[0]+1; i++){
    if(gate_all[i]!=0){
      if(gate_all[i]->getfanout_size() == 0){
      rec.push_back(gate_all[i]->getid());
      }
    }
  }
  std::sort(rec.begin(),rec.end());
  if(!rec.empty()){
    cout<<"Gates defined but not used  :";
    for(unsigned i=0;i<rec.size();i++){
      cout<<" "<<rec[i]; 
    }
    cout<<endl;
    rec.clear();
  }  
}
void
CirMgr::writeAag(ostream& outfile) const
{
  GateList AIG ;
  for(unsigned i=0;i<cir_para[3];i++){
    gate_all[cir_para[0]+1+i]->Wirte_AIG(AIG);
  }
  resetvisit();
  outfile.write("aag",3);
  outfile.put(' ');
  
  for(unsigned i=0;i<4;i++){
    outfile<<cir_para[i];
    outfile.put(' ');
  }
  outfile<<AIG.size();
  outfile.put('\n');
  for(unsigned i=0;i<ID_IP.size();i++){
    outfile<<2*ID_IP[i];
    outfile.put('\n');
  }
  for(unsigned i=0;i<ID_OP.size();i++){
    outfile<<ID_OP[i];
    outfile.put('\n');
  }
  for(unsigned i=0;i<AIG.size();i++){
    outfile<<2*AIG[i]->getid();
    const vector<fan>* tmp;
    if( AIG[i]->getfanin(tmp))
    for(unsigned j =0;j<(*tmp).size();j++){
      outfile.put(' ');
      if((*tmp)[j].invert) outfile<<(2*(*tmp)[j].gate->getid()+1);
      else outfile<<(2*(*tmp)[j].gate->getid());
    }
    outfile.put('\n');
  }
  for(unsigned i=0;i<cir_para[1];i++){
    if(gate_all[ID_IP[i]]->getsymbol() != ""){
      outfile<<'i'<<i<<' '<<gate_all[ID_IP[i]]->getsymbol()<<'\n';
    }
  }
  for(unsigned i=0;i<cir_para[3];i++){
    if(gate_all[cir_para[0]+i+1]->getsymbol() != ""){
      outfile<<'o'<<i<<' '<<gate_all[cir_para[0]+i+1]->getsymbol()<<'\n';
    }
  }  
}


CirGate* 
CirMgr::getGate(unsigned gid) const {  
  if(gid<cir_para[0]+cir_para[3]+1) return gate_all[gid];
  return 0;
}

void 
CirMgr::resetvisit() const{
  for(unsigned i=0;i<=cir_para[0]+cir_para[3];i++){
    if(gate_all[i]!=0){
      gate_all[i]->reset();
    }
  }
}

bool
CirMgr::isPrintable(const string& str)
{
   size_t n = str.size();
   if (n == 0) return false;
   for (size_t i = 0; i < n; ++i)
      //if ( (str[i])<32 || str[i] >126)
      if(!isprint(str[i]))
        return false;
   return true;
}
